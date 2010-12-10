#ifndef _UP_MIB_H_
#define _UP_MIB_H_

#include "up_type.h"
#include "wu/message.h"


typedef struct _OIDOps OIDOps;
typedef union {
		long *integer;
		char *string;
		IPAddress *ipaddr;
		float *pfloat;
		unsigned long *unsigned32;
		struct bin_data {
			void *data;
			int size;
		} bin;
} ValueUnion;

/*
 * oid type, refer to net-snmp define
 */
#define OID_TYPE_OPAQUE		9
enum {
	OID_TYPE_OTHER = 0,			// TYPE_OTHER
	OID_TYPE_STRING = 2,			// TYPE_OCTETSTR
	OID_TYPE_INTEGER = 3,			// TYPE_INTEGER
	OID_TYPE_IPADDR = 5,			// TYPE_IPADDR
	OID_TYPE_TIMETICKS = 8,			// TYPE_TIMETICKS
	OID_TYPE_FLOAT = OID_TYPE_OPAQUE,
	OID_TYPE_UNSIGNED32 = 14,			// TYPE_UINTEGER
};

/*
 * access
 */
enum {
	ACCESS_READONLY = 18,	// MIB_ACCESS_READONLY
	ACCESS_READWRITE = 19,	// MIB_ACCESS_READWRITE
	ACCESS_WRITEONLY = 20,	// MIB_ACCESS_WRITEONLY
};
/*
 * status
 */
enum {
	STATUS_MANDATORY = 23,	// MIB_STATUS_MANDATORY
	STATUS_OPTIONAL = 24,	// MIB_STATUS_OPTIONAL
	STATUS_OBSOLETE = 25,	// MIB_STATUS_OBSOLETE
	STATUS_DEPRECATED = 39,	// MIB_STATUS_DEPRECATED
	STATUS_CURRENT = 57,	// MIB_STATUS_CURRENT
};
#ifndef NET_SNMP_HEAD
struct range_list {
	struct range_list *next;
	int low;
	int high;
};
struct enum_list {
	struct enum_list *next;
	int value;
	char *label;
};
#endif
typedef int (*OidGetFunc)(OIDOps *oidops, ValueUnion *vu);
typedef int (*OidSetFunc)(OIDOps *oidops, ValueUnion *vu);

struct _OIDOps {
	OidGetFunc get_func;
	OidSetFunc set_func;
	int oidtype;
	int oidlen;
	oid_t oid[OID_MAX_SIZE];
	ValueUnion vu;
	int flag;
	void *data;

	char *label;
	unsigned short access;
	unsigned short status;
	struct range_list *ranges;
	struct enum_list *enums;
	char *units;
	char *hint;
};
#define OIDOPS_FLAG_GET_USE_VU		(1 << 0)
#define OIDOPS_FLAG_SET_USE_VU		(1 << 1)
#define OIDOPS_FLAG_VU_DIRTY		(1 << 2)

typedef struct _UPMIB UPMIB;
typedef int (*OIDOpsRegisterFunc)(OIDOps *oidops);

void oid_dump (oid_t *oid, int oidlen);
void oid_dump_mo (oid_t *oid, int oidlen, msgobj *mo);
void oidops_dump (OIDOps *oidops);
void oidops_dump_mo (OIDOps *oidops, msgobj *mo);
int oidops_set (OIDOps *oidops, ValueUnion *vu);
int oidops_get (OIDOps *oidops, ValueUnion *vu);
UPMIB * up_mib_new (char *name, OIDOpsRegisterFunc reg_func);
void up_mib_destroy (UPMIB *upmib);
void up_mib_set_root_oid (UPMIB *upmib, oid_t *oid, int oidlen);
int up_mib_register_oidops (UPMIB *upmib, OIDOps *oidops);
OIDOps * up_mib_find_oidops (UPMIB *upmib, oid_t *oid, int oidlen);
int up_mib_get_node_count (UPMIB *upmib);
void up_mib_dump_info (UPMIB *upmib);


#endif /* _UP_MIB_H_ */

