#include "xmux.h"
#include "xmux_tunner.h"
#include "tuner_device.h"


int tunner_device_get_param(int id, struct tunner_param *param)
{
	/* FIXME: direct get from storage? */
	xmux_config_get_tunner_param(id, param);

	return 0;
}

int tunner_device_set_param(int id, struct tunner_param *param)
{
	/* TODO set param to tunner chip */

	/* save to storage */
	xmux_config_put_tunner_param(id, param);

	return 0;
}

int tunner_device_get_status(int id, struct tunner_status *status)
{
	/* TODO: get status from tunner chip  */

	return 0;
}

