#include "xmux_bcm3033.h"
#include "ctrl_mcu.h"


int bcm3033_set_param(struct bcm3033_param *param)
{
	int len, rc;
	uint8_t send_buf[128];

	len = fp_build_cmd(send_buf, false, CTRL_MCU_CMD_BCM3033,
			param, sizeof(*param));
	ctrl_mcu_send_cmd(send_buf, len);

	return 0;
}
int bcm3033_get_param(struct bcm3033_param *param)
{
	int len, rc;
	uint8_t send_buf[128];

	memset(param, 0, sizeof(*param));
	len = fp_build_cmd(send_buf, true, CTRL_MCU_CMD_BCM3033,
			param, sizeof(*param));
	rc = ctrl_mcu_send_and_recv_cmd(send_buf, len,
			param, CTRL_MCU_CMD_BCM3033, sizeof(*param));
	if (rc) {
		return -1;
	}

	return 0;
}

