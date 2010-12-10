#include "wu/wu_byte_stream.h"

#include "front_panel_intstr.h"
#include "front_panel_define.h"


//from buf to struct
int mcu_arm_head_buf_to_t(struct fp_cmd_header * pHead, char *pbuf)
{
	memcpy(pHead, pbuf, sizeof(struct fp_cmd_header));
	pHead->len = SWAP_U16(pHead->len);
	pHead->seq = SWAP_U16(pHead->seq);
	return 0;
}

//from struct to buf
int mcu_arm_head_t_to_buf(struct fp_cmd_header * pHead, char *pbuf)
{
	struct fp_cmd_header *buf_header = (struct fp_cmd_header *)pbuf;
	*buf_header = *pHead;
	buf_header->len = SWAP_U16(buf_header->len);
	buf_header->seq = SWAP_U16(buf_header->seq);
	return 0;
}

//from buf to struct
int mcu_arm_prog_buf_to_t(PROG_INFO_T * pProg, char *pbuf)
{
	int i, j;
	memcpy(pProg, pbuf, sizeof(PROG_INFO_T));
	pProg->prognum = SWAP_U16(pProg->prognum);
	pProg->PMT_PID_IN = SWAP_U16(pProg->PMT_PID_IN);
	pProg->PMT_PID_OUT = SWAP_U16(pProg->PMT_PID_OUT);
	pProg->PCR_PID_IN = SWAP_U16(pProg->PCR_PID_IN);
	pProg->PCR_PID_OUT = SWAP_U16(pProg->PCR_PID_OUT);

	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
		pProg->pids[i].in = SWAP_U16(pProg->pids[i].in);
		pProg->pids[i].out = SWAP_U16(pProg->pids[i].out);
	}

	return 0;
}

//from struct to buf
int mcu_arm_prog_t_to_buf(PROG_INFO_T * pProg, char *pbuf)
{
	int i;
	PROG_INFO_T *buf_prog_info = (PROG_INFO_T *)pbuf;

	*buf_prog_info = *pProg;
	buf_prog_info->prognum = SWAP_U16(buf_prog_info->prognum);
	buf_prog_info->PMT_PID_IN = SWAP_U16(buf_prog_info->PMT_PID_IN);
	buf_prog_info->PMT_PID_OUT = SWAP_U16(buf_prog_info->PMT_PID_OUT);
	buf_prog_info->PCR_PID_IN = SWAP_U16(buf_prog_info->PCR_PID_IN);
	buf_prog_info->PCR_PID_OUT = SWAP_U16(buf_prog_info->PCR_PID_OUT);
	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
		buf_prog_info->pids[i].in = SWAP_U16(buf_prog_info->pids[i].in);
		buf_prog_info->pids[i].out = SWAP_U16(buf_prog_info->pids[i].out);
	}

	return 0;
}

//from buf to struct
int mcu_arm_rate_buf_to_t(OUT_RATE_T * pRate, char *pbuf)
{
	memcpy(pRate, pbuf, sizeof(OUT_RATE_T));
	pRate->rate = SWAP_U32(pRate->rate);
	return 0;
}

void conv_progpid_to_buf(uint8_t * pbuf, uint16_t u16ProgPid)
{
	if (u16ProgPid == 0)
		u16ProgPid = defPidIdler;
	WRITE_U16_BE(pbuf, u16ProgPid);
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
		// 4 is PMT_PID_IN,PMT_PID_OUT,PCR_PID_IN,PCR_PID_OUT
		// defProgPidNum*2 is PIDS
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
