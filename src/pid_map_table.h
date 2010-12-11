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

void pid_map_table_clear(ACCESS_HFPGA_PID_MAP *pid_map);
void pid_map_table_reset();
int pid_map_table_apply(void *pid_map_data, int size);
void pid_map_table_generate_from_fp(uint8_t * buf);

/*
 * route to generate pid_map table
 */
struct pid_map_table_gen_context {
	ACCESS_HFPGA_PID_MAP pid_map;

	uint8_t cur_chan_idx;
	uint8_t cur_chan_map_pid_cnt;
};

void pid_map_table_gen_start(struct pid_map_table_gen_context *ctx);
void pid_map_table_gen_end(struct pid_map_table_gen_context *ctx, uint8_t chan_bitmap);
void pid_map_table_push_pid_pair(struct pid_map_table_gen_context *ctx,
		uint8_t chan_idx, uint16_t in_pid, uint16_t out_pid);


#endif /* _PID_MAP_TABLE_H_ */

