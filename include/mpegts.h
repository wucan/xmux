#ifndef _MPEGTS_H_
#define _MPEGTS_H_

#include "wu/wu_base_type.h"


/*
 * mpeg ts defines
 */
#define TS_PACKET_BYTES				188
#define TS_HEAD_LEN					4
#define TS_DATA_LEN					180

/*
 * PSI/SI PID value
 */
#define NIT_PID				0x10

struct mpeg_ts_header {
	uint32_t sync_byte                        : 8;    /* should be 0x47 */
	uint32_t pid_hi                           : 5;
	uint32_t transport_priority               : 1;
	uint32_t payload_unit_start_indicator     : 1;
	uint32_t transport_error_indicator        : 1;
	uint32_t pid_lo                           : 8;
	uint32_t continuity_counter               : 4;
	uint32_t adaptation_field_control         : 2;
	uint32_t transport_scrambling_control     : 2;
};

struct mpeg_ts_packet {
	uint8_t bytes[TS_PACKET_BYTES];
};

/*
 * get pid from ts packet
 */
#define GET_PID(bs)			(((bs[1] & 0x1F) << 5) | bs[2])


#endif /* _MPEGTS_H_ */

