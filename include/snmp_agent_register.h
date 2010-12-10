#ifndef _SNMP_AGENT_REGISTER_H_
#define _SNMP_AGENT_REGISTER_H_

#include "up_mib.h"


int snmp_agent_register_oid (oid_t *oid, int oidlen, int type, int mode);


#endif /* _SNMP_AGENT_REGISTER_H_ */

