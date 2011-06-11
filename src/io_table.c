#include <string.h>

#include "wu/wu_base_type.h"
#include "wu/message.h"

#include "front_panel_intstr.h"
#include "front_panel_define.h"
#include "io_table.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "io_table"};


static uint16_t pick_free_pid();
static bool is_out_pid_usable(uint8_t sel_chan_idx,
		uint16_t in_pid, uint16_t out_pid);
static void update_channel_other_program_out_pid(int sel_prog_idx,
		uint16_t in_pid, uint16_t out_pid);

struct io_pid_entry io_table[CHANNEL_MAX_NUM][NULL_PID + 1];

void build_io_table()
{
	uint8_t chan_idx, prog_idx, i;
	PROG_INFO_T *prog;
	struct io_pid_entry *chan_io_table;
	struct pid_trans_entry *e;

	memset(io_table, 0, sizeof(io_table));
	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) { 
		chan_io_table = io_table[chan_idx];
		for (prog_idx = 0; prog_idx < g_chan_num.num[chan_idx]; prog_idx++) { 
			prog = &g_prog_info_table[chan_idx * PROGRAM_MAX_NUM + prog_idx];
			e = &prog->info.pmt;
			for (i = 0; i < (2 + PROGRAM_DATA_PID_MAX_NUM); i++) {
				if (!prog_pid_val_isvalid(e[i].in))
					break;
				if (i == 0)
					chan_io_table[e[i].in].pid_type = IO_PID_TYPE_PMT;
				chan_io_table[e[i].in].flags |= IO_PID_FLAG_IS_INPUT_PID;
				chan_io_table[e[i].in].out_pid = e[i].out;
				if (FP_PROG_SELECTED(prog))
					chan_io_table[e[i].in].flags |= IO_PID_FLAG_SELECTED;
			}
		}
	}
}

void dump_io_table(const char *ctx)
{
	uint8_t chan_idx;
	uint16_t pid;

	trace_info("===== io table - %s =====", ctx);
	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) { 
		trace_info("  channel #%d:", chan_idx);
		for (pid = 0x20; pid < NULL_PID; pid++) {
			if (io_table[chan_idx][pid].flags & IO_PID_FLAG_SELECTED) {
				if (io_table[chan_idx][pid].flags & IO_PID_FLAG_JUST_ADDED)
					trace_info("    pid %#x => %#x [+]", pid, io_table[chan_idx][pid].out_pid);
				else
					trace_info("    pid %#x => %#x", pid, io_table[chan_idx][pid].out_pid);
			}
		}
	}
}

static void io_table_set_pid(int g_prog_idx, uint16_t in_pid, uint16_t out_pid)
{
	uint8_t chan_idx = g_prog_idx / PROGRAM_MAX_NUM;

	if (!(io_table[chan_idx][in_pid].flags & IO_PID_FLAG_SELECTED) || io_table[chan_idx][in_pid].out_pid != out_pid)
		io_table[chan_idx][in_pid].flags |= IO_PID_FLAG_JUST_ADDED;
	io_table[chan_idx][in_pid].out_pid = out_pid;
	io_table[chan_idx][in_pid].flags |= IO_PID_FLAG_SELECTED;
}

static uint16_t io_table_get_out_pid(int g_prog_idx, uint16_t in_pid)
{
	uint8_t chan_idx = g_prog_idx / PROGRAM_MAX_NUM;

	if (io_table[chan_idx][in_pid].flags & IO_PID_FLAG_SELECTED)
		return (io_table[chan_idx][in_pid].out_pid);

	if (is_out_pid_usable(chan_idx, in_pid, in_pid))
		return in_pid;

	return pick_free_pid();
}

static uint16_t io_table_get_pmt_out_pid(int g_prog_idx, uint16_t in_pid)
{
	uint8_t chan_idx = g_prog_idx / PROGRAM_MAX_NUM;

	trace_info("program #%d get pmt pid (in=%#x)", g_prog_idx, in_pid);
	if (io_table[chan_idx][in_pid].flags & IO_PID_FLAG_SELECTED)
		return pick_free_pid();

	if (is_out_pid_usable(chan_idx, in_pid, in_pid))
		return in_pid;

	return pick_free_pid();
}

static bool is_out_pid_usable(uint8_t sel_chan_idx,
		uint16_t in_pid, uint16_t out_pid)
{
	uint8_t chan_idx;
	uint16_t tmp_in_pid;

	trace_info("test out pid %#x of channel #%d ...", out_pid, sel_chan_idx);

	if (io_table[sel_chan_idx][in_pid].flags & IO_PID_FLAG_SELECTED &&
		io_table[sel_chan_idx][in_pid].out_pid == out_pid) {
		trace_info("%#x => %#x had been mapped in this channel",
			in_pid, out_pid);
		return true;
	}

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		for (tmp_in_pid = 0x20; tmp_in_pid < NULL_PID; tmp_in_pid++) {
			if ((io_table[chan_idx][tmp_in_pid].flags & IO_PID_FLAG_SELECTED) &&
				(io_table[chan_idx][tmp_in_pid].out_pid == out_pid)) {
				if (chan_idx == sel_chan_idx && (tmp_in_pid == in_pid))
					continue;
				trace_warn("out pid %#x had been used as output pid for input pid %#x in channel %#x",
					out_pid, tmp_in_pid, chan_idx);
				return false;
			}
		}
	}

	trace_info("out pid %#x is free and available for using", out_pid);

	return true;
}

static uint16_t pick_free_pid()
{
	uint8_t chan_idx;
	struct io_pid_entry *chan_io_table;
	uint16_t out_pid, pid;

	for (out_pid = 0x20; out_pid < NULL_PID; out_pid++) {
		/* don't use input pid */
		for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) { 
		 	if (io_table[chan_idx][out_pid].flags)
				goto try_next_free_pid;
		}

		/* don't use output pid */
		for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
			for (pid = 0x20; pid < NULL_PID; pid++) {
		 		if ((io_table[chan_idx][pid].flags & IO_PID_FLAG_SELECTED) &&
					(io_table[chan_idx][pid].out_pid == out_pid))
						goto try_next_free_pid;
			}
		}
		return out_pid;

try_next_free_pid:
		;;;
	}

	// unreachable!

	return NULL_PID;
}

/*
 * check user changed output pid validator
 */
bool check_user_output_pid(PROG_INFO_T *old_prog,
		PROG_INFO_T *sel_prog, int sel_prog_idx, PROG_INFO_T *prog_table)
{
	int i;
	struct pid_trans_entry *e, *old_e;
	uint8_t chan_idx = sel_prog_idx / PROGRAM_MAX_NUM;

	trace_info("check user changed program #%d output pid ...", sel_prog_idx);

	build_io_table();
	dump_io_table("start");

	// pmt
	if (sel_prog->info.pmt.out != old_prog->info.pmt.out) {
		if (!is_out_pid_usable(chan_idx, sel_prog->info.pmt.in, sel_prog->info.pmt.out)) {
			trace_err("user set pmt %#x are used!", sel_prog->info.pmt.out);
			return false;
		}
		trace_info("user set pmt %#x ok", sel_prog->info.pmt.out);
	}
	io_table_set_pid(sel_prog_idx, sel_prog->info.pmt.in, sel_prog->info.pmt.out);

	// pcr + data
	e = &sel_prog->info.pcr;
	old_e = &old_prog->info.pcr;
	for (i = 0; i <= PROGRAM_DATA_PID_MAX_NUM; i++) {
		if (!prog_pid_val_isvalid(e[i].in)) {
			continue;
		}
		if (e[i].out != old_e[i].out) {
			if (!is_out_pid_usable(chan_idx, e[i].in, e[i].out)) {
				trace_err("user set pid %#x are used!", e[i].out);
				return false;
			}
			trace_info("user set data %#x ok", e[i].out);
		}
		io_table_set_pid(sel_prog_idx, e[i].in, e[i].out);

		update_channel_other_program_out_pid(sel_prog_idx, e[i].in, e[i].out);
	}

	dump_io_table("end");

	return true;
}

/*
 * check selectecd program output pid is validate
 */
void fix_selected_program_output_pid(PROG_INFO_T *sel_prog, int sel_prog_idx,
		PROG_INFO_T *prog_table, bool do_fix_selected_programs)
{
	int i;
	struct pid_trans_entry *e;

	trace_info("fix and select program #%d output pid ...", sel_prog_idx);

	build_io_table();
	dump_io_table("start");

	sel_prog->info.pmt.out = io_table_get_pmt_out_pid(sel_prog_idx,
								sel_prog->info.pmt.in);
	io_table_set_pid(sel_prog_idx, sel_prog->info.pmt.in,
				sel_prog->info.pmt.out);
	trace_info("fix pmt pid %#x => %#x",
		sel_prog->info.pmt.in, sel_prog->info.pmt.out);

	// check pcr and data pid
	e = &sel_prog->info.pcr;
	for (i = 0; i <= PROGRAM_DATA_PID_MAX_NUM; i++) {
		if (!prog_pid_val_isvalid(e[i].in)) {
			continue;
		}
		e[i].out = io_table_get_out_pid(sel_prog_idx, e[i].in);
		io_table_set_pid(sel_prog_idx, e[i].in, e[i].out);
		trace_info("fix pid %#x => %#x", e[i].in, e[i].out);

		if (do_fix_selected_programs)
			update_channel_other_program_out_pid(sel_prog_idx, e[i].in, e[i].out);
	}

	dump_io_table("end");
}

static void update_channel_other_program_out_pid(int sel_prog_idx,
		uint16_t in_pid, uint16_t out_pid)
{
	uint8_t chan_idx = sel_prog_idx / PROGRAM_MAX_NUM, prog_idx, i;
	PROG_INFO_T *prog;
	struct io_pid_entry *chan_io_table;
	struct pid_trans_entry *e;

	chan_io_table = io_table[chan_idx];
	for (prog_idx = 0; prog_idx < g_chan_num.num[chan_idx]; prog_idx++) { 
		if (sel_prog_idx == (PROGRAM_MAX_NUM * chan_idx + prog_idx))
			continue;
		prog = &g_prog_info_table[chan_idx * PROGRAM_MAX_NUM + prog_idx];
		e = &prog->info.pcr;
		for (i = 0; i < (1 + PROGRAM_DATA_PID_MAX_NUM); i++) {
			if (!prog_pid_val_isvalid(e[i].in))
				break;
			if (FP_PROG_SELECTED(prog) && e[i].in == in_pid)
				e[i].out = out_pid;
		}
	}
}


