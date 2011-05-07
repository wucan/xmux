#include <string.h>

#include "wu/wu_base_type.h"
#include "mpegts.h"


int section_to_ts_length(int sec_len)
{
	int ts_len = sec_len + 3 + 1;
	int num = ts_len / 184;

	if ((ts_len % 184) > 0)
		num +=1;
	ts_len = num * 188;

	return ts_len;
}
/*
 * Convert section data to ts packet
 *
 * @sec_data: section data
 * @len: section data len
 * @ts_buf: out ts
 * Return output ts len
 */
int section_to_ts(uint8_t *sec_data, int sec_len, uint8_t *ts_buf,
					 uint16_t pid, uint8_t *p_cc)
{
	struct mpeg_ts_header ts_head;
	unsigned int itotal_leave = sec_len;
	int pkt_cnt = 0;
	uint8_t cc = *p_cc;
	int tslen = 0;

	while (itotal_leave > 0) {
		/*
		 * setup header
		 */
		ts_head.sync_byte = 0x47;
		ts_head.pid_hi = pid >> 8;
		ts_head.transport_priority = 0;
		ts_head.payload_unit_start_indicator = ((pkt_cnt == 0) ? 1 : 0);
		ts_head.transport_error_indicator = 0;

		ts_head.pid_lo = (pid << 24) >> 24;

		ts_head.continuity_counter = cc;
		ts_head.adaptation_field_control = 0x01;
		ts_head.transport_scrambling_control = 0x00;

		memcpy(ts_buf, &ts_head, TS_HEAD_LEN);
		ts_buf += TS_HEAD_LEN;

		/*
		 * the first packet had a header 0x00
		 */
		if (pkt_cnt == 0) {
			*ts_buf = 0x00;
			ts_buf++;
		}

		if ((pkt_cnt == 0 && itotal_leave >= TS_PACKET_BYTES - TS_HEAD_LEN - 1)
			|| (pkt_cnt != 0 && itotal_leave >= TS_PACKET_BYTES - TS_HEAD_LEN)) {
			if (pkt_cnt == 0) {
				memcpy(ts_buf, sec_data, TS_PACKET_BYTES - TS_HEAD_LEN - 1);
				sec_data += TS_PACKET_BYTES - TS_HEAD_LEN - 1;
				itotal_leave =
					itotal_leave - TS_PACKET_BYTES + TS_HEAD_LEN + 1;
				ts_buf += TS_PACKET_BYTES - TS_HEAD_LEN - 1;
			} else {
				memcpy(ts_buf, sec_data, TS_PACKET_BYTES - TS_HEAD_LEN);
				sec_data += TS_PACKET_BYTES - TS_HEAD_LEN;
				itotal_leave = itotal_leave - TS_PACKET_BYTES + TS_HEAD_LEN;
				ts_buf += TS_PACKET_BYTES - TS_HEAD_LEN;
			}
		} else {
			if (pkt_cnt == 0) {
				memset(ts_buf, 0xff, TS_PACKET_BYTES - TS_HEAD_LEN - 1);
				memcpy(ts_buf, sec_data, itotal_leave);

			} else {
				memset(ts_buf, 0xff, TS_PACKET_BYTES - TS_HEAD_LEN);
				memcpy(ts_buf, sec_data, itotal_leave);
			}

			sec_data += itotal_leave;
			itotal_leave = 0;
			ts_buf += TS_PACKET_BYTES - TS_HEAD_LEN;
		}

		tslen += TS_PACKET_BYTES;

		pkt_cnt++;
		// section had max data size 4096 bytes(23 packet)
		if (pkt_cnt > 22)
			pkt_cnt = 0;

		cc = (++cc) & 0x0F;
	}

	*p_cc = cc;

	return tslen;
}

/*
 * extract section from ts, only for one ts packet section
 */
int ts_to_section(uint8_t *ts, uint8_t *sec)
{
	uint16_t sec_len;

	if (ts[0] != 0x47)
		return -1;
	sec_len = (ts[7] & 0x0F) | ts[8];
	if (sec_len > (TS_PACKET_BYTES - 8)) {
		return -1;
	}

	memcpy(sec, ts + 8, sec_len + 3);

	return sec_len;
}

