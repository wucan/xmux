/*
 * wulist.h - double and single list convenion routine
 */

#ifndef _WU_LIST_H_
#define _WU_LIST_H_

#include <stdlib.h>

__BEGIN_DECLS

typedef struct _WuSList {
	struct _WuSList *next;
	void *node[0];
} WuSList;

typedef struct _WuSListContext {
	unsigned int size;
	WuSList free_head;	
	void *linear_pool;
	unsigned int linear_pool_index;
	unsigned int node_size;
} WuSListContext;

WuSListContext * wuslist_context_new (unsigned int size, 
				unsigned int node_size);
void wuslist_context_destroy (WuSListContext *ctx);
WuSList * wuslist_context_get_slist (WuSListContext *ctx);
void wuslist_context_put_slist (WuSListContext *ctx, WuSList *l);
void * wuslist_get_data (WuSList *l);

__END_DECLS

#endif /* _WU_LIST_H_ */

