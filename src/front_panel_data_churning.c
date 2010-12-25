#include "wu/wu_byte_stream.h"
#include "wu/wu_csc.h"
#include "wu/message.h"

#include "xmux_config.h"
#include "front_panel_intstr.h"
#include "front_panel_define.h"
#include "front_panel_data_churning.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "fp-data-churning"};

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

static inline void churning_fp_program_info(struct fp_program_info *info)
{
	int i;

	info->prog_num = SWAP_U16(info->prog_num);
	info->pmt.in = SWAP_U16(info->pmt.in);
	info->pmt.out = SWAP_U16(info->pmt.out);
	info->pcr.in = SWAP_U16(info->pcr.in);
	info->pcr.out = SWAP_U16(info->pcr.out);

	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
		info->data[i].in = SWAP_U16(info->data[i].in);
		info->data[i].out = SWAP_U16(info->data[i].out);
	}
}

void buf_2_prog_info(PROG_INFO_T *prog_info, char *buf)
{
	memcpy(prog_info, buf, sizeof(PROG_INFO_T));
	churning_fp_program_info(&prog_info->info);
}

void prog_info_2_buf(PROG_INFO_T *prog_info, char *buf)
{
	PROG_INFO_T *buf_prog_info = (PROG_INFO_T *)buf;

	*buf_prog_info = *prog_info;
	churning_fp_program_info(&buf_prog_info->info);
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

void fp_net_2_xmux_net(NET_ETH0_T *fp_net, struct xmux_net_param *xmux_net)
{
	memcpy(&xmux_net->server_ip, fp_net->nSvrIp, 4);
	memcpy(&xmux_net->ip, fp_net->LocIp, 4);
	memcpy(&xmux_net->netmask, fp_net->Mask, 4);
	memcpy(&xmux_net->gateway, fp_net->gw, 4);
	memcpy(&xmux_net->mac, fp_net->strMac, 6);
}

void xmux_net_2_fp_net(struct xmux_net_param *xmux_net, NET_ETH0_T *fp_net)
{
	memcpy(fp_net->nSvrIp, &xmux_net->server_ip, 4);
	memcpy(fp_net->LocIp, &xmux_net->ip, 4);
	memcpy(fp_net->Mask, &xmux_net->netmask, 4);
	memcpy(fp_net->gw, &xmux_net->gateway, 4);
	memcpy(fp_net->strMac, &xmux_net->mac, 6);
}

void pid_trans_info_2_prog_info_of_channel(uint8_t chan_idx)
{
	PROG_INFO_T *prog;
	struct fp_program_info *fp_prog;
	struct pid_trans_info_snmp_data *pid_trans_info;
	struct xmux_program_info *xmux_prog;
	uint8_t prog_idx, pid_idx;
	uint16_t in_pid, out_pid;

	pid_trans_info = &g_eeprom_param.pid_trans_info_area.pid_trans_info[chan_idx];

	g_chan_num.num[chan_idx] = pid_trans_info->nprogs;
	if (pid_trans_info->nprogs == 0)
		return;

	for (prog_idx = 0; prog_idx < pid_trans_info->nprogs; prog_idx++) {
		xmux_prog = &pid_trans_info->programs[prog_idx];
		prog = &g_prog_info_table[chan_idx * PROGRAM_MAX_NUM + prog_idx];
		fp_prog = &prog->info;

		prog->status = PROGRAM_SELECTED(pid_trans_info->status, prog_idx) ? 1: 0;

		fp_prog->prog_num = xmux_prog->prog_num;
		fp_prog->pmt = xmux_prog->pmt;
		fp_prog->pcr = xmux_prog->pcr;
		for (pid_idx = 0; pid_idx < PROGRAM_DATA_PID_MAX_NUM; pid_idx++) {
			in_pid = xmux_prog->data[pid_idx].in;
			out_pid = xmux_prog->data[pid_idx].out;
			if (!data_pid_validate(in_pid)) {
				fp_prog->data[pid_idx].in = DATA_PID_PAD_VALUE;
				fp_prog->data[pid_idx].out = DATA_PID_PAD_VALUE;
				fp_prog->data[pid_idx].type = 0;
				continue;
			}
			fp_prog->data[pid_idx].in = DATA_PID_VALUE(in_pid);
			fp_prog->data[pid_idx].out = DATA_PID_VALUE(out_pid);
			fp_prog->data[pid_idx].type = pid_type_xmux_2_es(DATA_PID_TYPE(in_pid));
		}
		memcpy(fp_prog->prog_name, xmux_prog->prog_name,
			sizeof(fp_prog->prog_name));
	}

	trace_info("channel #%d original pid_trans_info csc = %#x",
		chan_idx, pid_trans_info->csc);
}

void pid_trans_info_2_prog_info()
{
	uint8_t chan_idx;

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		pid_trans_info_2_prog_info_of_channel(chan_idx);
	}
}

void prog_info_2_pid_trans_info_of_channel(uint8_t chan_idx)
{
	PROG_INFO_T *prog;
	struct fp_program_info *fp_prog;
	struct pid_trans_info_snmp_data *pid_trans_info;
	struct xmux_program_info *xmux_prog;
	uint8_t prog_idx, pid_idx;
	uint16_t in_pid, out_pid;
	uint8_t pid_type;

	pid_trans_info = &g_eeprom_param.pid_trans_info_area.pid_trans_info[chan_idx];

	pid_trans_info->data_len = sizeof(*pid_trans_info) - 2;
	pid_trans_info->update_flag_and_chan_num = chan_idx;
	pid_trans_info->nprogs = g_chan_num.num[chan_idx];
	if (pid_trans_info->nprogs == 0)
		return;

	for (prog_idx = 0; prog_idx < pid_trans_info->nprogs; prog_idx++) {
		xmux_prog = &pid_trans_info->programs[prog_idx];
		prog = &g_prog_info_table[chan_idx * PROGRAM_MAX_NUM + prog_idx];
		fp_prog = &prog->info;

		prog->status = PROGRAM_SELECTED(pid_trans_info->status, prog_idx) ? 1: 0;
		if (prog->status) {
			SELECT_PROGRAM(pid_trans_info, prog_idx);
		} else {
			DESELECT_PROGRAM(pid_trans_info, prog_idx);
		}

		xmux_prog->prog_num = fp_prog->prog_num;
		xmux_prog->pmt = fp_prog->pmt;
		xmux_prog->pcr = fp_prog->pcr;
		for (pid_idx = 0; pid_idx < PROGRAM_DATA_PID_MAX_NUM; pid_idx++) {
			in_pid = fp_prog->data[pid_idx].in;
			out_pid = fp_prog->data[pid_idx].out;
			if (in_pid == DATA_PID_PAD_VALUE && out_pid == DATA_PID_PAD_VALUE) {
				xmux_prog->data[pid_idx].in = DATA_PID_PAD_VALUE;
				xmux_prog->data[pid_idx].out = DATA_PID_PAD_VALUE;
				continue;
			}
			pid_type = pid_type_es_2_xmux(fp_prog->data[pid_idx].type);
			xmux_prog->data[pid_idx].in = PACK_DATA_PID(pid_type, in_pid);
			xmux_prog->data[pid_idx].out = PACK_DATA_PID(pid_type, out_pid);
		}
		memcpy(xmux_prog->prog_name, fp_prog->prog_name,
			sizeof(fp_prog->prog_name));
	}
	pid_trans_info->csc = wu_csc(pid_trans_info, sizeof(*pid_trans_info) - 1);

	trace_info("channel #%d generated pid_trans_info csc = %#x",
		chan_idx, pid_trans_info->csc);
}

void prog_info_2_pid_trans_info()
{
	uint8_t chan_idx;

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		prog_info_2_pid_trans_info_of_channel(chan_idx);
	}
}

