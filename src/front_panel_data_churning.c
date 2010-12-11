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

