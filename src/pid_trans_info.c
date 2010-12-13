#include <string.h>

#include "xmux.h"
#include "wu_snmp_agent.h"
#include "psi_parse.h"
#include "xmux_snmp_intstr.h"
#include "eeprom.h"


struct pid_trans_info_snmp_data sg_mib_pid_trans_info[CHANNEL_MAX_NUM];

#define NODE_MAX_SIZE			1422
struct access_data {
	uint8_t node[3][NODE_MAX_SIZE];
};

void pid_trans_info_read_data_snmp(uint8_t trans_idx, struct wu_snmp_value *v)
{
	uint8_t chan_idx = trans_idx / 3;
	int idx = trans_idx % 3;
	uint16_t mib_data_len = sg_mib_pid_trans_info[chan_idx].data_len + 2;
	struct access_data *d = (struct access_data *)&sg_mib_pid_trans_info[chan_idx];

	v->data = d->node[idx];
	if (idx < mib_data_len / NODE_MAX_SIZE) {
		v->size = NODE_MAX_SIZE;
	} else {
		v->size = mib_data_len % NODE_MAX_SIZE;
	}
}

void pid_trans_info_write_data_snmp(uint8_t trans_idx, struct wu_snmp_value *v)
{
	uint8_t chan_idx = trans_idx / 3;
	int idx = trans_idx % 3;
	struct access_data *d = (struct access_data *)&sg_mib_pid_trans_info[chan_idx];

	memcpy(d->node[idx], v->data, v->size);

	/*
	 * save to eeprom if channel's pid trans info all got
	 */
	if (v->size < NODE_MAX_SIZE || chan_idx == 2) {
		eeprom_write(EEPROM_OFF_PID_TRANS_INFO + PID_TRANS_INFO_SIZE * chan_idx,
			&sg_mib_pid_trans_info[chan_idx], PID_TRANS_INFO_SIZE);
	}
}

