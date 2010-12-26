/*
 * product config file
 */

#ifndef _UP_CONFIG_H_
#define _UP_CONFIG_H_


/*
 * don't save to armboot eeprom, else halt system. just save to xmux eeprom
 */
#define SAVE_NET_PARAM_TO_BOOTROM		0

/*
 * MIB loaded need long time, so the fast method is to not use it,
 * but use the OIDOps infomation
 */
#define MIB_LOADED		0
/*
 *
 */
#define OIDOPS_LOAD_MIB_FROM_FILE		0

/*
 * reduce the duplicate mib nodes, and add some index to track
 * the process node, use it or not?
 */
#define MIB_INDEXED			1

/*
 * save oid mib cache codes?
 */
#define OID_MIB_SAVE		0

/*
 * the MIB's root oid
 */
#define VENDOR_OID		1, 3, 6, 1, 4, 1, 5188
#define PRODUCT_OID		VENDOR_OID, 1

/*
 * the uCLibc's pthread_cond_timedwait() had bug? so emulate if
 * the macro is 0, that is not ok.
 */
#define PTHREAD_COND_TIMEDWAIT_OK		0


#define HAD_SNMPAGENT				1

/*
 * uCbootstrap, we use it to set and get bootrom env
 */
#ifdef _UCLINUX_
	#define HAD_UCBOOTSTRAP		1
#else
	#define HAD_UCBOOTSTRAP		0
#endif


#endif /* _UP_CONFIG_H_ */

