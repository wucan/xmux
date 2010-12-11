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

/*
 * 5188.12
 */
struct pid_trans_entry {
	uint16_t in;
	uint16_t out;
};
struct pid_trans_info_program_snmp_data {
	uint16_t prog_num;

	/*
	 * pid
	 */
	struct pid_trans_entry pmt;
	struct pid_trans_entry pcr;
	struct pid_trans_entry data[PROGRAM_DATA_PID_MAX_NUM];

	char prog_name[2][PROGRAM_NAME_SIZE];
} __attribute__ ((__packed__));
struct pid_trans_info_snmp_data {
	uint16_t data_len;

	uint8_t update_flag_and_chan_num;
	uint8_t nprogs;
	uint32_t status;
	struct pid_trans_info_program_snmp_data programs[PROGRAM_MAX_NUM];

	uint8_t csc;
} __attribute__((packed));
#define PID_TRANS_INFO_SIZE			sizeof(struct pid_trans_info_snmp_data)

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

