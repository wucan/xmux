#ifndef _WU_SWAIT_H_
#define _WU_SWAIT_H_

#include "lock.h"


typedef struct wu_swait {
	Lock lock;
	int alive;
	int setted;
	void *data;
} WuSWait;

void wu_swait_init(WuSWait *swait);
void wu_swait_destroy(WuSWait *swait);
void * wu_swait_timedwait(WuSWait *swait, int usec);
int wu_swait_wakeup(WuSWait *swait, void *data);
int wu_swait_is_alive(WuSWait *swait);


#endif /* _WU_SWAIT_H_ */

