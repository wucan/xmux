#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

#include "message_queue.h"
#include "mem.h"
#include "up_config.h"


#ifdef MB_TRACE
#define MB_MEM_STATIC_POOL_SIZE		100
struct mb_mem_static {
	MessageBlock *mb;
	int used;
};
static struct mb_mem_static mbms[MB_MEM_STATIC_POOL_SIZE];
static int mbcnt;

static void mbms_dump ()
{
	int i;

	printf("mb pool info, there %d mbs:\n", mbcnt);
	for (i = 0; i < MB_MEM_STATIC_POOL_SIZE; i++) {
		if (mbms[i].used) {
			printf("#%d: mb %p\n", i, mbms[i].mb);
		}
	}
}
static void mbms_add (MessageBlock *mb)
{
	int i;

	printf("add mb %p, mbcnt %d\n", mb, mbcnt);
	for (i = 0; i < MB_MEM_STATIC_POOL_SIZE; i++) {
		if (!mbms[i].used) {
			mbms[i].mb = mb;
			mbms[i].used = 1;
			mbcnt++;
			break;
		}
	}
	if (i == MB_MEM_STATIC_POOL_SIZE) {
		printf("add mb %p, mbms fulllllllll\n", mb);
	}
}
static void mbms_remove (MessageBlock *mb)
{
	int i;

	printf("remove mb %p, mbcnt %d\n", mb, mbcnt);
	for (i = 0; i < MB_MEM_STATIC_POOL_SIZE; i++) {
		if (mbms[i].used && mbms[i].mb == mb) {
			mbms[i].used = 0;
			mbcnt--;
			break;
		}
	}
	if (i == MB_MEM_STATIC_POOL_SIZE) {
		printf("mb %p not found\n", mb);
	}
}
#else
static void mbms_dump ()
{
}
static void mbms_add (MessageBlock *mb)
{
}
static void mbms_remove (MessageBlock *mb)
{
}
#endif
int up_usleep (int us)
{
	struct timeval tv;
	int rc;

	tv.tv_sec = 0;
	tv.tv_usec = us;
	rc = select(1, NULL, NULL, NULL, &tv);
	if (rc == 0) {
		return 0;
	}

	return -1;
}
MessageBlock * message_block_new (void *data)
{
	MessageBlock *mb = (MessageBlock *)mem_malloc(sizeof(MessageBlock));
	if (!mb)
		return NULL;
	mb->data = data;
	mbms_add(mb);

	return mb;
}
void message_block_release (MessageBlock *mb)
{
	mbms_remove(mb);
	mem_free(mb);
}
void message_block_set_data (MessageBlock *mb, void *data)
{
	mb->data = data;
}
void * message_block_get_data (MessageBlock *mb)
{
	return mb->data;
}
MessageQueue * message_queue_create ()
{
	MessageQueue *mq;

	mq = (MessageQueue *)mem_malloc(sizeof(MessageQueue));
	if (!mq) {
		return NULL;
	}
	pthread_cond_init(&mq->cond, NULL);
	pthread_mutex_init(&mq->mutex, NULL);
	INIT_LIST_HEAD(&mq->mblist);

	return mq;
}
void message_queue_destroy (MessageQueue *mq)
{
	pthread_cond_destroy(&mq->cond);
	pthread_mutex_destroy(&mq->mutex);
	mem_free(mq);
}
/*
 * dump the message queue's mbs
 */
static void message_queue_dump_mb (MessageQueue *mq)
{
	struct list_head *l;
	MessageBlock *mb;

	printf("message queue %p had mbs:\n", mq);
	list_for_each (l, &mq->mblist) {
		mb = list_entry(l, MessageBlock, list);
		printf("mb: %p\n", mb);
	}
}
MessageBlock * message_queue_get_message_block (MessageQueue *mq, int timeout)
{
	struct timeval now;
	struct timespec to;
	MessageBlock *mb = NULL;
	int rc = 0;
	unsigned long long nsec;

	if (timeout == 0) {
		pthread_mutex_lock(&mq->mutex);
		if (!list_empty(&mq->mblist)) {
			mb = list_entry(mq->mblist.next, MessageBlock, list);
			list_del(mq->mblist.next);
		}
		pthread_mutex_unlock(&mq->mutex);
		return mb;
	} else if (timeout < 0) {
again:
		pthread_mutex_lock(&mq->mutex);
		if (!list_empty(&mq->mblist)) {
			mb = list_entry(mq->mblist.next, MessageBlock, list);
			list_del(mq->mblist.next);
			pthread_mutex_unlock(&mq->mutex);
			return mb;
		} else {
			rc = pthread_cond_wait(&mq->cond, &mq->mutex);
			pthread_mutex_unlock(&mq->mutex);
			goto again;
		}
	}

#if PTHREAD_COND_TIMEDWAIT_OK
	gettimeofday(&now, NULL);
	nsec = (now.tv_usec + (timeout % 1000) * 1000) * 1000;
	to.tv_sec = now.tv_sec + (timeout / 1000) + nsec / 1000000000;
	to.tv_nsec = nsec % 1000000000;
	pthread_mutex_lock(&mq->mutex);
	while (list_empty(&mq->mblist) && rc != ETIMEDOUT) {
		rc = pthread_cond_timedwait(&mq->cond, &mq->mutex, &to);
	}
	if (!list_empty(&mq->mblist)) {
		mb = list_entry(mq->mblist.next, MessageBlock, list);
		list_del(mq->mblist.next);
	}
	pthread_mutex_unlock(&mq->mutex);
#else
	/* sleep every 100 ms, then check the list, so emulate
	 the cond_timedwait function */
	{
		int cnt, i;

		cnt = timeout / 100;
		for (i = 0; i < cnt; i++) {
			pthread_mutex_lock(&mq->mutex);
			if (!list_empty(&mq->mblist)) {
				mb = list_entry(mq->mblist.next, MessageBlock, list);
				list_del(mq->mblist.next);
				pthread_mutex_unlock(&mq->mutex);
				return mb;
			}
			pthread_mutex_unlock(&mq->mutex);
			usleep(100000);
		}
	}
#endif

	return mb;
}
int message_queue_put_message_block (MessageQueue *mq, MessageBlock *mb)
{
	pthread_mutex_lock(&mq->mutex);
	list_add(&mb->list, &mq->mblist);
	pthread_mutex_unlock(&mq->mutex);
	pthread_cond_signal(&mq->cond);

	return 0;
}

