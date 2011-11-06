#ifndef _XMUX_CONFIG_H_
#define _XMUX_CONFIG_H_

#include "xmux.h"


/*
 * global variable
 */
extern struct xmux_eeprom_param g_eeprom_param;

int xmux_config_init();
void xmux_config_load_from_eeprom();
void xmux_config_dump(uint32_t off, int len);

void xmux_config_save_output_bitrate(uint32_t bitrate);
void xmux_config_save_packet_format(uint8_t fmt);

void xmux_config_save_management_mode();
void xmux_config_save_ci_info(struct ci_info_param *ci_info);
void xmux_config_save_pid_trans_info_all();
void xmux_config_save_output_psi_data();
void xmux_config_save_pid_map_table(struct xmux_pid_map_table *t);
#if CHANNEL_MAX_NUM == 1
void xmux_config_save_input_pmt_section();
void xmux_config_get_tunner_param(int id, struct tunner_param *param);
void xmux_config_put_tunner_param(int id, struct tunner_param *param);
#endif
void xmux_config_save_net_param(struct xmux_net_param *net);
void xmux_config_save_mux_program_info(struct xmux_mux_program_info *info);


#endif /* _XMUX_CONFIG_H_ */

