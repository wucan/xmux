#include <stdint.h>


/*
 * calculate check sum
 */
static inline uint8_t wu_csc(uint8_t *buf, int len)
{
	int i;
	uint8_t csc = 0;

	for (i = 0; i < len; i++)
		csc += buf[i];

	return csc;
}

