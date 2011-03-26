#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#include "uvdebug_def.h"

#include "host_fpga.h"

//#define UV_HFPGA_DEV_PATH ("/var/XC3S")
#define UV_HFPGA_DEV_PATH ("/dev/XC3S")

static ACCESS_HFPGA_REGS hregs;
static ACCESS_HFPGA_PID_MAP pid_map;
static ACCESS_HFPGA_STR_DATA str_data;
static unsigned char sg_buf[10*1024];

void logdata(unsigned char *data, unsigned int len)
{
	int written;
	FILE *fp = fopen("/var/mydata.ts", "w");
	if(NULL == fp)
	{
		dbg_prt("[TEST-HFPGA-RW] Log data fails, can not write log file: /var/mydata.ts ! \n");
		return;
	}
	
	written = fwrite(data, 1, len, fp);

	if(written == len)
	{
		dbg_prt("[TEST-HFPGA-RW] Log data success \n");
	}
	else
	{
		dbg_prt("[TEST-HFPGA-RW] Log data fails: fwrite returns %d / %d bytes! \n", written, len);
	}

	fclose(fp);
}

void outputHex(unsigned char *data, unsigned int len, unsigned char width)
{
	int i, col;
	
	dbg_prt("[TEST-HFPGA-RW] Data Length is: %d \n", len);	
	for(i = 0; i < len;)
	{
		for(col = 0; col < width && i < len; col++)
		{
			dbg_prt("%02x", data[i++]);
		}
		dbg_prt("\n");
	}
}

// offset to 0x47 and output integral n * 188 bytes
void outputTS(unsigned char *ts, unsigned int len, unsigned char width)
{
	int i, offset;
	int n, rem;
	
	// The condition can be more strictly,
	// ts[i] = 0x47
	// ts[i+1] = 0x00; or ts[i+1] = 0x40;
	// ts[i+2] = 0x00;
	while((0x47 != ts[i]) && i < len) i++;
	
	if(len == i)
	{
		dbg_prt("[TEST-HFPGA-RW] Can not find sync byte 0x47 \n");	
		return;
	}
	
	n = (len - i) / 188;
	rem = (len - i) - 188 * n;
	
	if(0 == n)
	{
		dbg_prt("[TEST-HFPGA-RW] less than 1 byte \n");	
		outputHex(ts+i, rem, width);	
	}
	else
	{
		dbg_prt("[TEST-HFPGA-RW] There are %d TS packes \n", n);	
		outputHex(ts+i, n*188, width);	
	}
}

void outputChaPidMap(ACCESS_HFPGA_PID_MAP *p_pid_map, unsigned char chno)
{
	int i;

	dbg_prt("[TEST-HFPGA-RW] Print Cha(0-7) [%d][%02x] Pid Maps \n", chno, p_pid_map->cha);	

	for(i = 0; i < 32; i++)
	{	
		dbg_prt("%02x_%02x ", p_pid_map->pid_map[chno*32+i].in_pid, p_pid_map->pid_map[chno*32+i].out_pid);
		if(0 == (i+1)%8)
		{
			dbg_prt("\n");
		}
	}
}


// args: chno(0-7), in_pid(0-NULL_PID), out_pid, count, out_width
// test_hfpga 
int main(int argc, char *argv[])
{
	int hdev = 0;
	int retval = 0;
	int i = 0;
	unsigned char chno;
	unsigned short i_pid,o_pid;

	if(argc < 6)
	{
		dbg_prt("[TEST-HFPGA-RW] Too few arguments !!! \n ");
		return -1;
	}

	dbg_prt("[TEST-HFPGA-RW] Begin to Test HFPGA dev ... \n\n ");
	
	hdev = open(UV_HFPGA_DEV_PATH, O_RDWR);
	if(-1 == hdev)
	{
		dbg_prt("[TEST-HFPGA-RW] Cannot open HFPGA dev !!! \n ");
		return -1;	
	}
	
	dbg_prt("[TEST-HFPGA-RW] File %d is opened.\n ", hdev);

	// 0. Read Regs
	
	// SYS_ID	0x8200,0000	16	RO	
  	// SYS_VERSION	0x8200,0004	16	RO

	for(i=0;i<1000;i++)	
	{
		hregs.reg = HFPGA_REG_ADDR_SYS_ID; 
		retval = ioctl(hdev, UV_HFPGA_IOCTL_CMD_READ_REGS, &hregs);
		if(0 > retval)
		{
			dbg_prt("[TEST-HFPGA-RW]  ioctl read id fails!, error code is: %d. \n", retval);
			goto ERR_RETURN;
		}
		dbg_prt("[TEST-HFPGA-RW] ID: %02x \n", hregs.data);
	}

#if 0	
	hregs.reg = HFPGA_REG_ADDR_SYS_VER;
	retval = ioctl(hdev, UV_HFPGA_IOCTL_CMD_READ_REGS, &hregs);
	if(0 > retval)
	{
		dbg_prt("[TEST-HFPGA-RW]  ioctl read version fails!, error code is: %d. \n", retval);
		goto ERR_RETURN;
	}
	dbg_prt("[TEST-HFPGA-RW] Version: %02x \n", hregs.data);
	
	// 1. set filter
	for(i = 0; i < 256; i++)
	{
		pid_map.pid_map[i].in_pid = UV_HFPGA_INVALID_PID_MAP;
		pid_map.pid_map[i].in_pid = UV_HFPGA_INVALID_PID_MAP;
	}
	
	chno = atoi(argv[1]);
	pid_map.cha = 0xFF&(1 << chno);
	i_pid = atoi(argv[2]);
	o_pid = atoi(argv[3]);
	for(i = 0; i < 32; i++)
	{
		pid_map.pid_map[chno*32+i].in_pid = i_pid+i;
		pid_map.pid_map[chno*32+i].out_pid = o_pid+i;
	}
	
	dbg_prt("[TEST-HFPGA-RW] Before write the input Pid Maps is... \n");
        outputChaPidMap(&pid_map, chno);

	retval = ioctl(hdev, UV_HFPGA_IOCTL_CMD_WRITE_PID_MAP, &pid_map);
	if(0 > retval)
	{
		dbg_prt("[TEST-HFPGA-RW]  ioctl write pid map fails!, error code is: %d. \n", retval);
		goto ERR_RETURN;
	}	
/*	
	dbg_prt("[TEST-HFPGA-RW] Read Pid Maps...\n");

	retval = ioctl(hdev, UV_HFPGA_IOCTL_CMD_READ_PID_MAP, &pid_map);
	if(0 > retval)
	{
		dbg_prt("[TEST-HFPGA-RW]  ioctl read pid map fails!, error code is: %d. \n", retval);
		goto ERR_RETURN;
	}
	outputChaPidMap(&pid_map, chno);
*/
	
	// 2. get data from fifo
	str_data.len = atoi(argv[4]);
	str_data.pdata = sg_buf;
	str_data.addr = atoi(argv[1]);
	dbg_prt("[TEST-HFPGA-RW] Start Read FIFO...\n");
  	retval = ioctl(hdev, UV_HFPGA_IOCTL_CMD_READ_STR_DATA, &str_data);
	if(0 > retval)
	{
		dbg_prt("[TEST-HFPGA-RW]  ioctl get data from fifo fails!, error code is: %d. \n", retval);
		goto ERR_RETURN;
	}
	dbg_prt("[TEST-HFPGA-RW] Return to Read FIFO..Data is: \n");
	outputHex(str_data.pdata, str_data.len, atoi(argv[5]));
	
	logdata(str_data.pdata, str_data.len);
#endif

SUCCESS_RETURN:
	close(hdev);
	dbg_prt("[TEST-HFPGA-RW] Success and End to Test HFPGA dev \n\n ");

	return 0;	

ERR_RETURN:	
	close(hdev);
	dbg_prt("[TEST-HFPGA-RW] Failed and End to Test HFPGA dev \n\n ");
	return -1;

}

