#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "wu/wutree.h"
#include "wu/message.h"

#include "up_config.h"
#include "up_mib.h"
#include "wu/lock.h"
#include "wu/mem.h"


#define NODE_SIZE	5000

struct _UPMIB {
	WuTree *oid_tree;
	char *name;
	Lock lock;
	OIDOpsRegisterFunc reg_func;
	int cnt;
	oid_t root_oid[OID_MAX_SIZE];
	int root_oidlen;
	WuTreeNode *root_tree_node;
};

static msgobj mo;
WuTreeNode * up_mib_find_node (UPMIB *upmib, oid_t *oid, int oidlen);

/*
 * oids dump
 */
void oid_dump (oid_t *oid, int oidlen)
{
	register int i;

	fprintf(stdout, "oidlen %d, oid: ", oidlen);
	for (i = 0; i < (oidlen - 1); i++) {
		fprintf(stdout, "%lu,", oid[i]);
	}
	fprintf(stdout, "%lu\n", oid[oidlen - 1]);
}
/*
 * oids dump with mo
 */
void oid_dump_mo (oid_t *oid, int oidlen, msgobj *mo)
{
	register int i;

	msg_dbg(mo, "oidlen %d, oid:", oidlen);
	for (i = 0; i < (oidlen - 1); i++) {
		msg_dbg(mo, "%lu,", oid[i]);
	}
	msg_dbg(mo, "%lu", oid[oidlen - 1]);
}
/*
 * dump oidops content
 */
void oidops_dump (OIDOps *oidops)
{
	oid_dump(oidops->oid, oidops->oidlen);
}
/*
 * dump oidops content with mo
 */
void oidops_dump_mo (OIDOps *oidops, msgobj *mo)
{
	oid_dump_mo(oidops->oid, oidops->oidlen, mo);
}
/*
 * oidops set
 */
int oidops_set (OIDOps *oidops, ValueUnion *vu)
{
	/* sanity check */
	if (!oidops) {
		return -1;
	}
	if (oidops->flag & OIDOPS_FLAG_SET_USE_VU) {
		switch (oidops->oidtype) {
			case OID_TYPE_INTEGER:
				*oidops->vu.integer = *vu->integer;
				break;
			case OID_TYPE_IPADDR:
				*oidops->vu.ipaddr = *vu->ipaddr;
				break;
			case OID_TYPE_STRING:
			{
				int slen = (int)oidops->data - 1;
				strncpy(oidops->vu.string, vu->string, slen);
				oidops->vu.string[slen] = 0;
			}
				break;
			default:
				break;
		}
		oidops->flag |= OIDOPS_FLAG_VU_DIRTY;
		return 0;
	}

	if (oidops->set_func) {
		return oidops->set_func(oidops, vu);
	}

	return -1;
}
/*
 * oidops get
 */
int oidops_get (OIDOps *oidops, ValueUnion *vu)
{
	/* sanity check */
	if (!oidops) {
		return -1;
	}
	if (oidops->flag & OIDOPS_FLAG_GET_USE_VU) {
		switch (oidops->oidtype) {
			case OID_TYPE_INTEGER:
				*vu->integer = *oidops->vu.integer;
				break;
			case OID_TYPE_IPADDR:
				*vu->ipaddr = *oidops->vu.ipaddr;
				break;
			case OID_TYPE_STRING:
				vu->string = oidops->vu.string;
				break;
			default:
				break;
		}
		oidops->flag |= OIDOPS_FLAG_VU_DIRTY;
		return 0;
	}

	if (oidops->get_func) {
		return oidops->get_func(oidops, vu);
	}

	return -1;
}
/*
 * oid compare algorithm
 */
static int up_mib_oid_finder (void *node_data, void *data)
{
	if (node_data == data) {
		return 0;
	}

	return -1;
}
#if !MIB_LOADED
/*
 * oid positioner algorithm
 */
static int up_mib_oid_positioner (WuTreeNode *prev, 
		WuTreeNode *cur, WuTreeNode *next)
{
	if (prev && ((unsigned int)prev->data > (unsigned int)cur->data)) {
		return -1;
	}
	if (next && ((unsigned int)cur->data > (unsigned int)next->data)) {
		return -1;
	}

	return 0;
}
#endif
/*
 * create a new UPMIB
 */
UPMIB * up_mib_new (char *name, OIDOpsRegisterFunc reg_func)
{
	UPMIB *upmib;
	WuTree *tree;
	WuTreeNode *root;

	/* init msgobj */
	if (!mo.name) {
		msgobj_init(&mo, MSG_DBG, 1, "mib");
	}
	tree = wutree_new(NODE_SIZE);
	root = wutree_node_pool_get_node(tree);
	wutreenode_set_data(root, name);
	wutree_set_root(tree, root);
	wutree_set_finder(tree, up_mib_oid_finder);
#if !MIB_LOADED
	wutree_set_positioner(tree, up_mib_oid_positioner);
#endif

	upmib = (UPMIB *)mem_mallocz(sizeof(UPMIB));
	upmib->oid_tree = tree;
	upmib->name = name;
	upmib->reg_func = reg_func;
	upmib->cnt = 0;
	lock_init(&upmib->lock);
	trace_info("mib created: %s", upmib->name);

	return upmib;
}
/*
 * destory UPMIB
 */
void up_mib_destroy (UPMIB *upmib)
{
	trace_info("mib destroied: %s", upmib->name);
	wutree_destroy(upmib->oid_tree);
	lock_destroy(&upmib->lock);
	mem_free(upmib);
}
/*
 * set the UPMIB's root oid, wish acceralte things
 */
void up_mib_set_root_oid (UPMIB *upmib, oid_t *oid, int oidlen)
{
	memcpy(upmib->root_oid, oid, sizeof(oid_t) * oidlen);
	upmib->root_oidlen = oidlen;
	upmib->root_tree_node = up_mib_find_node(upmib, oid, oidlen);
	if (!upmib->root_tree_node) {
		trace_err("upmib %s cannot set root tree node!",
			upmib->name);
	}
}
OIDOps * up_mib_find_most_like (UPMIB *upmib, oid_t *oid, int oidlen)
{
	oid_t o;
	WuTreeNode *node, *n;
	WuTree *tree = upmib->oid_tree;
	OIDOps *oidops = NULL;

	if (oidlen <= 0) {
		trace_err("oid length error %d", oidlen);
		return NULL;
	}
	/*
	 * if there had root_tree_node, we should use it quickly
	 */
	if (upmib->root_tree_node) {
		node = upmib->root_tree_node;
		oidlen -= upmib->root_oidlen;
		oid += upmib->root_oidlen;
	} else {
		node = tree->root;
	}

	for (o = *oid; oidlen > 0; oidlen--, oid++, o = *oid) {
		n = wutree_find_in_sons(tree, node, (void *)o);
		if (n) {
			node = n;
			continue;
		}
		break;
	}
	
	if (node == tree->root) {
		while (node->eldestson)
			node = node->eldestson;
	}

	if (node) {
		oidops = (OIDOps *)node->ext_data;
	}

	return oidops;
}
/*
 * find a WuTreeNode according to oid in the mib tree
 */
WuTreeNode * up_mib_find_node (UPMIB *upmib, oid_t *oid, int oidlen)
{
	WuTree *tree = upmib->oid_tree;
	WuTreeNode *node;
	oid_t o;

	if (oidlen <= 0) {
		trace_err("oid length error %d", oidlen);
		return NULL;
	}
	lock_lock(&upmib->lock);
	/*
	 * if there had root_tree_node, we should use it quickly
	 */
	if (upmib->root_tree_node) {
		node = upmib->root_tree_node;
		oidlen -= upmib->root_oidlen;
		oid += upmib->root_oidlen;
	} else {
		node = tree->root;
	}

	for (o = *oid; oidlen > 0; oidlen--, oid++, o = *oid) {
		node = wutree_find_in_sons(tree, node, (void *)o);
		if (!node) {
			lock_unlock(&upmib->lock);
			return NULL;
		}
	}
	lock_unlock(&upmib->lock);

	return node;
}
OIDOps * up_mib_find_oidops (UPMIB *upmib, oid_t *oid, int oidlen)
{
	WuTreeNode *node;

	node = up_mib_find_node(upmib, oid, oidlen);
	if (!node) {
		return NULL;
	}
	return (OIDOps *)node->ext_data;
}
int up_mib_register_oidops (UPMIB *upmib, OIDOps *oidops)
{
	WuTree *tree = upmib->oid_tree;
	WuTreeNode *node, *temp_node, *added = NULL;
	oid_t o, *oid = oidops->oid, oidlen = oidops->oidlen;
	OIDOps *oo;
	int rc;

	/* check */
	if (oidlen > OID_MAX_SIZE) {
		trace_err("oid length exceed %d > %d", oidlen, OID_MAX_SIZE);
		oid_dump(oidops->oid, oidops->oidlen);
		return -1;
	}
	lock_lock(&upmib->lock);
	/*
	 * if there had root_tree_node, we should use it quickly
	 */
	if (upmib->root_tree_node) {
		node = upmib->root_tree_node;
		oidlen -= upmib->root_oidlen;
		oid += upmib->root_oidlen;
	} else {
		node = tree->root;
	}

	for (o = *oid; oidlen > 0; oidlen--, oid++, o = *oid) {
		temp_node = node;
		node = wutree_find_in_sons(tree, node, (void *)o);
		if (node) {
			continue;
		}
		added = wutree_node_pool_get_node(tree);
		if (!added) {
			trace_err("wutree node pool empty, expand it!");
			oid_dump(oidops->oid, oidops->oidlen);
			lock_unlock(&upmib->lock);
			return -1;
		}
		added->data = (void *)o;
#if MIB_LOADED
		wutreenode_add_littleson(temp_node, added);
#else
		wutreenode_add_ordered_son(tree, temp_node, added);
#endif
		node = added;
	}
	if (added) {
		added->left_leaf = tree->cur_leaf;
		if (tree->cur_leaf) {
			tree->cur_leaf->right_leaf = added;
		}
		tree->cur_leaf = added;
		oo = mem_malloc(sizeof(OIDOps));
		if (!oo) {
			trace_warn("register oidops out of mem!");
			added->ext_data = oo;
			lock_unlock(&upmib->lock);
			return -1;

		}
		*oo = *oidops;
#if (!MIB_LOADED && OIDOPS_LOAD_MIB_FROM_FILE)
		/* load oidops's mib info from file, only the leaf node */
		if (oo->oidtype != OID_TYPE_OTHER) {
			rc = oidops_load_oid_mib(oo);
			if (rc) {
				trace_err("cannot find the oidops in oid_mib cache!");
				oid_dump(oidops->oid, oidops->oidlen);
				lock_unlock(&upmib->lock);
				return -1;
			}
		}
#endif
		added->ext_data = oo;
		upmib->cnt++;
		/* now had add the oidops to tree, could unlock it */
		lock_unlock(&upmib->lock);
		/* register to snmp agent */
		if ((oo->oidtype != OID_TYPE_OTHER) && upmib->reg_func) {
			rc = upmib->reg_func(oo);
			if (rc) {
				trace_err("register oidops failed!");
				oid_dump(oidops->oid, oidops->oidlen);
				return -1;
			}
		}
		return 0;
	}
	lock_unlock(&upmib->lock);
	

	return 0;
}
/*
 * register none leaf oidops to upmib, 
 * must called after all leaf oid loaded
 *
 */
int up_mib_register_none_leaf_oidops (UPMIB *upmib, OIDOps *oidops)
{
	WuTreeNode *node;
	OIDOps *oo;
	
	node = up_mib_find_node(upmib, oidops->oid, oidops->oidlen);
	if (!node) {
		trace_warn("cannot find the node, ");
		oidops_dump(oidops);
		return -1;
	}
	oo = mem_malloc(sizeof(OIDOps));
	if (!oo) {
		trace_warn("register oidops out of mem!");
		return -1;
	}
	*oo = *oidops;
	node->ext_data = oo;
	upmib->cnt++;
}
int up_mib_get_node_count (UPMIB *upmib)
{
	return upmib->cnt;
}
void up_mib_dump_info (UPMIB *upmib)
{
	trace_info("mib %s info:", upmib->name);
	trace_info("oidops cnt is %d", upmib->cnt);
	trace_info("tree node used %d of %d",
		upmib->oid_tree->node_used, upmib->oid_tree->pool_node_cnt);
}
WuTreeNode * up_mib_get_root_tree_node (UPMIB *upmib)
{
	return upmib->root_tree_node;
}

