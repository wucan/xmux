#include <fcntl.h>

#include "wu/message.h"
#include "wu/wu_byte_stream.h"
#include "wu/wu_csc.h"

#include "xmux_config.h"
#include "rs232.h"
#include "front_panel_intstr.h"
#include "front_panel_define.h"
#include "psi_parse.h"

#include "hfpga.h"
#include "gen_dvb_si.h"
#include "pid_map_table.h"
#include "program_pid_validater.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "mcu_rs232"};

extern uv_dvb_io hfpga_dev;

PROG_INFO_T g_prog_info_table[CHANNEL_MAX_NUM * PROGRAM_MAX_NUM];
CHN_NUM_T g_chan_num;
NET_ETH0_T neteth0;

static int fp_create_response_cmd(uint8_t *buf, struct fp_cmd_header *req_cmd_header, void *param, int size)
{
	req_cmd_header->len = size;
	mcu_arm_head_t_to_buf(req_cmd_header, buf);
	memcpy(buf + sizeof(struct fp_cmd_header), param, size);
	buf[sizeof(struct fp_cmd_header) + size] =  wu_csc(buf, sizeof(struct fp_cmd_header) + size);

	return (sizeof(struct fp_cmd_header) + size + FP_MSG_CRC_SIZE);
}

static int cmd_program_info_handler(struct fp_cmd_header *cmd_header, int is_read,
				uint8_t *recv_msg_buf, uint8_t *resp_msg_buf,
				uint16_t *p_resp_msg_len)
{
	int nlen;
	uint16_t cmd = cmd_header->seq & 0x7FFF;

	trace_info("%s program #%d info ...", is_read ? "read" : "write", cmd);
	if (is_read && cmd_header->len > 0) {
		trace_err("read encounter len %d!", cmd_header->len);
		return -1;
	}
	if (is_read == 0 && cmd_header->len == 0) {
		trace_err("write encounter len 0!");
		return -1;
	}

	if (is_read) {
		PROG_INFO_T *pProg = &(g_prog_info_table[cmd]);
		cmd_header->len = sizeof(PROG_INFO_T);
		nlen = sizeof(struct fp_cmd_header) + cmd_header->len;
		*p_resp_msg_len = nlen + FP_MSG_CRC_SIZE;

		mcu_arm_head_t_to_buf(cmd_header, resp_msg_buf);
		mcu_arm_prog_t_to_buf(pProg, resp_msg_buf + sizeof(struct fp_cmd_header));
		*(resp_msg_buf + nlen) = wu_csc(resp_msg_buf, nlen);
	} else {
		uint8_t tmpbuf[5];
		int isprogvalid = 0;
		int chan_idx_sel = cmd / PROGRAM_MAX_NUM;
		PROG_INFO_T refProg;
		PROG_INFO_T *pProg = &(g_prog_info_table[cmd]);
		memcpy(&refProg, pProg, sizeof(PROG_INFO_T));
		mcu_arm_prog_buf_to_t(pProg, recv_msg_buf + sizeof(struct fp_cmd_header));
		tmpbuf[0] = 0xFF;
		tmpbuf[1] = 0xFF;
		if (pProg->status == 1) {
			if (pids_isvalid_in_program(pProg) != enm_prog_pid_valid ||
				current_prog_pids_is_repeat(cmd, &g_prog_info_table[0]) != enm_prog_pid_valid ||
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
		mcu_arm_head_t_to_buf(cmd_header, resp_msg_buf);
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
		tmp_out_rate.rate = SWAP_U32(g_xmux_root_param.sys.output_bitrate);
		*p_resp_msg_len = fp_create_response_cmd(resp_msg_buf, cmd_header, &tmp_out_rate, sizeof(tmp_out_rate));
		return 1;
	}

	mcu_arm_rate_buf_to_t(&tmp_out_rate, recv_msg_buf + sizeof(struct fp_cmd_header));
	xmux_config_update_output_bitrate(tmp_out_rate.rate);

	return 1;
}

static int cmd_0x102_handler(struct fp_cmd_header *cmd_header, int is_read,
				uint8_t *recv_msg_buf, uint8_t *resp_msg_buf,
				uint16_t *p_resp_msg_len)
{
	if (is_read) {
		int nlen;
		cmd_header->len = sizeof(NET_ETH0_T);
		nlen = cmd_header->len + sizeof(struct fp_cmd_header);
		*p_resp_msg_len = nlen + FP_MSG_CRC_SIZE;
		mcu_arm_head_t_to_buf(cmd_header, resp_msg_buf);
		memcpy(resp_msg_buf + sizeof(struct fp_cmd_header), &neteth0, cmd_header->len);

		*(resp_msg_buf + nlen) = wu_csc(resp_msg_buf, nlen);
		return 1;
	}

	memcpy(&neteth0, recv_msg_buf + sizeof(struct fp_cmd_header), sizeof(NET_ETH0_T));

	return 1;
}

/*
 * system cmd 4, ???
 */
static void test_gen_psi()
{
	int total_progs = 0;

	trace_info("test gen psi on all channels ...");
	pid_map_table_reset();
	total_progs = psi_parsing();
	trace_info("there are total %d prognums", total_progs);
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
			management_mode = MANAGEMENT_MODE_FP;
			trace_info("switch to front panel management mode!");
			break;
		case FP_SYS_CMD_EXIT_FP_MANAGEMENT_MODE:
			management_mode = MANAGEMENT_MODE_SNMP;
			trace_info("switch to net management mode!");
			break;
		case FP_SYS_CMD_START_ANALYSE_PSI:
			test_gen_psi();
			break;
		case FP_SYS_CMD_READ_MAP_ANALYSE_STATUS:
		{
			int nlen;
			cmd_header->len = sizeof(CHN_NUM_T);
			nlen = cmd_header->len + sizeof(struct fp_cmd_header);
			*p_resp_msg_len = nlen + FP_MSG_CRC_SIZE;
			mcu_arm_head_t_to_buf(cmd_header, resp_msg_buf);
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
			mcu_arm_head_t_to_buf(cmd_header, resp_msg_buf);
			memcpy(resp_msg_buf + sizeof(struct fp_cmd_header), &ts_status_u8, cmd_header->len);
			*(resp_msg_buf + nlen) = wu_csc(resp_msg_buf, nlen);
			return 1;
		}
			break;
		case FP_SYS_CMD_APPLY_MAP_ANALYSE:
		{
			uint8_t *packpara[8192];
			PROG_INFO_T *pProg = &g_prog_info_table[0];
			int howto = 1;
			ACCESS_HFPGA_PID_MAP pid_map;
			int chan_cnt, prog_cnt;

#if 1
			for (chan_cnt = 0; chan_cnt < CHANNEL_MAX_NUM; chan_cnt++) {
				int npidcount = 0;
				int j;
				for (j = 0; j < FPGA_PID_MAP_TABLE_CHAN_PIDS; j++) {
					pid_map_table_set_in_pid(&pid_map, chan_cnt, j, 0x000F);
					pid_map_table_set_out_pid(&pid_map, chan_cnt, j, 0x000F);
				}
				for (prog_cnt = 0; prog_cnt < PROGRAM_MAX_NUM; prog_cnt++) {
					PROG_INFO_T *pProg = &g_prog_info_table[chan_cnt * PROGRAM_MAX_NUM + prog_cnt];
					trace_info("#%d/%d status %d, input PCR PID %u",
					   pProg->status, pProg->PCR_PID_IN);
					if (pProg->status == 1) {
						pid_map_table_set_in_pid(&pid_map, chan_cnt, npidcount, pProg->PCR_PID_IN);
						pid_map_table_set_out_pid(&pid_map, chan_cnt, npidcount, pProg->PCR_PID_OUT);
						npidcount++;

						for (j = 0; j < PROGRAM_DATA_PID_MAX_NUM; j++) {
							if (pProg->PCR_PID_IN != pProg->pids[j].in &&
								prog_pid_val_isvalid(pProg->pids[j].in) &&
								prog_pid_val_isvalid(pProg->pids[j].out)) {
								pid_map_table_set_in_pid(&pid_map, chan_cnt, npidcount, pProg->pids[j].in);
								pid_map_table_set_out_pid(&pid_map, chan_cnt, npidcount, pProg->pids[j].out);

								npidcount++;
							}
						}
					}
					if (npidcount >= FPGA_PID_MAP_TABLE_CHAN_PIDS)
						break;
				}
			}

			pid_map.cha = 0xFF;	//chan_cnt;
			hfpga_write_pid_map(pid_map);
#endif

#if 1
			dvbSI_Start(&hfpga_dev);
			trace_info("stop gen si");
			dvbSI_GenSS(HFPGA_CMD_SI_STOP);
			///////////// map in_pid to out_pid must be brfore generate psi section - ruan 2010-11-04
			gen_pat_pmt_fr_mcu(packpara, pProg);
			//gen_sdt_fr_mcu(packpara, pProg);
			trace_info("start gen si");
			dvbSI_GenSS(HFPGA_CMD_SI_START);
			dvbSI_Stop();
#endif
			return 1;

		}
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

	mcu_arm_head_buf_to_t(&cmd_header, recv_msg_buf);
	if (cmd_header.sync != defMcuSyncFlag) {
		trace_err("invalid sync byte!");
		return 0;
	}
	if (cmd_header.seq & 0x8000)
		is_read = 1;
	else
		is_read = 0;
	cmd = cmd_header.seq & 0x7FFF;
	trace_info("cmd %d(%#x), len %d, seq %d",
		cmd, cmd, cmd_header.len, cmd_header.seq);
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

	memset(resp_msg_buf, 0x00, sizeof(resp_msg_buf));
	rc = __parse_mcu_cmd(recv_msg_buf, resp_msg_buf, &resp_msg_len);
	trace_info("respose msg len %d", resp_msg_len);
	if (resp_msg_len > 0) {
		write(fdttyS1, resp_msg_buf, resp_msg_len);
	}

	return rc;

}

