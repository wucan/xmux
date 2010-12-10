#include "up_mib_wrap.h"


extern UPMIB *gd_upmib;

#if HAD_SNMPAGENT
OIDOps * up_mib_wrap_find_oidops (oid_t *oid, int oidlen)
{
	return up_mib_find_oidops(gd_upmib, oid, oidlen);
}
int up_mib_wrap_register_oidops (OIDOps *oidops)
{
	return up_mib_register_oidops(gd_upmib, oidops);
}
int up_mib_wrap_get_node_count ()
{
	return up_mib_get_node_count(gd_upmib);
}
void up_mib_wrap_dump_info ()
{
	up_mib_dump_info(gd_upmib);
}
#endif

const char * fp_dict_get_trans(const char *word)
{
	return word;
}

