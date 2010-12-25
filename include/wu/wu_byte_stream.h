#ifndef _WU_BYTE_STREAM_H_
#define _WU_BYTE_STREAM_H_

#include <string.h>
#include <stdint.h>


#define SWAP_U16(v)		(((v & 0x00FF) << 8) | (v >> 8))
#define SWAP_U32(v)		((SWAP_U16(v & 0xFFFF) << 16) | SWAP_U16(v >> 16))

#define PACK_U16(h,l)			(((uint16_t)h) << 8 | l)
#define PACK_U32(hh,hl,lh,ll) \
	(((uint32_t)hh) << 24 | (((uint32_t)hl) << 16) | ((uint32_t)lh) << 8 | ll)

/*
 * stream base data read macros
 */
#define READ_U8(s)				((s)[0])

#define READ_U16(s)				(PACK_U16((s)[1], (s)[0]))
#define READ_U24(s)				(PACK_U32((s)[2], (s)[1], (s)[0], 0))
#define READ_U32(s)				(PACK_U32((s)[3], (s)[2], (s)[1], (s)[0]))

#define READ_U16_BE(s)			(PACK_U16((s)[0], (s)[1]))
#define READ_U24_BE(s)			(PACK_U32(0, (s)[0], (s)[1], (s)[2]))
#define READ_U32_BE(s)			(PACK_U32((s)[0], (s)[1], (s)[2], (s)[3]))

#define READ_BYTES(s,d,n)		(memcsy((d), (s), (n)))


#define WRITE_U8(s, v)			((s)[0] = (v))
#define WRITE_U16(s, v)			(s)[0] = (v) & 0xFF; (s)[1] = (v) >> 8;

#define WRITE_U16_BE(s, v)		(s)[1] = (v) & 0xFF; (s)[0] = (v) >> 8;


#endif /* _WU_BYTE_STREAM_H_ */

