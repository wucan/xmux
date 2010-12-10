#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#define NET_SNMP_HEAD

#include "wu/message.h"

#include "up_mib.h"

#include "snmp_agent.h"


static msgobj mo;
static unsigned char *pdata;
static int datasize;

/* local function */
static int
snmp_agent_scalar_handler (netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests);
static int snmp_agent_register_scalar (oid_t *oid, int oidlen, int mode);
static int
snmp_agent_ipaddr_handler (netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests);
static int snmp_agent_register_ipaddr (oid_t *oid, int oidlen, int mode);
static int
snmp_agent_string_handler (netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests);
int snmp_agent_register_string (oid_t *oid, int oidlen, int mode);
static int
snmp_agent_opaque_handler (netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests);
int snmp_agent_register_opaque (oid_t *oid, int oidlen, int mode);
static int
snmp_agent_unsigned32_handler (netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests);
static int snmp_agent_register_unsigned32 (oid_t *oid, int oidlen, int mode);
static int
snmp_agent_timeticks_handler (netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests);
static int snmp_agent_register_timeticks (oid_t *oid, int oidlen, int mode);

/*
 * register a node to agent
 */
int snmp_agent_register_oid (oid_t *oid, int oidlen, int type, int mode)
{
	int rc = -1;

	switch (type) {
		case OID_TYPE_INTEGER:
			return snmp_agent_register_scalar(oid, oidlen, mode);
			break;
		case OID_TYPE_IPADDR:
			return snmp_agent_register_ipaddr(oid, oidlen, mode);
			break;
		case OID_TYPE_STRING:
			return snmp_agent_register_string(oid, oidlen, mode);
			break;
		case OID_TYPE_OPAQUE:
			return snmp_agent_register_opaque(oid, oidlen, mode);
			break;
		case OID_TYPE_UNSIGNED32:
			return snmp_agent_register_unsigned32(oid, 
					oidlen, mode);
			break;
		case OID_TYPE_TIMETICKS:
			return snmp_agent_register_timeticks(oid, 
					oidlen, mode);
			break;
		default:
			break;
	}

	return rc;
}
/*
 * register a scalar node to agent
 */
static int snmp_agent_register_scalar (oid_t *oid, int oidlen, int mode)
{
	netsnmp_handler_registration *reginfo;

	trace_dbg("SNMP agent register scalar");
	reginfo = netsnmp_create_handler_registration("name?",
		snmp_agent_scalar_handler, oid, oidlen, mode);
	netsnmp_register_scalar(reginfo);

	return 0;
}
/*
 * scalar handler
 *
 * it's call OIDOps's operation
 *
 */
static int
snmp_agent_scalar_handler (netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests)
{
	int ret;
	netsnmp_variable_list *vl = requests->requestvb;

    /*
     * We are never called for a GETNEXT if it's registered as a
     * "instance", as it's "magically" handled for us.  
     */

    /*
     * a instance handler also only hands us one request at a time, so
     * we don't need to loop over a list of requests; we'll only get one. 
     */

	trace_dbg("snmp reqinfo mode: %d", reqinfo->mode);
    switch (reqinfo->mode) {
    case MODE_GET:
		ret = process_get_data(reginfo->rootoid, reginfo->rootoid_len - 1, 
				OID_TYPE_INTEGER, &pdata, &datasize);
		if (ret) {
        		return SNMP_ERR_GENERR;
		}
        snmp_set_var_typed_value(vl, ASN_INTEGER,
                         (u_char *)pdata,
						 4);
        break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
    case MODE_SET_RESERVE1:
        /*
         * or you could use netsnmp_check_vb_type_and_size instead 
         */
		/* the uClinux's net-snmp version had no this functin */
        //ret = netsnmp_check_vb_type(vl, ASN_INTEGER);
        ret = SNMP_ERR_NOERROR;
        if (ret != SNMP_ERR_NOERROR) {
            netsnmp_set_request_error(reqinfo, requests, ret);
        }
        break;

    case MODE_SET_RESERVE2:
        /*
         * XXX malloc "undo" storage buffer 
         */
        //if ( /* XXX if malloc, or whatever, failed: */ ) {
		if (0) {
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_RESOURCEUNAVAILABLE);
        }
        break;

    case MODE_SET_FREE:
        /*
         * XXX: free resources allocated in RESERVE1 and/or
         * RESERVE2.  Something failed somewhere, and the states
         * below won't be called. 
         */
        break;

    case MODE_SET_ACTION:
        /*
         * XXX: perform the value change here 
         */
		trace_dbg("MODE_SET_ACTION");
		ret = process_set_data(reginfo->rootoid, reginfo->rootoid_len - 1,
			OID_TYPE_INTEGER, (unsigned char *)vl->val.integer, 4);
		if (ret) {
        	return SNMP_ERR_GENERR;
		}

        //if ( /* XXX: error? */ ) {
		if (0) {
         //   netsnmp_set_request_error(reqinfo, requests, /* some error */
          //                            );
        }
        break;

    case MODE_SET_COMMIT:
        /*
         * XXX: delete temporary storage 
         */
        //if ( /* XXX: error? */ ) {
		if (0) {
            /*
             * try _really_really_ hard to never get to this point 
             */
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_COMMITFAILED);
        }
        break;

    case MODE_SET_UNDO:
        /*
         * XXX: UNDO and return to previous value for the object 
         */
        //if ( /* XXX: error? */ ) {
		if (0) {
            /*
             * try _really_really_ hard to never get to this point 
             */
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_UNDOFAILED);
        }
        break;

    default:
        /*
         * we should never get here, so this is a really bad error 
         */
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
/*
 * register a IPADDR node to agent
 */
int snmp_agent_register_ipaddr (oid_t *oid, int oidlen, int mode)
{
	netsnmp_handler_registration *reginfo;

	trace_dbg("SNMP agent register ipaddr");

	reginfo = netsnmp_create_handler_registration("name?",
		snmp_agent_ipaddr_handler, oid, oidlen, mode);
	netsnmp_register_scalar(reginfo);

	return 0;
}
/*
 * ipaddr handler
 *
 * it's call OIDOps's operation
 *
 */
static int
snmp_agent_ipaddr_handler (netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests)
{
	int ret;
	netsnmp_variable_list *vl = requests->requestvb;

    /*
     * We are never called for a GETNEXT if it's registered as a
     * "instance", as it's "magically" handled for us.  
     */

    /*
     * a instance handler also only hands us one request at a time, so
     * we don't need to loop over a list of requests; we'll only get one. 
     */

    switch (reqinfo->mode) {
    case MODE_GET:
		ret = process_get_data(reginfo->rootoid, reginfo->rootoid_len - 1, 
				OID_TYPE_IPADDR, &pdata, &datasize);
		if (ret) {
        	return SNMP_ERR_GENERR;
		}
       	snmp_set_var_typed_value(vl, ASN_IPADDRESS,
              (u_char *)pdata,
			 4);

        break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
    case MODE_SET_RESERVE1:
        /*
         * or you could use netsnmp_check_vb_type_and_size instead 
         */
        //ret = netsnmp_check_vb_type(vl, ASN_IPADDRESS);
        ret = SNMP_ERR_NOERROR;
        if (ret != SNMP_ERR_NOERROR) {
            netsnmp_set_request_error(reqinfo, requests, ret);
        }
        break;

    case MODE_SET_RESERVE2:
        /*
         * XXX malloc "undo" storage buffer 
         */
        //if ( /* XXX if malloc, or whatever, failed: */ ) {
		if (0) {
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_RESOURCEUNAVAILABLE);
        }
        break;

    case MODE_SET_FREE:
        /*
         * XXX: free resources allocated in RESERVE1 and/or
         * RESERVE2.  Something failed somewhere, and the states
         * below won't be called. 
         */
        break;

    case MODE_SET_ACTION:
        /*
         * XXX: perform the value change here 
         */
		trace_dbg("MODE_SET_ACTION");
		ret = process_set_data(reginfo->rootoid, reginfo->rootoid_len - 1,
			OID_TYPE_IPADDR, (unsigned char *)vl->val.integer, 4);
		if (ret) {
        	return SNMP_ERR_GENERR;
		}

        //if ( /* XXX: error? */ ) {
		if (0) {
         //   netsnmp_set_request_error(reqinfo, requests, /* some error */
          //                            );
        }
        break;

    case MODE_SET_COMMIT:
        /*
         * XXX: delete temporary storage 
         */
        //if ( /* XXX: error? */ ) {
		if (0) {
            /*
             * try _really_really_ hard to never get to this point 
             */
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_COMMITFAILED);
        }
        break;

    case MODE_SET_UNDO:
        /*
         * XXX: UNDO and return to previous value for the object 
         */
        //if ( /* XXX: error? */ ) {
		if (0) {
            /*
             * try _really_really_ hard to never get to this point 
             */
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_UNDOFAILED);
        }
        break;

    default:
        /*
         * we should never get here, so this is a really bad error 
         */
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
/*
 * register a STRING node to agent
 */
int snmp_agent_register_string (oid_t *oid, int oidlen, int mode)
{
	netsnmp_handler_registration *reginfo;

	trace_dbg("SNMP agent register string");

	reginfo = netsnmp_create_handler_registration("name?",
		snmp_agent_string_handler, oid, oidlen, mode);
	netsnmp_register_scalar(reginfo);

	return 0;
}
/*
 * string handler
 *
 * it's call OIDOps's operation
 *
 */
static int
snmp_agent_string_handler (netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests)
{
	int ret;
	netsnmp_variable_list *vl = requests->requestvb;

    /*
     * We are never called for a GETNEXT if it's registered as a
     * "instance", as it's "magically" handled for us.  
     */

    /*
     * a instance handler also only hands us one request at a time, so
     * we don't need to loop over a list of requests; we'll only get one. 
     */

    switch (reqinfo->mode) {
    case MODE_GET:
		ret = process_get_data(reginfo->rootoid, reginfo->rootoid_len - 1, 
				OID_TYPE_STRING, &pdata, &datasize);
		if (ret) {
        	return SNMP_ERR_GENERR;
		}
       	snmp_set_var_typed_value(vl, ASN_OCTET_STR,
              (u_char *)pdata, datasize);

        break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
    case MODE_SET_RESERVE1:
        /*
         * or you could use netsnmp_check_vb_type_and_size instead 
         */
        //ret = netsnmp_check_vb_type(vl, ASN_OCTET_STR);
        ret = SNMP_ERR_NOERROR;
        if (ret != SNMP_ERR_NOERROR) {
            netsnmp_set_request_error(reqinfo, requests, ret);
        }
        break;

    case MODE_SET_RESERVE2:
        /*
         * XXX malloc "undo" storage buffer 
         */
        //if ( /* XXX if malloc, or whatever, failed: */ ) {
		if (0) {
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_RESOURCEUNAVAILABLE);
        }
        break;

    case MODE_SET_FREE:
        /*
         * XXX: free resources allocated in RESERVE1 and/or
         * RESERVE2.  Something failed somewhere, and the states
         * below won't be called. 
         */
        break;

    case MODE_SET_ACTION:
        /*
         * XXX: perform the value change here 
         */
		trace_dbg("MODE_SET_ACTION");
		ret = process_set_data(reginfo->rootoid, reginfo->rootoid_len - 1,
			OID_TYPE_STRING, (unsigned char *)vl->val.string, vl->val_len);
		trace_dbg("string: %d, %s",	vl->val_len, vl->val.string);
		if (ret) {
        	return SNMP_ERR_GENERR;
		}

        //if ( /* XXX: error? */ ) {
		if (0) {
         //   netsnmp_set_request_error(reqinfo, requests, /* some error */
          //                            );
        }
        break;

    case MODE_SET_COMMIT:
        /*
         * XXX: delete temporary storage 
         */
        //if ( /* XXX: error? */ ) {
		if (0) {
            /*
             * try _really_really_ hard to never get to this point 
             */
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_COMMITFAILED);
        }
        break;

    case MODE_SET_UNDO:
        /*
         * XXX: UNDO and return to previous value for the object 
         */
        //if ( /* XXX: error? */ ) {
		if (0) {
            /*
             * try _really_really_ hard to never get to this point 
             */
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_UNDOFAILED);
        }
        break;

    default:
        /*
         * we should never get here, so this is a really bad error 
         */
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
/*
 * register a OPAQUE node to agent
 */
int snmp_agent_register_opaque (oid_t *oid, int oidlen, int mode)
{
	netsnmp_handler_registration *reginfo;

	trace_dbg("SNMP agent register opaque");

	reginfo = netsnmp_create_handler_registration("name?",
		snmp_agent_opaque_handler, oid, oidlen, mode);
	netsnmp_register_scalar(reginfo);

	return 0;
}
/*
 * opaque handler
 *
 * it's call OIDOps's operation
 *
 */
static int
snmp_agent_opaque_handler (netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests)
{
	int ret;
	netsnmp_variable_list *vl = requests->requestvb;

    /*
     * We are never called for a GETNEXT if it's registered as a
     * "instance", as it's "magically" handled for us.  
     */

    /*
     * a instance handler also only hands us one request at a time, so
     * we don't need to loop over a list of requests; we'll only get one. 
     */

    switch (reqinfo->mode) {
    case MODE_GET:
		ret = process_get_data(reginfo->rootoid, reginfo->rootoid_len - 1, 
				OID_TYPE_OPAQUE, &pdata, &datasize);
		if (ret) {
        	return SNMP_ERR_GENERR;
		}
       	snmp_set_var_typed_value(vl, ASN_OPAQUE,
              (u_char *)pdata, datasize);

        break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
    case MODE_SET_RESERVE1:
        /*
         * or you could use netsnmp_check_vb_type_and_size instead 
         */
        //ret = netsnmp_check_vb_type(vl, ASN_IPADDRESS);
        ret = SNMP_ERR_NOERROR;
        if (ret != SNMP_ERR_NOERROR) {
            netsnmp_set_request_error(reqinfo, requests, ret);
        }
        break;

    case MODE_SET_RESERVE2:
        /*
         * XXX malloc "undo" storage buffer 
         */
        //if ( /* XXX if malloc, or whatever, failed: */ ) {
		if (0) {
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_RESOURCEUNAVAILABLE);
        }
        break;

    case MODE_SET_FREE:
        /*
         * XXX: free resources allocated in RESERVE1 and/or
         * RESERVE2.  Something failed somewhere, and the states
         * below won't be called. 
         */
        break;

    case MODE_SET_ACTION:
        /*
         * XXX: perform the value change here 
         */
		trace_dbg("MODE_SET_ACTION");
		trace_dbg("opaque: len %d", vl->val_len);
		/*
		 * the uclinux's net-snmp had no .floatVal, FIXME
		 */
#ifndef _UCLINUX_
	{
		float f;
		f = *vl->val.floatVal;
		trace_dbg("set float %f", f);
	}
		ret = process_set_data(reginfo->rootoid, reginfo->rootoid_len - 1,
			OID_TYPE_OPAQUE, (unsigned char *)vl->val.floatVal, vl->val_len);
		if (ret) {
        	return SNMP_ERR_GENERR;
		}
#endif


        //if ( /* XXX: error? */ ) {
		if (0) {
         //   netsnmp_set_request_error(reqinfo, requests, /* some error */
          //                            );
        }
        break;

    case MODE_SET_COMMIT:
        /*
         * XXX: delete temporary storage 
         */
        //if ( /* XXX: error? */ ) {
		if (0) {
            /*
             * try _really_really_ hard to never get to this point 
             */
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_COMMITFAILED);
        }
        break;

    case MODE_SET_UNDO:
        /*
         * XXX: UNDO and return to previous value for the object 
         */
        //if ( /* XXX: error? */ ) {
		if (0) {
            /*
             * try _really_really_ hard to never get to this point 
             */
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_UNDOFAILED);
        }
        break;

    default:
        /*
         * we should never get here, so this is a really bad error 
         */
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
/*
 * register a unsigned32 node to agent
 */
static int snmp_agent_register_unsigned32 (oid_t *oid, int oidlen, int mode)
{
	netsnmp_handler_registration *reginfo;

	trace_dbg("SNMP agent register scalar");
	reginfo = netsnmp_create_handler_registration("name?",
		snmp_agent_unsigned32_handler, oid, oidlen, mode);
	netsnmp_register_scalar(reginfo);

	return 0;
}
/*
 * unsigned32 handler
 *
 * it's call OIDOps's operation
 *
 */
static int
snmp_agent_unsigned32_handler (netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests)
{
	int ret;
	netsnmp_variable_list *vl = requests->requestvb;

    /*
     * We are never called for a GETNEXT if it's registered as a
     * "instance", as it's "magically" handled for us.  
     */

    /*
     * a instance handler also only hands us one request at a time, so
     * we don't need to loop over a list of requests; we'll only get one. 
     */

	trace_dbg("snmp reqinfo mode: %d", reqinfo->mode);
    switch (reqinfo->mode) {
    case MODE_GET:
		ret = process_get_data(reginfo->rootoid, reginfo->rootoid_len - 1, 
				OID_TYPE_UNSIGNED32, &pdata, &datasize);
		if (ret) {
        		return SNMP_ERR_GENERR;
		}
        snmp_set_var_typed_value(vl, ASN_INTEGER,
                         (u_char *)pdata, 4);
        break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
    case MODE_SET_RESERVE1:
        /*
         * or you could use netsnmp_check_vb_type_and_size instead 
         */
		/* the uClinux's net-snmp version had no this functin */
        //ret = netsnmp_check_vb_type(vl, ASN_INTEGER);
        ret = SNMP_ERR_NOERROR;
        if (ret != SNMP_ERR_NOERROR) {
            netsnmp_set_request_error(reqinfo, requests, ret);
        }
        break;

    case MODE_SET_RESERVE2:
        /*
         * XXX malloc "undo" storage buffer 
         */
        //if ( /* XXX if malloc, or whatever, failed: */ ) {
		if (0) {
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_RESOURCEUNAVAILABLE);
        }
        break;

    case MODE_SET_FREE:
        /*
         * XXX: free resources allocated in RESERVE1 and/or
         * RESERVE2.  Something failed somewhere, and the states
         * below won't be called. 
         */
        break;

    case MODE_SET_ACTION:
        /*
         * XXX: perform the value change here 
         */
		trace_dbg("MODE_SET_ACTION");
		ret = process_set_data(reginfo->rootoid, reginfo->rootoid_len - 1,
			OID_TYPE_UNSIGNED32, (unsigned char *)vl->val.integer, 4);
		if (ret) {
        	return SNMP_ERR_GENERR;
		}

        //if ( /* XXX: error? */ ) {
		if (0) {
         //   netsnmp_set_request_error(reqinfo, requests, /* some error */
          //                            );
        }
        break;

    case MODE_SET_COMMIT:
        /*
         * XXX: delete temporary storage 
         */
        //if ( /* XXX: error? */ ) {
		if (0) {
            /*
             * try _really_really_ hard to never get to this point 
             */
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_COMMITFAILED);
        }
        break;

    case MODE_SET_UNDO:
        /*
         * XXX: UNDO and return to previous value for the object 
         */
        //if ( /* XXX: error? */ ) {
		if (0) {
            /*
             * try _really_really_ hard to never get to this point 
             */
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_UNDOFAILED);
        }
        break;

    default:
        /*
         * we should never get here, so this is a really bad error 
         */
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
/*
 * register a timeticks node to agent
 */
static int snmp_agent_register_timeticks (oid_t *oid, int oidlen, int mode)
{
	netsnmp_handler_registration *reginfo;

	trace_dbg("SNMP agent register timeticks");
	reginfo = netsnmp_create_handler_registration("name?",
		snmp_agent_timeticks_handler, oid, oidlen, mode);
	netsnmp_register_scalar(reginfo);

	return 0;
}
/*
 * timeticks handler
 *
 * it's call OIDOps's operation
 *
 */
static int
snmp_agent_timeticks_handler (netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests)
{
	int ret;
	netsnmp_variable_list *vl = requests->requestvb;

    /*
     * We are never called for a GETNEXT if it's registered as a
     * "instance", as it's "magically" handled for us.  
     */

    /*
     * a instance handler also only hands us one request at a time, so
     * we don't need to loop over a list of requests; we'll only get one. 
     */

	trace_dbg("snmp reqinfo mode: %d", reqinfo->mode);
    switch (reqinfo->mode) {
    case MODE_GET:
		ret = process_get_data(reginfo->rootoid, reginfo->rootoid_len - 1, 
				OID_TYPE_TIMETICKS, &pdata, &datasize);
		if (ret) {
        		return SNMP_ERR_GENERR;
		}
        snmp_set_var_typed_value(vl, ASN_INTEGER,
                         (u_char *)pdata, 4);
        break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
    case MODE_SET_RESERVE1:
        /*
         * or you could use netsnmp_check_vb_type_and_size instead 
         */
		/* the uClinux's net-snmp version had no this functin */
        //ret = netsnmp_check_vb_type(vl, ASN_INTEGER);
        ret = SNMP_ERR_NOERROR;
        if (ret != SNMP_ERR_NOERROR) {
            netsnmp_set_request_error(reqinfo, requests, ret);
        }
        break;

    case MODE_SET_RESERVE2:
        /*
         * XXX malloc "undo" storage buffer 
         */
        //if ( /* XXX if malloc, or whatever, failed: */ ) {
		if (0) {
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_RESOURCEUNAVAILABLE);
        }
        break;

    case MODE_SET_FREE:
        /*
         * XXX: free resources allocated in RESERVE1 and/or
         * RESERVE2.  Something failed somewhere, and the states
         * below won't be called. 
         */
        break;

    case MODE_SET_ACTION:
        /*
         * XXX: perform the value change here 
         */
		trace_dbg("MODE_SET_ACTION");
		ret = process_set_data(reginfo->rootoid, reginfo->rootoid_len - 1,
			OID_TYPE_TIMETICKS, (unsigned char *)vl->val.integer, 4);
		if (ret) {
        	return SNMP_ERR_GENERR;
		}

        //if ( /* XXX: error? */ ) {
		if (0) {
         //   netsnmp_set_request_error(reqinfo, requests, /* some error */
          //                            );
        }
        break;

    case MODE_SET_COMMIT:
        /*
         * XXX: delete temporary storage 
         */
        //if ( /* XXX: error? */ ) {
		if (0) {
            /*
             * try _really_really_ hard to never get to this point 
             */
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_COMMITFAILED);
        }
        break;

    case MODE_SET_UNDO:
        /*
         * XXX: UNDO and return to previous value for the object 
         */
        //if ( /* XXX: error? */ ) {
		if (0) {
            /*
             * try _really_really_ hard to never get to this point 
             */
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_UNDOFAILED);
        }
        break;

    default:
        /*
         * we should never get here, so this is a really bad error 
         */
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
void snmp_agent_register_set_debug_level (int verbose)
{
	mo.verbose = verbose;
}
void snmp_agent_register_init ()
{
	msgobj_init(&mo, MSG_WARN, 1, "SNMP-AGENT-REG");
}

