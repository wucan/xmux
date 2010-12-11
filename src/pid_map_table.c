#include <fcntl.h>

#include "wu/message.h"

#include "hfpga.h"
#include "pid_map_table.h"
#include "front_panel_intstr.h"


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

int pid_map_table_apply(void *pid_map_data, int size)
{
	ACCESS_HFPGA_PID_MAP pid_map;

	if (size != sizeof(pid_map.pid_map))
		return;

	pid_map.cha = 0xFF;
	memcpy(pid_map.pid_map, pid_map_data, sizeof(pid_map.pid_map));

	hfpga_write_pid_map(pid_map);
}

int gen_fpga_pid_map_fr_mcu(uint8_t * pbuf, PROG_INFO_T * pProgpara)
{
	uint8_t buf[PROGRAM_MAX_NUM * (4 + PROGRAM_DATA_PID_MAX_NUM * 2) * sizeof(uint16_t)];
	int i;
	int nProgSel = 0;
	PROG_INFO_T *pProg = pProgpara;
	memset(buf, 0x0F, sizeof(buf));

	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		int j;
		int noff = nProgSel * (4 + PROGRAM_DATA_PID_MAX_NUM * 2) * sizeof(uint16_t);
		pProg = pProgpara + i;
		if (pProg->status == 1) {
			conv_progpid_to_buf(&buf[noff + 0], pProg->PMT_PID_IN);
			conv_progpid_to_buf(&buf[noff + 2], pProg->PMT_PID_OUT);
			conv_progpid_to_buf(&buf[noff + 4], pProg->PCR_PID_IN);
			conv_progpid_to_buf(&buf[noff + 6], pProg->PCR_PID_OUT);

			for (j = 0; j < PROGRAM_DATA_PID_MAX_NUM; j++) {
				conv_progpid_to_buf(&buf[noff + 8 + j * 4],
									pProg->pids[j].in);
				conv_progpid_to_buf(&buf[noff + 10 + j * 4],
									pProg->pids[j].out);
			}
			nProgSel++;
			if (nProgSel >= PROGRAM_MAX_NUM)
				break;
		}
	}

	for (i = 0;
		 i < PROGRAM_MAX_NUM * (4 + PROGRAM_DATA_PID_MAX_NUM * 2) * sizeof(uint16_t);
		 i++) {
		if (i % 0x10 == 0)
			printf("\n");
		printf("%02X ", buf[i]);
	}
	printf("\ngen_fpga_pid_map_fr_mcu_buf_len=%d\n", sizeof(buf));

	memcpy(pbuf, buf, sizeof(buf));
	printf("gen_fpga_pid_map_fr_mcu_buf_len=%d\n", sizeof(buf));

	return 0;
}

