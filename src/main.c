#include <stdlib.h>

#include "wu/message.h"
#include "wu/mem.h"
#include "front_panel.h"
#include "psi_worker.h"
#include "xmux_config.h"
#include "wu_snmp_agent.h"
#include "xmux_snmp.h"
#include "xmux_net.h"
#include "xmux_misc.h"
#include "psi_gen.h"
#include "pid_map_table.h"
#include "ctrl_mcu.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "main"};

uint8_t management_mode = MANAGEMENT_MODE_SNMP;
struct xmux_param_management_info g_param_mng_info;

static void restore_work_field();

int main(int argc, char **argv)
{
	trace_info("xmux %s", XMUX_VERSION_STR);
	trace_info("support %d channels", CHANNEL_MAX_NUM);

	mem_init_size(1024 * 1024 * 3);

	xmux_config_init();
	xmux_config_load_from_eeprom();

	xmux_net_restore();

	/*
	 * prepare the correct data for fp
	 */
	enter_fp_management_mode();
	leave_fp_management_mode();

	ctrl_mcu_open();

	/*
	 * restore all system to work again!
	 */
	restore_work_field();

	front_panel_open();
	psi_worker_open();

	psi_worker_run();

	wu_snmp_agent_init();
	xmux_snmp_register_all_oids();
	wu_snmp_agent_run();

	/*
	 * after all module startup, then start UI interface
	 */
	front_panel_run();

#ifndef _UCLINUX_
	sim_fp_run();
#endif

	while (1) {
		xmux_snmp_check_connection();
		sleep(1);
	}

	wu_snmp_agent_fini();
	psi_worker_close();
	front_panel_close();
	ctrl_mcu_close();

	exit(EXIT_SUCCESS);
}

static void restore_work_field()
{
	/* download psi and pid map table */
	pid_map_table_apply(&g_eeprom_param.pid_map_table_area.pid_map_table);
	psi_apply_from_output_psi();
	/* set output bitrate and packet length */
	hfpga_write_sys_output_bitrate(g_eeprom_param.sys.output_bitrate);
	hfpga_write_sys_packet_length(g_eeprom_param.sys.format);
}

