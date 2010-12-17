#ifndef _FRONT_PANEL_DATA_CHURING_H_
#define _FRONT_PANEL_DATA_CHURING_H_

#include "front_panel_intstr.h"


struct xmux_net_param;

void buf_2_fp_cmd_header(struct fp_cmd_header * hdr, char *buf);
void fp_cmd_header_2_buf(struct fp_cmd_header * hdr, char *buf);
void buf_2_prog_info(PROG_INFO_T *prog_info, char *buf);
void prog_info_2_buf(PROG_INFO_T *prog_info, char *buf);
void buf_2_out_rate(OUT_RATE_T *out_rate, char *buf);
void pid_2_buf(uint8_t *buf, uint16_t pid);
void xmux_net_2_fp_net(struct xmux_net_param *xmux_net, NET_ETH0_T *fp_net);
void fp_net_2_xmux_net(NET_ETH0_T *fp_net, struct xmux_net_param *xmux_net);
void pid_trans_info_2_prog_info_of_channel(uint8_t chan_idx);
void pid_trans_info_2_prog_info();
void prog_info_2_pid_trans_info_of_channel(uint8_t chan_idx);
void prog_info_2_pid_trans_info();


#endif /* _FRONT_PANEL_DATA_CHURING_H_ */

