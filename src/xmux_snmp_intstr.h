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
	uint8_t reserve;
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

struct user_param_snmp_data {
	uint8_t user_len;
	char user[16];
	uint8_t password_len;
	char password[16];
} __attribute__((packed));
#define USER_INFO_SIZE			sizeof(struct user_param_snmp_data)

struct heart_device_snmp_data {
	uint16_t flag; // SNMP_LOGIN_STATUS_XXX
	uint8_t lock_state;
	uint8_t mode;
} __attribute__((packed));
#define HEART_DEVICE_SIZE			sizeof(struct heart_device_snmp_data)

struct apply_psi_snmp_data {
	uint16_t magic; // 0xAA55
	uint8_t other_table_flag;
	uint32_t pmt_flag;
	uint16_t pmt_pid_table[PROGRAM_MAX_NUM];
} __attribute__((packed));
#define PAT_FLAG_SHIFT		7
#define CAT_FLAG_SHIFT		6
#define NIT_FLAG_SHIFT		5
#define SDT_FLAG_SHIFT		4


#endif /* _XMUX_SNMP_INTSTR_H_ */

