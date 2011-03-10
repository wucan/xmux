#ifndef _OUTPUT_PID_CHECK_H_
#define _OUTPUT_PID_CHECK_H_

#include "xmux.h"


/*
 * output_pid reference table, used for checking pid validate
 */
struct pid_ref_info {
	uint8_t type;
	uint8_t ref_cnt;
	uint8_t prog_idx;
	uint16_t in_pid;
};
// type bits
#define PMT_BIT			(1 << 0)
#define PCR_BIT			(1 << 1)
#define DATA_BIT		(1 << 2)

bool build_pid_ref_table(PROG_INFO_T *prog_table);
void dump_pid_ref_table(const char *context);

extern struct pid_ref_info pid_ref_table[];


#endif /* _OUTPUT_PID_CHECK_H_ */

