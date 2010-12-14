#include <string.h>

#include "wu/message.h"
#include "wu/wu_csc.h"

#include "xmux.h"
#include "eeprom.h"
#include "pid_trans_info.h"
#include "xmux_snmp.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "xmux_config"};

struct xmux_eeprom_param g_eeprom_param;

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
	}

	/*TODO */
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
	xmux_config_load_management_mode();

	eeprom_read(0, (uint8_t *)&g_eeprom_param, sizeof(g_eeprom_param));
	/* checkint */
	if (!xmux_eeprom_param_validate(&g_eeprom_param)) {
		trace_err("invalidate xmux root param load from eeprom, fallback to default!");
		xmux_eeprom_param_init_default(&g_eeprom_param);
	} else {
		trace_info("xmux root param success load from eeprom");
	}

	/*
	 * copy to system runtime data
	 */
	memcpy(&sg_mib_pid_trans_info, g_eeprom_param.pid_trans_info_area.bytes,
		sizeof(g_eeprom_param.pid_trans_info_area.pid_trans_info));
	memcpy(&sg_mib_trans.table, g_eeprom_param.pid_map_table_area.bytes,
		sizeof(g_eeprom_param.pid_map_table_area.pid_map_table));
}

void xmux_config_update_output_bitrate(uint32_t bitrate)
{
	g_eeprom_param.sys.output_bitrate = bitrate;
}

void xmux_config_save_management_mode()
{
	uint8_t byte;
	switch (management_mode) {
		case MANAGEMENT_MODE_SNMP: byte = 0x55; break;
		case MANAGEMENT_MODE_FP: byte = 0xAA; break;
		default: return; break;
	}
	eeprom_write(EEPROM_OFF_MNG_MODE, &byte, 1);
}

void xmux_config_load_management_mode()
{
	uint8_t byte;

	eeprom_read(EEPROM_OFF_MNG_MODE, &byte, 1);
	switch (byte) {
		default:
		case 0x55: management_mode = MANAGEMENT_MODE_SNMP; break;
		case 0xAA: management_mode = MANAGEMENT_MODE_FP; break;
	}
}

void xmux_config_save_output_psi_data()
{
	eeprom_write(EEPROM_OFF_OUTPUT_PSI, &g_eeprom_param.output_psi_area,
		sizeof(g_eeprom_param.output_psi_area));
}

