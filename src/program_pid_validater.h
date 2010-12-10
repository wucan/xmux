#ifndef PROGRAM_PID_VALIDATER_H_
#define PROGRAM_PID_VALIDATER_H_

#include "wu/wu_base_type.h"


int prog_pid_val_isvalid(uint16_t prog_pid);
int pids_isvalid_in_program(PROG_INFO_T * pProg);
int is_prog_pidsel_in_program(uint16_t npid, PROG_INFO_T * pProgPara);
int current_prog_pids_is_repeat(int selporg, PROG_INFO_T * pProgPara);
int valid_map_pids_in_one_program(PROG_INFO_T * pProg);
int valid_map_pids_in_one_channel(int nselchn, PROG_INFO_T * pProgpara);
int seleted_programs_quant(PROG_INFO_T * pProgPara);


#endif /* PROGRAM_PID_VALIDATER_H_ */

