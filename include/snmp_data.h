#ifndef _SNMP_DATA_H_
#define _SNMP_DATA_H_

#include "up_type.h"
#include "up_mib.h"


/*
 * Opaque type represent the operation object, that's the 
 * net-snmp's tree or the WuTree
 */
typedef void * snmp_data_tree;

#define MIB_NODE_DEEPTH_MAX		20

/*
 * node's operation type
 */
enum {
	MIB_NODE_OPER_TYPE_NONE = 0,
	MIB_NODE_OPER_TYPE_INT,
	MIB_NODE_OPER_TYPE_STRING,
	MIB_NODE_OPER_TYPE_INT_RANGE,
	MIB_NODE_OPER_TYPE_INT_ENUM,
	MIB_NODE_OPER_TYPE_IPADDR,
	MIB_NODE_OPER_TYPE_TIMETICKS,
	MIB_NODE_OPER_TYPE_UINT,
};

snmp_data_tree snmp_data_get_first_tree ();
int snmp_data_load_mib ();
void snmp_data_shutdown ();
void snmp_data_load_mib_fix ();
snmp_data_tree snmp_data_get_node (oid_t *oid, int oidlen);
int snmp_data_goto_son_node (snmp_data_tree *pcur);
int snmp_data_goto_parent_node (snmp_data_tree *pcur);
int snmp_data_goto_next_node (snmp_data_tree *pcur);
int snmp_data_goto_prev_node (snmp_data_tree *pcur);
BOOL snmp_data_can_goto_son_node (snmp_data_tree *pcur);
BOOL snmp_data_can_goto_parent_node (snmp_data_tree *pcur);
BOOL snmp_data_can_goto_next_node (snmp_data_tree *pcur);
BOOL snmp_data_can_goto_prev_node (snmp_data_tree *pcur);
int snmp_data_get_node_string_array (snmp_data_tree *pcur,
		char **sa, int sasize);
int snmp_data_get_node_oper_type (snmp_data_tree *pcur);
struct enum_list * snmp_data_get_node_enum_list (snmp_data_tree *pcur);
struct range_list * snmp_data_get_node_range_list (snmp_data_tree *pcur);
int snmp_data_get_node_oid (snmp_data_tree *pcur, oid_t *oid);
BOOL snmp_data_editable (snmp_data_tree *pcur);
int snmp_data_get_node_handler_mode (oid_t *oid, int oidlen);
int snmp_data_get_cur_type_and_handler_mode (snmp_data_tree *pcur,
		int *type, int *mode);
int snmp_data_get_node_type_and_handler_mode (oid_t *oid, int oidlen,
								int *type, int *mode);
char * snmp_data_get_node_units (snmp_data_tree *pcur);
int snmp_data_check_integer (oid_t *oid, int oidlen, long *integer);
int snmp_data_get_octstr_size (snmp_data_tree *pcur);
char * snmp_data_get_node_hint (snmp_data_tree *pcur);
void snmp_data_fix_oidops (snmp_data_tree *pcur, OIDOps *oidops);


#endif /* _SNMP_DATA_H_ */

