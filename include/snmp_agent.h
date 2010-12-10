#ifndef _SNMP_AGENT_H_
#define _SNMP_AGENT_H_

#include "up_type.h"
#include "up_mib.h"


BOOL SNMPAgentInit (const char *inst_name);
BOOL SNMPAgentStartup ();
void SNMPAgentShutdown ();
void SNMPAgentSendTrap (oid_t *oid, int oidlen,
	int type, unsigned char *pvalue, int value_len);
BOOL SNMPAgentInitTrap (IPAddress ip);


#endif /* _SNMP_AGENT_H_ */

