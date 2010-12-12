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
int pids_isvalid_in_program(PROG_INFO_T * pProg)
{
	int i;

	trace_info("check program output pids valid ...");
	if (!prog_pid_val_isvalid(pProg->info.pmt.out)
		|| !prog_pid_val_isvalid(pProg->info.pcr.out)) {
		trace_warn("PMT/PCR invalid!");
		return enm_prog_pid_val_err;
	}
	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
		uint16_t out_pid = pProg->info.data[i].out;

		if (out_pid != 0x000F && out_pid != 0x0000 &&
			(!prog_pid_val_isvalid(out_pid))) {
			trace_warn("data pid %d invalid!", out_pid);
			return enm_prog_pid_val_err;
		}
	}

	if (pProg->info.pmt.out == pProg->info.pcr.out) {
		trace_warn("PMT == PCR!");
		return enm_prog_pid_pmt_pcr;
	}

	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
		if (pProg->info.data[i].in == pProg->info.pcr.in)
			pProg->info.data[i].out = pProg->info.pcr.out;
	}

	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
		int j;
		for (j = i + 1; j < PROGRAM_DATA_PID_MAX_NUM; j++) {
			if (prog_pid_val_isvalid(pProg->info.data[j].out)) {
				if (pProg->info.data[i].out == pProg->info.data[j].out) {
					trace_warn("data pid %d repeated!", pProg->info.data[j].out);
					return enm_prog_pid_other_other;
				}
			}
		}
	}

	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
		uint16_t in_pid = pProg->info.data[i].in;
		uint16_t out_pid = pProg->info.data[i].out;
		if (pProg->info.pmt.out == out_pid) {
			trace_warn("PMT pid had used in data pids!");
			return enm_prog_pid_pmt_other;
		}
		if (pProg->info.pcr.out == out_pid && pProg->info.pcr.in != in_pid) {
			trace_warn("PCR pid had used in data pids!");
			return enm_prog_pid_pcr_other;
		}
	}

	trace_info("check passed");

	return enm_prog_pid_valid;
}

int is_prog_pidsel_in_program(uint16_t npid, PROG_INFO_T * pProgPara)
{
	int i;

	trace_info("check program selected pid %d valide ...");
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

	trace_info("check passed");

	return enm_prog_pid_valid;
}

int current_prog_pids_is_repeat(int selporg, PROG_INFO_T * pProgPara)
{
	int i;
	PROG_INFO_T *pProgsel = pProgPara + selporg;

	trace_info("check program %d pid repeat ...", selporg);
	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		int j;
		PROG_INFO_T *pProgtmp = pProgPara + i;
		if (selporg == i)
			continue;
		if (pProgtmp->status != 1)
			continue;
		if (is_prog_pidsel_in_program(pProgsel->info.pmt.out, pProgtmp) !=
			enm_prog_pid_valid
			|| is_prog_pidsel_in_program(pProgsel->info.pcr.out,
										 pProgtmp) != enm_prog_pid_valid) {
			trace_warn("PMT/PCR output pid invalid!");
			return enm_prog_pid_program;
		}
		for (j = 0; j < PROGRAM_DATA_PID_MAX_NUM; j++) {
			if (is_prog_pidsel_in_program(pProgsel->info.data[i].out, pProgtmp)
				!= enm_prog_pid_valid) {
				trace_warn("data output pid #%d invalid!", i);
				return enm_prog_pid_program;
			}
		}
	}

	trace_info("check passed");

	return enm_prog_pid_valid;
}

///////////////////
// the count of pcr_pid adn other_pids in the program 
////////////////////
int valid_map_pids_in_one_program(PROG_INFO_T * pProg)
{
	int j;
	int npidcount = 1;

	trace_info("valid map pids in program");
	for (j = 0; j < PROGRAM_DATA_PID_MAX_NUM; j++) {
		uint16_t in_pid = pProg->info.data[j].in;
		if (pProg->info.pcr.in != in_pid && prog_pid_val_isvalid(in_pid)) {
			npidcount++;
		}
	}
	trace_info("there are %d mapped pids valid", npidcount);

	return npidcount;
}

///////////////////
// the count of pcr_pid adn other_pids the selected programs in a channel 
////////////////////
int valid_map_pids_in_one_channel(int nselchn, PROG_INFO_T * pProgpara)
{
	int npidcountchn = 0;		//
	int prog_cnt;

	trace_info("valid map pids in channel %d", nselchn);
	for (prog_cnt = 0; prog_cnt < PROGRAM_MAX_NUM; prog_cnt++) {
		PROG_INFO_T *pProg =
			pProgpara + nselchn * PROGRAM_MAX_NUM + prog_cnt;
		if (pProg->status == 1) {
			npidcountchn += valid_map_pids_in_one_program(pProg);
		}
	}
	trace_info("there are %d mapped pids valid", npidcountchn);

	return npidcountchn;
}

int seleted_programs_quant(PROG_INFO_T * pProgPara)
{
	int i;
	int nselected = 0;

	trace_info("select program quant ...");
	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		PROG_INFO_T *pProg = pProgPara + i;
		if (pProg->status != 1)
			continue;
		nselected++;
	}
	trace_info("%d programs selected", nselected);

	return nselected;
}

