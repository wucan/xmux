#include <stdlib.h>
#include <string.h>
#include "wulist.h"


WuSListContext * wuslist_context_new (unsigned int size, 
				unsigned int node_size)
{
	WuSListContext *ctx;

	ctx = malloc(sizeof(*ctx));
	if (!ctx) {
		return NULL;
	}
	memset(ctx, 0, sizeof(*ctx));
	ctx->linear_pool = malloc((sizeof(WuSList) + node_size) * size);
	if (!ctx->linear_pool) {
		free(ctx);
		return NULL;
	}
	memset(ctx->linear_pool, 0, (sizeof(WuSList) + node_size) * size);

	/* fill ctx */
	ctx->size = size;
	ctx->node_size = node_size;
	ctx->linear_pool_index = 0;
	ctx->free_head.next = &ctx->free_head;

	return ctx;
}
void wuslist_context_destroy (WuSListContext *ctx)
{
	free(ctx->linear_pool);
	free(ctx);
}
WuSList * wuslist_context_get_slist (WuSListContext *ctx)
{
	WuSList *l = NULL;

	if (ctx->linear_pool_index < ctx->size) {
		l = (WuSList *) (ctx->linear_pool + 
			ctx->linear_pool_index * (sizeof(WuSList) + ctx->node_size));
		ctx->linear_pool_index++;
	} else if (ctx->free_head.next != &ctx->free_head) {
		l = ctx->free_head.next;
		ctx->free_head.next = l->next;
	}

	return l;
}

void wuslist_context_put_slist (WuSListContext *ctx, WuSList *l)
{
	l->next = ctx->free_head.next;
	ctx->free_head.next = l;
}
inline void * wuslist_get_data (WuSList *l)
{
	return l->node;
}

