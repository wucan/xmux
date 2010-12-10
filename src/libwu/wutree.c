#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "wutree.h"

/*
 * allocation a new node from the sky
 */
WuTreeNode * wutreenode_new (void *data)
{
	WuTreeNode *node;

	node = (WuTreeNode *)malloc(sizeof(*node));
	if (node) {
		memset(node, 0, sizeof(*node));
		node->data = data;
	}
	return node;
}
/*
 * get node's depth, the depth info is set in the node already
 */
int wutreenode_get_depth (WuTreeNode *node)
{
	return node->depth;
}
/*
 * get node's private data
 */
void * wutreenode_get_data (WuTreeNode *node)
{
	return node->data;
}
/*
 * set node's private data
 */
void wutreenode_set_data (WuTreeNode *node, void *data)
{
	node->data = data;
}
/*
 * get node's eldestson
 */
WuTreeNode * wutreenode_eldestson (WuTreeNode *node)
{
	return node->eldestson;
}
/*
 * get node's littleson
 */
WuTreeNode * wutreenode_littleson (WuTreeNode *node)
{
	WuTreeNode *n;
	
	n = wutreenode_eldestson(node);
	if (!n) {
		return NULL;
	}
	while (n->right) {
		n = n->right;
	}

	return n;
}
/*
 * get node's right
 */
WuTreeNode * wutreenode_right (WuTreeNode *node)
{
	return node->right;
}
/*
 * get node's left
 */
WuTreeNode * wutreenode_left (WuTreeNode *node)
{
	return node->left;
}
/*
 * add left node
 */
void wutreenode_add_left (WuTreeNode *node, WuTreeNode *added)
{
	if (node->left) {
		node->left->right = added;
	} else if (node->parent) {
		node->parent->eldestson = added;
	}
	added->right = node;
	added->parent = node->parent;
	added->depth = node->depth;
}
/*
 * add right node
 */
void wutreenode_add_right (WuTreeNode *node, WuTreeNode *added)
{
	if (node->right) {
		node->right->left = added;
	}
	added->left = node;
	added->parent = node->parent;
	added->depth = node->depth;
}
/*
 * add eldestson node
 */
void wutreenode_add_eldestson (WuTreeNode *node, WuTreeNode *added)
{
	if (node->eldestson) {
		node->eldestson->left = added;
	}
	added->right = node->eldestson;
	added->parent = node;
	node->eldestson= added;
	added->depth = node->depth + 1;
}
/*
 * add littlesone node
 */
void wutreenode_add_littleson (WuTreeNode *node, WuTreeNode *added)
{
	WuTreeNode *littleson;

	littleson = wutreenode_littleson(node);
	if (littleson) {
		littleson->right = added;
		added->left = littleson;
	} else {
		node->eldestson = added;
	}
	added->parent = node;
	added->depth = node->depth + 1;
}
/*
 * add node in ordered
 */
int wutreenode_add_ordered_son (WuTree *tree, 
		WuTreeNode *node, WuTreeNode *added)
{
	WuTreeNode *n = node->eldestson;

	if (!tree->positioner) {
		printf("tree had no positioner, you should define it!\n");
		return -1;
	}
	if (n) {
		if (!tree->positioner(NULL, added, n)) {
			added->right = n;
			n->left = added;
			node->eldestson = added;
			goto out;
		}
		for (; n; n = n->right) {
			if (!tree->positioner(n, added, n->right)) {
				added->left = n;
				if (n->right) {
					n->right->left = added;
				}
				added->right = n->right;
				n->right = added;
				break;
			}
		}
	} else {
		node->eldestson = added;
	}
out:
	added->parent = node;
	added->depth = node->depth + 1;
}
/*
 * allocation a new tree
 *
 * @node_cnt: point out the node we will use in future
 *
 * @return: the tree or NULL on no mem
 */
WuTree * wutree_new (unsigned int node_cnt)
{
	WuTree *tree;

	tree = (WuTree *)malloc(sizeof(*tree));
	if (tree) {
		memset(tree, 0, sizeof(*tree));
	}
	tree->node_pool = wuslist_context_new(node_cnt, sizeof(WuTreeNode));
	tree->pool_node_cnt = node_cnt;

	return tree;
}
void wutree_foreach_node (WuTree *tree)
{
	
	
}
/*
 * destroy a tree and it's node poll
 */
void wutree_destroy (WuTree *tree)
{
	wuslist_context_destroy(tree->node_pool);
}
/*
 * set tree's root node
 */
void wutree_set_root (WuTree *tree, WuTreeNode *root)
{
	tree->root = root;
	root->depth = 0;
}
/*
 * set tree's positioner function
 */
void wutree_set_positioner (WuTree *tree, WuTreePositioner positioner)
{
	tree->positioner = positioner;
}
/*
 * set tree's finder function
 */
void wutree_set_finder (WuTree *tree, WuTreeFinder finder)
{
	tree->finder = finder;
}
/*
 * the tree node finder internal
 */
static WuTreeNode * _wutreenode_find (WuTree *tree, WuTreeNode *node, void *data)
{
	WuTreeNode *n = NULL;

	for (; node; node = node->right) {
		if (tree->finder(node->data, data) == 0) {
			return node;
		}
		/* recuse to it's sons */
		n = wutreenode_eldestson(node);
		if (n) {
			n = _wutreenode_find(tree, n, data);
			if (n) {
				return n;
			}
		}
		/* right brother */
	}
	return NULL;
}
/*
 * to find a node
 */
WuTreeNode * wutree_find (WuTree *tree, void *data)
{
	WuTreeNode *node, *finded = NULL;

	node = wutreenode_eldestson(tree->root);
	if (node) {
		finded = _wutreenode_find(tree, node, data);
	}
	return finded;
}
/*
 * fbls: first brothers last sons
 */
static WuTreeNode * _wutreenode_find_fbls (WuTree *tree, 
				WuTreeNode *node, void *data)
{
	WuTreeNode *old_node = node, *n = NULL;

	/* brothers */
	for (; node; node = node->right) {
		if (tree->finder(node->data, data) == 0) {
			return node;
		}
	}
	/* recuse to it's sons */
	for (node = old_node; node; node = node->right) {
		n = wutreenode_eldestson(node);
		if (n) {
			n = _wutreenode_find_fbls(tree, n, data);
			if (n) {
				return n;
			}
		}
		/* right brother */
	}
	return NULL;
}
/*
 * find node, first brothers last sons
 */
WuTreeNode * wutree_find_fbls (WuTree *tree, void *data)
{
	WuTreeNode *node, *finded = NULL;

	node = wutreenode_eldestson(tree->root);
	if (node) {
		finded = _wutreenode_find_fbls(tree, node, data);
	}
	return finded;
}
/*
 * find a node in sons
 */
WuTreeNode * wutree_find_in_sons (WuTree *tree, WuTreeNode *parent, void *data)
{
	WuTreeNode *son;

	son = wutreenode_eldestson(parent);
	if (!son) {
		return NULL;
	}
	/* sons */
	for (; son; son = son->right) {
		if (tree->finder(son->data, data) == 0) {
			return son;
		}
	}

	return NULL;
}
/*
 * 
 */
static void _wutreenode_foreach (WuTreeNode *node)
{
	WuTreeNode *old_node = node, *son;

	for (node = old_node; node; node = node->right) {
		printf("%*s%d\n", node->depth * 4, " ",
			(unsigned int)(node->data));
		son = wutreenode_eldestson(node);
		_wutreenode_foreach(son);
	}
}
void wutree_dump (WuTree *tree)
{
	printf("tree struct:\n");
	_wutreenode_foreach(tree->root);
}
/*
 * allocation a node from node poll
 */
WuTreeNode * wutree_node_pool_get_node (WuTree *tree)
{
	WuSList *l;
	WuTreeNode *node = NULL;

	l = wuslist_context_get_slist(tree->node_pool);

	if (l) {
		node = wuslist_get_data(l);
		tree->node_used++;
	}

	return node;
}
/*
 * free a node to node pool
 */
void wutree_node_pool_put_node (WuTree *tree, WuTreeNode *node)
{
	WuSList *l;

	l = (WuSList *)(node - sizeof(WuSList));
	wuslist_context_put_slist(tree->node_pool, l);
	tree->node_used--;
}

