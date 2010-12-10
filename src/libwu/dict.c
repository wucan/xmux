#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "wu/wutree.h"
#include "wu/message.h"

#include "dict.h"


#define NODE_SIZE	2000

struct _DICT {
	WuTree *word_tree;
	char *name;
};

static msgobj mo;

/*
 * word compare algorithm
 */
static int dict_word_finder (void *node_data, void *data)
{
	if (node_data == data) {
		return 0;
	}

	return -1;
}
/*
 * create a new DICT
 */
DICT * dict_new (char *name)
{
	DICT *dict;
	WuTree *tree;
	WuTreeNode *root;

	/* init msgobj */
	if (!mo.name) {
		msgobj_init(&mo, MSG_DBG, 1, "DICT");
	}
	tree = wutree_new(NODE_SIZE);
	root = wutree_node_pool_get_node(tree);
	wutreenode_set_data(root, name);
	wutree_set_root(tree, root);
	wutree_set_finder(tree, dict_word_finder);

	dict = (DICT *)malloc(sizeof(DICT));
	dict->word_tree = tree;
	dict->name = name;
	trace_info("dict created: %s", dict->name);

	return dict;
}
/*
 * destory DICT
 */
void dict_destroy (DICT *dict)
{
	trace_info("dict destroied: %s", dict->name);
	wutree_destroy(dict->word_tree);
	free(dict);
}
WORD_ITEM * dict_find_worditem (DICT *dict, char *word)
{
	WuTree *tree = dict->word_tree;
	WuTreeNode *node;
	char o;
	WORD_ITEM *worditem = NULL;

	node = tree->root;
	for (o = *word; o; word++, o = *word) {
		node = wutree_find_in_sons(tree, node, (void *)(int)o);
		if (node) {
			continue;
		}
		break;
	}
	if (node) {
		worditem = (WORD_ITEM *)node->ext_data;
	}

	return worditem;
}
int dict_register_worditem (DICT *dict, WORD_ITEM *worditem)
{
	WuTree *tree = dict->word_tree;
	WuTreeNode *node, *temp_node, *added = NULL;
	char o, *word = worditem->word;

	node = tree->root;
	for (o = *word; o; word++, o = *word) {
		temp_node = node;
		node = wutree_find_in_sons(tree, node, (void *)(int)o);
		if (node) {
			continue;
		}
		added = wutree_node_pool_get_node(tree);
		if (!added) {
			trace_err("wutree node pool empty, expand it!");
			return -1;
		}
		added->data = (void *)(int)o;
		wutreenode_add_littleson(temp_node, added);
		node = added;
	}
	if (added) {
		added->ext_data = worditem;
		added->left_leaf = tree->cur_leaf;
		if (tree->cur_leaf) {
			tree->cur_leaf->right_leaf = added;
		}
		tree->cur_leaf = added;
	}
	
	return 0;
}

