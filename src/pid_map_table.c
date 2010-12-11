#include <fcntl.h>

#include "wu/message.h"

#include "hfpga.h"
#include "pid_map_table.h"
#include "front_panel_intstr.h"
#include "front_panel_data_churning.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "pid_map_table"};


void pid_map_table_clear(ACCESS_HFPGA_PID_MAP *pid_map)
{
	int chan_idx, pid_idx;

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		for (pid_idx = 0; pid_idx < FPGA_PID_MAP_TABLE_CHAN_PIDS; pid_idx++) {
			pid_map_table_set_in_pid(pid_map, chan_idx, pid_idx, 0x000F);
			pid_map_table_set_out_pid(pid_map, chan_idx, pid_idx, 0x000F);
		}
	}
	pid_map->cha = 0xFF;
}

void pid_map_table_reset()
{
	ACCESS_HFPGA_PID_MAP pid_map;
	pid_map_table_clear(&pid_map);
	hfpga_write_pid_map(&pid_map);
}

int pid_map_table_apply(struct xmux_pid_map_table *pid_map_data)
{
	ACCESS_HFPGA_PID_MAP pid_map;
	int size = sizeof(struct xmux_pid_map_table);

	if (size != sizeof(pid_map.pid_map))
		return -1;

	pid_map.cha = 0xFF;
	memcpy(pid_map.pid_map, pid_map_data, sizeof(pid_map.pid_map));

	hfpga_write_pid_map(&pid_map);

	return 0;
}

void pid_map_table_gen_start(struct pid_map_table_gen_context *ctx)
{
	pid_map_table_clear(&ctx->pid_map);
	ctx->cur_chan_idx = 0;
	ctx->cur_chan_map_pid_cnt = 0;
}

void pid_map_table_gen_end(struct pid_map_table_gen_context *ctx, uint8_t chan_bitmap)
{
	ctx->pid_map.cha = chan_bitmap;
}

int pid_map_table_push_pid_pair(struct pid_map_table_gen_context *ctx,
		uint8_t chan_idx, uint16_t in_pid, uint16_t out_pid)
{
	if (ctx->cur_chan_map_pid_cnt >= FPGA_PID_MAP_TABLE_CHAN_PIDS)
		return -1;

	if (chan_idx != ctx->cur_chan_idx) {
		ctx->cur_chan_idx = chan_idx;
		ctx->cur_chan_map_pid_cnt = 0;
	}

	pid_map_table_set_in_pid(&ctx->pid_map, chan_idx,
		ctx->cur_chan_map_pid_cnt, in_pid);
	pid_map_table_set_out_pid(&ctx->pid_map, chan_idx,
		ctx->cur_chan_map_pid_cnt, out_pid);

	ctx->cur_chan_map_pid_cnt++;

	return 0;
}

