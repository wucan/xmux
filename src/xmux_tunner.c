#include "xmux_tunner.h"
#include "ctrl_mcu.h"


static uint8_t send_buf[256];

int tunner_get_param(int id, struct tunner_param *param)
{
	int len, rc;

	memset(param, 0, sizeof(*param));
	len = fp_build_cmd(send_buf, true, CTRL_MCU_CMD_TUNNER1_PARAM + id,
			param, sizeof(*param));
	rc = ctrl_mcu_send_and_recv_cmd(send_buf, len,
			param, CTRL_MCU_CMD_TUNNER1_PARAM + id, sizeof(*param));
	if (rc) {
		return -1;
	}

	return 0;
}
int tunner_set_param(int id, struct tunner_param *param)
{
	int len, rc;

#if 0
	uint8_t work_param[] = {0x74, 01, 01, 00, 0x0f, 0x60, 0xe7, 0xbc, 0x00,
		0xe0, 0x9d, 0xa3, 0x01, 0xa0, 0x6c, 0xac, 0x00, 0x01, 0x1, 01, 0x00};

	ctrl_mcu_send_cmd(work_param, sizeof(work_param));
	return 0;
#endif

	len = fp_build_cmd(send_buf, false, CTRL_MCU_CMD_TUNNER1_PARAM + id,
			param, sizeof(*param));
	ctrl_mcu_send_cmd(send_buf, len);

	return 0;
}
int tunner_get_status(int id, struct tunner_status *status)
{
	int len, rc;

	memset(status, 0, sizeof(*status));
	len = fp_build_cmd(send_buf, true, CTRL_MCU_CMD_TUNNER1_STATUS + id,
			status, sizeof(*status));
	rc = ctrl_mcu_send_and_recv_cmd(send_buf, len,
			status, CTRL_MCU_CMD_TUNNER1_STATUS + id, sizeof(*status));
	if (rc) {
		return -1;
	}

	return 0;
}

