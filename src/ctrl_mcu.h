#ifndef _CTRL_MCU_H_
#define _CTRL_MCU_H_

#include "wu/wu_base_type.h"


enum {
	CTRL_MCU_CMD_TUNNER1_PARAM = 0x100,
	CTRL_MCU_CMD_TUNNER2_PARAM = 0x101,
	CTRL_MCU_CMD_TUNNER1_STATUS = 0x102,
	CTRL_MCU_CMD_TUNNER2_STATUS = 0x103,
	CTRL_MCU_CMD_BCM3033 = 0x104,
};

int ctrl_mcu_open();
int ctrl_mcu_close();

int ctrl_mcu_send_cmd(uint8_t *send_data, int send_data_len);
int ctrl_mcu_send_and_recv_cmd(uint8_t *send_data, int send_data_len,
		uint8_t *recv_param_buf, uint16_t expect_cmd, int expect_param_len);


#endif /* _CTRL_MCU_H_ */

