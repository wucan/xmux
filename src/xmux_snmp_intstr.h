#ifndef _XMUX_SNMP_INTSTR_H_
#define _XMUX_SNMP_INTSTR_H_

#include "wu/wu_base_type.h"


/*
 * 5188.11
 */
struct pid_trans_snmp_data {
	uint16_t data_len;
	uint32_t output_bitrate;
	uint8_t format;
	uint8_t pid_trans_table[1024];
} __attribute__((packed));


#endif /* _XMUX_SNMP_INTSTR_H_ */

