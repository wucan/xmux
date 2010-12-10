#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "thread.h"


static void * _thread_func_wrap (void *thrdata)
{
	Thread *thr = (Thread *)thrdata;

	thr->func(thr->data);
	free(thr);

	return NULL;
}
Thread * thread_create (thread_func func, void *data)
{
	Thread *thr;
	int rc;

	thr = (Thread *)malloc(sizeof(Thread));
	if (!thr) {
		fprintf(stderr, "thread: cannot not alloc mem!\n");
		return NULL;
	}
	thr->func = func;
	thr->data = data;
	rc = pthread_create(&thr->tid, NULL, _thread_func_wrap, (void *)thr);
	if (rc) {
		fprintf(stderr, "thread: pthread_create error!\n");
		free(thr);
		return NULL;
	}

	return thr;
}
void thread_join (Thread *thr)
{
	pthread_join(thr->tid, NULL);
}
void thread_free(Thread *thr)
{
	free(thr);
}

