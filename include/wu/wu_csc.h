#include <stdint.h>


/*
 * calculate check sum
 */
static inline uint8_t wu_csc(void *buf, int len)
{
	int i;
	uint8_t csc = 0;
	uint8_t *p = (uint8_t *)buf;

	for (i = 0; i < len; i++)
		csc += p[i];

	return csc;
}

