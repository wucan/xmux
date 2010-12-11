#include "wu/message.h"

#include "xmux.h"
#include "eeprom.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "xmux_config"};

struct xmux_root_param g_xmux_root_param;

int xmux_config_init()
{
	int rc = eeprom_open();
	if (rc < 0) {
		trace_err("eeprom open failed! rc %d", rc);
		return rc;
	}

	return 0;
}

static bool xmux_root_param_validate(struct xmux_root_param *p)
{
	/*TODO */
	return true;
}

static void xmux_root_param_init_default(struct xmux_root_param *p)
{
	/* TODO */
}

/*
 * if load failed then fallback to default
 */
void xmux_config_load_from_eeprom()
{
	xmux_config_load_management_mode();

	eeprom_read(0, (uint8_t *)&g_xmux_root_param, sizeof(g_xmux_root_param));
	/* checkint */
	if (!xmux_root_param_validate(&g_xmux_root_param)) {
		trace_err("invalidate xmux root param load from eeprom, fallback to default!");
		xmux_root_param_init_default(&g_xmux_root_param);
	} else {
		trace_info("xmux root param success load from eeprom");
	}
}

void xmux_config_update_output_bitrate(uint32_t bitrate)
{
	g_xmux_root_param.sys.output_bitrate = bitrate;
}

void xmux_config_save_management_mode()
{
	uint8_t byte;
	switch (management_mode) {
		case MANAGEMENT_MODE_SNMP: byte = 0x55; break;
		case MANAGEMENT_MODE_FP: byte = 0xAA; break;
		default: return; break;
	}
	eeprom_write(0xFFFF, &byte, 1);
}

void xmux_config_load_management_mode()
{
	uint8_t byte;

	eeprom_read(0xFFFF, &byte, 1);
	switch (byte) {
		default:
		case 0x55: management_mode = MANAGEMENT_MODE_SNMP; break;
		case 0xAA: management_mode = MANAGEMENT_MODE_FP; break;
	}
}

void xmux_config_save_output_psi_data()
{
	int off = offsetof(struct xmux_root_param, output_psi_area);

	eeprom_write(off, &g_xmux_root_param.output_psi_area,
		sizeof(g_xmux_root_param.output_psi_area));
}

