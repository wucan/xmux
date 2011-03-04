#include <string.h>

#include "wu/wu_base_type.h"
#include "wu/message.h"

#include "front_panel_intstr.h"
#include "front_panel_define.h"
#include "output_pid_check.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "output_pid_check"};

extern int prog_pid_val_isvalid(uint16_t prog_pid);

struct pid_ref_info pid_ref_table[0x1FFF + 1];

static bool pid_is_used(uint16_t pid)
{
	if (pid_ref_table[pid].ref_cnt > 0) {
		return true;
	}

	return false;
}

static void add_pid_to_table(uint16_t pid, int type_bit, int prog_idx)
{
	struct pid_ref_info *ref = &pid_ref_table[pid];

	ref->type |= type_bit;
	ref->ref_cnt++;
	ref->prog_idx = prog_idx;
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

	memset(pid_ref_table, 0, sizeof(pid_ref_table));
	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		prog = &prog_table[i];
		attr = &g_prog_attr_table[i];
		if (prog->status != 1)
			continue;

		/*
		 * PMT pid should be unique
		 */
		ref = &pid_ref_table[prog->info.pmt.out];
		if (ref->ref_cnt > 0) {
			trace_err("pmt pid %#x used! build pid ref table failed!",
				prog->info.pmt.out);
			return false;
		}
		ref->ref_cnt++;
		ref->type = PMT_BIT;
		ref->prog_idx = i;

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
				ref->type |= DATA_BIT;
				ref->ref_cnt++;
				ref->prog_idx = i;
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
		ref->ref_cnt++;
		ref->type |= PCR_BIT;
		ref->prog_idx = i;
	}

	trace_info("pid ref table:");
	for (i = 0; i < 0x1FFF; i++) {
		ref = &pid_ref_table[i];
		if (ref->type == 0)
			continue;
		trace_info("pid %#x, type %s, ref_cnt %d, prog_idx %d",
			i, pid_ref_type_name(ref->type),
			ref->ref_cnt, ref->prog_idx);
	}

	return true;
}

/*
 * check selectecd program output pid is validate
 */
bool check_selected_program_output_pid(int sel_prog_idx, PROG_INFO_T * prog_table)
{
	int i;
	PROG_INFO_T *sel_prog = prog_table + sel_prog_idx;
	struct pid_ref_info *ref;
	int chan_idx = sel_prog_idx / PROGRAM_MAX_NUM;
	struct program_attribute *attr = &g_prog_attr_table[sel_prog_idx];

	build_pid_ref_table(prog_table);

	// check pmt
	ref = &pid_ref_table[sel_prog->info.pmt.out];
	if (ref->ref_cnt > 0) {
		trace_err("pmt pid %#x had been used!", sel_prog->info.pmt.out);
		return false;
	}

	// check data pid
	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
		ref = &pid_ref_table[sel_prog->info.data[i].out];
		if (ref->ref_cnt > 0) {
			trace_err("data pid %#x had been used by other programs!",
				sel_prog->info.data[i].out);
			return false;
		}
	}

	// check pcr
	ref = &pid_ref_table[sel_prog->info.pcr.out];
	if (attr->pcr_type == SOLO_PCR || attr->pcr_type == COM_PCR) {
		if (ref->ref_cnt > 0) {
			trace_err("%s pcr pid %#x had been used!",
				pcr_type_name(g_prog_attr_table[sel_prog_idx].pcr_type),
				sel_prog->info.pcr.out);
			return false;
		}
	} else {
		struct program_attribute *ref_attr;
		// PUB PCR
		if (ref->ref_cnt > 0) {
			ref_attr = &g_prog_attr_table[ref->prog_idx];
			if (ref->type != PCR_BIT || attr->pcr_group_id != ref_attr->pcr_group_id) {
				trace_err("PUB pcr pid %#x had been used!",
					sel_prog->info.pcr.out);
				return false;
			}
		}

		// ok, fix other programs pub pcr value
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
				tmp_prog->info.pcr.out = sel_prog->info.pcr.out;
			}
		}
	}

	return true;
}

