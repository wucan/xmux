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

static int save_pid_trans_info;

struct pid_trans_info_snmp_data sg_mib_pid_trans_info[CHANNEL_MAX_NUM];

struct access_data {
	uint8_t node[NODE_NUM][NODE_MAX_SIZE];
};

void pid_trans_info_read_data_snmp(uint8_t trans_idx, struct wu_snmp_value *v)
{
	uint8_t chan_idx = trans_idx / NODE_NUM;
	int idx = trans_idx % NODE_NUM;
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
	uint8_t chan_idx = trans_idx / NODE_NUM;
	int idx = trans_idx % NODE_NUM;
	struct access_data *d = (struct access_data *)&sg_mib_pid_trans_info[chan_idx];

	if (idx == 0) {
		trace_info("begin recv pid_trans_info of channel #%d, clear it at first",
			chan_idx);
		memset(&sg_mib_pid_trans_info[chan_idx], 0, sizeof(sg_mib_pid_trans_info[0]));
	}
	memcpy(d->node[idx], v->data, v->size);

	/*
	 * save to eeprom if channel's pid trans info all got
	 */
	if (v->size < NODE_MAX_SIZE || idx == (NODE_NUM - 1)) {
		if (!pid_trans_info_validate(&sg_mib_pid_trans_info[chan_idx])) {
			trace_err("channel #%d pid trans info invalidate!", chan_idx);
			return;
		}
		pid_trans_info_dump(chan_idx, &sg_mib_pid_trans_info[chan_idx]);
	}
}

bool pid_trans_info_validate(struct pid_trans_info_snmp_data *data)
{
	uint8_t csc;
	uint8_t prog_idx;
	struct xmux_program_info_with_csc *prog;

	if (data->data_len && data->data_len > sizeof(*data) - 2) {
		trace_err("data_len %d > %d!",
			data->data_len, sizeof(*data) - 2);
		return false;
	}
	for (prog_idx = 0; prog_idx < data->nprogs; prog_idx++) {
		prog = &data->programs[prog_idx];
		if (prog->csc != wu_csc(prog, sizeof(*prog) - 1)) {
			trace_err("program #%d csc error!", prog_idx);
			hex_dump("program data", prog, sizeof(*prog) - 1);
			return false;
		}
	}

	return true;
}

void pid_trans_info_dump(uint8_t chan_idx, struct pid_trans_info_snmp_data *data)
{
	struct xmux_program_info *prog;
	uint8_t prog_idx, pid_idx;
	char prog_name[33];
	int prog_name_len;

	trace_info("channel #%d pid trans info:", chan_idx);
	hex_dump("pid_trans_info", data, 64);
	trace_info("len %02d, chan %d, update flag %d, nprogs %d",
		data->data_len, data->update_flag_and_chan_num & 0x07,
		data->update_flag_and_chan_num >> 7, data->nprogs);
	hex_dump("select status", &data->sel_status, 8);

	/*
	 * program info
	 */
	for (prog_idx = 0; prog_idx < data->nprogs; prog_idx++) {
		if (prog_idx >= 4) {
			trace_info("...");
			break;
		}
		prog = &data->programs[prog_idx];
		prog_name_len = MIN(prog->prog_name[0][0], PROGRAM_NAME_SIZE - 1);
		memcpy(prog_name, &prog->prog_name[0][1], prog_name_len);
		prog_name[prog_name_len] = 0;
		trace_info("program #%d, %s, num %d, pmt(%#x => %#x), pcr(%#x => %#x), %s",
			prog_idx, prog_name, prog->prog_num,
			prog->pmt.in, prog->pmt.out, prog->pcr.in, prog->pcr.out,
			PROGRAM_SELECTED(data->sel_status, prog_idx) ? "selected": "discard");
		trace_info("data pid:");
		for (pid_idx = 0; pid_idx < PROGRAM_DATA_PID_MAX_NUM; pid_idx++) {
			uint16_t data_pid_in = prog->data[pid_idx].in;
			if (data_pid_in == DATA_PID_PAD_VALUE)
				continue;
			if (!data_pid_validate(prog->data[pid_idx].type)) {
				trace_err("  #%d, data pid %#x invalidate!(type %d)",
					pid_idx, data_pid_in,
					prog->data[pid_idx].type);
				continue;
			}
			trace_info("  #%d, pid(%#x => %#x, type %d)",
				pid_idx,
				data_pid_in, prog->data[pid_idx].out, prog->data[pid_idx].type);
		}
	}
}

void pid_trans_info_save_check()
{
	if (save_pid_trans_info) {
		save_pid_trans_info = 0;
		xmux_config_save_pid_trans_info_all();
	}
}

void pid_trans_info_request_save()
{
	uint8_t chan_idx;
	struct pid_trans_info_snmp_data *dst_info;

	/* update with the new ones */
	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		dst_info = &g_eeprom_param.pid_trans_info_area.table[chan_idx].data;
		memcpy(dst_info, &sg_mib_pid_trans_info[chan_idx], sizeof(*dst_info));
	}
	save_pid_trans_info = 1;
}

