#include <string.h>

#include "wu/wu_csc.h"
#include "wu/message.h"

#include "xmux.h"
#include "xmux_config.h"
#include "wu_snmp_agent.h"
#include "psi_parse.h"
#include "xmux_snmp_intstr.h"
#include "eeprom.h"
#include "pid_trans_info.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "pid_trans_info"};

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
		/* checking */
		if (!pid_trans_info_validate(&sg_mib_pid_trans_info[chan_idx])) {
			trace_err("#%d pid trans info invalidate set!", chan_idx);
			return;
		}
		pid_trans_info_dump(&sg_mib_pid_trans_info[chan_idx]);
		/* save it */
		memcpy(g_eeprom_param.pid_trans_info_area.bytes,
			&sg_mib_pid_trans_info[chan_idx], PID_TRANS_INFO_SIZE);
		eeprom_write(EEPROM_OFF_PID_TRANS_INFO + PID_TRANS_INFO_SIZE * chan_idx,
			&sg_mib_pid_trans_info[chan_idx], PID_TRANS_INFO_SIZE);
	}
}

bool pid_trans_info_validate(struct pid_trans_info_snmp_data *data)
{
	uint8_t csc;

	if (data->data_len != sizeof(*data) - 2) {
		trace_err("data_len 0, but expect %d!", sizeof(*data) - 2);
		return false;
	}
	csc = wu_csc(data, sizeof(*data) - 1);
	if (csc != data->csc) {
		return false;
	}

	return true;
}

void pid_trans_info_dump(struct pid_trans_info_snmp_data *data)
{
	struct xmux_program_info *prog;
	uint8_t prog_idx, pid_idx;
	char prog_name[33];
	int prog_name_len;

	trace_info("pid trans info:");
	trace_info("len %02d, chan %d, update flag %d, nprogs %d, status %#x",
		data->data_len, data->update_flag_and_chan_num & 0x07,
		data->update_flag_and_chan_num >> 7, data->nprogs, data->status);

	/*
	 * program info
	 */
	for (prog_idx = 0; prog_idx < data->nprogs; prog_idx++) {
		prog = &data->programs[prog_idx];
		prog_name_len = MIN(prog->prog_name[0][0], PROGRAM_NAME_SIZE - 1);
		memcpy(prog_name, &prog->prog_name[0][1], prog_name_len);
		prog_name[prog_name_len] = 0;
		trace_info("program #%d, %s, num %d, pmt(%d => %d), pcr(%d => %d), %s",
			prog_idx, prog_name, prog->prog_num,
			prog->pmt.in, prog->pmt.out, prog->pcr.in, prog->pcr.out,
			PROGRAM_SELECTED(data->status, prog_idx) ? "selected": "discard");
		trace_info("data pid:");
		for (pid_idx = 0; pid_idx < PROGRAM_DATA_PID_MAX_NUM; pid_idx++) {
			uint16_t data_pid_in = prog->data[pid_idx].in;
			if (data_pid_in == DATA_PID_PAD_VALUE)
				continue;
			if (!data_pid_validate(prog->data[pid_idx].in)) {
				trace_err("  #%d, data pid %d(%#x) invalidate!",
					pid_idx, data_pid_in, data_pid_in);
				continue;
			}
			trace_info("  #%d, pid(%d => %d, type %d)",
				pid_idx,
				DATA_PID_VALUE(prog->data[pid_idx].in),
				DATA_PID_VALUE(prog->data[pid_idx].out),
				DATA_PID_TYPE(prog->data[pid_idx].in));
		}
	}
}

