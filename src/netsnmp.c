#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "wu/wu_base_type.h"

#include "wu_snmp_agent.h"

#include "xmux.h"
#include "xmux_snmp.h"


int netsnmp_agent_init()
{
	init_agent("xmux");

	return 0;
}

void netsnmp_agent_loop(void *data)
{
	bool quit_flag = (bool)data;

	init_snmp("xmux");
	/* If we're going to be a SNMP master agent...  */
	init_master_agent();  /* Listen on default port (161). */

	/* Your main loop here... */
	while (!quit_flag) {
		/* if you use select(), see snmp_api(3) */
		int rc = agent_check_and_process(1); /* 0 == don't block */
	}
	snmp_shutdown("xmux");
}

#ifdef _UCLINUX_
static int netsnmp_check_vb_type_and_max_size(netsnmp_variable_list *var,
                               int type, size_t max_size)
{
	register int rc = SNMP_ERR_NOERROR;

	if (var ==NULL)
		return SNMP_ERR_GENERR;
	if (var->type != type) {
		rc = SNMP_ERR_WRONGTYPE;
	} else if (var->val_len > max_size) {
		rc = SNMP_ERR_WRONGLENGTH;
	}

	return rc;
}
#endif

static int netsnmp_oid_handler(netsnmp_mib_handler *handler,
						  netsnmp_handler_registration *reginfo,
						  netsnmp_agent_request_info *reqinfo,
						  netsnmp_request_info *requests)
{
	int rc;
	struct wu_snmp_value v;
	struct wu_oid_object *obj = (struct wu_oid_object *)handler->myvoid;
	static wu_oid_t load_oid[] = {XMUX_ROOT_OID, 100};

	switch (reqinfo->mode) {
		case MODE_GET:
			printf("snmp: %s get\n", oid_str(obj));
			obj->getter(obj, &v);
			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
				(u_char *)v.data, v.size);
			break;
		/*
		 * SET REQUEST
		 */
		case MODE_SET_RESERVE1:
			if (obj->max_set_size > 0) {
				rc = netsnmp_check_vb_type_and_max_size(requests->requestvb, ASN_OCTET_STR, obj->max_set_size);
				if (rc != SNMP_ERR_NOERROR) {
					netsnmp_set_request_error(reqinfo, requests, rc);
					return SNMP_ERR_GENERR;
				}
			}
			break;
		case MODE_SET_RESERVE2:
			/* XXX malloc "undo" storage buffer */
			if (0) {
				netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
			}
			break;
		case MODE_SET_FREE:
			/* XXX: free resources allocated in RESERVE1 and/or
			   RESERVE2.  Something failed somewhere, and the states
			   below won't be called. */
			break;
		case MODE_SET_ACTION:
			if ((sg_mib_heartDevice.flag != SNMP_LOGIN_STATUS_SUCCESS) &&
				!oid_is(obj, load_oid, 7)) {
				return SNMP_ERR_GENERR;
			}
			xmux_snmp_update_heart_device_time();
			v.data = requests->requestvb->val.string;
			v.size = requests->requestvb->val_len;
			printf("snmp: %s set, data %d bytes\n", oid_str(obj), v.size);
			hex_dump("snmp set", v.data, MIN(v.size, 48));
			obj->setter(obj, &v);
			if (0) {
				netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_WRONGVALUE);
			}
			break;
		case MODE_SET_COMMIT:
			/* XXX: delete temporary storage */
			if (0) {
				/* try _really_really_ hard to never get to this point */
				netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
			}
			break;
		case MODE_SET_UNDO:
			/* XXX: UNDO and return to previous value for the object */
			if (0) {
				/* try _really_really_ hard to never get to this point */
				netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
			}
			break;
		default:
			/* we should never get here, so this is a really bad error */
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_uvC1PAT\n", reqinfo->mode );
			return SNMP_ERR_GENERR;
	}

	return SNMP_ERR_NOERROR;
}

int netsnmp_agent_register(struct wu_oid_object *reg_obj)
{
	netsnmp_handler_registration *reg;
	struct wu_oid_object *obj = wu_oid_object_dup(reg_obj);
	int modes;

	/*
	 * setup modes
	 */
	switch (obj->status) {
		case OID_STATUS_READ: modes = HANDLER_CAN_RONLY; break;
		case OID_STATUS_WRITE: modes = HANDLER_CAN_SET; break;
		case OID_STATUS_RWRITE: modes = HANDLER_CAN_RWRITE; break;
		default:
			printf("unsupport status %d!\n", obj->status);
			wu_oid_object_free(obj);
			return -1;
			break;
	}

	reg = netsnmp_create_handler_registration(obj->name,
		netsnmp_oid_handler, obj->oid, obj->oid_len,
		modes);
	/* bind obj to handler's myvoid field */
	reg->handler->myvoid = obj;
	netsnmp_register_scalar(reg);

	return 0;
}

