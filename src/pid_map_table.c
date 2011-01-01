#include <fcntl.h>
#include <string.h>

#include "wu/message.h"

#include "hfpga.h"
#include "pid_map_table.h"
#include "pid_map_rule.h"
#include "front_panel_intstr.h"
#include "front_panel_data_churning.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "pid_map_table"};

static ACCESS_HFPGA_PID_MAP tmp_pid_map;

void pid_map_table_clear(struct xmux_pid_map_table *pid_map)
{
	int chan_idx, pid_idx;

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		for (pid_idx = 0; pid_idx < FPGA_PID_MAP_TABLE_CHAN_PIDS; pid_idx++) {
			pid_map_table_set_in_pid(pid_map, chan_idx, pid_idx, PID_MAP_TABLE_PAD_PID);
			pid_map_table_set_out_pid(pid_map, chan_idx, pid_idx, PID_MAP_TABLE_PAD_PID);
		}
	}
}

void fpga_pid_map_table_clear(ACCESS_HFPGA_PID_MAP *pid_map)
{
	pid_map_table_clear((struct xmux_pid_map_table *)&pid_map->pid_map);
	pid_map->cha = 0xFF;
}

void pid_map_table_reset()
{
	fpga_pid_map_table_clear(&tmp_pid_map);
	hfpga_write_pid_map(&tmp_pid_map);
}

int pid_map_table_apply(struct xmux_pid_map_table *pid_map_data)
{
	int size = sizeof(struct xmux_pid_map_table);

	if (size != sizeof(tmp_pid_map.pid_map))
		return -1;

	tmp_pid_map.cha = 0xFF;
	memcpy((unsigned char *)&tmp_pid_map + 4, pid_map_data, sizeof(tmp_pid_map.pid_map));

	hfpga_write_pid_map(&tmp_pid_map);

	return 0;
}

void pid_map_table_gen_start(struct pid_map_table_gen_context *ctx)
{
	fpga_pid_map_table_clear(&ctx->fpga_pid_map);
	ctx->cur_chan_idx = 0;
	ctx->cur_chan_map_pid_cnt = 0;
}

void pid_map_table_gen_end(struct pid_map_table_gen_context *ctx, uint8_t chan_bitmap)
{
	ctx->fpga_pid_map.cha = chan_bitmap;
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

	pid_map_table_set_in_pid((struct xmux_pid_map_table *)ctx->fpga_pid_map.pid_map, chan_idx,
		ctx->cur_chan_map_pid_cnt, in_pid);
	pid_map_table_set_out_pid((struct xmux_pid_map_table *)ctx->fpga_pid_map.pid_map, chan_idx,
		ctx->cur_chan_map_pid_cnt, out_pid);

	ctx->cur_chan_map_pid_cnt++;

	return 0;
}

bool pid_map_table_validate(struct xmux_pid_map_table *pid_map)
{
	uint8_t chan_idx, pid_idx;
	struct pid_map_entry *ent;
	uint16_t in, out;

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		for (pid_idx = 0; pid_idx < FPGA_PID_MAP_TABLE_CHAN_PIDS; pid_idx++) {
			/*
			 * ignore pad pid
			 */
			ent= &pid_map->chans[chan_idx].ents[pid_idx];
			in = ent->input_pid;
			out = ent->output_pid;
			if ((in == 15) && (out == 15)) {
				continue;
			}
			/*
			 * check output_pid which got from pid_map_rule on input_pid
			 */
			if (!pid_map_rule_channel_output_pid_validate(chan_idx, out)) {
				trace_err("chan #%d pid(%d(%#x) => %d(%#x)), output pid invalidate!",
					chan_idx, in, in, out, out);
				trace_err("discard all channel's pid map table!");
				return false;
			}
		}
	}

	return true;
}

void pid_map_table_dump(struct xmux_pid_map_table *pid_map)
{
	uint8_t chan_idx, pid_idx;
	struct pid_map_entry *ent;
	uint16_t in, out;

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		trace_info("chan #%d pid map:", chan_idx);
		for (pid_idx = 0; pid_idx < FPGA_PID_MAP_TABLE_CHAN_PIDS; pid_idx++) {
			ent= &pid_map->chans[chan_idx].ents[pid_idx];
			in = ent->input_pid;
			out = ent->output_pid;
			if (in == PID_MAP_TABLE_PAD_PID &&
				out == PID_MAP_TABLE_PAD_PID) {
				continue;
			}
			trace_info("  #%d, pid_idx #%d pid(%d(%#x) => %d(%#x))",
				chan_idx, pid_idx, in, in, out, out);
		}
	}
}

