#include <arpa/inet.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#define NET_SNMP_HEAD

#include "up_config.h"
#include "wu/message.h"

#include "wu/thread.h"
#include "wu/message_queue.h"
#include "up_mib.h"

#include "snmp_agent.h"
#include "snmp_data.h"
#include "up_mib_wrap.h"
#include "wu_snmp_agent.h"


UPMIB *gd_upmib;
const char *agent_inst_name = "UP";
static Thread *thr_snmp_agent;
static BOOL quit;
static msgobj mo;

int snmp_agent_register_oidops_hack (OIDOps *oidops)
{
	int type, mode, rc, bodylen;
	snmp_data_tree cur;
	
	/*
	 * FIXME/XXX
	 */
#if 0
	/* get type and handler mode from MIB */
	cur = snmp_data_get_node(oidops->oid, oidops->oidlen);
	if (!cur) {
		return -1;
	}
	/* fix the oidops with MIB info */
	snmp_data_fix_oidops(&cur, oidops);

	rc = snmp_data_get_cur_type_and_handler_mode(&cur, &type, &mode);
	if (rc) {
		trace_err("the node type or access mode error!");
		trace_err("type %d, flag 0x%08x", oidops->oidtype, oidops->flag);
		oid_dump(oidops->oid, oidops->oidlen);
		return -1;
	}
#else
	type = oidops->oidtype;
	mode = HANDLER_CAN_RWRITE;
#endif

	rc = snmp_agent_register_oid(oidops->oid, oidops->oidlen, type, mode);
	if (rc) {
		return rc;
	}

	return 0;
}
int process_get_data (oid *o, int olen, int type, unsigned char **pdata, int *datasize)
{
	OIDOps *oidops;
	ValueUnion vu;
	unsigned char *data;
	int datalen;
	int rc;

	oid_dump_mo(o, olen, &mo);
	oidops = up_mib_wrap_find_oidops(o, olen);
	if (!oidops) {
		trace_warn("cannot found oidops!");
		goto error;
	}
	if (oidops->flag & OIDOPS_FLAG_GET_USE_VU) {
		vu = oidops->vu;
	} else {
		if (!oidops->get_func) {
			trace_warn("oidops had no get func!");
			goto error;
		}
		rc = oidops->get_func(oidops, &vu);
		if (rc) {
			trace_err("oidops %s get error!", oidops->label);
			goto error;
		}
	}

	switch (type) {
		case OID_TYPE_INTEGER:
			trace_dbg("INTEGER");
			data = (unsigned char *)vu.integer;
			trace_dbg("value: %d", *vu.integer);
			datalen = 4;
			break;
		case OID_TYPE_IPADDR:
			trace_dbg("IPADDR");
			data = (unsigned char *)vu.ipaddr;
			trace_dbg("value: %08x", *vu.ipaddr);
			datalen = 4;
			break;
		case OID_TYPE_STRING:
			trace_dbg("STRING");
			data = (unsigned char *)vu.string;
			datalen = strlen(vu.string) + 1;
			if (data) {
				trace_dbg("value: %s", vu.string);
			}
			break;
		case OID_TYPE_OPAQUE:
			trace_dbg("OPAQUE");
			data = (unsigned char *)vu.bin.data;
			datalen = vu.bin.size;
			break;
		case OID_TYPE_UNSIGNED32:
			trace_dbg("UNSIGNED32");
			data = (unsigned char *)vu.unsigned32;
			trace_dbg("value: %d", *vu.unsigned32);
			datalen = 4;
			break;
		case OID_TYPE_TIMETICKS:
			trace_dbg("TIMETICKS");
			data = (unsigned char *)vu.unsigned32;
			trace_dbg("value: %d", *vu.unsigned32);
			datalen = 4;
			break;
		default:
			return -1;
			break;
	}
	*pdata = data;
	*datasize = datalen;

	return 0;
error:
	trace_warn("process get error!");
	oid_dump(o, olen);

	return -1;
}
int process_set_data (oid *o, int olen, int type, unsigned char *pdata, int datasize)
{
	OIDOps *oidops;
	ValueUnion vu;
	int rc;

	oid_dump_mo(o, olen, &mo);
	oidops = up_mib_wrap_find_oidops(o, olen);
	if (!oidops) {
		trace_err("cannot found oidops!");
		goto error;
	}
	if (!oidops->set_func) {
		trace_warn("oidops had no set func!");
		goto error;
	}
	switch (type) {
		case OID_TYPE_INTEGER:
			vu.integer = (long *)pdata;
			/* check value from mib */
			rc = snmp_data_check_integer(o,	olen, vu.integer);
			if (rc) {
				trace_err("check integer %d error", *vu.integer);
				goto error;
			}
			break;
		case OID_TYPE_IPADDR:
			vu.ipaddr = (IPAddress *)pdata;
			break;
		case OID_TYPE_STRING:
			vu.string = (char *)pdata;
			vu.string[(int)oidops->data] = 0;
			if (vu.string) {
				trace_dbg("value: %s", vu.string);
			}
			break;
		case OID_TYPE_OPAQUE:
			vu.pfloat = (float *)pdata;
			break;
		case OID_TYPE_UNSIGNED32:
			vu.unsigned32 = (unsigned long *)pdata;
			break;
		case OID_TYPE_TIMETICKS:
			vu.unsigned32 = (unsigned long *)pdata;
			break;
		default:
			break;
	}
	oidops->set_func(oidops, &vu);
	return 0;

error:
	trace_err("process set error!");
	oid_dump(o, olen);

	return -1;
}
static int sa_thread (void *data)
{
	snmp_agent_startup();

	return 0;
}
/*
 * init the SNMP system, must be called before other module 
 *
 */
BOOL SNMPAgentInit (const char *inst_name)
{
	int rc;
	oid_t root_oid[] = {XMUX_ROOT_OID};

	agent_inst_name = inst_name;

	msgobj_init(&mo, MSG_WARN, 1, "snmp_agent_entry");
	snmp_agent_main(agent_inst_name);

	snmp_data_load_mib();
	trace_dbg("now snmp_agent_init...");
	/* create the gloable upmib */
	trace_dbg("now load UPMIB");
	gd_upmib = up_mib_new("UPMIB", snmp_agent_register_oidops_hack);
	root_none_leaf_oidops_register();
	up_mib_set_root_oid(gd_upmib, root_oid, ARRAY_CNT(root_oid));
	snmp_data_load_mib_fix();
	trace_dbg("Agent Inited.");

	return TRUE;
}
/*
 * startup and run the agent thread
 *
 */
BOOL SNMPAgentStartup ()
{
	trace_info("SNMP agent startup now ...");
	oid_mib_dump_info();
	/* create the snmp agent thread */
	thread_create(sa_thread, NULL);
	
	return TRUE;
}

/*
 * shutdown the agent
 */
void SNMPAgentShutdown ()
{
	trace_info("SNMP agent shutdown ...");
	/* request quit the agent thread */
	quit = TRUE;
	snmpd_free_trapsinks();
	snmp_shutdown(agent_inst_name);
}
static oid vendor_oid[] = {XMUX_ROOT_OID};
/*
 * sned trap
 */
void SNMPAgentSendTrap (oid_t *o, int oidlen,
	int type, unsigned char *pvalue, int value_len)
{
	netsnmp_variable_list *vars = NULL;

	switch (type) {
		case OID_TYPE_STRING:
			type = ASN_OCTET_STR;
			break;
		case OID_TYPE_INTEGER:
		case OID_TYPE_IPADDR:
		case OID_TYPE_TIMETICKS:
		case OID_TYPE_UNSIGNED32:
			type = ASN_INTEGER;
			break;
		default:
			trace_warn("send trap: the type %d not mapped!\n",
				type);
			break;
	}

	/* alloc a var */
	vars = SNMP_MALLOC_TYPEDEF(netsnmp_variable_list);
	snmp_set_var_objid(vars, (oid *)o, oidlen);
	vars->type = type;
	snmp_set_var_value(vars, pvalue, value_len);
	/* or use one line to create it */
	//snmp_varlist_add_variable(&vars, o, oidlen, type, pvalue, valuelen);
	
	/* send it */
	send_enterprise_trap_vars(6, 1, vendor_oid, ARRAY_CNT(vendor_oid), vars);

	SNMP_FREE(vars);
}
BOOL SNMPAgentInitTrap (IPAddress ip)
{
	int rc;
	struct in_addr ia;
	char *trapipstr;

	/* first, free all setted trap session */
	snmpd_free_trapsinks();

	ia.s_addr = ip;
	trapipstr = inet_ntoa(ia);
	trace_dbg("trap server ip: %s", trapipstr);
	rc = create_trap_session(trapipstr, 162, "public", 
			SNMP_VERSION_1, SNMP_MSG_TRAP);
	if (!rc) {
		trace_err("create V1 trap session error rc is %d", rc);
		return FALSE;
	}

	return TRUE;
}
/*
 * set module debug level
 */
int SNMPDebugLevel_set (OIDOps *oidops, ValueUnion *vu)
{
	mo.verbose = *vu->integer;
	snmp_data_set_debug_level(mo.verbose);

	return 0;
}
/*
 * get module debug level
 */
int SNMPDebugLevel_get (OIDOps *oidops, ValueUnion *vu)
{
	vu->integer = &mo.verbose;

	return 0;
}

