#ifndef _FRONT_PANEL_DATA_CHURING_H_
#define _FRONT_PANEL_DATA_CHURING_H_

#include "front_panel_intstr.h"


void buf_2_fp_cmd_header(struct fp_cmd_header * hdr, char *buf);
void fp_cmd_header_2_buf(struct fp_cmd_header * hdr, char *buf);
void buf_2_prog_info(PROG_INFO_T *prog_info, char *buf);
void prog_info_2_buf(PROG_INFO_T *prog_info, char *buf);
void buf_2_out_rate(OUT_RATE_T *out_rate, char *buf);
void pid_2_buf(uint8_t *buf, uint16_t pid);


#endif /* _FRONT_PANEL_DATA_CHURING_H_ */

