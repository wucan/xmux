#ifndef _WU_CONVERT_H_
#define _WU_CONVERT_H_

#include <stdint.h>


// 0027 4567 BCD -> 0x00274567
static inline uint32_t wu_bcd2hex(uint32_t bcd)
{
	int i;
	uint32_t hex = 0, div = bcd, rem;

	for (i = 0; i < 8; i++) {
		rem = div % 10;
		rem <<= 4 * i;
		hex |= rem;
		div = div / 10;
	}

	return hex;
}

#endif /* _WU_CONVERT_H_ */

