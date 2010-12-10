#ifndef _WU_SNMP_AGENT_H_
#define _WU_SNMP_AGENT_H_

#include <stdint.h>


#define XMUX_ROOT_OID		1, 3, 6, 1, 3, 5188
#define WU_OID_MAX_SIZE		20

struct wu_oid_object;

typedef unsigned long wu_oid_t;

struct wu_snmp_value {
	void *data;
	int size;
};

typedef int (*oid_get_func)(struct wu_oid_object *obj, struct wu_snmp_value *v);
typedef int (*oid_set_func)(struct wu_oid_object *obj, struct wu_snmp_value *v);

struct wu_oid_object {
	const char *name;
	wu_oid_t oid[WU_OID_MAX_SIZE];
	int oid_len;
	int flags;
	int status;
	
	oid_get_func getter;
	oid_set_func setter;
	
	int max_set_size;
	struct wu_snmp_value value;
};

/*
 * status
 */
#define OID_STATUS_READ					(1 << 0)
#define OID_STATUS_WRITE				(1 << 1)
#define OID_STATUS_RWRITE				(OID_STATUS_READ | OID_STATUS_WRITE)

/*
 * flags
 */
#define OID_FLAGS_GET_USE_VALUE			(1 << 0)
#define OID_FLAGS_SET_USE_VALUE			(1 << 1)


int wu_snmp_agent_init();
void wu_snmp_agent_fini();
int wu_snmp_agent_run();

void wu_oid_object_free(struct wu_oid_object *obj);
struct wu_oid_object * wu_oid_object_dup(struct wu_oid_object *obj);
int wu_snmp_agent_register(struct wu_oid_object *obj);


#endif /* _WU_SNMP_AGENT_H_ */

