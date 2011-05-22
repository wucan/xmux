#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#include "wu/wu_base_type.h"
#include "wu/thread.h"
#include "wu/message.h"
#include "xmux.h"
#include "xmux_config.h"
#include "front_panel_intstr.h"
#include "front_panel_define.h"


#define ctrl_mcu_tty	"/dev/ttyS1"

static msgobj mo = {MSG_INFO, ENCOLOR, "ctrl_mcu"};

extern int openport(const char *dev_path, int baud);
static int parse_ctrl_mcu_cmd(int fd, uint8_t *recv_msg_buf);

static Thread *ctrl_mcu_thr;
static bool ctrl_mcu_thread_quit;
static int fd;

int ctrl_mcu_open()
{
	fd = openport(ctrl_mcu_tty, B115200);
	if (fd <= 0) {
		trace_err("failed to open port %s!", ctrl_mcu_tty);
		return -1;
	}
	trace_info("success open control mcu port");

	return 0;
}

int ctrl_mcu_close()
{
	trace_info("close front panel ...");
	if (ctrl_mcu_thr) {
		ctrl_mcu_thread_quit = true;
		thread_join(ctrl_mcu_thr);
		ctrl_mcu_thr = NULL;
	}
	if (fd > 0) {
		close(fd);
		fd = 0;
	}
	trace_info("closed"); 

	return 0;
}

static int read_bytes(char *buf, int expect_size)
{
	int rc;
	fd_set rset;
	struct timeval tv;
	int readed_len = 0;

	while (1) {
		FD_ZERO(&rset);
		FD_SET(fd, &rset);
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		rc = select(fd + 1, &rset, NULL, NULL, &tv);
		if (rc <= 0) {
			return readed_len;
		} else if (FD_ISSET(fd, &rset)) {
			rc = read(fd, buf + readed_len, expect_size - readed_len);
			if (rc > 0) {
				readed_len += rc;
				if (readed_len == expect_size) {
					return expect_size;
				}
			}
		}
	}

	return expect_size;
}

static int ctrl_mcu_thread(void *data)
{
	int rc;
	fd_set rset;
	struct timeval tv;
	int readed_len = 0;
	uint8_t recv_buf[FP_RECV_MSG_MAX_SIZE + 10];

	trace_info("control mcu thread running ...");
	while (!ctrl_mcu_thread_quit) {
		FD_ZERO(&rset);
		FD_SET(fd, &rset);
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		rc = select(fd + 1, &rset, NULL, NULL, &tv);
		if (rc <= 0) {
			readed_len = 0;
			continue;
		} else if (FD_ISSET(fd, &rset)) {
			int nlen = 0;
			struct fp_cmd_header hdr;

			// read header
			nlen = read(fd, recv_buf + readed_len, sizeof(hdr) - readed_len);
			if (nlen < 0) {
				trace_err("failed to read header, readed %d!", readed_len);
				readed_len = 0;
				continue;
			} else {
				readed_len += nlen;
				if (readed_len == sizeof(struct fp_cmd_header)) {
					// got
					readed_len = 0;
				} else {
					continue;
				}
			}
			if (recv_buf[0] != defMcuSyncFlag) {
				trace_err("invalid sync byte %#x! flush buffer..", recv_buf[0]);
				hex_dump("invalid header", recv_buf, sizeof(hdr));
				nlen = read(fd, recv_buf, FP_RECV_MSG_MAX_SIZE);
				if (nlen > 0) {
					trace_err("discard %d bytes data...", nlen);
					hex_dump("discard data", recv_buf, MIN(nlen, 16));
				}
				continue;
			}
			buf_2_fp_cmd_header(&hdr, recv_buf);
			if (hdr.len + sizeof(hdr) + FP_MSG_CRC_SIZE > FP_RECV_MSG_MAX_SIZE) {
				trace_err("error! len %d too large!", hdr.len);
				hex_dump("header", &hdr, sizeof(hdr));
				continue;
			}
			// read body and crc
			nlen = read_bytes(recv_buf + sizeof(hdr), hdr.len + FP_MSG_CRC_SIZE);
			if (nlen == hdr.len + FP_MSG_CRC_SIZE) {
				// ok, parse the msg
				parse_mcu_cmd(fd, recv_buf);
			}
		}
	}
	trace_info("front panel thread quited");

	return 0;
}

/*
 * XXX: not used!
 */
static int ctrl_mcu_run()
{
	if (fd <= 0) {
		trace_err("port not opened! disable control muc!");
		return -1;
	}

	ctrl_mcu_thr = thread_create(ctrl_mcu_thread, NULL);
	if (!ctrl_mcu_thr) {
		return -1;
	}

	return 0;
}

static int __parse_ctrl_mcu_cmd(uint8_t *recv_msg_buf, uint8_t *resp_msg_buf,
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

	switch (cmd) {
	default:
		trace_warn("unsupport cmd %d\n", cmd);
		break;
	}

	return 0;
}

static int parse_ctrl_mcu_cmd(int fd, uint8_t *recv_msg_buf)
{
	int rc = 0;
	uint8_t resp_msg_buf[defMcuBufMax];
	uint16_t resp_msg_len = 0;

	hex_dump("recv", recv_msg_buf, 32);
	memset(resp_msg_buf, 0x00, sizeof(resp_msg_buf));
	rc = __parse_ctrl_mcu_cmd(recv_msg_buf, resp_msg_buf, &resp_msg_len);
	trace_info("respose msg len %d", resp_msg_len);
	if (resp_msg_len > 0) {
		hex_dump("resp", resp_msg_buf, resp_msg_len);
		write(fd, resp_msg_buf, resp_msg_len);
	}

	return rc;
}

int ctrl_mcu_send_cmd(uint8_t *send_data, int send_data_len)
{
	int rc;

	rc = write(fd, send_data, send_data_len);
	if (rc < send_data_len) {
		trace_err("send failed! rc %d\n", rc);
		return -1;
	}

	return 0;
}

int ctrl_mcu_send_and_recv_cmd(uint8_t *send_data, int send_data_len,
		uint8_t *recv_param_buf, uint16_t expect_cmd, int expect_param_len)
{
	int rc;
	int nlen;
	struct fp_cmd_header hdr;
	static uint8_t recv_buf[256];

	rc = write(fd, send_data, send_data_len);
	if (rc < send_data_len) {
		trace_err("send failed! rc %d\n", rc);
		return -1;
	}

	// read header
	nlen = read(fd, recv_buf, sizeof(hdr));
	if (nlen < 0) {
		trace_err("failed to read header, readed %d!", nlen);
		return -1;
	}
	if (recv_buf[0] != defMcuSyncFlag) {
		trace_err("invalid sync byte %#x! flush buffer..", recv_buf[0]);
		hex_dump("invalid header", recv_buf, sizeof(hdr));
		nlen = read(fd, recv_buf, 256);
		if (rc > 0) {
			trace_err("discard %d bytes data...", nlen);
			hex_dump("discard data", recv_buf, MIN(nlen, 16));
		}
		return -1;
	}

	buf_2_fp_cmd_header(&hdr, recv_buf);
	if (hdr.len + sizeof(hdr) + FP_MSG_CRC_SIZE > 256) {
		trace_err("error! len %d too large!", hdr.len);
		hex_dump("header", &hdr, sizeof(hdr));
		return -1;
	}
	// read body and crc
	nlen = read_bytes(recv_buf + sizeof(hdr), hdr.len + FP_MSG_CRC_SIZE);
	if (nlen != hdr.len + FP_MSG_CRC_SIZE) {
		trace_err("recv body and crc failed!");
		return -1;
	}
	if ((hdr.seq & 0x7FFF) != expect_cmd) {
		trace_err("recv cmd invalidate! expect %d\n", expect_cmd);
		return -1;
	}
	if (nlen != expect_param_len + FP_MSG_CRC_SIZE) {
		trace_err("recv param size invalidate! expect %d\n", expect_param_len);
		return -1;
	}
	memcpy(recv_param_buf, recv_buf + sizeof(hdr), expect_param_len);

	return 0;
}

