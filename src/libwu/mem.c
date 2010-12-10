#ifndef _UCLINUX_
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "bget.h"
#include "lock.h"
#include "mem.h"


static Lock lock;
static void *pool;
static int lock_cnt;

int mem_init_size (size_t size)
{
	void *bp;
	
	bp = malloc(size);
	if (!bp) {
		printf("mem: cannot alloc mem pool! size %d\n", size);
		return -1;
	}
	bpool(bp, (bufsize)size);
	pool = bp;
	lock_init(&lock);

	return 0;
}
void * mem_malloc (size_t size)
{
	void *buf;

	lock_lock(&lock);
	if (lock_cnt) {
		printf("mem_malloc: lock bug, cnt %d\n", lock_cnt);
		while (lock_cnt) {
			;
		}
	}
	lock_cnt++;
	buf = bget((bufsize)size);
	if (!buf) {
		printf("mem: cannnot alloc %d mem!\n", size);
	}
	lock_cnt--;
	lock_unlock(&lock);

	return buf;
}
void * mem_mallocz (size_t size)
{
	void *buf;

	lock_lock(&lock);
	if (lock_cnt) {
		printf("mem_mallocz: lock bug, cnt %d\n", lock_cnt);
		while (lock_cnt) {
			;
		}
	}
	lock_cnt++;
	buf = bget((bufsize)size);
	lock_cnt--;
	lock_unlock(&lock);
	if (buf) {
		memset(buf, 0, size);
	}
	if (!buf) {
		printf("mem: cannnot allocz %d mem!\n", size);
	}

	return buf;
}
void mem_free (void *buf)
{
	lock_lock(&lock);
	if (lock_cnt) {
		printf("mem_free: lock bug, cnt %d\n", lock_cnt);
		while (lock_cnt) {
			;
		}
	}
	lock_cnt++;
	brel(buf);
	lock_cnt--;
	lock_unlock(&lock);
}
#endif

