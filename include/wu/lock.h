#ifndef _LOCK_H_
#define _LOCK_H_

#include <pthread.h>


typedef struct _Lock {
	pthread_mutex_t mutex;
} Lock;

int lock_init (Lock *lock);
void lock_destroy (Lock *lock);
void lock_lock (Lock *lock);
void lock_unlock (Lock *lock);


#endif /* _LOCK_H_ */

