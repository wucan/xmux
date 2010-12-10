#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_

#include <pthread.h>

#include "list.h"


enum {
	MB_TYPE_DATA = 0,
	MB_TYPE_CONTROL,
	MB_TYPE_QUIT,
};

typedef struct _MessageBlock {
	struct list_head list;
	/*
	 * the message type: MB_TYPE_XXX
	 */
	int type;
	void *data;
} MessageBlock;

typedef struct _MessageQueue {
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	struct list_head mblist;
} MessageQueue;

#define MQ_WAIT_FOREVER		(-1)


MessageBlock * message_block_new (void *data);
void message_block_release (MessageBlock *mb);
void message_block_set_data (MessageBlock *mb, void *data);
void * message_block_get_data (MessageBlock *mb);
MessageQueue * message_queue_create ();
void message_queue_destroy (MessageQueue *mq);
MessageBlock * message_queue_get_message_block (MessageQueue *mq, int timeout);
int message_queue_put_message_block (MessageQueue *mq, MessageBlock *mb);


#endif /* _MESSAGE_QUEUE_H_ */

