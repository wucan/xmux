#include "wu/wu_byte_stream.h"

#include "front_panel_intstr.h"
#include "front_panel_define.h"
#include "front_panel_data_churning.h"


void buf_2_fp_cmd_header(struct fp_cmd_header * hdr, char *buf)
{
	memcpy(hdr, buf, sizeof(struct fp_cmd_header));
	hdr->len = SWAP_U16(hdr->len);
	hdr->seq = SWAP_U16(hdr->seq);
}

void fp_cmd_header_2_buf(struct fp_cmd_header * hdr, char *buf)
{
	struct fp_cmd_header *buf_header = (struct fp_cmd_header *)buf;
	*buf_header = *hdr;
	buf_header->len = SWAP_U16(buf_header->len);
	buf_header->seq = SWAP_U16(buf_header->seq);
}

void buf_2_prog_info(PROG_INFO_T *prog_info, char *buf)
{
	int i, j;

	memcpy(prog_info, buf, sizeof(PROG_INFO_T));
	prog_info->prognum = SWAP_U16(prog_info->prognum);
	prog_info->PMT_PID_IN = SWAP_U16(prog_info->PMT_PID_IN);
	prog_info->PMT_PID_OUT = SWAP_U16(prog_info->PMT_PID_OUT);
	prog_info->PCR_PID_IN = SWAP_U16(prog_info->PCR_PID_IN);
	prog_info->PCR_PID_OUT = SWAP_U16(prog_info->PCR_PID_OUT);

	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
		prog_info->pids[i].in = SWAP_U16(prog_info->pids[i].in);
		prog_info->pids[i].out = SWAP_U16(prog_info->pids[i].out);
	}
}

void prog_info_2_buf(PROG_INFO_T *prog_info, char *buf)
{
	int i;
	PROG_INFO_T *buf_prog_info = (PROG_INFO_T *)buf;

	*buf_prog_info = *prog_info;
	buf_prog_info->prognum = SWAP_U16(buf_prog_info->prognum);
	buf_prog_info->PMT_PID_IN = SWAP_U16(buf_prog_info->PMT_PID_IN);
	buf_prog_info->PMT_PID_OUT = SWAP_U16(buf_prog_info->PMT_PID_OUT);
	buf_prog_info->PCR_PID_IN = SWAP_U16(buf_prog_info->PCR_PID_IN);
	buf_prog_info->PCR_PID_OUT = SWAP_U16(buf_prog_info->PCR_PID_OUT);
	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
		buf_prog_info->pids[i].in = SWAP_U16(buf_prog_info->pids[i].in);
		buf_prog_info->pids[i].out = SWAP_U16(buf_prog_info->pids[i].out);
	}
}

void buf_2_out_rate(OUT_RATE_T *out_rate, char *buf)
{
	memcpy(out_rate, buf, sizeof(OUT_RATE_T));
	out_rate->rate = SWAP_U32(out_rate->rate);
}

void pid_2_buf(uint8_t *buf, uint16_t pid)
{
	if (pid == 0)
		pid = defPidIdler;
	WRITE_U16_BE(buf, pid);
}

