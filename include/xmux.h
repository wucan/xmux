#ifndef _XMUX_H_
#define _XMUX_H_

#include "wu/wu_base_type.h"
#include "mpegts.h"


#define SECTION_MAX_SIZE				1024

#define SDT_SECTION_NUM					5
#define EIT_SECTION_NUM					8

/*
 * configure macros
 */
#define CHANNEL_MAX_NUM					8

#define PROGRAM_NAME_SIZE				33
#define PROGRAM_MAX_NUM					32
#define PROGRAM_DATA_PID_MAX_NUM		6
// data pid + pcr pid + pmt pid
#define PROGRAM_PID_MAX_NUM				(2 + PROGRAM_DATA_PID_MAX_NUM)
#define CHANNEL_PID_MAX_NUM				(PROGRAM_MAX_NUM * PROGRAM_PID_MAX_NUM)


/*
 * front panel
 */
#define FP_RECV_MSG_MAX_SIZE			120
#define FP_MSG_CRC_SIZE					1

/*
 * FPGA pid map table max PID number per channel
 */
#define FPGA_PID_MAP_TABLE_CHAN_PIDS	0x20

/*
 * management mode: snmp or front panel
 */
enum {
	MANAGEMENT_MODE_SNMP = 0,
	MANAGEMENT_MODE_FP,
};
extern int management_mode;

/*
 * input/program relay table, 25904B
 */
struct program_relay_entry {
	uint16_t prog_num;
	uint16_t pmt_pid[2];
	uint16_t pcr_pid[2];
	uint16_t data_pids[2][PROGRAM_DATA_PID_MAX_NUM]; // zero terminated
	char prog_name[2][PROGRAM_NAME_SIZE];
	uint8_t csc; // include all above field
} __attribute__((packed));

struct xmux_program_relay_table {
	uint8_t chan_id;
	uint8_t nprogs;
	uint32_t status;
	struct program_relay_entry progs[PROGRAM_MAX_NUM];
} __attribute__((packed));

#define PROGRAM_RELAYABLE(status, prog_id)		(status & (1 << prog_id))
#define DATA_PID_TYPE(data_pid_value)			(data_pid_value >> 13)

#define DATA_PID_TYPE_VIDEO		0x1
#define DATA_PID_TYPE_AUDIO		0x2
#define DATA_PID_TYPE_DATA		0x3

static inline bool pcr_video_same(uint16_t pcr_pid_value)
{
	if ((pcr_pid_value >> 13) == 0x4) {
		return true;
	}
	return false;
}
static inline bool pcr_audio_same(uint16_t pcr_pid_value)
{
	if ((pcr_pid_value >> 13) == 0x5) {
		return true;
	}
	return false;
}

#define DATA_PID_PAD_VALUE			0x000F
#define PID_NO_PAD_VALUE			0x00FF

/*
 * PID relay table, 1024B
 */
struct pid_relay_entry {
	uint16_t input_pid;
	uint16_t output_pid;
};
struct channel_pid_relay_table {
	struct pid_relay_entry ents[PROGRAM_MAX_NUM];
};
struct xmux_pid_relay_table {
	struct channel_pid_relay_table chans[CHANNEL_MAX_NUM];
};
#define DEFAULT_RELAY_PID			0x000F

/*
 * output psi/si ts packets, 8KB
 */
#define OUTPUT_PSI_TYPE_MAX_NUM		8
struct output_psi_data_entry {
	uint8_t offset;
	uint8_t nr_ts_pkts;
} __attribute__((packed));
struct xmux_output_psi_data {
	struct output_psi_data_entry psi_ents[OUTPUT_PSI_TYPE_MAX_NUM];
	struct mpeg_ts_packet ts_pkts[0];
} __attribute__((packed));

/*
 * system parameter
 */
struct xmux_system_param {
	uint32_t output_bitrate;
	uint8_t format;
	uint8_t vendor_info[8];
	uint32_t version;
	uint32_t update_time;
} __attribute__((packed));
#define FORMAT_BC			0xBC
#define FORMAT_CC			0xCC

/*
 * net parameter
 */
struct xmux_net_param {
	uint32_t server_ip;
	uint8_t mac[6];
	uint32_t ip;
	uint32_t netmask;
	uint32_t gateway;
	uint8_t csc;
} __attribute__((packed));

/*
 * user parameter
 */
struct xmux_user_param {
	uint8_t user_len;
	char user[16];
	uint8_t password_len;
	char password[16];
	uint8_t csc;
	uint8_t reserved;
} __attribute__((packed));

/*
 * EEPROM parameter layout:
 *   struct xmux_program_relay_table prog_relay_table;
 *   struct xmux_pid_relay_table pid_relay_table;
 *   struct xmux_output_psi_data output_psi;
 *   struct xmux_system_param sys;
 *   struct xmux_net_param net;
 *   struct xmux_user_param user;
 */

struct xmux_root_param {
	union {
		uint8_t bytes[25904];
		struct xmux_program_relay_table prog_relay_table;
	} prog_relay_table_area;
	union {
		uint8_t bytes[1024];
		struct xmux_pid_relay_table pid_relay_table;
	} pid_relay_table_area;
	union {
		uint8_t bytes[1024 * 8];
		struct xmux_output_psi_data output_psi;
	} output_psi_area;
	struct xmux_system_param sys;
	struct xmux_net_param net;
	struct xmux_user_param user;
};


#endif /* _XMUX_H_ */

