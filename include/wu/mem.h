#ifndef _MEM_H_
#define _MEM_H_

#include <stdlib.h>
#ifdef _UCLINUX_
#include <string.h>
#endif


#ifdef _UCLINUX_
static inline int mem_init_size (size_t size)
{
	return 0;
}
static inline void * mem_malloc (size_t size)
{
	return malloc(size);
}
static inline void * mem_mallocz (size_t size)
{
	void *p = malloc(size);
	if (p)
		memset(p, 0, size);
	return p;
}
static inline void mem_free (void *buf)
{
	free(buf);
}
#else
int mem_init_size (size_t size);
void * mem_malloc (size_t size);
void * mem_mallocz (size_t size);
void mem_free (void *buf);
#endif


#endif /* _MEM_H_ */

