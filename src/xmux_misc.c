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
	/* TODO: set */

	xmux_config_save_packet_format(new_fmt);
}

