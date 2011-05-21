#include "wu/wu_csc.h"

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

	param->csc = wu_csc(param, sizeof(*param) - 1);
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

