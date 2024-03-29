#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>

#include "fpga_api.h"

#define DEBUG_PRT

#ifdef DEBUG_PRT
#ifndef dbg_prt
#define dbg_prt printf
#endif
#else

#ifndef dbg_prt
#define dbg_prt(...)
#endif
#endif

uv_dvb_io dvb_io_dev;

static ACCESS_HFPGA_REGS hregs;
static ACCESS_HFPGA_PID_MAP pid_map;
static ACCESS_HFPGA_STR_DATA str_data;
static ACCESS_HFPGA_CMD hfpga_cmd;

/*
 * user supplied write data recorder function hook
 */
static FPGAWriteHook write_hook;
void fpga_set_write_hook(FPGAWriteHook hook)
{
	write_hook = hook;
}

void dbgoutputHex(unsigned char *data, unsigned int len, unsigned char width)
{
	int i, col;

	dbg_prt("[HFPGA-API] Data Length is: %d \n", len);	

	for(i = 0; i < len;)
	{
		for(col = 0; col < width && i < len; col++)
		{
			dbg_prt("%02x", data[i++]);
		}
		dbg_prt("\n");
	}
}


int hfpga_open(const char *name, int flags)
{
	return open(name, flags);
}

void hfpga_close(int hdev)
{
	close(hdev);
}

int hfpga_pid_map(void* p_pid_map)
{
	int  retval = 0;

	printf("hfpga_pid_map\n");
	hregs.reg = HFPGA_REG_ADDR_SYS_ID; 
	retval = ioctl(dvb_io_dev.hdev, UV_HFPGA_IOCTL_CMD_READ_REGS, &hregs);
	if(0 > retval)
	{
		dbg_prt("[HFPGA-API] ioctl read id fails!, error code is: %d. \n", retval);
		dbg_prt("[HFPGA-API] dvb_io_dev.hdev %02x  errno %02x \n", dvb_io_dev.hdev, errno);
		goto ERR_RETURN;
	}
	dbg_prt("[TEST-HFPGA-RW] ID: %02x \n", hregs.data);

	((ACCESS_HFPGA_PID_MAP *)(&p_pid_map))->mode = 0;
	retval = ioctl(dvb_io_dev.hdev, UV_HFPGA_IOCTL_CMD_WRITE_PID_MAP, &p_pid_map);
	if(0 > retval)
	{
		dbg_prt("[HFPGA-API] dvb_io_dev.hdev %02x  errno %02x \n", dvb_io_dev.hdev, errno);
		goto ERR_RETURN;
	}


SUCCESS_RETURN:
	return 1; 

ERR_RETURN:	
	return 0;
}

// read data of length len
static int _hfpga_readn(unsigned char *p_buf, unsigned int len, void *p_param,int nbgn, int *read_len)
{
	int i, retval = 0;
	unsigned char chno;
	unsigned short i_pid,o_pid;
	uv_io_param *param = (uv_io_param *)p_param;

	if(nbgn)
	{
		hregs.reg = HFPGA_REG_ADDR_SYS_ID; 
		retval = ioctl(dvb_io_dev.hdev, UV_HFPGA_IOCTL_CMD_READ_REGS, &hregs);
		if(0 > retval)
		{
			dbg_prt("[HFPGA-API] ioctl read id fails!, error code is: %d. \n", retval);
			dbg_prt("[HFPGA-API] dvb_io_dev.hdev %02x  errno %02x \n", dvb_io_dev.hdev, errno);
			goto ERR_RETURN;
		}
		dbg_prt("[TEST-HFPGA-RW] ID: %02x \n", hregs.data);

		// set filter
		chno = param->cha;
		pid_map.cha = 0xFF&(1 << chno);
		i_pid = param->pid;
		o_pid = param->pid;
		for(i = 0; i < 32; i++)
		{
			pid_map.pid_map[chno*32+i].in_pid = i_pid;
			pid_map.pid_map[chno*32+i].out_pid = o_pid;
		}
		pid_map.mode = 1;
		retval = ioctl(dvb_io_dev.hdev, UV_HFPGA_IOCTL_CMD_WRITE_PID_MAP, &pid_map);
		//dbg_prt("retval_001=%d\n",retval);
		if(0 > retval)
		{
			dbg_prt("[HFPGA-API] dvb_io_dev.hdev %02x  errno %02x \n", dvb_io_dev.hdev, errno);
			goto ERR_RETURN;
		}
	}


	// get ts
	str_data.len = len;
	str_data.pdata = p_buf;
	str_data.addr = param->cha;
	str_data.nbgn = nbgn;

	//dbg_prt("UV_HFPGA_IOCTL_CMD_READ_STR_DATA_bgn\n");
	retval = ioctl(dvb_io_dev.hdev, UV_HFPGA_IOCTL_CMD_READ_STR_DATA, &str_data);
	//dbg_prt("UV_HFPGA_IOCTL_CMD_READ_STR_DATA_end\n");

	//dbg_prt("retval_002=%d\n",retval);

	if(0 > retval)
	{
		dbg_prt("[HFPGA-API] dvb_io_dev.hdev %02x  errno %02x \n", dvb_io_dev.hdev, errno);
		goto ERR_RETURN;
	}

SUCCESS_RETURN:
	*read_len = str_data.len;
	return 1; 

ERR_RETURN:	
	return 0;
}
#define READAHEAD_TS_PKT_NUM		10
int hfpga_readn(unsigned char *p_buf, unsigned int len, void *p_param,int nbgn)
{
	uv_io_param *param = (uv_io_param *)p_param;
	uint8_t ts_buf[188 * READAHEAD_TS_PKT_NUM];
	static int left_pkt = 0;
	static int left_pkt_tot = 0;
	int read_len = 0;

	/*
	 * caching only for SDT
	 */
	if (param->pid != 0x11)
		return _hfpga_readn(p_buf, len, p_param, nbgn, &read_len);

	/*
	 * caching the ts in the begin
	 */
	if (nbgn) {
		printf("caching for %d pkts begin ...\n", READAHEAD_TS_PKT_NUM);
		if (!_hfpga_readn(ts_buf, 188 * READAHEAD_TS_PKT_NUM, p_param, nbgn, &read_len)) {
			return 0;
		}
		left_pkt = read_len / 188;
		left_pkt_tot = left_pkt;
		printf("begin cached %d packets\n", left_pkt);
	}
	if (left_pkt <= 0) {
		printf("caching for %d pkts again ...\n", READAHEAD_TS_PKT_NUM);
		if (!_hfpga_readn(ts_buf, 188 * READAHEAD_TS_PKT_NUM, p_param, 0, &read_len)) {
			return 0;
		}
		left_pkt = read_len / 188;
		left_pkt_tot = left_pkt;
		printf("cached %d packets\n", left_pkt);
	}
	if (left_pkt > 0) {
		memcpy(p_buf, ts_buf + (left_pkt_tot - left_pkt) * 188, 188);
		left_pkt--;
		hex_dump("sdt pkt", p_buf, 40);
		return 1;
	}

	return 0;
}

int hfpga_writen(unsigned char *p_data, unsigned int len, void *p_param)
{
	int i, retval = 0;

	if (write_hook) {
		write_hook(p_data, len);
	}
	// get ts
	str_data.len = len;
	str_data.pdata = p_data;
	str_data.addr = *(unsigned char *)p_param;

	//dbgoutputHex(str_data.pdata, str_data.len, 16);

	retval = ioctl(dvb_io_dev.hdev, UV_HFPGA_IOCTL_CMD_WRITE_STR_DATA, &str_data);
	if(0 > retval)
	{
		dbg_prt("[HFPGA-API] dvb_io_dev.hdev %02x  errno %02x \n", dvb_io_dev.hdev, errno);
		goto ERR_RETURN;
	}

SUCCESS_RETURN:
	return 1; 

ERR_RETURN:	
	return 0;
}

int hfpga_ioctl(unsigned int cmd, void *p_param)
{
	int i, retval = 0;

	hfpga_cmd.cmd = cmd;
	retval = ioctl(dvb_io_dev.hdev, UV_HFPGA_IOCTL_CMD_CMDS, &hfpga_cmd);
	if(0 > retval)
	{
		dbg_prt("[HFPGA-API] dvb_io_dev.hdev %02x  errno %02x \n", dvb_io_dev.hdev, errno);
		goto ERR_RETURN;
	}
SUCCESS_RETURN:
	return 1; 

ERR_RETURN:	
	return 0;
}
