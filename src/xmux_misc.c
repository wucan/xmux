#include "wu/message.h"
#include "wu/wu_csc.h"

#include "xmux.h"
#include "xmux_config.h"
#include "xmux_misc.h"
#include "xmux_snmp.h"
#include "pid_trans_info.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "misc"};

void set_output_bitrate(uint32_t new_bitrate)
{
	hfpga_write_sys_output_bitrate(new_bitrate);

	xmux_config_save_output_bitrate(new_bitrate);
}

void set_packet_format(uint8_t new_fmt)
{
	if (new_fmt != FORMAT_BC && new_fmt != FORMAT_CC)
		return;

	hfpga_write_sys_packet_length(new_fmt);

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

	eeprom_write(EEPROM_OFF_SYS, sys, sizeof(*sys));
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
	if (user->user_len <= 0 || user->user_len > 16)
		return false;

	return true;
}

void xmux_user_param_init_default(struct xmux_user_param *user)
{
	user->user_len = snprintf(user->user, 16, "%s", "root");
	user->password_len = snprintf(user->password, 16, "%s", "root");
	user->csc = wu_csc(user, offsetof(struct xmux_user_param, csc));

	eeprom_write(EEPROM_OFF_USER, user, sizeof(*user));
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

void leave_fp_management_mode()
{
	uint8_t chan_idx;

	sg_mib_heartDevice.flag = SNMP_LOGIN_STATUS_IDLE;
	if (g_param_mng_info.eeprom_pid_trans_info_version !=
		g_param_mng_info.snmp_pid_trans_info_version) {
		g_param_mng_info.snmp_pid_trans_info_version =
			g_param_mng_info.eeprom_pid_trans_info_version;
		for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
			memcpy(&sg_mib_pid_trans_info[chan_idx],
				&g_eeprom_param.pid_trans_info_area.table[chan_idx].data,
				sizeof(sg_mib_pid_trans_info[0]));
		}
	}
	if (g_param_mng_info.eeprom_pid_map_table_version !=
		g_param_mng_info.snmp_pid_map_table_version) {
		g_param_mng_info.snmp_pid_map_table_version =
			g_param_mng_info.eeprom_pid_map_table_version;
		memcpy(&sg_mib_trans.table, &g_eeprom_param.pid_map_table_area,
			sizeof(g_eeprom_param.pid_map_table_area.pid_map_table));
	}

	management_mode = MANAGEMENT_MODE_SNMP;
	xmux_config_save_management_mode();
}

void enter_fp_management_mode()
{
	sg_mib_heartDevice.flag = SNMP_LOGIN_STATUS_BUSY;
	pid_trans_info_2_prog_info();
	fp_build_program_input_pid_table();

	management_mode = MANAGEMENT_MODE_FP;
	xmux_config_save_management_mode();
}

void xmux_program_info_dump(struct xmux_program_info *prog, const char *which)
{
	int i;

	trace_info("%s program information(prog_num %#x):", which, prog->prog_num);
	trace_info("  pmt(%#x -> %#x), pcr(%#x -> %#x)",
		prog->pmt.in, prog->pmt.out, prog->pcr.in, prog->pcr.out);
	for (i = 0; i < PROGRAM_DATA_PID_MAX_NUM; i++) {
		if (prog->data[i].in == DATA_PID_PAD_VALUE)
			continue;
		trace_info("  data #%d(%#x -> %#x)", i, prog->data[i].in, prog->data[i].out);
	}
}

