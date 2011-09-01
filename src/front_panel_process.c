#include <fcntl.h>

#include "wu/message.h"
#include "wu/wu_byte_stream.h"
#include "wu/wu_csc.h"

#include "xmux_config.h"
#include "xmux_misc.h"
#include "xmux_net.h"
#include "front_panel.h"
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

int fp_create_response_cmd(uint8_t *buf,
	struct fp_cmd_header *req_cmd_header, void *param, int size)
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
void fp_cmd_copy(struct fp_cmd *dst, struct fp_cmd *src)
{
	memcpy(dst, src, FP_CMD_SIZE(src));
}

static PROG_INFO_T old_prog;
static int cmd_program_info_handler(struct fp_cmd_header *cmd_header, int is_read,
				uint8_t *recv_msg_buf, uint8_t *resp_msg_buf,
				uint16_t *p_resp_msg_len)
{
	int nlen;
	uint16_t prog_idx = cmd_header->seq & 0x7FFF;
	uint8_t chan_idx = prog_idx / PROGRAM_MAX_NUM;

	trace_info("%s program #%d info ...", is_read ? "read" : "write", prog_idx);
	if (is_read && cmd_header->len > 0) {
		trace_err("read encounter len %d!", cmd_header->len);
		return -1;
	}
	if (is_read == 0 && cmd_header->len == 0) {
		trace_err("write encounter len 0!");
		return -1;
	}
	if (prog_idx % PROGRAM_MAX_NUM >=  g_chan_num.num[chan_idx]) {
		trace_err("select program #%d not presented!", prog_idx);
		return -1;
	}

	if (is_read) {
		PROG_INFO_T tmp_prog;
		PROG_INFO_T prog = g_prog_info_table[prog_idx];
		prepare_program_output_pid(prog_idx, &prog);
		old_prog = prog;
		xmux_program_info_dump(&prog.info, "read");
		prog_info_2_buf(&prog, &tmp_prog);
		*p_resp_msg_len = fp_create_response_cmd(resp_msg_buf, cmd_header,
			&tmp_prog, sizeof(tmp_prog));
	} else {
		uint8_t tmpbuf[5];
		PROG_INFO_T new_prog;
		buf_2_prog_info(&new_prog, recv_msg_buf + sizeof(struct fp_cmd_header));
		xmux_program_info_dump(&new_prog.info, "write");
		tmpbuf[0] = 0xFF;
		tmpbuf[1] = 0xFF;
		if (FP_PROG_SELECTED(&new_prog)) {
			if (!check_and_select_program(prog_idx, &new_prog, &old_prog)) {
				tmpbuf[1] = 0xFE;
			}
		} else {
			fp_deselect_program(prog_idx);
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

static int cmd_out_rate_handler(struct fp_cmd_header *cmd_header, int is_read,
				uint8_t *recv_msg_buf, uint8_t *resp_msg_buf,
				uint16_t *p_resp_msg_len)
{
	OUT_RATE_T tmp_out_rate = {0};

	if (is_read) {
		tmp_out_rate.rate = SWAP_U32(g_eeprom_param.sys.output_bitrate);
		tmp_out_rate.len = g_eeprom_param.sys.format;
		*p_resp_msg_len = fp_create_response_cmd(resp_msg_buf, cmd_header, &tmp_out_rate, sizeof(tmp_out_rate));
		return 1;
	}

	buf_2_out_rate(&tmp_out_rate, recv_msg_buf + sizeof(struct fp_cmd_header));
	set_output_bitrate(tmp_out_rate.rate);
	set_packet_format(tmp_out_rate.len);

	return 1;
}

static int cmd_net_handler(struct fp_cmd_header *cmd_header, int is_read,
				uint8_t *recv_msg_buf, uint8_t *resp_msg_buf,
				uint16_t *p_resp_msg_len)
{
	NET_ETH0_T neteth0;
	struct xmux_net_param xmux_net;

	if (is_read) {
		xmux_net_2_fp_net(&g_eeprom_param.net, &neteth0);
		*p_resp_msg_len = fp_create_response_cmd(resp_msg_buf, cmd_header,
			&neteth0, sizeof(neteth0));
		return 1;
	}

	memcpy(&neteth0, recv_msg_buf + sizeof(struct fp_cmd_header), sizeof(NET_ETH0_T));
	fp_net_2_xmux_net(&neteth0, &xmux_net);
	xmux_net_set(&xmux_net);

	return 1;
}

static void _apply_pid_map_table_and_psi()
{
	pid_map_table_gen_and_apply_from_fp();
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

#if CHANNEL_MAX_NUM == 1
	/*
	 * save raw input pmt section for descramble use
	 */
	xmux_config_save_input_pmt_section();
	xmux_ci_apply();
#endif
}
static int cmd_sys_handler(struct fp_cmd_header *cmd_header, int is_read,
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
			psi_worker_request_parse_psi(0, NULL, MANAGEMENT_MODE_FP);
			break;
		case FP_SYS_CMD_READ_MAP_ANALYSE_STATUS:
		{
			int nlen;
			cmd_header->len = CHANNEL_MAX_NUM;
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
			xmux_user_param_init_default(&g_eeprom_param.user);
			xmux_system_param_init_default(&g_eeprom_param.sys);
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

	if (front_panel_check_recv_cmd(recv_msg_buf)) {
		return 0;
	}

	if (cmd < PROGRAM_MAX_NUM * CHANNEL_MAX_NUM) {
		return cmd_program_info_handler(&cmd_header, is_read,
			recv_msg_buf, resp_msg_buf, p_resp_msg_len);
	}

	switch (cmd) {
	case FP_CMD_OUT_RATE:
		return cmd_out_rate_handler(&cmd_header, is_read,
			recv_msg_buf, resp_msg_buf, p_resp_msg_len);
		break;
	case FP_CMD_NET:
		return cmd_net_handler(&cmd_header, is_read,
			recv_msg_buf, resp_msg_buf, p_resp_msg_len);
		break;
	case FP_CMD_SYS:
		return cmd_sys_handler(&cmd_header, is_read,
			recv_msg_buf, resp_msg_buf, p_resp_msg_len);
		break;
	case FP_CMD_TUNNER1_PARAM:
	case FP_CMD_TUNNER1_STATUS:
	case FP_CMD_TUNNER2_PARAM:
	case FP_CMD_TUNNER2_STATUS:
	case FP_CMD_BCM3033:
		return xmux_4_channel_rf_cmds_handler(&cmd_header, is_read,
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

void fp_select_program(uint16_t prog_idx)
{
	PROG_INFO_T prog = g_prog_info_table[prog_idx];
	int chan_idx_sel = prog_idx / PROGRAM_MAX_NUM;

	if ((prog_idx % PROGRAM_MAX_NUM) >= g_chan_num.num[chan_idx_sel]) {
		return;
	}
	if (FP_PROG_SELECTED(&prog)) {
		return;
	}

	xmux_program_info_dump(&prog.info, "select");
	if (!check_and_select_program(prog_idx, &prog, NULL)) {
		return;
	}
	trace_info("select program #%d success", prog_idx);
}

void fp_deselect_program(uint16_t prog_idx)
{
	PROG_INFO_T *prog = &g_prog_info_table[prog_idx];
	int chan_idx_sel = prog_idx / PROGRAM_MAX_NUM;

	if ((prog_idx % PROGRAM_MAX_NUM) >= g_chan_num.num[chan_idx_sel]) {
		return;
	}
	if (!FP_PROG_SELECTED(prog)) {
		trace_warn("program #%d had not select yet!", prog_idx);
		return;
	}
	FP_DESELECT_PROG(prog);
	trace_info("deselect program #%d success", prog_idx);
}

