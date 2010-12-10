#include <stdlib.h>

#include "wu/mem.h"
#include "front_panel.h"
#include "psi_worker.h"
#include "xmux_config.h"
#include "wu_snmp_agent.h"
#include "xmux_snmp.h"


#define XMUX_VERSION_STR			"0.01"


int management_mode = MANAGEMENT_MODE_SNMP;

static void restore_work_field();

int main(int argc, char **argv)
{
	printf("xmux %s\n", XMUX_VERSION_STR);

	mem_init_size(1024 * 1024 * 3);

	xmux_config_init();
	xmux_config_load_from_eeprom();

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

	while (1) {
		sleep(1);
	}

	wu_snmp_agent_fini();
	psi_worker_close();
	front_panel_close();

	exit(EXIT_SUCCESS);
}

static void restore_work_field()
{
	/* TODO */
}

