/*
 *  use the OIDOps MIB info to implement snmp_data
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#define NET_SNMP_HEAD

#include "wu/message.h"
#include "wu/wutree.h"

#include "up_config.h"
#include "up_mib.h"

#include "snmp_agent.h"
#include "snmp_data.h"
#include "fp_dict.h"


#if !MIB_LOADED


/* the uClinux's net-snmp need this */ 
#ifndef HANDLER_CAN_SET_ONLY
	#define HANDLER_CAN_SET_ONLY	HANDLER_CAN_SET
#endif

extern UPMIB *gd_upmib;
static WuTreeNode *mib_tree_head;
static msgobj mo;

extern WuTreeNode * up_mib_find_node (UPMIB *upmib, oid_t *oid, int oidlen);
extern WuTreeNode * up_mib_get_root_tree_node (UPMIB *upmib);
static BOOL tree_accessable (WuTreeNode *t);

snmp_data_tree snmp_data_get_first_tree ()
{
	snmp_data_tree t = mib_tree_head;

	snmp_data_goto_son_node(&t);

	return t;
}
int snmp_data_load_mib ()
{
	msgobj_init(&mo, MSG_WARN, 1, "snmp_data_oidops");
#if OIDOPS_LOAD_MIB_FROM_FILE
	oid_mib_load_from_file();
#endif

	return 0;
}
void snmp_data_load_mib_fix ()
{
	mib_tree_head = up_mib_get_root_tree_node(gd_upmib);
}
void snmp_data_shutdown ()
{
}
/*
 * get snmp_data_tree from oid
 *
 * @return: NULL on failed
 */
snmp_data_tree snmp_data_get_node (oid_t *oid, int oidlen)
{
	WuTreeNode *node;
	
	node = up_mib_find_node(gd_upmib, oid, oidlen);

	return (snmp_data_tree)node;
}
/*
 * walk into sons node
 *
 * @return: 0 on change, -1 on no change
 */
int snmp_data_goto_son_node (snmp_data_tree *pcur)
{
	WuTreeNode *t, *told = *pcur;

	t = told->eldestson;
	if (t) {
		if (tree_accessable(t)) {
			*pcur = t;
			return 0;
		}
		*pcur = t;
		if (snmp_data_goto_next_node(pcur)) {
			*pcur = told;
			return -1;
		}
		return 0;
	}

	return -1;
}
/*
 * walk into parent node
 *
 * @return: 0 on change, -1 on no change
 */
int snmp_data_goto_parent_node (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;

	/* are I'm the top node? */
	if (t == mib_tree_head ||
		t->parent == mib_tree_head) {
		return -1;
	}

	if (t->parent) { 
		*pcur = t->parent;
		return 0;
	}

	return -1;
}
/*
 * walk to next node
 *
 * @return: 0 on change, -1 on no change
 */
int snmp_data_goto_next_node (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;

	/* are I'm the top node? */
	if (t == mib_tree_head) {
		return -1;
	}

	while (t->right) {
		if (tree_accessable(t->right)) {
			*pcur = t->right;
			return 0;
		}
		t = t->right;
	}

	return -1;
}
/*
 * walk to prev node
 *
 * @return: 0 on change, -1 on no change
 */
int snmp_data_goto_prev_node (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;

	/* are I'm the top node? */
	if (t == mib_tree_head) {
		return -1;
	}

	while (t->left) {
		if (tree_accessable(t->left)) {
			*pcur = t->left;
			return 0;
		}
		t = t->left;
	}

	return -1;
}
/*
 * can goto so node?
 */
BOOL snmp_data_can_goto_son_node (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;

	if (t->eldestson) {
		return TRUE;
	}

	return FALSE;
}
/*
 * can goto parent node?
 */
BOOL snmp_data_can_goto_parent_node (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;

	/* are I'm the top node? */
	if (t == mib_tree_head ||
		t->parent == mib_tree_head) {
		return FALSE;
	}
	if (t->parent) { 
		return TRUE;
	}

	return FALSE;
}
/*
 * can goto next node?
 *
 */
BOOL snmp_data_can_goto_next_node (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;

	/* are I'm the top node? */
	if (t == mib_tree_head) {
		return FALSE;
	}

	if (t->right) {
		return TRUE;
	}

	return FALSE;
}
/*
 * can goto prev node?
 */
BOOL snmp_data_can_goto_prev_node (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;

	/* are I'm the top node? */
	if (t == mib_tree_head) {
		return FALSE;
	}

	if (t->left) {
		return TRUE;
	}

	return FALSE;
}
/*
 * get the node label as array, the array is from top to down
 */
int snmp_data_get_node_string_array (snmp_data_tree *pcur,
		char **sa, int sasize)
{
	WuTreeNode *t = *pcur;
	char *labels[MIB_NODE_DEEPTH_MAX + 1];
	int labelcnt = 0, i;
	char *trans;
	OIDOps *oidops;

	while (t != mib_tree_head) {
		oidops = t->ext_data;
		labels[labelcnt++] = oidops->label;
		t = t->parent;
		if (labelcnt >= MIB_NODE_DEEPTH_MAX) {
			/* exceed the node deepth! */
			break;
		}
	}
	for (i = labelcnt - 1; i >= 0; i--) {
		trans = fp_dict_get_trans(labels[i]);
		sa[labelcnt - i - 1] = trans;
	}

	return labelcnt;
}
BOOL snmp_data_editable (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;
	OIDOps *oidops = t->ext_data;

	if (oidops->access == ACCESS_READWRITE ||
		oidops->access == ACCESS_WRITEONLY) {
		/* check status */
		if (oidops->status == STATUS_OBSOLETE ||
			oidops->status == STATUS_DEPRECATED) {
			return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}
/*
 * @return: MIB_NODE_OPER_TYPE_XXX
 *
 * access = 0 => not leafe
 *
 */
int snmp_data_get_node_oper_type (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;
	OIDOps *oidops = t->ext_data;

	if (oidops->access == 0) {
		return MIB_NODE_OPER_TYPE_NONE;
	}
	/* check status */
	if (oidops->status == STATUS_OBSOLETE ||
		oidops->status == STATUS_DEPRECATED) {
		return MIB_NODE_OPER_TYPE_NONE;
	}
	if (oidops->oidtype == TYPE_INTEGER) {
		if (oidops->enums) {
			return MIB_NODE_OPER_TYPE_INT_ENUM;
		}
		if (oidops->ranges) {
			return MIB_NODE_OPER_TYPE_INT_RANGE;
		}
		return MIB_NODE_OPER_TYPE_INT;
	} else if (oidops->oidtype == TYPE_OCTETSTR) {
		return MIB_NODE_OPER_TYPE_STRING;
	} else if (oidops->oidtype == TYPE_TIMETICKS) {
		return MIB_NODE_OPER_TYPE_TIMETICKS;
	} else if (oidops->oidtype == TYPE_IPADDR) {
		return MIB_NODE_OPER_TYPE_IPADDR;
	} else if (oidops->oidtype == TYPE_UNSIGNED32 ||
		oidops->oidtype == OID_TYPE_UNSIGNED32) {
		return MIB_NODE_OPER_TYPE_UINT;
	}

	return MIB_NODE_OPER_TYPE_NONE;
}
struct enum_list * snmp_data_get_node_enum_list (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;
	OIDOps *oidops = t->ext_data;

	return oidops->enums;
}
struct range_list * snmp_data_get_node_range_list (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;
	OIDOps *oidops = t->ext_data;

	return oidops->ranges;
}
int snmp_data_get_node_oid (snmp_data_tree *pcur, oid_t *oid)
{
	WuTreeNode *t = *pcur;
	OIDOps *oidops = t->ext_data;

	memcpy(oid, oidops->oid, oidops->oidlen * sizeof(oidops->oid[0]));

	return oidops->oidlen;
}
/*
 * get access mode from MIB, value as: HANDLER_CAN_XXX
 *
 * MIB node access value: MIB_ACCESS_XXX -> HANDLER_CAN_XXX
 *
 */
int snmp_data_get_node_handler_mode (oid_t *oid, int oidlen)
{
	WuTreeNode *t;
	OIDOps *oidops;
	int mode;
	
	t = up_mib_find_node(gd_upmib, oid, oidlen);
	if (!t) {
		return -1;
	}
	oidops = t->ext_data;
	
	switch (oidops->access) {
		case ACCESS_READONLY: 
			mode = HANDLER_CAN_RONLY;
			break;
		case ACCESS_READWRITE:
			mode = HANDLER_CAN_RWRITE;
			break;
		case ACCESS_WRITEONLY:
			mode = HANDLER_CAN_SET_ONLY;
			break;
		default:
			mode = -1;
			break;
	}

	return mode;
}
int snmp_data_get_cur_type_and_handler_mode (snmp_data_tree *pcur,
		int *type, int *mode)
{
	WuTreeNode *t = *pcur;
	OIDOps *oidops = t->ext_data;

	/* type */
	*type = oidops->oidtype;
	/* mode */
	switch (oidops->access) {
		case MIB_ACCESS_READONLY: 
			*mode = HANDLER_CAN_RONLY;
			break;
		case MIB_ACCESS_READWRITE:
			*mode = HANDLER_CAN_RWRITE;
			break;
		case MIB_ACCESS_WRITEONLY:
			*mode = HANDLER_CAN_SET_ONLY;
			break;
		default:
			trace_warn("unsupport oid access: %d", oidops->access);
			return -1;
			break;
	}

	return 0;
}
/*
 * get node type from MIB and access mode
 */
int snmp_data_get_node_type_and_handler_mode (oid_t *oid, int oidlen,
			int *type, int *mode)
{
	WuTreeNode *t;
	
	t = up_mib_find_node(gd_upmib, oid, oidlen);
	if (!t) {
		return -1;
	}
	return snmp_data_get_cur_type_and_handler_mode(
			(snmp_data_tree *)&t, type, mode);
}
char * snmp_data_get_node_units (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;
	OIDOps *oidops = t->ext_data;

	return oidops->units;
}
/*
 * check the integer value
 * don't correct it's value, just return error
 *
 */
int snmp_data_check_integer (oid_t *oid, int oidlen, long *integer)
{
	WuTreeNode *t;
	long iv = *integer;
	OIDOps *oidops;
	
	t = up_mib_find_node(gd_upmib, oid, oidlen);
	if (!t) {
		return -1;
	}
	oidops = t->ext_data;
	if (oidops->enums) {
		/* enums */
		struct enum_list *e = oidops->enums;
		while (e) {
			if (iv == e->value) {
				return 0;
			}
			e = e->next;
		}
	} else if (oidops->ranges) {
		/* ranges */
		struct range_list *range = oidops->ranges;
		while (range) {
			if (iv >= range->low && iv <= range->high) {
				return 0;
			}
			range = range->next;
		}
	} else {
		return 0;
	}

	return -1;
}
/*
 * get the octstr type node's string length
 */
int snmp_data_get_octstr_size (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;
	OIDOps *oidops = t->ext_data;

	return oidops->ranges->high;
}
char * snmp_data_get_node_hint (snmp_data_tree *pcur)
{
	WuTreeNode *t = *pcur;
	OIDOps *oidops = t->ext_data;

	return oidops->hint;
}
static BOOL tree_accessable (WuTreeNode *t)
{
	OIDOps *oidops = t->ext_data;

	/* check the oidops */
	if (!oidops) {
		return FALSE;
	}
	if (oidops->status == STATUS_OBSOLETE ||
		oidops->status == STATUS_DEPRECATED) {
			return FALSE;
	}

	return TRUE;
}
void snmp_data_fix_oidops (snmp_data_tree *pcur, OIDOps *oidops)
{
	WuTreeNode *t = *pcur;

	if (oidops->oidtype == TYPE_OCTETSTR) {
		oidops->data = (void *)(snmp_data_get_octstr_size(pcur) - 1);
	}
}
void snmp_data_set_debug_level (int verbose)
{
	mo.verbose = verbose;
}


#endif /* !MIB_LOEADE */

