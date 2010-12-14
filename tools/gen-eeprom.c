#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#include "wu/wu_csc.h"

#include "xmux.h"
#include "pid_map_rule.h"


static struct xmux_root_param  root;

static void build_pid_trans_info()
{
	struct pid_trans_info_snmp_data *d;
	struct xmux_program_info *prog;
	uint8_t chan_idx, prog_idx, pid_idx;

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		d = &root.pid_trans_info_area.pid_trans_info[chan_idx];

		d->data_len = sizeof(*d) - 2;
		d->update_flag_and_chan_num = chan_idx;
		d->nprogs = 8;
		d->status = 0x5A;
		for (prog_idx = 0; prog_idx < d->nprogs; prog_idx++) {
			prog = &d->programs[prog_idx];
			prog->prog_num = 100 + prog_idx;
			prog->pmt.in = 64 + 10 * prog_idx + 0;
			prog->pcr.in = 64 + 10 * prog_idx + 1;
			prog->data[0].in = PACK_VIDEO_DATA_PID(64 + 10 * prog_idx + 2);
			prog->data[1].in = PACK_AUDIO_DATA_PID(64 + 10 * prog_idx + 3);
			for (pid_idx = 2; pid_idx < PROGRAM_DATA_PID_MAX_NUM; pid_idx++) {
				prog->data[pid_idx].in = DATA_PID_PAD_VALUE;
			}
			prog->prog_name[0][0] = sprintf(&prog->prog_name[0][1],
				"CCTV%d", prog_idx);
		}
		d->csc = wu_csc((uint8_t *)d, sizeof(*d) - 1);
	}
}
static void build_pid_map_table()
{
	struct xmux_pid_map_table *pid_map = &root.pid_map_table_area.pid_map_table;
	uint8_t chan_idx, pid_idx;
	struct pid_map_entry *ent;
	uint16_t input_pids[2];

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		for (pid_idx = 0; pid_idx < 2; pid_idx++) {
			ent= &pid_map->chans[chan_idx].ents[pid_idx];
			ent->input_pid = 64 + 10 * chan_idx + pid_idx;
			/* 2 input pid, used for construct output pid */
			input_pids[0] = ent->input_pid;
			input_pids[1] = ent->input_pid + 1;
			ent->output_pid = pid_map_rule_map_psi_pid(chan_idx,
				0, DSW_PID_VIDEO, ent->input_pid, input_pids, 2);
		}
		for (pid_idx = 2; pid_idx < FPGA_PID_MAP_TABLE_CHAN_PIDS; pid_idx++) {
			ent= &pid_map->chans[chan_idx].ents[pid_idx];
			ent->input_pid = PID_MAP_TABLE_PAD_PID;
			ent->output_pid = PID_MAP_TABLE_PAD_PID;
		}
	}

}
static void build_test_param()
{
	/* pid_trans_info */
	build_pid_trans_info();

	/* pid_map */
	build_pid_map_table();

	/* sys */
	root.sys.version = 0x00000001;

	/* net */

	/* user */
}

int main(int argc, char **argv)
{
	int fd;

	fd = open("eeprom.data", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		exit(EXIT_FAILURE);
	}

	posix_fallocate(fd, 0, EEPROM_SIZE);

	build_test_param();
	write(fd, &root, sizeof(root));

	close(fd);
	exit(EXIT_SUCCESS);
}

