#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>

#include "wu/message.h"
#include "mpegts.h"

#include "fpga_api.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "fake_hfpga"};

static int ts_fd;
static int ts_out_fd;
static unsigned short filter_pid;
static unsigned short pid_table[NULL_PID + 1];

int fake_hfpga_open(const char *name, int flags)
{
	char filename[PATH_MAX];

	ts_fd = open(name, O_RDONLY);
	if (ts_fd < 0) {
		perror("open");
		return -1;
	}

	sprintf(filename, "%s-out", name);
	ts_out_fd = open(filename, O_CREAT | O_APPEND | O_WRONLY,
		S_IWUSR | S_IRUSR);

	return ts_fd;
}

void fake_hfpga_close(int hdev)
{
	if (ts_fd > 0) {
		close(ts_fd);
		ts_fd = 0;
	}
	if (ts_out_fd > 0) {
		close(ts_out_fd);
		ts_out_fd = 0;
	}
}

int fake_hfpga_readn(unsigned char *p_buf, unsigned int len,
		void *p_param, int nbgn)
{
#define PLEN		188
	uv_io_param *param = (uv_io_param *)p_param;
	int rc;
	unsigned short pid;
	int rewind_cnt = 0;
	uint8_t buf[PLEN * 2];
	uint8_t next_buf[PLEN];
	int i;

	if (nbgn) {
		filter_pid = param->pid;
		trace_info("readn(), begin filter pid %d(%#x)", filter_pid, filter_pid);
	}
read_again:
	rc = read(ts_fd, buf, len);
	if (rc != len) {
		lseek(ts_fd, 0, SEEK_SET);
		rewind_cnt++;
		if (rewind_cnt >= 2) {
			trace_err("file rewind exceed for filter pid %d", filter_pid);
			return 0;
		}
		rc = read(ts_fd, buf, len);
		if (rc != len) {
			return 0;
		}
	}
	// sync stream
sync_again:
	for (i = 0; i < len; i++) {
		if (buf[i] == 0x47) {
			break;
		}
	}
	if (i > 0) {
		//trace_err("lost synced! skip %d bytes", i);
		read(ts_fd, buf + len, i);
		memcpy(buf, buf + i, PLEN);
		goto sync_again;
	}
	memcpy(p_buf, buf, PLEN);

	//trace_err("seems sync at fpos %#x", lseek(ts_fd, 0, SEEK_CUR) - PLEN);
	/*
	 * ensure really synced
	 */
	i = read(ts_fd, next_buf, PLEN);
	if (i == PLEN) {
		lseek(ts_fd, -PLEN, SEEK_CUR);
		if (next_buf[0] != 0x47) {
			//trace_err("next not sync at fpos %#x", lseek(ts_fd, 0, SEEK_CUR));
			memcpy(buf, p_buf, PLEN);
			buf[0] = 0x00;
			goto sync_again;
		}
		//trace_err("next sync at fpos %#x", lseek(ts_fd, 0, SEEK_CUR));
	} else {
		goto read_again;
	}

	pid = GET_PID(p_buf);
	if (pid_table[pid] == 0) {
		trace_info("pid %d seen", pid);
	}
	pid_table[pid]++;
	if (pid != filter_pid) {
		goto read_again;
	}

	return 1;
}

int fake_hfpga_writen(unsigned char *p_buf, unsigned int len, void *p_param)
{
	unsigned short pid = GET_PID(p_buf);

	trace_info("writen(), len %d, pid %d(%#x)", len, pid, pid);
	write(ts_out_fd, p_buf, len);

	return 1;
}

int fake_hfpga_ioctl(unsigned int cmd, void *p_param)
{
	trace_info("ioctl(), cmd %d", cmd);

	return 0;
}

