#ifndef _XMUX_H_
#define _XMUX_H_

#include "wu/wu_base_type.h"
#include "mpegts.h"


/*
 * EEPROM size
 */
#define EEPROM_SIZE						(1024 * 128)
#define EEPROM_PAGE_SIZE				64

#define SECTION_MAX_SIZE				1024

#define SDT_SECTION_NUM					5
#define EIT_SECTION_NUM					8

/*
 * configure macros
 */
#define CHANNEL_MAX_NUM					8

#define PROGRAM_NAME_SIZE				33
#define PROGRAM_MAX_NUM					32
#define PROGRAM_DATA_PID_MAX_NUM		14
// data pid + pcr pid + pmt pid
#define PROGRAM_PID_MAX_NUM				(2 + PROGRAM_DATA_PID_MAX_NUM)
#define CHANNEL_PID_MAX_NUM				(PROGRAM_MAX_NUM * PROGRAM_PID_MAX_NUM)


/*
 * front panel
 */
#define FP_RECV_MSG_MAX_SIZE			1200
#define FP_MSG_CRC_SIZE					1

/*
 * FPGA pid map table max PID number per channel
 */
#define FPGA_PID_MAP_TABLE_CHAN_PIDS	0x20

/*
 * management mode: snmp or front panel
 */
enum {
	MANAGEMENT_MODE_SNMP = 0x55,
	MANAGEMENT_MODE_FP = 0xAA,
};
extern uint8_t management_mode;
static inline const char *management_mode_str(mode)
{
	if (mode == MANAGEMENT_MODE_SNMP)
		return "SNMP";

	return "FP";
}

/*
 * snmp client login stats
 */
enum {
	SNMP_LOGIN_STATUS_SUCCESS = 0,
	SNMP_LOGIN_STATUS_BUSY,
	SNMP_LOGIN_STATUS_USER_NAME_INVALID,
	SNMP_LOGIN_STATUS_PASSWORD_INVALID,
	SNMP_LOGIN_STATUS_IDLE,
};

/*
 * pid trans info table
 * snmp node 5188.12
 */
struct pid_trans_entry {
	uint8_t type;
	uint16_t in;
	uint16_t out;
} __attribute__ ((__packed__));
struct xmux_program_info {
	uint16_t prog_num;

	/*
	 * pid
	 */
	struct pid_trans_entry pmt;
	struct pid_trans_entry pcr;
	struct pid_trans_entry data[PROGRAM_DATA_PID_MAX_NUM];

	char prog_name[2][PROGRAM_NAME_SIZE];
} __attribute__ ((__packed__));

struct xmux_program_info_with_csc {
	struct xmux_program_info info;
	uint8_t csc;
} __attribute__ ((__packed__));

struct pid_trans_info_snmp_data {
	uint16_t data_len;

	uint8_t update_flag_and_chan_num;
	uint8_t nprogs;
	uint32_t status;
	struct xmux_program_info_with_csc programs[PROGRAM_MAX_NUM];
} __attribute__((packed));
#define PID_TRANS_INFO_SIZE			sizeof(struct pid_trans_info_snmp_data)

#define PROGRAM_SELECTED(status, prog_id)		(status & (1 << prog_id))
#define SELECT_PROGRAM(info, prog_idx) \
	(info->status |= (1 << prog_idx))
#define DESELECT_PROGRAM(info, prog_idx) \
	(info->status &= ~(1 << prog_idx))

/*
 * FIXME: xmux define pid type for pcr, pmt, pcr_audio, pcr=video, pid=pad
 */
enum {
	PID_TYPE_PMT = 0x80,
	PID_TYPE_PCR = 0x81,
	PID_TYPE_PCR_VIDEO = 0x82,
	PID_TYPE_PCR_AUDIO = 0x83,
	PID_TYPE_PAD = 0x84,
};

static inline bool es_is_video(int es_type)
{
	switch (es_type) {
		case 0x01:
		case 0x02:
		case 0x1b:
			return true;
		default:
			break;
	}

	return false;
}

#define DATA_PID_PAD_VALUE			0x000F
#define PID_NO_PAD_VALUE			0x00FF
static inline bool data_pid_validate(uint8_t type)
{
	if (type == PID_TYPE_PAD) {
		return false;
	}

	return true;
}

/*
 * PID map table, 1024B
 */
struct pid_map_entry {
	uint16_t input_pid;
	uint16_t output_pid;
};
struct channel_pid_map_table {
	struct pid_map_entry ents[FPGA_PID_MAP_TABLE_CHAN_PIDS];
};
struct xmux_pid_map_table {
	struct channel_pid_map_table chans[CHANNEL_MAX_NUM];
};
#define PID_MAP_TABLE_PAD_PID			0x000F

/*
 * output psi/si ts packets, 8KB
 */
#define OUTPUT_PSI_AREA_SIZE		(1024 * 8)
#define OUTPUT_PSI_TYPE_MAX_NUM		8
struct output_psi_data_entry {
	uint8_t offset;
	uint8_t nr_ts_pkts;
} __attribute__((packed));
struct xmux_output_psi_data {
	struct output_psi_data_entry psi_ents[OUTPUT_PSI_TYPE_MAX_NUM];
	struct mpeg_ts_packet ts_pkts[0];
} __attribute__((packed));
#define OUTPUT_PSI_TS_DATA_SIZE \
	(OUTPUT_PSI_AREA_SIZE - offsetof(struct xmux_output_psi_data, ts_pkts))
#define OUTPUT_PSI_PACKET_MAX_NUM \
	(OUTPUT_PSI_TS_DATA_SIZE / TS_PACKET_BYTES)

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
#define XMUX_DEFAULT_OUTPUT_BITRATE			38000

#define FORMAT_BC			0xBC
#define FORMAT_CC			0xCC

#define XMUX_VENDOR			"SOUKA"

#define XMUX_VERSION_MAJOR	0
#define XMUX_VERSION_MINOR	5
#define XMUX_VERSION_NUM	((XMUX_VERSION_MAJOR << 16) | XMUX_VERSION_MINOR)
#define XMUX_VERSION_STR	"0.05"

#define XMUX_UPDATE_TIME	((2011 << 16) | (1 << 8) | 22)

/*
 * net parameter
 */
struct xmux_net_param {
	uint32_t server_ip;
	uint32_t ip;
	uint32_t netmask;
	uint32_t gateway;
	uint8_t mac[6];
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
 * output mux program info
 */
struct xmux_mux_program_info {
	uint16_t nprogs;

	struct mux_program_entry {
		uint8_t chan_idx;
		uint8_t prog_idx;
	} programs[PROGRAM_MAX_NUM];
} __attribute__ ((__packed__));

/*
 * EEPROM parameter layout:
 *   struct xmux_program_map_table prog_map_table;
 *   struct pid_trans_info_snmp_data pid_trans_info;
 *   struct xmux_output_psi_data output_psi;
 *   struct xmux_system_param sys;
 *   struct xmux_net_param net;
 *   struct xmux_user_param user;
 *   management_mode;
 *	 struct xmux_mux_program_info mux_prog_info;
 */

#define EEPROM_PAGE_ALIGN	 __attribute__ ((aligned(EEPROM_PAGE_SIZE)))
struct xmux_eeprom_param {
	union {
		uint8_t bytes[25904];
		struct pid_trans_info_snmp_data pid_trans_info[CHANNEL_MAX_NUM];
	} pid_trans_info_area EEPROM_PAGE_ALIGN;
	union {
		uint8_t bytes[1024];
		struct xmux_pid_map_table pid_map_table;
	} pid_map_table_area EEPROM_PAGE_ALIGN;
	union {
		uint8_t bytes[1024 * 8];
		struct xmux_output_psi_data output_psi;
	} output_psi_area EEPROM_PAGE_ALIGN;
	struct xmux_system_param sys EEPROM_PAGE_ALIGN;
	struct xmux_net_param net EEPROM_PAGE_ALIGN;
	struct xmux_user_param user EEPROM_PAGE_ALIGN;
	uint8_t mng_mode EEPROM_PAGE_ALIGN;

	struct xmux_mux_program_info mux_prog_info EEPROM_PAGE_ALIGN;
};

/*
 * param address offset in eeprom
 */
#define EEPROM_OFF_PID_TRANS_INFO			0
#define EEPROM_OFF_PID_MAP_TABLE			(offsetof(struct xmux_eeprom_param, pid_map_table_area))
#define EEPROM_OFF_OUTPUT_PSI				(offsetof(struct xmux_eeprom_param, output_psi_area))
#define EEPROM_OFF_SYS						(offsetof(struct xmux_eeprom_param, sys))
#define EEPROM_OFF_NET						(offsetof(struct xmux_eeprom_param, net))
#define EEPROM_OFF_USER						(offsetof(struct xmux_eeprom_param, user))
#define EEPROM_OFF_MNG_MODE					(offsetof(struct xmux_eeprom_param, mng_mode))
#define EEPROM_OFF_MUX_PROG_INFO			(offsetof(struct xmux_eeprom_param, mux_prog_info))

#define EEPROM_OFF_SYS_OUTPUT_BITRATE \
	(EEPROM_OFF_SYS + (offsetof(struct xmux_system_param, output_bitrate)))
#define EEPROM_OFF_SYS_PACKET_FORMAT \
	(EEPROM_OFF_SYS + (offsetof(struct xmux_system_param, format)))

struct xmux_param_management_info {
	uint32_t eeprom_pid_trans_info_version;
	uint32_t eeprom_pid_map_table_version;

	uint32_t snmp_pid_trans_info_version;
	uint32_t snmp_pid_map_table_version;
};
extern struct xmux_param_management_info g_param_mng_info;

void xmux_program_info_dump(struct xmux_program_info *prog);


#endif /* _XMUX_H_ */

