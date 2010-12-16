#include "xmux.h"
#include "xmux_config.h"
#include "xmux_misc.h"


void set_output_bitrate(uint32_t new_bitrate)
{
	/* TODO: set */

	xmux_config_save_output_bitrate(new_bitrate);
}

void set_packet_format(uint8_t new_fmt)
{
	if (new_fmt != FORMAT_BC && new_fmt != FORMAT_CC)
		return;

	/* TODO: set */

	xmux_config_save_packet_format(new_fmt);
}

bool xmux_system_param_validate(struct xmux_system_param *sys)
{
	if (sys->format != FORMAT_BC && sys->format != FORMAT_CC) {
		return false;
	}

	return true;
}

void xmux_system_param_init_default(struct xmux_system_param *sys)
{
	sys->format = FORMAT_BC;
}

