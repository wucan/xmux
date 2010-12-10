#include <fcntl.h>

#include "wu/message.h"

#include "hfpga.h"
#include "pid_map_table.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "pid_map_table"};


void pid_map_table_reset()
{
	ACCESS_HFPGA_PID_MAP pid_map;
	int chan_idx, pid_idx;

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		for (pid_idx = 0; pid_idx < FPGA_PID_MAP_TABLE_CHAN_PIDS; pid_idx++) {
			pid_map_table_set_in_pid(&pid_map, chan_idx, pid_idx, 0x000F);
			pid_map_table_set_out_pid(&pid_map, chan_idx, pid_idx, 0x000F);
		}
	}
	pid_map.cha = 0xFF;

	hfpga_write_pid_map(pid_map);
}

