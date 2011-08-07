#ifndef _WU_BITMAP_H_
#define _WU_BITMAP_H_

#include "wu_base_type.h"


typedef struct wu_bitmap_t {
	uint8_t bytes[0];
} wu_bitmap_t;

#define WU_BITMAP_DECLARE(name, nbits)		\
	wu_bitmap_t name; uint8_t name##_bytes[nbits / 8];

#define wu_bitmap_set_bit(bmp, bit) \
	bmp.bytes[bit / 8] |= (1 << (bit % 8))

#define wu_bitmap_clear_bit(bmp, bit) \
	bmp.bytes[bit / 8] &= ~(1 << (bit % 8))

#define wu_bitmap_test_bit(bmp, bit) \
	((bmp.bytes[bit / 8] & (1 << (bit % 8))) ? 1 : 0)


#endif /* _WU_BITMAP_H_ */

