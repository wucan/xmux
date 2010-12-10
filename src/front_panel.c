#include <stdio.h>
#include <stdlib.h>

#include "wu/wu_base_type.h"
#include "wu/thread.h"
#include "wu/message.h"
#include "xmux.h"
#include "front_panel.h"
#include "front_panel_intstr.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "fp"};

extern int front_panel_parse_msg(int fd, unsigned char *recv_msg_buf);
extern int openport();

static Thread *fp_thr;
static bool fp_thread_quit;
static int fd;

int front_panel_open()
{
	fd = openport();
	if (fd <= 0) {
		trace_err("failed to open port!");
		return -1;
	}
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
	trace_info("closed"); 

	return 0;
}

static uint8_t recv_buf[FP_RECV_MSG_MAX_SIZE + 10];
static int fp_thread(void *data)
{
	int rc;
	fd_set rset;
	struct timeval tv;

	trace_info("front panel thread running ...");
	while (!fp_thread_quit) {
		FD_ZERO(&rset);
		FD_SET(fd, &rset);
		tv.tv_sec = 10;
		tv.tv_usec = 0;

		rc = select(fd + 1, &rset, NULL, NULL, &tv);
		if (rc <= 0) {
			continue;
		} else if (FD_ISSET(fd, &rset)) {
			int i;
			int nlen = 0;
			int nwantlen = FP_RECV_MSG_MAX_SIZE;
			uint16_t cmd_len;
			memset(recv_buf, 0x00, sizeof(recv_buf));

			for (i = 0; i < 20; i++) {
				int ntmp = read(fd, &recv_buf[nlen], nwantlen);
				if (ntmp > 0) {
					nlen += ntmp;
					if (nlen >= sizeof(struct fp_cmd_header)) {
						struct fp_cmd_header cmd_header;
						mcu_arm_head_buf_to_t(&cmd_header, recv_buf);
						cmd_len = cmd_header.len + sizeof(struct fp_cmd_header) + FP_MSG_CRC_SIZE;
						if (cmd_len > FP_RECV_MSG_MAX_SIZE)
							break;
						if (nlen >= cmd_len) {
							parse_mcu_cmd(fd, recv_buf);
							break;
						} else
							nwantlen = cmd_len - nlen;
					}
				}
				usleep(5000);
			}
			sleep(1);
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

