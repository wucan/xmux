#ifndef _XMUX_CONFIG_H_
#define _XMUX_CONFIG_H_

#include "xmux.h"


/*
 * global variable
 */
extern struct xmux_root_param g_xmux_root_param;

int xmux_config_init();
void xmux_config_load_from_eeprom();

void xmux_config_update_output_bitrate(uint32_t bitrate);


#endif /* _XMUX_CONFIG_H_ */

