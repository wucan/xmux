#include <fcntl.h>

#include "hfpga.h"
#include "gen_dvb_si.h"
#ifndef _UCLINUX_
#include "fake_hfpga.h"
#endif


uv_dvb_io hfpga_dev = {
#ifdef _UCLINUX_
	hfpga_open,
	hfpga_close,
	hfpga_readn,
	hfpga_writen,
	hfpga_ioctl,
	"/dev/XC3S",
#else
	fake_hfpga_open,
	fake_hfpga_close,
	fake_hfpga_readn,
	fake_hfpga_writen,
	fake_hfpga_ioctl,
	"fake_hfpga.ts",
#endif
	O_RDWR,
	-1,
	0x00						//cha number
};

