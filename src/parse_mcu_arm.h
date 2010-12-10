
#ifndef _parse_mcu_arm_H_
#define _parse_mcu_arm_H_


#include "psi_mcu_panel.h"

//from buf to struct
int mcu_arm_head_buf_to_t(CMD_HEAD_T * pHead, char *pbuf);
//from struct to buf
int mcu_arm_head_t_to_buf(CMD_HEAD_T * pHead, char *pbuf);
//from buf to struct
int mcu_arm_prog_buf_to_t(PROG_INFO_T * pProg, char *pbuf);
//from struct to buf
int mcu_arm_prog_t_to_buf(PROG_INFO_T * pProg, char *pbuf);
//from buf to struct
int mcu_arm_rate_buf_to_t(OUT_RATE_T * pRate, char *pbuf);
//from struct to buf
int mcu_arm_rate_t_to_buf(OUT_RATE_T * pRate, char *pbuf);

#endif
