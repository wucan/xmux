#include <unistd.h>
#include <stdlib.h>

#include "lock.h"


int lock_init (Lock *lock)
{
	return pthread_mutex_init(&lock->mutex, NULL);
}
void lock_destroy (Lock *lock)
{
	pthread_mutex_destroy(&lock->mutex);
}
void lock_lock (Lock *lock)
{
	pthread_mutex_lock(&lock->mutex);
}
void lock_unlock (Lock *lock)
{
	pthread_mutex_unlock(&lock->mutex);
}

