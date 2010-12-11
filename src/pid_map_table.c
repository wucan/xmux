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

int pid_map_table_apply(void *pid_map_data, int size)
{
	ACCESS_HFPGA_PID_MAP pid_map;

	if (size != sizeof(pid_map.pid_map))
		return;

	pid_map.cha = 0xFF;
	memcpy(pid_map.pid_map, pid_map_data, sizeof(pid_map.pid_map));

	hfpga_write_pid_map(&pid_map);
}

/*
 * generate pid_map_table from front panel data
 */
void pid_map_table_generate_from_fp(uint8_t * buf)
{
	int i, j, n;
	int nProgSel = 0;
	PROG_INFO_T *prog_info;

	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		int noff = nProgSel * (PROGRAM_PID_MAX_NUM * 2) * sizeof(uint16_t);
		prog_info = &g_prog_info_table[i];
		if (prog_info->status == 1) {
			pid_2_buf(&buf[noff + 0], prog_info->PMT_PID_IN);
			pid_2_buf(&buf[noff + 2], prog_info->PMT_PID_OUT);
			pid_2_buf(&buf[noff + 4], prog_info->PCR_PID_IN);
			pid_2_buf(&buf[noff + 6], prog_info->PCR_PID_OUT);

			for (j = 0; j < PROGRAM_DATA_PID_MAX_NUM; j++) {
				pid_2_buf(&buf[noff + 8 + j * 4],
									prog_info->pids[j].in);
				pid_2_buf(&buf[noff + 10 + j * 4],
									prog_info->pids[j].out);
			}
			nProgSel++;
			if (nProgSel >= PROGRAM_MAX_NUM)
				break;
		}
	}
}

