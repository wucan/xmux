#include <fcntl.h>
#include <string.h>

#include "wu/message.h"

#include "xmux_config.h"
#include "hfpga.h"
#include "pid_map_table.h"
#include "pid_map_rule.h"
#include "front_panel_intstr.h"
#include "front_panel_data_churning.h"
#include "output_pid_check.h"


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
	pid_map->cha = CHANNEL_ALL_BITMAP;
}

void pid_map_table_reset()
{
	fpga_pid_map_table_clear(&tmp_pid_map);
	hfpga_write_pid_map(&tmp_pid_map);
}

int pid_map_table_apply(struct xmux_pid_map_table *pid_map_data)
{
	int size = sizeof(struct xmux_pid_map_table);

	if (size > sizeof(tmp_pid_map.pid_map)) {
		trace_warn("pid map table struct exceed! check it!");
		return -1;
	}

	tmp_pid_map.cha = CHANNEL_ALL_BITMAP;
	memcpy((unsigned char *)&tmp_pid_map.pid_map, pid_map_data, sizeof(*pid_map_data));

	hfpga_write_pid_map(&tmp_pid_map);

	return 0;
}

void pid_map_table_gen_start(struct pid_map_table_gen_context *ctx)
{
	fpga_pid_map_table_clear(&ctx->fpga_pid_map);
	memset(ctx->chan_map_pid_cnt, 0, sizeof(ctx->chan_map_pid_cnt));
}

void pid_map_table_gen_end(struct pid_map_table_gen_context *ctx, uint8_t chan_bitmap)
{
	ctx->fpga_pid_map.cha = chan_bitmap;
}

int pid_map_table_push_pid_pair(struct pid_map_table_gen_context *ctx,
		uint8_t chan_idx, uint16_t in_pid, uint16_t out_pid)
{
	if (ctx->chan_map_pid_cnt[chan_idx] >= FPGA_PID_MAP_TABLE_CHAN_PIDS)
		return -1;

	pid_map_table_set_in_pid((struct xmux_pid_map_table *)ctx->fpga_pid_map.pid_map, chan_idx,
		ctx->chan_map_pid_cnt[chan_idx], in_pid);
	pid_map_table_set_out_pid((struct xmux_pid_map_table *)ctx->fpga_pid_map.pid_map, chan_idx,
		ctx->chan_map_pid_cnt[chan_idx], out_pid);

	ctx->chan_map_pid_cnt[chan_idx]++;

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

void pid_map_table_gen_and_apply_from_fp()
{
	struct pid_map_table_gen_context gen_ctx;
	int chan_idx, prog_idx;
	int j;
	struct pid_ref_info *ref;

	/* clear mux program info */
	memset(&g_eeprom_param.mux_prog_info, 0, sizeof(struct xmux_mux_program_info));

	pid_map_table_gen_start(&gen_ctx);

	/*
	 * gather pid map from pid ref table
	 */
	build_pid_ref_table(g_prog_info_table);
	for (j = 0; j < NULL_PID; j++) {
		ref = &pid_ref_table[j];
		if (ref->type == 0)
			continue;
#if CHANNEL_MAX_NUM == 1
		/* FIXME: should we set the output pid to NULL_PID? */
		if (!(ref->type & PMT_BIT))
#else
		if (ref->type & PMT_BIT)
#endif
			continue;
		chan_idx = ref->prog_idx / PROGRAM_MAX_NUM;
		prog_idx = ref->prog_idx % PROGRAM_MAX_NUM;
		if (pid_map_table_push_pid_pair(&gen_ctx, chan_idx, ref->in_pid, j)) {
			trace_err("channel #%d pid exceed! discard start from pid %#x", j);
			break;
		}
		/* fill mux program info */
		g_eeprom_param.mux_prog_info.programs[g_eeprom_param.mux_prog_info.nprogs].chan_idx = chan_idx;
		g_eeprom_param.mux_prog_info.programs[g_eeprom_param.mux_prog_info.nprogs].prog_idx = prog_idx;
		g_eeprom_param.mux_prog_info.nprogs++;
	}

pid_map_gen_done:
	pid_map_table_gen_end(&gen_ctx, CHANNEL_ALL_BITMAP);
	xmux_config_save_pid_map_table(gen_ctx.fpga_pid_map.pid_map);
	hfpga_write_pid_map(&gen_ctx.fpga_pid_map);
}

