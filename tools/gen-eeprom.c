#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#include "wu/wu_csc.h"

#include "xmux.h"
#include "pid_map_rule.h"


static struct xmux_eeprom_param  eeprom;

static void build_pid_trans_info()
{
	struct pid_trans_info_snmp_data *d;
	struct xmux_program_info *prog;
	struct xmux_program_info_with_csc *prog_csc;
	uint8_t chan_idx, prog_idx, pid_idx;

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		d = &eeprom.pid_trans_info_area.table[chan_idx].data;

		d->data_len = sizeof(*d) - 2;
		d->update_flag_and_chan_num = chan_idx;
		d->nprogs = 8;
		SELECT_PROGRAM(d, 1);
		SELECT_PROGRAM(d, 3);
		SELECT_PROGRAM(d, 4);
		SELECT_PROGRAM(d, 6);
		for (prog_idx = 0; prog_idx < d->nprogs; prog_idx++) {
			uint16_t pids[3];
			prog = &d->programs[prog_idx];
			prog_csc = &d->programs[prog_idx];
			prog->prog_num = 100 + prog_idx;
			prog->pmt.in = 64 + 10 * prog_idx + 0;
			prog->pmt.out = pid_map_rule_map_psi_pid(chan_idx,
				0, DSW_PID_PMT, prog->pmt.in, NULL, 0);
			prog->pcr.in = 64 + 10 * prog_idx + 1;
			prog->data[0].in = (64 + 10 * prog_idx + 2);
			prog->data[1].in = (64 + 10 * prog_idx + 3);

			pids[0] = prog->pcr.in;
			pids[1] = (prog->data[0].in);
			pids[2] = (prog->data[1].in);

			prog->pcr.out = pid_map_rule_map_psi_pid(chan_idx,
				prog_idx, DSW_PID_PCR, prog->pcr.in, pids, 3);
			prog->data[0].out = pid_map_rule_map_psi_pid(chan_idx,
				prog_idx, DSW_PID_VIDEO, (prog->data[0].in), pids, 3);
			prog->data[1].out = pid_map_rule_map_psi_pid(chan_idx,
				prog_idx, DSW_PID_VIDEO, (prog->data[1].in), pids, 3);

			for (pid_idx = 2; pid_idx < PROGRAM_DATA_PID_MAX_NUM; pid_idx++) {
				prog->data[pid_idx].in = DATA_PID_PAD_VALUE;
				prog->data[pid_idx].out = DATA_PID_PAD_VALUE;
			}
			prog->prog_name[0][0] = sprintf(&prog->prog_name[0][1],
				"CCTV%d", prog_idx);
			prog_csc->csc = wu_csc((uint8_t *)prog_csc, sizeof(*prog_csc) - 1);
		}
	}
}
static void build_pid_map_table()
{
	struct xmux_pid_map_table *pid_map = &eeprom.pid_map_table_area.pid_map_table;
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
	eeprom.sys.version = 0x00000001;

	/* net */

	/* user */

	eeprom.mng_mode = MANAGEMENT_MODE_SNMP;
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
	write(fd, &eeprom, sizeof(eeprom));

	close(fd);
	exit(EXIT_SUCCESS);
}

