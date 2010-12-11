#include <string.h>

#include "xmux.h"
#include "wu_snmp_agent.h"
#include "psi_parse.h"
#include "xmux_snmp_intstr.h"
#include "eeprom.h"


struct pid_trans_info_snmp_data sg_mib_pid_trans_info[CHANNEL_MAX_NUM];

void pid_trans_info_read_data_snmp(uint8_t trans_idx, struct wu_snmp_value *v)
{
	uint8_t chan_idx = (trans_idx - 1) / 3;
	int idx = (trans_idx - 1) % 3;
	uint8_t *mib_data = (uint8_t *)&sg_mib_pid_trans_info[chan_idx];
	uint16_t mib_data_len = sg_mib_xxx_len(mib_data);
	int cnt = mib_data_len / 1422;

	v->data = mib_data + idx * 1422;
	v->size = 1422;
}

void pid_trans_info_write_data_snmp(uint8_t trans_idx, struct wu_snmp_value *v)
{
	uint8_t chan_idx = (trans_idx - 1) / 3;
	bool chan_start = (trans_idx % 3) == 1;
	uint8_t *mib_data = (uint8_t *)&sg_mib_pid_trans_info[chan_idx];
	uint16_t write_off = 0;

	if (chan_start) {
		write_off = 0;
	} else {
		write_off = sg_mib_xxx_len(mib_data) + 2;
	}
	memcpy(mib_data + write_off, v->data, v->size);
	sg_mib_xxx_update_len(mib_data, write_off + v->size - 2);

	/*
	 * save to eeprom if channel's pid trans info all got
	 */
	if (v->size < 1422 || chan_idx == 2) {
		eeprom_write(EEPROM_OFF_PID_TRANS_INFO + PID_TRANS_INFO_SIZE * chan_idx,
			&sg_mib_pid_trans_info[chan_idx], PID_TRANS_INFO_SIZE);
	}
}

