#ifndef _IO_TABLE_H_
#define _IO_TABLE_H_

#include <string.h>

#include "wu/wu_base_type.h"


#define IO_PID_FLAG_SELECTED		(1 << 0)
#define IO_PID_FLAG_JUST_ADDED		(1 << 1)
#define IO_PID_FLAG_IS_INPUT_PID	(1 << 2)
#define IO_PID_FLAG_PMT_PID			(1 << 3)

struct io_pid_entry {
	uint8_t flags;
	uint8_t reserved;
	uint16_t out_pid;
};

void build_io_table();
void dump_io_table(const char *ctx);
bool check_user_output_pid(PROG_INFO_T *old_prog,
		PROG_INFO_T *sel_prog, int sel_prog_idx, PROG_INFO_T *prog_table);
void fix_selected_program_output_pid(PROG_INFO_T *sel_prog, int sel_prog_idx,
		PROG_INFO_T *prog_table, bool do_fix_selected_programs);


extern struct io_pid_entry io_table[CHANNEL_MAX_NUM][NULL_PID + 1];


#endif /* _IO_TABLE_H_ */

