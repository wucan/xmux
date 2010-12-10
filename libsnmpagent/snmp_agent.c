#include <signal.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#define NET_SNMP_HEAD

#include "wu/message.h"

#include "snmp_agent.h"
#include "snmp_agent_register.h"
#include "up_mib.h"


extern const char *agent_inst_name;

static msgobj mo;
int agentx_subagent = 0;  /* Change this if you're a master agent. */
static int restart;

static void handle_sigpipe (int signo)
{
	restart = 1;
}
int snmp_agent_init ()
{
	trace_info("SNMP agent init ...");
	/* If we're an AgentX subagent... */
	if (agentx_subagent) {
		/* make us an AgentX client. */
		netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
				NETSNMP_DS_AGENT_ROLE, 1);
	}
	init_agent(agent_inst_name);
	/* Now could initialize MIB node */
	trace_info("now you could register mib node to me.");

	return 0;
}
void snmp_agent_startup ()
{
	int rc;

	trace_info("SNMP agent startup ...");
	/*  agent_inst_name will be used to read agent_inst_name.conf files. */
	init_snmp(agent_inst_name);
	init_traps();

	/* If we're going to be a SNMP master agent...  */
	if (!agentx_subagent)
		init_master_agent();  /* Listen on default port (161). */

	/* Your main loop here... */
	while (!restart) {
		/* if you use select(), see snmp_api(3) */
		rc = agent_check_and_process(1); /* 0 == don't block */
	}
	snmp_shutdown(agent_inst_name);
	restart = 0;
}
/*
 *
 */
int snmp_agent_main ()
{
	msgobj_init(&mo, MSG_ERR, 1, "snmp_agent");
	snmp_agent_register_init();
	snmp_agent_init();

	return 0;
}
/*
 * set module debug level
 */
int SNMPAgentDebugLevel_set (OIDOps *oidops, ValueUnion *vu)
{
	mo.verbose = *vu->integer;
	snmp_agent_register_set_debug_level(mo.verbose);

	return 0;
}
/*
 * get module debug level
 */
int SNMPAgentDebugLevel_get (OIDOps *oidops, ValueUnion *vu)
{
	vu->integer = &mo.verbose;

	return 0;
}

