#ifndef _XMUX_SNMP_INTSTR_H_
#define _XMUX_SNMP_INTSTR_H_

#include "wu/wu_base_type.h"

#include "xmux.h"


/*
 * 5188.11
 */
struct pid_trans_snmp_data {
	uint16_t data_len;
	uint32_t output_bitrate;
	uint8_t format;
	struct xmux_pid_map_table table;
} __attribute__((packed));
#define PID_TRANS_SIZE		sizeof(struct pid_trans_snmp_data)

/*
 * 5188.13
 */
struct ip_info_snmp_data {
	uint16_t len;
	uint32_t server_ip;
	uint32_t ip;
	uint32_t netmask;
	uint32_t gateway;
	uint8_t mac[6];
} __attribute__((packed));
#define IP_INFO_SIZE		(sizeof(struct ip_info_snmp_data))


#endif /* _XMUX_SNMP_INTSTR_H_ */

