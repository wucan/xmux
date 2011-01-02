#ifndef _XMUX_CONFIG_H_
#define _XMUX_CONFIG_H_

#include "xmux.h"


/*
 * global variable
 */
extern struct xmux_eeprom_param g_eeprom_param;

int xmux_config_init();
void xmux_config_load_from_eeprom();

void xmux_config_save_output_bitrate(uint32_t bitrate);
void xmux_config_save_packet_format(uint8_t fmt);

void xmux_config_save_management_mode();
void xmux_config_save_pid_trans_info_channel(uint8_t chan_idx,
	struct pid_trans_info_snmp_data *chan_info);
void xmux_config_save_pid_trans_info_all();
void xmux_config_save_output_psi_data();
void xmux_config_save_pid_map_table(struct xmux_pid_map_table *t);
void xmux_config_save_net_param(struct xmux_net_param *net);
void xmux_config_save_mux_program_info(struct xmux_mux_program_info *info);


#endif /* _XMUX_CONFIG_H_ */

