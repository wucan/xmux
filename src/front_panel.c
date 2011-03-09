#include <stdio.h>
#include <stdlib.h>

#include "wu/wu_base_type.h"
#include "wu/thread.h"
#include "wu/message.h"
#include "wu/wuswait.h"
#include "xmux.h"
#include "xmux_config.h"
#include "front_panel.h"
#include "front_panel_define.h"
#include "front_panel_intstr.h"
#include "front_panel_data_churning.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "fp"};

extern int front_panel_parse_msg(int fd, unsigned char *recv_msg_buf);
extern int openport();

static Thread *fp_thr;
static bool fp_thread_quit;
static int fd;
static WuSWait fp_swait;

int front_panel_open()
{
	fd = openport();
	if (fd <= 0) {
		trace_err("failed to open port!");
		return -1;
	}
	wu_swait_init(&fp_swait);
	trace_info("success open front panel port");

	return 0;
}

int front_panel_close()
{
	trace_info("close front panel ...");
	if (fp_thr) {
		fp_thread_quit = true;
		thread_join(fp_thr);
		fp_thr = NULL;
	}
	if (fd > 0) {
		close(fd);
		fd = 0;
	}
	wu_swait_destroy(&fp_swait);
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

static uint8_t recv_buf[FP_RECV_MSG_MAX_SIZE + 10];
static int fp_thread(void *data)
{
	int rc;
	fd_set rset;
	struct timeval tv;
	int readed_len = 0;

	trace_info("front panel thread running ...");
	while (!fp_thread_quit) {
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

int front_panel_run()
{
	if (fd <= 0) {
		trace_err("port not opened! disable front panel!");
		return -1;
	}

	fp_thr = thread_create(fp_thread, NULL);
	if (!fp_thr) {
		return -1;
	}

	return 0;
}

static int fp_expect_cmd = -1;
struct fp_cmd * front_panel_send_cmd(struct fp_cmd *cmd, int expect_cmd)
{
	int rc;
	struct fp_cmd *resp_cmd = NULL;

	if (wu_swait_is_alive(&fp_swait)) {
		trace_warn("busy! ignore this send cmd request!");
		return NULL;
	}
	rc = write(fd, cmd, FP_CMD_SIZE(cmd));
	if (rc < FP_CMD_SIZE(cmd)) {
		trace_err("wirte cmd failed!");
		return NULL;
	}
	fp_expect_cmd = expect_cmd;
	resp_cmd = wu_swait_timedwait(&fp_swait, 2000000);
	fp_expect_cmd = -1;

	return resp_cmd;
}

/*
 * @return: 1 the @cmd is response cmd
 */
int front_panel_check_recv_cmd(struct fp_cmd *recv_cmd)
{
	int cmd = SWAP_U16(recv_cmd->header.seq) & 0x7FFF;

	if (cmd == fp_expect_cmd) {
		static char buf[1024];
		struct fp_cmd *resp_cmd = (struct fp_cmd *)buf;
		fp_cmd_copy(resp_cmd, recv_cmd);
		if (wu_swait_wakeup(&fp_swait, resp_cmd)) {
			trace_err("recv respone cmd %d, but maybe had timeouted!", cmd);
		}
		return 1;
	}

	return 0;
}

