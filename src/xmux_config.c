#include <string.h>

#include "wu/message.h"
#include "wu/wu_csc.h"

#include "xmux.h"
#include "eeprom.h"
#include "pid_trans_info.h"
#include "xmux_snmp.h"
#include "xmux_misc.h"
#include "xmux_net.h"
#include "output_psi_data.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "xmux_config"};

struct xmux_eeprom_param g_eeprom_param;

static void eeprom_rw_test();

int xmux_config_init()
{
	int rc = eeprom_open();
	if (rc < 0) {
		trace_err("eeprom open failed! rc %d", rc);
		return rc;
	}

	return 0;
}

static bool xmux_eeprom_param_validate(struct xmux_eeprom_param *p)
{
	struct pid_trans_info_snmp_data *pid_trans_info;
	uint8_t chan_idx;

	/*
	 * management mode
	 */
	management_mode = p->mng_mode;
	if (management_mode != MANAGEMENT_MODE_SNMP &&
		management_mode != MANAGEMENT_MODE_FP) {
		trace_err("management mode invalidate! force to SNMP!");
		management_mode = MANAGEMENT_MODE_SNMP;
		xmux_config_save_management_mode();
	}

	/*
	 * pid trans info
	 */
	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		pid_trans_info = &p->pid_trans_info_area.pid_trans_info[chan_idx];
		if (!pid_trans_info_validate(pid_trans_info)) {
			trace_err("#%d pid trans info invalidate!", chan_idx);
			memset(pid_trans_info, 0, sizeof(struct pid_trans_info_snmp_data));
			continue;
		}
		pid_trans_info_dump(pid_trans_info);
	}

	/*
	 * pid map table
	 */
	if (pid_map_table_validate(&p->pid_map_table_area.pid_map_table)) {
		pid_map_table_dump(&p->pid_map_table_area.pid_map_table);
	} else {
		trace_err("pid map table invalidate!");
		pid_map_table_clear(&p->pid_map_table_area.pid_map_table);
	}

	/*
	 * output psi
	 */
	if (output_psi_data_validate(&p->output_psi_area.output_psi)) {
		output_psi_data_dump(&p->output_psi_area.output_psi);
	} else {
		trace_err("output psi data invalidate!");
		output_psi_data_clear(&p->output_psi_area.output_psi);
	}

	/*
	 * sys
	 */
	if (xmux_system_param_validate(&p->sys)) {
		xmux_system_param_dump(&p->sys);
	} else {
		trace_err("system param invalidate!");
		xmux_system_param_init_default(&p->sys);
		eeprom_write(EEPROM_OFF_SYS, &p->sys, sizeof(p->sys));
	}

	/*
	 * net
	 */
	if (xmux_net_param_validate(&p->net)) {
		xmux_net_param_dump(&p->net);
	} else {
		trace_err("net param invalidate!");
		xmux_net_param_init_default(&p->net);
		eeprom_write(EEPROM_OFF_NET, &p->net, sizeof(p->net));
	}

	/*
	 * user
	 */
	if (xmux_user_param_validate(&p->user)) {
		xmux_user_param_dump(&p->user);
	} else {
		trace_err("user param invalidate!");
		xmux_user_param_init_default(&p->user);
		eeprom_write(EEPROM_OFF_USER, &p->user, sizeof(p->user));
	}

	return true;
}

static void xmux_eeprom_param_init_default(struct xmux_eeprom_param *p)
{
	/* TODO */
}

/*
 * if load failed then fallback to default
 */
void xmux_config_load_from_eeprom()
{
	eeprom_read(0, (uint8_t *)&g_eeprom_param, sizeof(g_eeprom_param));
	/* checkint */
	if (!xmux_eeprom_param_validate(&g_eeprom_param)) {
		trace_err("invalidate xmux root param load from eeprom, fallback to default!");
		xmux_eeprom_param_init_default(&g_eeprom_param);
	} else {
		trace_info("xmux root param success load from eeprom");
	}
	g_param_mng_info.eeprom_pid_trans_info_version++;
	g_param_mng_info.eeprom_pid_map_table_version++;
}

void xmux_config_save_output_bitrate(uint32_t bitrate)
{
	g_eeprom_param.sys.output_bitrate = bitrate;
	eeprom_write(EEPROM_OFF_SYS_OUTPUT_BITRATE, &bitrate, sizeof(bitrate));
}

void xmux_config_save_packet_format(uint8_t fmt)
{
	g_eeprom_param.sys.format = fmt;
	eeprom_write(EEPROM_OFF_SYS_PACKET_FORMAT, &fmt, sizeof(fmt));
}

void xmux_config_save_management_mode()
{
	g_eeprom_param.mng_mode = management_mode;
	eeprom_write(EEPROM_OFF_MNG_MODE, &g_eeprom_param.mng_mode, 1);
}

void xmux_config_save_pid_trans_info()
{
	uint8_t chan_idx;
	struct pid_trans_info_snmp_data *info;

	trace_info("save pid trans info...");
	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		info = &g_eeprom_param.pid_trans_info_area.pid_trans_info[chan_idx];
		if (!pid_trans_info_validate(info)) {
			trace_err("#%d pid trans info invalidate!", chan_idx);
			return;
		}
		pid_trans_info_dump(info);
	}

	eeprom_write(EEPROM_OFF_PID_TRANS_INFO,
		&g_eeprom_param.pid_trans_info_area.bytes,
		sizeof(g_eeprom_param.pid_trans_info_area.pid_trans_info));
}

void xmux_config_save_output_psi_data()
{
	eeprom_write(EEPROM_OFF_OUTPUT_PSI, &g_eeprom_param.output_psi_area,
		sizeof(g_eeprom_param.output_psi_area));
}

void xmux_config_save_pid_map_table(struct xmux_pid_map_table *t)
{
	trace_info("save pid map table...");
	if (!pid_map_table_validate(t)) {
		trace_err("pid map table invalidate! save nothing!");
		return;
	}
	pid_map_table_dump(t);
	g_eeprom_param.pid_map_table_area.pid_map_table = *t;
	eeprom_write(EEPROM_OFF_PID_MAP_TABLE, t, sizeof(*t));
}

void xmux_config_save_net_param(struct xmux_net_param *net)
{
	net->csc = wu_csc(net, sizeof(*net) - 1);
	eeprom_write(EEPROM_OFF_NET, net, sizeof(*net));
}

static void eeprom_rw_test()
{
	uint8_t data[1024], read_data[1024];
	int i;

	trace_info("eeprom test...");
	for (i = 0; i < 1024; i++)
		data[i] = i;

	eeprom_write(0, data, 1024);
	eeprom_read(0, read_data, 1024);
	for (i = 0; i < 1024; i++) {
		if (read_data[i] != data[i]) {
			trace_err("#%d write %#x, read %#x",
				i, data[i], read_data[i]);
		}
	}
}

