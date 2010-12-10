#ifndef _FRONT_PANEL_INTSTR_H_
#define _FRONT_PANEL_INTSTR_H_

#include "xmux.h"


typedef struct					// pids for store PID type, modify 2010-10-31
{
	uint8_t type;
	uint16_t in;
	uint16_t out;
} __attribute__ ((__packed__)) PROG_PIDS_T;

typedef struct					// output one program to mcu_panel
{
	uint8_t status;

	uint16_t prognum;
	uint16_t PMT_PID_IN;
	uint16_t PMT_PID_OUT;
	uint16_t PCR_PID_IN;
	uint16_t PCR_PID_OUT;

	PROG_PIDS_T pids[PROGRAM_DATA_PID_MAX_NUM];	// pids for store PID type, modify 2010-10-31
	unsigned char progname[2][PROGRAM_NAME_SIZE];
} __attribute__ ((__packed__)) PROG_INFO_T;

struct fp_cmd_header {
	uint8_t sync;
	uint16_t seq;
	uint16_t len;
} __attribute__ ((__packed__));

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


#endif /* _FRONT_PANEL_INTSTR_H_ */

