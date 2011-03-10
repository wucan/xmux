#include <string.h>

#include "wu/wu_base_type.h"
#include "wu/message.h"

#include "front_panel_intstr.h"
#include "front_panel_define.h"
#include "output_pid_check.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "output_pid_check"};

extern int prog_pid_val_isvalid(uint16_t prog_pid);

static int pid_ref_table_nprogs;
struct pid_ref_info pid_ref_table[0x1FFF + 1];

static bool pid_is_used(uint16_t pid)
{
	if (pid_ref_table[pid].ref_cnt > 0) {
		return true;
	}

	return false;
}

static void add_pid_to_table(struct pid_trans_entry pid_ent, int type_bit, int prog_idx)
{
	struct pid_ref_info *ref = &pid_ref_table[pid_ent.out];

	ref->type |= type_bit;
	ref->ref_cnt++;
	ref->prog_idx = prog_idx;
	ref->in_pid = pid_ent.in;
}

static void remove_pid_from_table(uint16_t pid)
{
	struct pid_ref_info *ref = &pid_ref_table[pid];

	if (ref->ref_cnt <= 0) {
		trace_err("%s Bug!", __func__);
	}
	ref->ref_cnt--;
	if (ref->ref_cnt == 0) {
		memset(ref, 0, sizeof(*ref));
	}
}

static uint16_t ref_2_pid(struct pid_ref_info *ref)
{
	return ((uint32_t)ref - (uint32_t)pid_ref_table) / sizeof(ref[0]);
}

static struct pid_ref_info *get_pub_pcr_ref(uint8_t pcr_group_id)
{
	struct program_attribute *ref_attr;
	struct pid_ref_info *ref;
	int i;

	for (i = 0; i < 0x1FFF; i++) {
		ref = &pid_ref_table[i];
		if (ref->ref_cnt && ref->type == PCR_BIT) {
			ref_attr = &g_prog_attr_table[ref->prog_idx];
			if (pcr_group_id == ref_attr->pcr_group_id) {
				return ref;
			}
		}
	}

	return NULL;
}

static uint16_t next_start_pid = 0x20;
static uint16_t pick_pid()
{
	uint16_t pid;
	struct pid_ref_info *ref;

	for (pid = next_start_pid; pid < 0x1FFE; pid++) {
		ref = &pid_ref_table[pid];
		if (!ref->ref_cnt) {
			/*
			 * FIXME: how about if pid had been conflict with current
			 * selected program pid?
			 */
			next_start_pid = pid + 1;
			break;
		}
	}

	return pid;
}

static const const char * pid_ref_type_name(uint8_t type)
{
	if (type == PMT_BIT)
		return "PMT";
	else if (type == PCR_BIT)
		return "PCR";
	else if (type == DATA_BIT)
		return "DATA";
	else if (type == PCR_BIT | DATA_BIT)
		return "PCR/DATA";

	return "INV";
}

/*
 * build output pid ref table
 */
static bool build_pid_ref_table(PROG_INFO_T *prog_table)
{
	int i, j;
	uint16_t out_pid;
	PROG_INFO_T *prog;
	struct pid_ref_info *ref;
	struct program_attribute *attr;
	int nprogs_sel = 0;

	memset(pid_ref_table, 0, sizeof(pid_ref_table));
	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		prog = &prog_table[i];
		attr = &g_prog_attr_table[i];
		if (prog->status != 1)
			continue;
		nprogs_sel++;
		/*
		 * PMT pid should be unique
		 */
		ref = &pid_ref_table[prog->info.pmt.out];
		if (ref->ref_cnt > 0) {
			trace_err("pmt pid %#x used! build pid ref table failed!",
				prog->info.pmt.out);
			return false;
		}
		add_pid_to_table(prog->info.pmt, PMT_BIT, i);

		/*
		 * DATA pid should be unique before put PCR
		 */
		for (j = 0; j < PROGRAM_DATA_PID_MAX_NUM; j++) {
			out_pid = prog->info.data[j].out;
			if (prog_pid_val_isvalid(out_pid)) {
				ref = &pid_ref_table[out_pid];
				if (ref->ref_cnt > 0) {
					trace_err("data pid %#x had been used!", out_pid);
					return false;
				}
				add_pid_to_table(prog->info.data[j], DATA_BIT, i);
			}
		}

		/*
		 * PCR
		 */
		ref = &pid_ref_table[prog->info.pcr.out];
		if (ref->ref_cnt > 0) {
			if (attr->pcr_type == PUB_PCR) {
				struct program_attribute *ref_attr;
				ref_attr = &g_prog_attr_table[ref->prog_idx];
				if (ref->type != PCR_BIT ||
					attr->pcr_group_id != ref_attr->pcr_group_id) {
					trace_err("PUB pcr pid %#x had been used!",
						prog->info.pcr.out);
					return false;
				}
			} else if (attr->pcr_type == COM_PCR) {
				// checked in DATA pid
			} else if (attr->pcr_type == SOLO_PCR) {
				trace_err("SOLO pcr pid %#x had been used!",
					prog->info.pcr.out);
				return false;
			}
		}
		add_pid_to_table(prog->info.pcr, PCR_BIT, i);
	}

	pid_ref_table_nprogs = nprogs_sel;

	return true;
}

static void dump_pid_ref_table(const char *context)
{
	int i;
	struct pid_ref_info *ref;

	trace_info("%s %d programs selected, and pid ref table is:",
		context, pid_ref_table_nprogs);
	for (i = 0; i < 0x1FFF; i++) {
		ref = &pid_ref_table[i];
		if (ref->type == 0)
			continue;
		trace_info("pid %#x(<= %#x), type %s, ref_cnt %d, prog_idx %d",
			i, ref->in_pid, pid_ref_type_name(ref->type),
			ref->ref_cnt, ref->prog_idx);
	}
}

/*
 * check selectecd program output pid is validate
 */
void fix_selected_program_output_pid(PROG_INFO_T *sel_prog, int sel_prog_idx,
		PROG_INFO_T * prog_table, bool do_fix_selected_programs)
{
	int i;
	struct pid_ref_info *ref;
	int chan_idx = sel_prog_idx / PROGRAM_MAX_NUM;
	struct program_attribute *attr = &g_prog_attr_table[sel_prog_idx];

	build_pid_ref_table(prog_table);
	dump_pid_ref_table("current");
	next_start_pid = 0x20;

	// check pmt
	if (pid_is_used(sel_prog->info.pmt.in)) {
		sel_prog->info.pmt.out = pick_pid();
	} else {
		sel_prog->info.pmt.out = sel_prog->info.pmt.in;
	}
	add_pid_to_table(sel_prog->info.pmt, PMT_BIT, sel_prog_idx);
	trace_info("pmt out pid = %#x", sel_prog->info.pmt.out);

	// check data pid
	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
		if (!prog_pid_val_isvalid(sel_prog->info.data[i].in)) {
			continue;
		}
		if (pid_is_used(sel_prog->info.data[i].in)) {
			sel_prog->info.data[i].out = pick_pid();
		} else {
			sel_prog->info.data[i].out = sel_prog->info.data[i].in;
		}
		add_pid_to_table(sel_prog->info.data[i], DATA_BIT, sel_prog_idx);

		/*
		 * COM PCR should had same output pid
		 */
		if (attr->pcr_type == COM_PCR &&
			sel_prog->info.data[i].in == sel_prog->info.pcr.in) {
			sel_prog->info.pcr.out = sel_prog->info.data[i].out;
			add_pid_to_table(sel_prog->info.pcr, PCR_BIT, sel_prog_idx);
		}
	}

	// check pcr
	if (attr->pcr_type == SOLO_PCR) {
		if (pid_is_used(sel_prog->info.pcr.in)) {
			sel_prog->info.pcr.out = pick_pid();
		} else {
			sel_prog->info.pcr.out = sel_prog->info.pcr.in;
		}
		add_pid_to_table(sel_prog->info.pcr, PCR_BIT, sel_prog_idx);
	} else if (attr->pcr_type == COM_PCR) {
		// had alread add in DATA PID!
	} else {
		// PUB PCR
		struct program_attribute *ref_attr;
		ref = &pid_ref_table[sel_prog->info.pcr.in];
		if (ref->ref_cnt > 0) {
			ref_attr = &g_prog_attr_table[ref->prog_idx];
			if (ref->type == PCR_BIT && attr->pcr_group_id == ref_attr->pcr_group_id) {
					sel_prog->info.pcr.out = ref_2_pid(ref);
					add_pid_to_table(sel_prog->info.pcr, PCR_BIT, sel_prog_idx);
				goto pub_pcr_done;
			}

			trace_warn("PUB in pcr %#x had been used!", sel_prog->info.pcr.in);
			trace_info("try use other PUB pcr programs pcr pid...");
			ref = get_pub_pcr_ref(attr->pcr_group_id);
			if (ref) {
				sel_prog->info.pcr.out = ref_2_pid(ref);
			} else {
				sel_prog->info.pcr.out = pick_pid();
			}
			add_pid_to_table(sel_prog->info.pcr, PCR_BIT, sel_prog_idx);
			goto pub_pcr_done;
		}

		trace_info("use PUB in pcr %#x...", sel_prog->info.pcr.in);
		sel_prog->info.pcr.out = sel_prog->info.pcr.in;
		add_pid_to_table(sel_prog->info.pcr, PCR_BIT, sel_prog_idx);
		// ok, fix other programs pub pcr value
		if (do_fix_selected_programs)
		for (i = chan_idx * PROGRAM_MAX_NUM;
			 i < chan_idx * PROGRAM_MAX_NUM + g_chan_num.num[chan_idx]; i++) {
			PROG_INFO_T *tmp_prog = &prog_table[i];
			if (sel_prog_idx == i)
				continue;
			if (tmp_prog->status != 1)
				continue;
			ref_attr = &g_prog_attr_table[i];
			if (attr->pcr_group_id == ref_attr->pcr_group_id) {
				trace_warn("fix program #%d output pcr pid(%#x -> %#x)!",
					i, tmp_prog->info.pcr.out, sel_prog->info.pcr.out);
				remove_pid_from_table(tmp_prog->info.pcr.out);
				tmp_prog->info.pcr.out = sel_prog->info.pcr.out;
			}
		}
pub_pcr_done:
		;;
	}

	pid_ref_table_nprogs++;
	dump_pid_ref_table("after fix and select");
}

