#include "wu/wu_base_type.h"
#include "wu/message.h"

#include "front_panel_intstr.h"
#include "front_panel_define.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "program_pid_validater"};

int prog_pid_val_isvalid(uint16_t prog_pid)
{
	if (defProgPidMin <= prog_pid && prog_pid <= defProgPidMax)
		return 1;
	return 0;
}

///////////////////
// if other_pids in the program is same
// same: 0 ;not same 1
////////////////////
static int valid_program_pids(PROG_INFO_T * pProg)
{
	int i, j;
	struct pid_trans_entry *pids = &pProg->info.pmt;

	/*
	 * check all output pid validate
	 */
	if (!prog_pid_val_isvalid(pProg->info.pmt.out)
		|| !prog_pid_val_isvalid(pProg->info.pcr.out)) {
		trace_warn("PMT/PCR invalid!");
		return enm_prog_pid_val_err;
	}
	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
		uint16_t out_pid = pProg->info.data[i].out;

		if (out_pid != DATA_PID_PAD_VALUE && out_pid != 0x0000 &&
			(!prog_pid_val_isvalid(out_pid))) {
			trace_warn("data pid %d invalid!", out_pid);
			return enm_prog_pid_val_err;
		}
	}

	/*
	 * we only ensure the output pids had same inter-pid relation
	 * with the input pids
	 */
	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM + 2; i++) {
		for (j = i + 1; j < PROGRAM_DATA_PID_MAX_NUM + 2; j++) {
			if (pids[i].in == pids[j].in) {
				if (pids[i].out != pids[j].out) {
					trace_warn("input pid same, but output pid diff!");
					return enm_prog_pid_other_other;
				}
			} else {
				if (pids[i].out == pids[j].out) {
					trace_warn("output pid same, but input pid diff!");
					return enm_prog_pid_other_other;
				}
			}
		}
	}

	return enm_prog_pid_valid;
}

int is_prog_pidsel_in_program(uint16_t npid, PROG_INFO_T * pProgPara)
{
	int i;

	if (prog_pid_val_isvalid(npid)) {
		if (npid == pProgPara->info.pmt.out ||
			npid == pProgPara->info.pcr.out) {
			trace_warn("this pid had already used by output PMT/PCR!");
			return enm_prog_pid_program;
		}
		for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
			if (npid == pProgPara->info.data[i].out) {
				trace_warn("this pid had already used by output data pid #%d!",
					i);
				return enm_prog_pid_program;
			}
		}
	}

	return enm_prog_pid_valid;
}

///////////////////
// the count of pcr_pid adn other_pids in the program 
////////////////////
static int program_mapped_pid_count(PROG_INFO_T * pProg)
{
	int j;
	int npidcount = 1;

	for (j = 0; j < PROGRAM_DATA_PID_MAX_NUM; j++) {
		uint16_t in_pid = pProg->info.data[j].in;
		if (pProg->info.pcr.in != in_pid && prog_pid_val_isvalid(in_pid)) {
			npidcount++;
		}
	}

	return npidcount;
}

///////////////////
// the count of pcr_pid adn other_pids the selected programs in a channel 
////////////////////
static int channel_mapped_pid_count(int nselchn)
{
	int npidcountchn = 0;		//
	int prog_cnt;

	for (prog_cnt = 0; prog_cnt < PROGRAM_MAX_NUM; prog_cnt++) {
		PROG_INFO_T *pProg =
			g_prog_info_table + nselchn * PROGRAM_MAX_NUM + prog_cnt;
		if (pProg->status == 1) {
			npidcountchn += program_mapped_pid_count(pProg);
		}
	}

	return npidcountchn;
}

static int program_selected_count()
{
	int i;
	int nselected = 0;

	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		PROG_INFO_T *pProg = g_prog_info_table + i;
		if (pProg->status != 1)
			continue;
		nselected++;
	}

	return nselected;
}

/*
 * @sel_prog - which include new program prameter
 */
bool check_and_select_program(int prog_idx, PROG_INFO_T *sel_prog,
	PROG_INFO_T *old_prog)
{
	int chan_idx_sel = prog_idx / PROGRAM_MAX_NUM;
	uint8_t old_status;

	if (g_prog_info_table[prog_idx].status == 1 &&
		memcmp(&sel_prog->info, &g_prog_info_table[prog_idx].info, sizeof(sel_prog->info)) == 0) {
		trace_warn("selected program's parameter same, do nothing!");
		return true;
	}

	// backup old status
	old_status = g_prog_info_table[prog_idx].status;
	g_prog_info_table[prog_idx].status = 0;

	if (program_selected_count() + 1 > defSelectedProgFpga) {
		trace_err("cann't select more programs!");
	} else if (valid_program_pids(sel_prog) != enm_prog_pid_valid) {
		trace_err("selected program had invalid pid!");
	} else if (channel_mapped_pid_count(chan_idx_sel) +
		program_mapped_pid_count(sel_prog) > FPGA_PID_MAP_TABLE_CHAN_PIDS) {
		trace_err("this channel had exceed pid count to map!");
	} else {
		if (old_prog) {
			if (!check_user_output_pid(old_prog, sel_prog, prog_idx,
				g_prog_info_table)) {
				trace_err("check user set program output pid failed!");
				goto failed_out;
			}
		} else {
			fix_selected_program_output_pid(sel_prog, prog_idx,
				g_prog_info_table, true);
		}
		sel_prog->status = 1;
		g_prog_info_table[prog_idx] = *sel_prog;
		trace_info("program #%d success selected", prog_idx);
		xmux_program_info_dump(&sel_prog->info, "fix selected");
		return true;
	}

failed_out:
	// restore old status
	g_prog_info_table[prog_idx].status = old_status;

	return false;
}

void prepare_program_output_pid(int prog_idx, PROG_INFO_T *prog)
{
	if (prog->status == 1) {
		return;
	}
	fix_selected_program_output_pid(prog, prog_idx, g_prog_info_table, false);
}

