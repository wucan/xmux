#include "up_mib.h"
#include "wu_snmp_agent.h"


extern UPMIB *gd_upmib;

/*
 * we should register the SNMP and root none leaf node early, so
 * the later oidops will accererated.
 */
static OIDOps root_oidops[] = {
	{
		.label = "iso",
		.oidlen = 1,
		.oid = {1},
	},
	{
		.label = "org",
		.oidlen = 2,
		.oid = {1, 3},
	},
	{
		.label = "dod",
		.oidlen = 3,
		.oid = {1, 3, 6},
	},
	{
		.label = "internet",
		.oidlen = 4,
		.oid = {1, 3, 6, 1},
	},
	{
		.label = "private",
		.oidlen = 5,
		.oid = {1, 3, 6, 1, 4},
	},
	{
		.label = "enterprises",
		.oidlen = 6,
		.oid = {1, 3, 6, 1, 4, 1},
	},
	{
		.label = "canus",
		.oidlen = 7,
		.oid = {VENDOR_OID},
	},
	{
		.label = "DVB",
		.oidlen = 8,
		.oid = {PRODUCT_OID},
	},
	{
		.label = "XMUX",
		.oidlen = 9,
		.oid = {XMUX_ROOT_OID},
	},
};
int root_none_leaf_oidops_register ()
{
	int i;

	for (i = 0; i < ARRAY_CNT(root_oidops); i++) {
		up_mib_wrap_register_oidops(&root_oidops[i]);
	}
}

