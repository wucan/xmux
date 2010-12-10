#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>


typedef int (*thread_func) (void *data);

typedef struct _Thread {
	pthread_t tid;
	void *data;
	thread_func func;
} Thread;

Thread * thread_create (thread_func func, void *data);
void thread_join (Thread *thr);
void thread_free(Thread *thr);


#endif /* _THREAD_H_ */

