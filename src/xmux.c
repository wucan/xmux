#include <fcntl.h>

#include "xmux_config.h"
#include "hfpga.h"
#include "gen_dvb_si_api.h"
#ifndef _UCLINUX_
#include "fake_hfpga.h"
#endif


#if CHANNEL_MAX_NUM == 1
void select_input_source_1ch()
{
	/* fix incorrect value */
	if (g_eeprom_param.misc.sel_src > 1)
		g_eeprom_param.misc.sel_src = 0;

	if (g_eeprom_param.misc.sel_src) {
		// switch to ASI source when startup
		hfpga_write_select_channel(0);
	} else {
		hfpga_write_select_channel(2);
	}
}
#ifdef _UCLINUX_
static int
hfpga_readn_1ch(unsigned char *p_buf, unsigned int len, void *p_param,int nbgn)
{
	int rc;
	uv_io_param *param = (uv_io_param *)p_param;
	int old_chan = param->cha;

	if (g_eeprom_param.misc.sel_src) {
		// ASI
		param->cha = 0;
	} else {
		// RF
		param->cha = 2;
	}

	rc = hfpga_readn(p_buf, len, param, nbgn);
	param->cha = old_chan;

	return rc;
}
#endif
#endif

uv_dvb_io hfpga_dev = {
#ifdef _UCLINUX_
	hfpga_open,
	hfpga_close,
#if CHANNEL_MAX_NUM == 1
	hfpga_readn_1ch,
#else
	hfpga_readn,
#endif
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

