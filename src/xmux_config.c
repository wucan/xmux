#include <string.h>

#include "wu/message.h"
#include "wu/wu_csc.h"

#include "xmux.h"
#include "eeprom.h"
#include "pid_trans_info.h"
#include "xmux_snmp.h"
#include "xmux_misc.h"
#include "xmux_net.h"
#include "output_psi_data.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "xmux_config"};

struct xmux_eeprom_param g_eeprom_param;

int xmux_config_init()
{
	int rc = eeprom_open();
	if (rc < 0) {
		trace_err("eeprom open failed! rc %d", rc);
		return rc;
	}

	trace_info("xmux eeprom size %#x, param size %#x",
		EEPROM_SIZE, sizeof(struct xmux_eeprom_param));
	trace_info("offset in eeprom: %#x, %#x, %#x, %#x, %#x, %#x, %#x, %#x",
		EEPROM_OFF_PID_TRANS_INFO, EEPROM_OFF_PID_MAP_TABLE,
		EEPROM_OFF_OUTPUT_PSI, EEPROM_OFF_SYS, EEPROM_OFF_NET,
		EEPROM_OFF_USER, EEPROM_OFF_MNG_MODE, EEPROM_OFF_MUX_PROG_INFO);

	return 0;
}

static bool xmux_eeprom_param_validate(struct xmux_eeprom_param *p)
{
	struct pid_trans_info_snmp_data *pid_trans_info;
	uint8_t chan_idx;

	/*
	 * management mode
	 */
	management_mode = MANAGEMENT_MODE_SNMP;

	/*
	 * pid trans info
	 */
	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		pid_trans_info = &p->pid_trans_info_area.table[chan_idx].data;
		if (!pid_trans_info_validate(pid_trans_info)) {
			trace_err("#%d pid trans info invalidate!", chan_idx);
			memset(pid_trans_info, 0, sizeof(struct pid_trans_info_snmp_data));
			continue;
		}
		pid_trans_info_dump(chan_idx, pid_trans_info);
	}

	/*
	 * pid map table
	 */
	if (pid_map_table_validate(&p->pid_map_table_area.pid_map_table)) {
		pid_map_table_dump(&p->pid_map_table_area.pid_map_table);
	} else {
		trace_err("pid map table invalidate!");
		pid_map_table_clear(&p->pid_map_table_area.pid_map_table);
	}

	/*
	 * output psi
	 */
	if (output_psi_data_validate(&p->output_psi_area.output_psi)) {
		output_psi_data_dump(&p->output_psi_area.output_psi);
	} else {
		trace_err("output psi data invalidate!");
		output_psi_data_clear(&p->output_psi_area.output_psi);
	}

#if CHANNEL_MAX_NUM == 1
	memcpy(g_input_pmt_sec, g_eeprom_param.input_pmt_sec,
		sizeof(g_input_pmt_sec));
#endif

	/*
	 * sys
	 */
	if (xmux_system_param_validate(&p->sys)) {
		xmux_system_param_dump(&p->sys);
	} else {
		trace_err("system param invalidate!");
		hex_dump("sys", &p->sys, sizeof(p->sys));
		xmux_system_param_init_default(&p->sys);
		eeprom_write(EEPROM_OFF_SYS, &p->sys, sizeof(p->sys));
	}

	/*
	 * net
	 */
	if (xmux_net_param_validate(&p->net)) {
		xmux_net_param_dump(&p->net);
	} else {
		trace_err("net param invalidate!");
		xmux_net_param_init_default(&p->net);
		eeprom_write(EEPROM_OFF_NET, &p->net, sizeof(p->net));
	}

	/*
	 * user
	 */
	if (xmux_user_param_validate(&p->user)) {
		xmux_user_param_dump(&p->user);
	} else {
		trace_err("user param invalidate!");
		xmux_user_param_init_default(&p->user);
	}

	return true;
}

static void xmux_eeprom_param_init_default(struct xmux_eeprom_param *p)
{
	/* TODO */
}

/*
 * if load failed then fallback to default
 */
void xmux_config_load_from_eeprom()
{
	uint8_t chan_idx;

	/*
	 * load real data, so accelerate load speed
	 */
	eeprom_read(EEPROM_OFF_MISC_PARAM,
		(uint8_t *)&g_eeprom_param.sys, EEPROM_MISC_PARAM_SIZE);

	/* pid_trans_info_area */
	// read head
	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		struct pid_trans_info_snmp_data *info;
		uint16_t data_len = 0;
		info = &g_eeprom_param.pid_trans_info_area.table[chan_idx].data;
		eeprom_read(EEPROM_OFF_PID_TRANS_INFO +
			sizeof(struct pid_trans_info_snmp_data) * chan_idx,
			(uint8_t *)&data_len, 2);
		trace_info("load pid trans info channel #%d data_len %d",
			chan_idx, data_len);
		eeprom_read(EEPROM_OFF_PID_TRANS_INFO +
			sizeof(struct pid_trans_info_snmp_data) * chan_idx,
			(uint8_t *)info,
			MIN(data_len + 2, sizeof(struct pid_trans_info_snmp_data)));
	}

	eeprom_read(EEPROM_OFF_PID_MAP_TABLE,
		(uint8_t *)&g_eeprom_param.pid_map_table_area,
		sizeof(g_eeprom_param.pid_map_table_area));

	/* output_psi_area */
	// read head
	eeprom_read(EEPROM_OFF_OUTPUT_PSI,
		(uint8_t *)&g_eeprom_param.output_psi_area,
		sizeof(g_eeprom_param.output_psi_area.output_psi));
	trace_info("output_psi had %d packages",
		g_eeprom_param.output_psi_area.output_psi.pkt_nr);
	// read all
	eeprom_read(EEPROM_OFF_OUTPUT_PSI,
		(uint8_t *)&g_eeprom_param.output_psi_area,
		MIN(sizeof(g_eeprom_param.output_psi_area),
			sizeof(g_eeprom_param.output_psi_area.output_psi) +
			g_eeprom_param.output_psi_area.output_psi.pkt_nr * TS_PACKET_BYTES));

#if CHANNEL_MAX_NUM == 1
	eeprom_read(EEPROM_OFF_INPUT_PMT_SEC,
		(uint8_t *)&g_eeprom_param.input_pmt_sec,
		sizeof(g_eeprom_param.input_pmt_sec));
#endif

	/*
	 * force to snmp management mode always in startup! else if we are starting
	 * and in fp management mode, if the fp is malfunction, then out of control!
	 */
	g_eeprom_param.mng_mode = MANAGEMENT_MODE_SNMP;
	/* checkint */
	if (!xmux_eeprom_param_validate(&g_eeprom_param)) {
		trace_err("invalidate xmux root param load from eeprom, fallback to default!");
		xmux_eeprom_param_init_default(&g_eeprom_param);
	} else {
		trace_info("xmux root param success load from eeprom");
	}
	g_param_mng_info.eeprom_pid_trans_info_version++;
	g_param_mng_info.eeprom_pid_map_table_version++;
}

void xmux_config_dump(uint32_t off, int len)
{
	int dump_len = MIN(len, 64);
	uint8_t buf[64];

	trace_info("dump config at offset %#x, len %d", off, dump_len);
	eeprom_read(off, buf, dump_len);
	hex_dump("config data", buf, dump_len);
}

void xmux_config_save_output_bitrate(uint32_t bitrate)
{
	g_eeprom_param.sys.output_bitrate = bitrate;
	eeprom_write(EEPROM_OFF_SYS_OUTPUT_BITRATE, &bitrate, sizeof(bitrate));
}

void xmux_config_save_packet_format(uint8_t fmt)
{
	g_eeprom_param.sys.format = fmt;
	eeprom_write(EEPROM_OFF_SYS_PACKET_FORMAT, &fmt, sizeof(fmt));
}

void xmux_config_save_management_mode()
{
	g_eeprom_param.mng_mode = management_mode;
	/* don't save again */
	//eeprom_write(EEPROM_OFF_MNG_MODE, &g_eeprom_param.mng_mode, 1);
}

void xmux_config_save_pid_trans_info_all()
{
	uint8_t chan_idx;
	struct pid_trans_info_snmp_data *info;
	uint32_t off;

	trace_info("save pid trans info...");
	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		info = &g_eeprom_param.pid_trans_info_area.table[chan_idx].data;
		if (!pid_trans_info_validate(info)) {
			trace_err("#%d pid trans info invalidate!", chan_idx);
			continue;
		}
		pid_trans_info_dump(chan_idx, info);

		off = EEPROM_OFF_PID_TRANS_INFO +
			sizeof(g_eeprom_param.pid_trans_info_area.table[0]) * chan_idx;
		trace_info("#%d pid trans info eeprom offset %#x", chan_idx, off);
		eeprom_write(off,
			&g_eeprom_param.pid_trans_info_area.table[chan_idx].data,
			g_eeprom_param.pid_trans_info_area.table[chan_idx].data.data_len + 2);
		usleep(100000);
	}
}

void xmux_config_save_output_psi_data()
{
	trace_info("save output psi data, %d packets ...",
		g_eeprom_param.output_psi_area.output_psi.pkt_nr);
	output_psi_data_dump(&g_eeprom_param.output_psi_area.output_psi);
	eeprom_write(EEPROM_OFF_OUTPUT_PSI, &g_eeprom_param.output_psi_area,
		MIN(sizeof(g_eeprom_param.output_psi_area),
			sizeof(g_eeprom_param.output_psi_area.output_psi) +
			g_eeprom_param.output_psi_area.output_psi.pkt_nr * TS_PACKET_BYTES));
}

void xmux_config_save_pid_map_table(struct xmux_pid_map_table *t)
{
	trace_info("save pid map table...");
	if (!pid_map_table_validate(t)) {
		trace_err("pid map table invalidate! save nothing!");
		return;
	}
	pid_map_table_dump(t);
	g_eeprom_param.pid_map_table_area.pid_map_table = *t;
	eeprom_write(EEPROM_OFF_PID_MAP_TABLE, t, sizeof(*t));
}

#if CHANNEL_MAX_NUM == 1
void xmux_config_save_input_pmt_section()
{
	memcpy(g_eeprom_param.input_pmt_sec, g_input_pmt_sec,
		sizeof(g_input_pmt_sec));
	eeprom_write(EEPROM_OFF_INPUT_PMT_SEC, g_eeprom_param.input_pmt_sec,
		sizeof(g_input_pmt_sec));
}
#endif

void xmux_config_save_net_param(struct xmux_net_param *net)
{
	net->csc = wu_csc(net, sizeof(*net) - 1);
	g_eeprom_param.net = *net;
	eeprom_write(EEPROM_OFF_NET, net, sizeof(*net));
	hex_dump("save net", &g_eeprom_param.net, sizeof(*net));
}

void xmux_config_save_mux_program_info(struct xmux_mux_program_info *info)
{
	eeprom_write(EEPROM_OFF_MUX_PROG_INFO, info, sizeof(*info));
}

#if 0
static void eeprom_rw_test()
{
	uint8_t data[1024], read_data[1024];
	int i;

	trace_info("eeprom test...");
	for (i = 0; i < 1024; i++)
		data[i] = i;

	eeprom_write(0, data, 1024);
	eeprom_read(0, read_data, 1024);
	for (i = 0; i < 1024; i++) {
		if (read_data[i] != data[i]) {
			trace_err("#%d write %#x, read %#x",
				i, data[i], read_data[i]);
		}
	}
}
static struct xmux_eeprom_param tmp_eeprom_param;
#define NBYTES	sizeof(struct xmux_eeprom_param)
static void eeprom_rw_test_1()
{
	int i, cnt = 0;
	uint8_t *wbuf = &g_eeprom_param;
	uint8_t *rbuf = &tmp_eeprom_param;

	while (1) {
		// write something
		for (i = 0; i < 1024; i++)
			wbuf[i] = i;
		memset(wbuf, cnt, NBYTES);
		cnt++;
		eeprom_write(0, wbuf, NBYTES);
		// read back
		memset(rbuf, 0xFF, NBYTES);
		eeprom_read(0, rbuf, NBYTES);
		// check
		for (i = 0; i < NBYTES; i++) {
			if (rbuf[i] != wbuf[i]) {
				trace_err("check failed begin at %#x!", i);
				hex_dump("write", &wbuf[i], NBYTES - i);
				hex_dump("read", &rbuf[i], NBYTES - i);
				break;
			}
		}
		if (i == NBYTES) {
			trace_info("check passed");
		}

		sleep(2);
		//break;
	}
	exit(0);
}
#endif

