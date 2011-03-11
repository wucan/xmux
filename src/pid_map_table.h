#ifndef _PID_MAP_TABLE_H_
#define _PID_MAP_TABLE_H_

#include "wu/wu_base_type.h"

#include "xmux.h"
#include "hfpga.h"


static inline void
pid_map_table_set_in_pid(struct xmux_pid_map_table *pid_map,
	 	int chan_idx, int pid_idx, uint16_t pid)
{
	pid_map->chans[chan_idx].ents[pid_idx].input_pid = pid;
}
static inline void
pid_map_table_set_out_pid(struct xmux_pid_map_table *pid_map,
	 	int chan_idx, int pid_idx, uint16_t pid)
{
	pid_map->chans[chan_idx].ents[pid_idx].output_pid = pid;
}

void pid_map_table_clear(struct xmux_pid_map_table *pid_map);
void fpga_pid_map_table_clear(ACCESS_HFPGA_PID_MAP *pid_map);
void pid_map_table_reset();
int pid_map_table_apply(struct xmux_pid_map_table *pid_map_data);

/*
 * route to generate pid_map table
 */
struct pid_map_table_gen_context {
	ACCESS_HFPGA_PID_MAP fpga_pid_map;

	uint8_t chan_map_pid_cnt[CHANNEL_MAX_NUM];
};

void pid_map_table_gen_start(struct pid_map_table_gen_context *ctx);
void pid_map_table_gen_end(struct pid_map_table_gen_context *ctx, uint8_t chan_bitmap);
int pid_map_table_push_pid_pair(struct pid_map_table_gen_context *ctx,
		uint8_t chan_idx, uint16_t in_pid, uint16_t out_pid);
bool pid_map_table_validate(struct xmux_pid_map_table *pid_map);
void pid_map_table_dump(struct xmux_pid_map_table *pid_map);
void pid_map_table_gen_and_apply_from_fp();


#endif /* _PID_MAP_TABLE_H_ */

