#include "wu/message.h"
#include "wu/wu_csc.h"

#include "xmux.h"
#include "xmux_config.h"
#include "xmux_misc.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "misc"};

void set_output_bitrate(uint32_t new_bitrate)
{
	/* TODO: set */

	xmux_config_save_output_bitrate(new_bitrate);
}

void set_packet_format(uint8_t new_fmt)
{
	if (new_fmt != FORMAT_BC && new_fmt != FORMAT_CC)
		return;

	/* TODO: set */

	xmux_config_save_packet_format(new_fmt);
}

bool xmux_system_param_validate(struct xmux_system_param *sys)
{
	if (sys->version != XMUX_VERSION_NUM)
		return false;
	 if (sys->update_time != XMUX_UPDATE_TIME)
		return false;
	if (sys->format != FORMAT_BC && sys->format != FORMAT_CC) {
		return false;
	}

	return true;
}

void xmux_system_param_init_default(struct xmux_system_param *sys)
{
	sys->output_bitrate = XMUX_DEFAULT_OUTPUT_BITRATE;
	sys->format = FORMAT_BC;
	snprintf(sys->vendor_info, sizeof(sys->vendor_info), "%s", XMUX_VENDOR);
	sys->version = XMUX_VERSION_NUM;
	sys->update_time = XMUX_UPDATE_TIME;
}

void xmux_system_param_dump(struct xmux_system_param *sys)
{
	trace_info("system: %dKbps %#x %s %d.%d %d-%d-%d",
		sys->output_bitrate, sys->format, sys->vendor_info,
		sys->version >> 16, sys->version & 0xFFFF,
		sys->update_time >> 16, (sys->update_time >> 8) & 0xFF,
		sys->update_time & 0xFF);
}

bool xmux_user_param_validate(struct xmux_user_param *user)
{
	uint8_t csc;

	csc = wu_csc(user, offsetof(struct xmux_user_param, csc));
	if (user->csc != csc)
		return false;

	return true;
}

void xmux_user_param_init_default(struct xmux_user_param *user)
{
	user->user_len = snprintf(user->user, 16, "%s", XMUX_VENDOR);
	user->password_len = snprintf(user->password, 16, "%s", XMUX_VENDOR);
	user->csc = wu_csc(user, offsetof(struct xmux_user_param, csc));
}

void xmux_user_param_dump(struct xmux_user_param *user)
{
	char user_name[17], password[17];
	uint8_t user_len = MIN(user->user_len, 16);
	uint8_t password_len = MIN(user->password_len, 16);

	memcpy(user_name, user->user, user_len);
	user_name[user_len] = 0;
	memcpy(password, user->password, password_len);
	password[password_len] = 0;
	trace_info("user: %s:%s", user_name, password);
}

