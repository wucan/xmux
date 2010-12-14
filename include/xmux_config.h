#ifndef _XMUX_CONFIG_H_
#define _XMUX_CONFIG_H_

#include "xmux.h"


/*
 * global variable
 */
extern struct xmux_eeprom_param g_eeprom_param;

int xmux_config_init();
void xmux_config_load_from_eeprom();

void xmux_config_update_output_bitrate(uint32_t bitrate);

void xmux_config_save_management_mode();
void xmux_config_load_management_mode();
void xmux_config_save_output_psi_data();


#endif /* _XMUX_CONFIG_H_ */

