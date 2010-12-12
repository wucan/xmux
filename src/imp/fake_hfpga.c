#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#include "wu/message.h"
#include "mpegts.h"

#include "fpga_api.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "fake_hfpga"};

static int ts_fd;
static int ts_out_fd;
static unsigned short filter_pid;
static unsigned short pid_table[0x1FFF + 1];

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
	uv_io_param *param = (uv_io_param *)p_param;
	int rc;
	unsigned short pid;

	if (nbgn) {
		filter_pid = param->pid;
		trace_info("readn(), begin filter pid %d", filter_pid);
	}
read_again:
	rc = read(ts_fd, p_buf, len);
	if (rc != len) {
		lseek(ts_fd, 0, SEEK_SET);
		rc = read(ts_fd, p_buf, len);
		if (rc != len) {
			return 0;
		}
	}
	pid = GET_PID(p_buf);
	if (pid_table[pid] == 0) {
		trace_info("pid %d seen", pid);
		pid_table[pid]++;
	}
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

