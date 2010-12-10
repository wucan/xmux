#ifndef _UP_MIB_WRAP_H_
#define _UP_MIB_WRAP_H_


#include "up_mib.h"

#if HAD_SNMPAGENT
int up_mib_wrap_register_oidops (OIDOps *oidops);
OIDOps * up_mib_wrap_find_oidops (oid_t *oid, int oidlen);
int up_mib_wrap_get_node_count ();
void up_mib_wrap_dump_info ();
#else
static int up_mib_wrap_register_oidops (OIDOps *oidops) {return 0;}
static OIDOps * up_mib_wrap_find_oidops (oid_t *oid, int oidlen) { }
static int up_mib_wrap_get_node_count () {return 0;}
static void up_mib_wrap_dump_info () { }
#endif


#endif /* _UP_MIB_WRAP_H_ */

