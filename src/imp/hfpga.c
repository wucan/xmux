#include <fcntl.h>

#include "wu/wu_base_type.h"
#include "wu/message.h"

#include "xmux.h"
#include "fpga_api.h"


#define UV_HFPGA_DEV_PATH				"/dev/XC3S"

static msgobj mo = {MSG_INFO, ENCOLOR, "hfpga"};


#ifdef _UCLINUX_
///////////////////////
//get the command read or write flag: 1 read ; 0 write
//------------------
///////////////////////

static int __get_ts_status(int hdev, uint16_t *ts_status)
{
	ACCESS_HFPGA_REGS hregs;
	int retval;

	trace_info("get ts status ...");
	*ts_status = 0;

	hregs.reg = HFPGA_REG_ADDR_SYS_ID;
	retval = ioctl(hdev, UV_HFPGA_IOCTL_CMD_READ_REGS, &hregs);
	if (0 > retval) {
		trace_err("read SYS_ID failed! rc %d!", retval);
		return -1;
	}
	trace_info("read HFPGA ID %#x", hregs.data);

	hregs.reg = HFPGA_REG_ADDR_SYS_VER;
	retval = ioctl(hdev, UV_HFPGA_IOCTL_CMD_READ_REGS, &hregs);
	if (0 > retval) {
		trace_err("read SYS_VER failed! rc %d!", retval);
		return -1;
	}
	trace_info("read HFPGA Version %#x", hregs.data);

	hregs.reg = HFPGA_REG_ADDR_TS_STATUS;
	retval = ioctl(hdev, UV_HFPGA_IOCTL_CMD_READ_REGS, &hregs);
	if (0 > retval) {
		trace_err("read TS_STATUS failed! rc %d!", retval);
		return -1;
	}
	trace_info("read TS Status %#x", hregs.data);
	*ts_status = hregs.data;

	return 0;
}

int hfpga_get_ts_status(int chan_idx, uint16_t *ts_status_para)
{
	uint16_t ts_status;
	int retval = 0;
	int hdev = 0;

	hdev = open(UV_HFPGA_DEV_PATH, O_RDWR);
	if (-1 == hdev) {
		trace_err("failed to open HFPGA dev, path %s!", UV_HFPGA_DEV_PATH);
		return -1;
	}
	retval = __get_ts_status(hdev, &ts_status);
	close(hdev);
	if (retval == 0) {
		*ts_status_para = ts_status;
		if (ts_status & (0x01 << chan_idx))
			retval = 1;
	}

	return retval;
}

static void prt_pid_map(ACCESS_HFPGA_PID_MAP * p_pid_map)
{
	uint8_t cha = p_pid_map->cha;
	int chn = 0, j = 0;
	int i;

	while (cha > 0) {
		if (0x01 & cha) {
			trace_info("channel %d[%d] pid map table:", chn, p_pid_map->cha);
			for (i = 0; i < 32; i++) {
				if (p_pid_map->pid_map[chn * 32 + i].in_pid == PID_MAP_TABLE_PAD_PID && p_pid_map->pid_map[chn * 32 + i].out_pid == PID_MAP_TABLE_PAD_PID) {
					continue;
				}
				trace_info("%d(%#x) => %d(%#x)",
					p_pid_map->pid_map[chn * 32 + i].in_pid,
					p_pid_map->pid_map[chn * 32 + i].in_pid,
					p_pid_map->pid_map[chn * 32 + i].out_pid,
					p_pid_map->pid_map[chn * 32 + i].out_pid);
			}
		}
		cha >>= 1;
		chn++;
	}
}

static int __write_pids_map(int hdev, ACCESS_HFPGA_PID_MAP *pid_map)
{
	ACCESS_HFPGA_REGS hregs;
	int retval = 0;
	int i = 0;
	uint8_t chno = 0;
	uint16_t i_pid, o_pid;

	trace_info("testing HFPGA ...");
	// 0. Read Regs

	// SYS_ID   0x8200,0000 16  RO  
	// SYS_VERSION  0x8200,0004 16  RO

#if 0
	for (i = 0; i < 1 /*000 */ ; i++) {
		hregs.reg = HFPGA_REG_ADDR_SYS_ID;
		retval = ioctl(hdev, UV_HFPGA_IOCTL_CMD_READ_REGS, &hregs);
		if (0 > retval) {
			trace_err("read SYS_ID failed! rc %d!", retval);
			return -1;
		}
		trace_info("read HFPGA ID %#x", hregs.data);
	}

	hregs.reg = HFPGA_REG_ADDR_SYS_VER;
	retval = ioctl(hdev, UV_HFPGA_IOCTL_CMD_READ_REGS, &hregs);
	if (0 > retval) {
		trace_err("read SYS_VER failed! rc %d!", retval);
		return -1;
	}
	trace_info("read HFPGA Version %#x", hregs.data);
#endif

	trace_info("write pid map table ...");
	retval = ioctl(hdev, UV_HFPGA_IOCTL_CMD_WRITE_PID_MAP, pid_map);
	if (0 > retval) {
		trace_err("write PID_MAP failed! rc %d!", retval);
		return -1;
	}

#if 0
	trace_info("read pid pap table ...");
	for (i = 0; i < 32; i++) {
		pid_map->pid_map[chno * 32 + i].in_pid = 0x0000;
		pid_map->pid_map[chno * 32 + i].out_pid = 0x0000;
	}
	retval = ioctl(hdev, UV_HFPGA_IOCTL_CMD_READ_PID_MAP, pid_map);
	if (0 > retval) {
		trace_err("read PID_MAP failed! rc %d!", retval);
		return -1;
	}
	prt_pid_map(pid_map);
#endif

	return 0;
}

int hfpga_write_pid_map(ACCESS_HFPGA_PID_MAP *pid_map)
{
	int hdev = 0;

	trace_info("write pid map table ...");
	hdev = open(UV_HFPGA_DEV_PATH, O_RDWR);
	if (-1 == hdev) {
		trace_err("failed to open HFPGA dev, path %s!", UV_HFPGA_DEV_PATH);
		return -1;
	}
	__write_pids_map(hdev, pid_map);
	close(hdev);

	return 0;
}
#else
int hfpga_get_ts_status(int chan_idx, uint16_t *ts_status_para)
{
	trace_warn("%s", __func__);
	/*
	 * only support one channel in our !uclinux test!
	 */
	*ts_status_para = 0;
	if (chan_idx != 0) {
		return 0;
	}
	*ts_status_para = (1 << 0);

	return 1;
}
int hfpga_write_pid_map(ACCESS_HFPGA_PID_MAP *pid_map)
{
	trace_warn("%s", __func__);

	return 0;
}
#endif

