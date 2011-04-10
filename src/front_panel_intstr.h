#ifndef _FRONT_PANEL_INTSTR_H_
#define _FRONT_PANEL_INTSTR_H_

#include "xmux.h"
#include "wu/wu_byte_stream.h"


#define FP_STATUS_SELECTED			(1 << 0)

typedef struct					// output one program to mcu_panel
{
	uint8_t status;

	struct xmux_program_info info;
} __attribute__ ((__packed__)) PROG_INFO_T;
#define FP_PROG_SELECTED(p)		((p)->status & FP_STATUS_SELECTED)
#define FP_SELECT_PROG(p)		((p)->status |= FP_STATUS_SELECTED)
#define FP_DESELECT_PROG(p)		((p)->status &= ~FP_STATUS_SELECTED)

struct fp_cmd_header {
	uint8_t sync;
	uint16_t seq;
	uint16_t len;
} __attribute__ ((__packed__));

struct fp_cmd {
	struct fp_cmd_header header;
	uint8_t data[0];
};

#define FP_CMD_SIZE(cmd) \
	(sizeof(cmd->header) + SWAP_U16(cmd->header.len) + FP_MSG_CRC_SIZE)


typedef struct {
	uint8_t nSvrIp[4];
	uint8_t strMac[6];
	uint8_t LocIp[4];
	uint8_t Mask[4];
	uint8_t gw[4];
} __attribute__ ((__packed__)) NET_ETH0_T;


typedef struct {
	uint32_t rate;
	uint8_t len;
} __attribute__ ((__packed__)) OUT_RATE_T;

typedef struct {
	uint8_t num[CHANNEL_MAX_NUM];
} __attribute__ ((__packed__)) CHN_NUM_T;

extern PROG_INFO_T g_prog_info_table[];
extern CHN_NUM_T g_chan_num;

/*
 * program attribute
 * 1. pcr type and grouping
 */
struct program_attribute {
	uint8_t pcr_type;
	uint8_t pcr_group_id;
};
enum {
	NA_PCR = 0,
	SOLO_PCR,
	COM_PCR,
	PUB_PCR,
};
extern struct program_attribute g_prog_attr_table[];
static inline const char *pcr_type_name(uint8_t type)
{
	switch (type) {
		case SOLO_PCR: return "SOLO"; break;
		case COM_PCR: return "COM"; break;
		case PUB_PCR: return "PUB"; break;
		default: return "NA"; break;
	}

	return "NA";
}


#endif /* _FRONT_PANEL_INTSTR_H_ */

