/*
 * wutree.h - tree
 */

#ifndef _WU_TREE_H_
#define _WU_TREE_H_

#include <stdlib.h>
#include "wulist.h"

__BEGIN_DECLS

/* node */
typedef struct _WuTreeNode {
	struct _WuTreeNode *left;
	struct _WuTreeNode *right;
	struct _WuTreeNode *parent;
	struct _WuTreeNode *eldestson;
	int depth;
	void *data;
	void *ext_data;
	struct _WuTreeNode *left_leaf;
	struct _WuTreeNode *right_leaf;
} WuTreeNode;

/* node find */
typedef int (* WuTreeFinder) (void *node_data, void *data);
/* node position func */
typedef int (* WuTreePositioner) (WuTreeNode *prev, 
		WuTreeNode *cur, WuTreeNode *next);

/* tree */
typedef struct _WuTree {
	WuTreeNode *root;
	WuTreeFinder finder;
	WuTreePositioner positioner;
	WuTreeNode *cur_leaf;
	WuSListContext *node_pool;
	int pool_node_cnt;
	int node_used;
} WuTree;

WuTreeNode * wutreenode_new (void *data);
void * wutreenode_get_data (WuTreeNode *node);
void wutreenode_set_data (WuTreeNode *node, void *data);
int wutreenode_get_depth (WuTreeNode *node);
WuTreeNode * wutreenode_eldestson (WuTreeNode *tree);
WuTreeNode * wutreenode_littleson (WuTreeNode *node);
WuTreeNode * wutreenode_right (WuTreeNode *treenode);
WuTreeNode * wutreenode_left (WuTreeNode *treenode);
void wutreenode_add_left (WuTreeNode *node, WuTreeNode *added);
void wutreenode_add_right (WuTreeNode *node, WuTreeNode *added);
void wutreenode_add_eldestson (WuTreeNode *node, WuTreeNode *added);
void wutreenode_add_littleson (WuTreeNode *node, WuTreeNode *added);
int wutreenode_add_ordered_son (WuTree *tree, 
		WuTreeNode *node, WuTreeNode *added);

WuTree * wutree_new ();
void wutree_destroy (WuTree *tree);
void wutree_set_root (WuTree *tree, WuTreeNode *root);
void wutree_set_finder (WuTree *tree, WuTreeFinder finder);
WuTreeNode * wutree_find (WuTree *tree, void *data);
WuTreeNode * wutree_find_fbls (WuTree *tree, void *data);
WuTreeNode * wutree_find_in_sons (WuTree *tree, WuTreeNode *parent, void *data);
void wutree_dump (WuTree *tree);
WuTreeNode * wutree_node_pool_get_node (WuTree *tree);
void wutree_node_pool_put_node (WuTree *tree, WuTreeNode *node);

__END_DECLS

#endif /* _WU_TREE_H_ */

