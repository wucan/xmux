#include <stdlib.h>
#include <unistd.h>

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
#include "tuner_device.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "main"};

uint8_t management_mode = MANAGEMENT_MODE_SNMP;
struct xmux_param_management_info g_param_mng_info;

static void restore_work_field();
static void parse_opt(int argc, char **argv);

int main(int argc, char **argv)
{
	trace_info("xmux %s, build %s, %s", XMUX_VERSION_STR, __DATE__, __TIME__);
	trace_info("support %d channels, %d programs",
		CHANNEL_MAX_NUM, PROGRAM_MAX_NUM);

	parse_opt(argc, argv);

	mem_init_size(1024 * 1024 * 3);

	xmux_config_init();
	xmux_config_load_from_eeprom();

	xmux_net_restore();

#if CHANNEL_MAX_NUM == 1
	xmux_ci_info_update(&g_eeprom_param.misc.ci_info);
	xmux_ci_apply();
	libci_test();
#endif

	/*
	 * prepare the correct data for fp
	 */
	enter_fp_management_mode();
	leave_fp_management_mode();

	ctrl_mcu_open();
#if CHANNEL_MAX_NUM == 1
	tuner_device_open();
	tunner_device_init();
#endif

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
		pid_trans_info_save_check();
#if CHANNEL_MAX_NUM == 1
		check_and_save_input_pmt_sec();
		tunner_device_check();
#endif
		sleep(1);
	}

	wu_snmp_agent_fini();
	psi_worker_close();
	front_panel_close();
	ctrl_mcu_close();
#if CHANNEL_MAX_NUM == 1
	tuner_device_close();
#endif

	exit(EXIT_SUCCESS);
}

static void restore_work_field()
{
#if CHANNEL_MAX_NUM == 1
	/* switch to the correct input source */
	select_input_source_1ch();

	tunner_device_do_set_param(0, &g_eeprom_param.tunner[0]);
#endif
	/* download psi and pid map table */
	pid_map_table_apply(&g_eeprom_param.pid_map_table_area.pid_map_table);
	psi_apply_from_output_psi();
	/* set output bitrate and packet length */
	hfpga_write_sys_output_bitrate(g_eeprom_param.sys.output_bitrate);
	hfpga_write_sys_packet_length(g_eeprom_param.sys.format);
}

static void parse_opt(int argc, char **argv)
{
	int c;

	if (argc <= 1)
		return;

	while ((c = getopt(argc, argv, "?htvd:")) != EOF) {
		switch (c) {
		case 'v':
			trace_info("version %s", XMUX_VERSION_STR);
			exit(0);
			break;
		case 'd':
		{
			uint32_t off = strtol(optarg, NULL, 16);
			xmux_config_init();
			xmux_config_dump(off, 64);
			exit(0);
		}
			break;
		case 'h':
		case '?':
			trace_info("xmux support %d channels, %d programs",
				CHANNEL_MAX_NUM, PROGRAM_MAX_NUM);
			exit(0);
			break;
		case 't':
			xmux_config_init();
			xmux_config_write_test_data();
			exit(0);
			break;
		default:
			trace_warn("unsupport option '%c'", c);
			break;
		}
	}

	if (optind < argc) {
		// last non option
	}
}

