#ifndef _PID_MAP_TABLE_H_
#define _PID_MAP_TABLE_H_

#include "wu/wu_base_type.h"

#include "xmux.h"


static inline void
pid_map_table_set_in_pid(ACCESS_HFPGA_PID_MAP *pid_map,
	 	int chan_idx, int pid_idx, uint16_t pid)
{
	pid_map->pid_map[chan_idx * FPGA_PID_MAP_TABLE_CHAN_PIDS + pid_idx].in_pid = pid;
}
static inline void
pid_map_table_set_out_pid(ACCESS_HFPGA_PID_MAP *pid_map,
	 	int chan_idx, int pid_idx, uint16_t pid)
{
	pid_map->pid_map[chan_idx * FPGA_PID_MAP_TABLE_CHAN_PIDS + pid_idx].out_pid = pid;
}

void pid_map_table_reset();
int pid_map_table_apply(void *pid_map_data, int size);
void pid_map_table_generate_from_fp(uint8_t * buf);


#endif /* _PID_MAP_TABLE_H_ */

