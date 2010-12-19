#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "wu/wu_base_type.h"
#include "wu/thread.h"

#include "wu_snmp_agent.h"

#include "xmux.h"


static Thread *agent_thr;
static bool agent_thread_quit;

int wu_snmp_agent_init()
{
	init_agent("xmux");

	return 0;
}

void wu_snmp_agent_fini()
{
	if (agent_thr) {
		agent_thread_quit = true;
		thread_join(agent_thr);
		agent_thr = NULL;
		agent_thread_quit = false;
	}
}

static int agent_thread(void *data)
{
	init_snmp("xmux");
	/* If we're going to be a SNMP master agent...  */
	init_master_agent();  /* Listen on default port (161). */

	/* Your main loop here... */
	while (!agent_thread_quit) {
		/* if you use select(), see snmp_api(3) */
		int rc = agent_check_and_process(1); /* 0 == don't block */
	}
	snmp_shutdown("xmux");

	return 0;
}
int wu_snmp_agent_run()
{
	agent_thr = thread_create(agent_thread, NULL);
	if (!agent_thr) {
		return -1;
	}

	return 0;
}

void wu_oid_object_free(struct wu_oid_object *obj)
{
	free(obj);
}
struct wu_oid_object * wu_oid_object_dup(struct wu_oid_object *obj)
{
	struct wu_oid_object *new_obj;

	new_obj = (struct wu_oid_object *)malloc(sizeof(*obj));
	if (new_obj) {
		*new_obj = *obj;
	}

	return new_obj;
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

	switch (reqinfo->mode) {
		case MODE_GET:
			obj->getter(obj, &v);
			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
				(u_char *)v.data, v.size);
			break;
		/*
		 * SET REQUEST
		 */
		case MODE_SET_RESERVE1:
			rc = netsnmp_check_vb_type_and_max_size(requests->requestvb, ASN_OCTET_STR, obj->max_set_size);
			if (rc != SNMP_ERR_NOERROR) {
				netsnmp_set_request_error(reqinfo, requests, rc);
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
			if (management_mode == MANAGEMENT_MODE_FP)
				return SNMP_ERR_GENERR;
			v.data = requests->requestvb->val.string;
			v.size = requests->requestvb->val_len;
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

int wu_snmp_agent_register(struct wu_oid_object *reg_obj)
{
	netsnmp_handler_registration *reg;
	struct wu_oid_object *obj = wu_oid_object_dup(reg_obj);
	int i;
	int modes;

#if 0
	printf("register oid: ");
	for (i = 0; i < obj->oid_len; i++) {
		printf("%d,", obj->oid[i]);
	}
	printf("\n");
#endif

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

