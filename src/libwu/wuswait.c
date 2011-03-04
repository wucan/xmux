#include "wuswait.h"
#include "lock.h"


void wu_swait_init(WuSWait *swait)
{
	lock_init(&swait->lock);
	swait->alive = 0;
	swait->setted = 0;
}

void wu_swait_destroy(WuSWait *swait)
{
	lock_destroy(&swait->lock);
}

void * wu_swait_timedwait(WuSWait *swait, int usec)
{
	void *data = NULL;
	int cnt = usec / 10000;

	if (cnt <= 0)
		cnt = 1;

	lock_lock(&swait->lock);
	swait->alive = 1;
	swait->setted = 0;
	lock_unlock(&swait->lock);

	while (cnt > 0) {
		if (swait->setted) {
			break;
		}
		usleep(10000);
	}

	lock_lock(&swait->lock);
	swait->alive = 0;
	swait->setted = 0;
	lock_unlock(&swait->lock);

	return swait->data;
}

int wu_swait_wakeup(WuSWait *swait, void *data)
{
	if (!swait->alive || swait->setted)
		return -1;

	lock_lock(&swait->lock);
	swait->data = data;
	swait->setted = 1;
	lock_unlock(&swait->lock);

	return 0;
}

int wu_swait_is_alive(WuSWait *swait)
{
	int alive = 0;

	lock_lock(&swait->lock);
	alive = swait->alive;
	lock_unlock(&swait->lock);

	return alive;
}

