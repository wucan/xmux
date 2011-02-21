#include <fcntl.h>

#include "wu/message.h"
#include "wu/wu_byte_stream.h"
#include "wu/wu_csc.h"

#include "xmux_config.h"
#include "xmux_misc.h"
#include "xmux_net.h"
#include "front_panel_intstr.h"
#include "front_panel_define.h"
#include "front_panel_data_churning.h"
#include "psi_worker.h"
#include "psi_gen.h"

#include "hfpga.h"
#include "pid_map_table.h"
#include "program_pid_validater.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "fp_process"};

PROG_INFO_T g_prog_info_table[CHANNEL_MAX_NUM * PROGRAM_MAX_NUM];
CHN_NUM_T g_chan_num;

static int fp_create_response_cmd(uint8_t *buf, struct fp_cmd_header *req_cmd_header, void *param, int size)
{
	req_cmd_header->len = size;
	fp_cmd_header_2_buf(req_cmd_header, buf);
	memcpy(buf + sizeof(struct fp_cmd_header), param, size);
	buf[sizeof(struct fp_cmd_header) + size] =  wu_csc(buf, sizeof(struct fp_cmd_header) + size);

	return (sizeof(struct fp_cmd_header) + size + FP_MSG_CRC_SIZE);
}
int fp_build_cmd(uint8_t *buf, bool is_read, int cmd,
		void *param, int param_size)
{
	struct fp_cmd_header hdr;

	hdr.sync = defMcuSyncFlag;
	if (is_read)
		hdr.seq = (1 << 15) | cmd;
	else
		hdr.seq = cmd;
	hdr.len = param_size;

	return fp_create_response_cmd(buf, &hdr, param, param_size);
}
bool fp_validate_cmd(uint8_t *buf, int size, int expect_param_size)
{
	struct fp_cmd_header hdr;

	if (size < (sizeof(struct fp_cmd_header) + 1))
		return false;
	buf_2_fp_cmd_header(&hdr, buf);
	if (sizeof(struct fp_cmd_header) + hdr.len + 1 != size)
		return false;
	if (hdr.len != expect_param_size)
		return false;
	if (wu_csc(buf, size - 1) != buf[size - 1])
		return false;

	return true;
}

static int cmd_program_info_handler(struct fp_cmd_header *cmd_header, int is_read,
				uint8_t *recv_msg_buf, uint8_t *resp_msg_buf,
				uint16_t *p_resp_msg_len)
{
	int nlen;
	uint16_t prog_idx = cmd_header->seq & 0x7FFF;

	trace_info("%s program #%d info ...", is_read ? "read" : "write", prog_idx);
	if (is_read && cmd_header->len > 0) {
		trace_err("read encounter len %d!", cmd_header->len);
		return -1;
	}
	if (is_read == 0 && cmd_header->len == 0) {
		trace_err("write encounter len 0!");
		return -1;
	}

	if (is_read) {
		PROG_INFO_T *pProg = &(g_prog_info_table[prog_idx]);
		cmd_header->len = sizeof(PROG_INFO_T);
		nlen = sizeof(struct fp_cmd_header) + cmd_header->len;
		*p_resp_msg_len = nlen + FP_MSG_CRC_SIZE;

		fp_cmd_header_2_buf(cmd_header, resp_msg_buf);
		prog_info_2_buf(pProg, resp_msg_buf + sizeof(struct fp_cmd_header));
		*(resp_msg_buf + nlen) = wu_csc(resp_msg_buf, nlen);
	} else {
		uint8_t tmpbuf[5];
		int isprogvalid = 0;
		int chan_idx_sel = prog_idx / PROGRAM_MAX_NUM;
		PROG_INFO_T refProg;
		PROG_INFO_T *pProg = &(g_prog_info_table[prog_idx]);
		memcpy(&refProg, pProg, sizeof(PROG_INFO_T));
		buf_2_prog_info(pProg, recv_msg_buf + sizeof(struct fp_cmd_header));
		tmpbuf[0] = 0xFF;
		tmpbuf[1] = 0xFF;
		if (pProg->status == 1) {
			if (pids_isvalid_in_program(pProg) != enm_prog_pid_valid ||
				current_prog_pids_is_repeat(prog_idx, &g_prog_info_table[0]) != enm_prog_pid_valid ||
				valid_map_pids_in_one_channel(chan_idx_sel, &g_prog_info_table[0]) > FPGA_PID_MAP_TABLE_CHAN_PIDS ||
				seleted_programs_quant(&g_prog_info_table[0]) > defSelectedProgFpga) {
				memcpy(pProg, &refProg, sizeof(PROG_INFO_T));
				trace_err("select program or pid error!");
				tmpbuf[1] = 0xFE;
			}
		}

		cmd_header->len = 2;
		nlen = cmd_header->len + sizeof(struct fp_cmd_header);
		*p_resp_msg_len = nlen + FP_MSG_CRC_SIZE;
		fp_cmd_header_2_buf(cmd_header, resp_msg_buf);
		memcpy(resp_msg_buf + sizeof(struct fp_cmd_header), tmpbuf, cmd_header->len);
		*(resp_msg_buf + nlen) = wu_csc(resp_msg_buf, nlen);
	}

	return 1;
}

static int cmd_0x101_handler(struct fp_cmd_header *cmd_header, int is_read,
				uint8_t *recv_msg_buf, uint8_t *resp_msg_buf,
				uint16_t *p_resp_msg_len)
{
	OUT_RATE_T tmp_out_rate = {0};

	if (is_read) {
		tmp_out_rate.rate = SWAP_U32(g_eeprom_param.sys.output_bitrate);
		*p_resp_msg_len = fp_create_response_cmd(resp_msg_buf, cmd_header, &tmp_out_rate, sizeof(tmp_out_rate));
		return 1;
	}

	buf_2_out_rate(&tmp_out_rate, recv_msg_buf + sizeof(struct fp_cmd_header));
	set_output_bitrate(tmp_out_rate.rate);

	return 1;
}

static int cmd_0x102_handler(struct fp_cmd_header *cmd_header, int is_read,
				uint8_t *recv_msg_buf, uint8_t *resp_msg_buf,
				uint16_t *p_resp_msg_len)
{
	NET_ETH0_T neteth0;
	struct xmux_net_param xmux_net;

	if (is_read) {
		int nlen;
		cmd_header->len = sizeof(NET_ETH0_T);
		nlen = cmd_header->len + sizeof(struct fp_cmd_header);
		*p_resp_msg_len = nlen + FP_MSG_CRC_SIZE;
		fp_cmd_header_2_buf(cmd_header, resp_msg_buf);
		xmux_net_2_fp_net(&g_eeprom_param.net, &neteth0);
		memcpy(resp_msg_buf + sizeof(struct fp_cmd_header), &neteth0, cmd_header->len);

		*(resp_msg_buf + nlen) = wu_csc(resp_msg_buf, nlen);
		return 1;
	}

	memcpy(&neteth0, recv_msg_buf + sizeof(struct fp_cmd_header), sizeof(NET_ETH0_T));
	fp_net_2_xmux_net(&neteth0, &xmux_net);
	xmux_net_set(&xmux_net);

	return 1;
}

static void _apply_pid_map_table_and_psi()
{
	int howto = 1;
	struct pid_map_table_gen_context pid_map_gen_ctx;
	int chan_idx, prog_idx;
	int j;

	/* clear mux program info */
	memset(&g_eeprom_param.mux_prog_info, 0, sizeof(struct xmux_mux_program_info));

	pid_map_table_gen_start(&pid_map_gen_ctx);
	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		if (!g_chan_num.num[chan_idx])
			continue;
		for (prog_idx = 0; prog_idx < PROGRAM_MAX_NUM; prog_idx++) {
			PROG_INFO_T *prog_info = &g_prog_info_table[chan_idx * PROGRAM_MAX_NUM + prog_idx];
			if (prog_info->status == 1) {
				/* fill pid map table */
				if (pid_map_table_push_pid_pair(&pid_map_gen_ctx, chan_idx,
					prog_info->info.pmt.in, prog_info->info.pmt.out)) {
					goto pid_map_gen_done;
				}
				if (pid_map_table_push_pid_pair(&pid_map_gen_ctx, chan_idx,
					prog_info->info.pcr.in, prog_info->info.pcr.out)) {
					goto pid_map_gen_done;
				}
				for (j = 0; j < PROGRAM_DATA_PID_MAX_NUM; j++) {
					uint16_t in_pid = prog_info->info.data[j].in;
					uint16_t out_pid = prog_info->info.data[j].out;
					if (prog_info->info.pcr.in != in_pid &&
						prog_pid_val_isvalid(in_pid) &&
						prog_pid_val_isvalid(out_pid)) {
						if (pid_map_table_push_pid_pair(&pid_map_gen_ctx, chan_idx,
							in_pid, out_pid)) {
							goto pid_map_gen_done;
						}
					}
				}
				/* fill mux program info */
				g_eeprom_param.mux_prog_info.programs[g_eeprom_param.mux_prog_info.nprogs].chan_idx = chan_idx;
				g_eeprom_param.mux_prog_info.programs[g_eeprom_param.mux_prog_info.nprogs].prog_idx = prog_idx;
				g_eeprom_param.mux_prog_info.nprogs++;
			}
		}
	}

pid_map_gen_done:
	pid_map_table_gen_end(&pid_map_gen_ctx, 0xFF);
	xmux_config_save_pid_map_table(pid_map_gen_ctx.fpga_pid_map.pid_map);
	hfpga_write_pid_map(&pid_map_gen_ctx.fpga_pid_map);
	g_param_mng_info.eeprom_pid_map_table_version++;

	/*
	 * generate and download psi to fpga
	 */
	psi_gen_and_apply_from_fp();

	/*
	 * and at last save pid_trans_info
	 */
	prog_info_2_pid_trans_info();
	xmux_config_save_pid_trans_info_all();
	g_param_mng_info.eeprom_pid_trans_info_version++;

	/*
	 * save mux program info
	 */
	xmux_config_save_mux_program_info(&g_eeprom_param.mux_prog_info);
}
static int cmd_0x103_handler(struct fp_cmd_header *cmd_header, int is_read,
				uint8_t *recv_msg_buf, uint8_t *resp_msg_buf,
				uint16_t *p_resp_msg_len)
{
	uint16_t sys_cmd;

	sys_cmd = READ_U16_BE(recv_msg_buf + sizeof(struct fp_cmd_header));
	trace_info("process system cmd %d", sys_cmd);
	switch (sys_cmd) {
		case FP_SYS_CMD_ENTER_FP_MANAGEMENT_MODE:
			if (management_mode == MANAGEMENT_MODE_SNMP) {
				trace_info("switch to fp management mode");
				enter_fp_management_mode();
			} else {
				trace_warn("enter again, alreay in fp management mode!");
			}
			break;
		case FP_SYS_CMD_LEAVE_FP_MANAGEMENT_MODE:
			if (management_mode == MANAGEMENT_MODE_FP) {
				trace_info("leave fp management mode");
				leave_fp_management_mode();
			} else {
				trace_warn("already leave fp management mode!");
			}
			break;
		case FP_SYS_CMD_START_ANALYSE_PSI:
			pid_map_table_reset();
			psi_worker_request_parse_psi(0, NULL, MANAGEMENT_MODE_FP);
			break;
		case FP_SYS_CMD_READ_MAP_ANALYSE_STATUS:
		{
			int nlen;
			cmd_header->len = sizeof(CHN_NUM_T);
			nlen = cmd_header->len + sizeof(struct fp_cmd_header);
			*p_resp_msg_len = nlen + FP_MSG_CRC_SIZE;
			fp_cmd_header_2_buf(cmd_header, resp_msg_buf);
			memcpy(resp_msg_buf + sizeof(struct fp_cmd_header), &g_chan_num, cmd_header->len);
			*(resp_msg_buf + nlen) = wu_csc(resp_msg_buf, nlen);
			return 1;
		}
			break;
		case FP_SYS_CMD_READ_TS_STATUS:
		{
			int nlen;
			uint16_t ts_status;
			uint8_t ts_status_u8 = 0;

			if (hfpga_get_ts_status(0, &ts_status) >= 0)
				ts_status_u8 = (uint8_t)ts_status;
			cmd_header->len = 1;
			nlen = cmd_header->len + sizeof(struct fp_cmd_header);
			*p_resp_msg_len = nlen + FP_MSG_CRC_SIZE;
			fp_cmd_header_2_buf(cmd_header, resp_msg_buf);
			memcpy(resp_msg_buf + sizeof(struct fp_cmd_header), &ts_status_u8, cmd_header->len);
			*(resp_msg_buf + nlen) = wu_csc(resp_msg_buf, nlen);
			return 1;
		}
			break;
		case FP_SYS_CMD_APPLY_MAP_ANALYSE:
			_apply_pid_map_table_and_psi();
			return 1;
			break;
		case FP_SYS_CMD_RESET_NET:
			xmux_net_param_init_default(&g_eeprom_param.net);
			xmux_net_set(&g_eeprom_param.net);
			return 1;
			break;
		default:
			return -1;
			break;
	}

	return -1;
}

int __parse_mcu_cmd(uint8_t *recv_msg_buf, uint8_t *resp_msg_buf,
					uint16_t *p_resp_msg_len)
{
	int is_read = 1;			//MCU send command to ARM for get parameter
	uint16_t cmd;
	struct fp_cmd_header cmd_header;

	buf_2_fp_cmd_header(&cmd_header, recv_msg_buf);
	if (cmd_header.sync != defMcuSyncFlag) {
		trace_err("invalid sync byte!");
		return 0;
	}
	if (cmd_header.seq & 0x8000)
		is_read = 1;
	else
		is_read = 0;
	cmd = cmd_header.seq & 0x7FFF;
	trace_info("cmd %d(%#x), len %d, %s",
		cmd, cmd, cmd_header.len, is_read ? "read" : "write");
	if (cmd < PROGRAM_MAX_NUM * CHANNEL_MAX_NUM) {
		return cmd_program_info_handler(&cmd_header, is_read,
			recv_msg_buf, resp_msg_buf, p_resp_msg_len);
	}

	switch (cmd) {
	case 0x101:
		return cmd_0x101_handler(&cmd_header, is_read,
			recv_msg_buf, resp_msg_buf, p_resp_msg_len);
		break;
	case 0x102:
		return cmd_0x102_handler(&cmd_header, is_read,
			recv_msg_buf, resp_msg_buf, p_resp_msg_len);
		break;
	case 0x103:
		return cmd_0x103_handler(&cmd_header, is_read,
			recv_msg_buf, resp_msg_buf, p_resp_msg_len);
		break;
	default:
		trace_warn("unsupport cmd %#x", cmd);
		break;
	}

	return 0;
}

int parse_mcu_cmd(int fdttyS1, uint8_t *recv_msg_buf)
{
	int rc = 0;
	uint8_t resp_msg_buf[defMcuBufMax];
	uint16_t resp_msg_len = 0;

	hex_dump("recv", recv_msg_buf, 32);
	memset(resp_msg_buf, 0x00, sizeof(resp_msg_buf));
	rc = __parse_mcu_cmd(recv_msg_buf, resp_msg_buf, &resp_msg_len);
	trace_info("respose msg len %d", resp_msg_len);
	if (resp_msg_len > 0) {
		hex_dump("resp", resp_msg_buf, resp_msg_len);
		write(fdttyS1, resp_msg_buf, resp_msg_len);
	}

	return rc;

}

void fp_select_program(uint8_t prog_idx)
{
	PROG_INFO_T prog = g_prog_info_table[prog_idx];
	int chan_idx_sel = prog_idx / PROGRAM_MAX_NUM;

	if ((prog_idx % PROGRAM_MAX_NUM) >= g_chan_num.num[chan_idx_sel]) {
		return;
	}
	if (prog.status == 1) {
		return;
	}

	xmux_program_info_dump(&prog.info);
	if (pids_isvalid_in_program(&prog) != enm_prog_pid_valid ||
		current_prog_pids_is_repeat(prog_idx, &g_prog_info_table[0]) != enm_prog_pid_valid ||
		valid_map_pids_in_one_channel(chan_idx_sel, &g_prog_info_table[0]) > FPGA_PID_MAP_TABLE_CHAN_PIDS ||
		seleted_programs_quant(&g_prog_info_table[0]) > defSelectedProgFpga) {
		trace_err("select program #%d failed!", prog_idx);
		return;
	}
	prog.status = 1;
	g_prog_info_table[prog_idx] = prog;
	trace_info("select program #%d success", prog_idx);
}

void fp_deselect_program(uint8_t prog_idx)
{
	PROG_INFO_T *prog = &g_prog_info_table[prog_idx];
	int chan_idx_sel = prog_idx / PROGRAM_MAX_NUM;

	if ((prog_idx % PROGRAM_MAX_NUM) >= g_chan_num.num[chan_idx_sel]) {
		return;
	}
	if (!prog->status) {
		return;
	}
	prog->status = 0;
}

