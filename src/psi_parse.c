#include <string.h>
#include <signal.h>

#include "wu/message.h"

#include "xmux.h"
#include "front_panel_intstr.h"
#include "front_panel_define.h"
#include "psi_parse.h"

#include "hfpga.h"
#include "gen_dvb_si_api.h"


extern uv_dvb_io hfpga_dev;

static msgobj mo = {MSG_INFO, ENCOLOR, "psi_parse"};

/*
 * psi parse result section data
 */
uint8_t sg_mib_pat[CHANNEL_MAX_NUM + 1][SECTION_MAX_SIZE];
uint8_t sg_mib_pmt[CHANNEL_MAX_NUM + 1][PROGRAM_MAX_NUM][SECTION_MAX_SIZE];
uint8_t sg_mib_cat[CHANNEL_MAX_NUM + 1][SECTION_MAX_SIZE];
uint8_t sg_mib_nit[CHANNEL_MAX_NUM + 1][SECTION_MAX_SIZE];
uint8_t sg_mib_sdt[CHANNEL_MAX_NUM + 1][SDT_SECTION_NUM][SECTION_MAX_SIZE];
uint8_t sg_mib_eit[CHANNEL_MAX_NUM + 1][EIT_SECTION_NUM][SECTION_MAX_SIZE];

static bool had_nit = false;

/*
 * psi parse timeout manangement
 */
static int psi_parse_timeouted;
static bool request_stop_parse;
int psi_parse_timer_is_timeouted()
{
	if (request_stop_parse)
		return 1;
	return psi_parse_timeouted;
}
static void sig_alarm(int signo)
{
	psi_parse_timeouted = 1;
}
void psi_parse_timer_start(int timeout_sec)
{
	uvPSI_SetTimeoutFunc(psi_parse_timer_is_timeouted);
	signal(SIGALRM, sig_alarm);
	psi_parse_timeouted = 0;
	alarm(timeout_sec);
}
void psi_parse_timer_stop()
{
	alarm(0);
	psi_parse_timeouted = 0;
}

/*
 * steal from uvSI.c
 */
uv_cha_si_stat All_Channel_Psi_Status;

static uv_pat_data pat;
static uv_pat_pid_data pid_data[PROGRAM_MAX_NUM + 1];
static uint16_t pid_num;
static uv_pmt_data pmt;
static uv_pmt_es_data es[PROGRAM_MAX_NUM];
static uint16_t es_num;

static uv_descriptor cat_descr[5];
static uint16_t cat_descr_num;

static uv_sdt_serv_data serv[PROGRAM_MAX_NUM];
static uv_sdt_data sdt;

static uv_nit_data nit;

static uv_nit_stream_data stream[PROGRAM_MAX_NUM];
static uint16_t stream_num;

static void clear_channel_section_data(uint8_t chan_id)
{
	int i;

	memset(sg_mib_pat[chan_id], 0, 2);
	for (i = 0; i < SDT_SECTION_NUM; i++)
		memset(sg_mib_sdt[chan_id][SDT_SECTION_NUM], 0, 2);
	memset(sg_mib_cat[chan_id], 0, 2);
	memset(sg_mib_nit[chan_id], 0, 2);
	for (i = 0; i < PROGRAM_MAX_NUM; i++)
		memset(sg_mib_pmt[chan_id][i], 0, 2);
	memset(sg_mib_eit[chan_id], 0, 2);
}
static void clear_section_data()
{
	uint8_t chan_id;

	memset(&All_Channel_Psi_Status, 0, sizeof(All_Channel_Psi_Status));
	for (chan_id = 0; chan_id < CHANNEL_MAX_NUM; chan_id++)
		clear_channel_section_data(chan_id);
}
void clean_snmp_psi_parse_data()
{
	clear_section_data();
}
static int parse_pat()
{
	int i, rc;
	unsigned short len;

	pid_num = 0;

	sg_si_param.cur_cnt = 0;
	sg_si_param.type = EUV_BOTH;
	sg_si_param.tbl_type = EUV_TBL_PAT;
	sg_si_param.sec[0] = sg_mib_pat[sg_si_param.cha];
	psi_parse_timer_start(5);
	rc = dvbSI_Dec_PAT(&pat, pid_data, &pid_num);
	psi_parse_timer_stop();
	if (rc) {
		trace_err("pat parse failed! rc %d", rc);
		return -1;
	}
	memcpy(&len, sg_mib_pat[sg_si_param.cha], 2);
	trace_info("channel %d got pat section, len %d",
		sg_si_param.cha + 1, len);
	trace_info("TS id %#x, %d programs",
		pat.i_tran_stream_id, pid_num);
	for (i = 0; i < pid_num; i++) {
		trace_info("program number %#x, PMT pid %#x",
			pid_data[i].i_pg_num, pid_data[i].i_pid);
	}

	return 0;
}

struct pmt_status_bitmap {
	WU_BITMAP_DECLARE(bitmap, PROGRAM_MAX_NUM);
};
static int parse_pmt()
{
	unsigned int cnt = 0;
	unsigned short len;
	int i, rc;
	unsigned char sg_mib_curpmt[SECTION_MAX_SIZE];
	uint8_t chan_idx = sg_si_param.cha;
	struct pmt_status_bitmap pmt_state = {0};

	had_nit = false;
	for (i = 0; i < pid_num; i++) {
		/* skip NIT */
		if (pid_data[i].i_pg_num == 0x00) {
			had_nit = true;
			continue;
		}

		/*
		 * reset section length field, in case no section got
		 */
		memset(sg_mib_curpmt, 0, 2);

		pmt.i_pg_num = pid_data[i].i_pg_num;
		pmt.i_pmt_pid = pid_data[i].i_pid;
		sg_si_param.cur_cnt = 0;
		sg_si_param.type = EUV_SECTION;
		sg_si_param.tbl_type = EUV_TBL_PMT;
		sg_si_param.sec[0] = sg_mib_curpmt;
		trace_info("parse pmt, pid %04x, program_number %d",
			pmt.i_pmt_pid, pmt.i_pg_num);
		psi_parse_timer_start(10);
		rc = dvbSI_Dec_PMT(&pmt, es, &es_num);
		psi_parse_timer_stop();
		if (rc) {
			trace_err("pmt parse failed! rc %d", rc);
			memcpy(&sg_si_param.cur_stat->tbl_s[chan_idx][1], &pmt_state,
				sizeof(pmt_state));
			continue;
		}
		memcpy(&len, sg_mib_curpmt, 2);
		trace_info("pmt pid %#x, got section, len %d",
			pmt.i_pmt_pid, len);
		if (len > 0) {
			memcpy(sg_mib_pmt[sg_si_param.cha][cnt], sg_mib_curpmt, len + 2);
#if CHANNEL_MAX_NUM == 1
			memcpy(g_input_pmt_sec[cnt], sg_mib_curpmt, len + 2);
#endif
			wu_bitmap_set_bit(pmt_state.bitmap, cnt);
		}
		cnt++;
	}
	memcpy(&sg_si_param.cur_stat->tbl_s[chan_idx][1], &pmt_state,
		sizeof(pmt_state));

	return 0;
}

static int parse_cat()
{
	unsigned short len;
	int rc;

	sg_si_param.cur_cnt = 0;
	sg_si_param.type = EUV_SECTION;
	sg_si_param.tbl_type = EUV_TBL_CAT;
	sg_si_param.sec[0] = sg_mib_cat[sg_si_param.cha];
	psi_parse_timer_start(10);
	rc = dvbSI_Dec_CAT(cat_descr, &cat_descr_num);
	psi_parse_timer_stop();
	if (rc) {
		trace_err("cat parse failed! rc %d", rc);
		return -1;
	}

	return 0;
}

static int parse_sdt()
{
	uint16_t serv_num;
	unsigned short len;
	int rc;
	int i;

	sg_si_param.cur_cnt = 0;
	sg_si_param.type = EUV_SECTION;
	sg_si_param.tbl_type = EUV_TBL_SDT;
	for (i = 0; i < SDT_SECTION_NUM; i++)
		sg_si_param.sec[i] = sg_mib_sdt[sg_si_param.cha][i];
	psi_parse_timer_start(20);
	rc = dvbSI_Dec_SDT(&sdt, serv, &serv_num);
	psi_parse_timer_stop();
	if (rc) {
		trace_err("sdt parse failed! rc %d", rc);
		return -1;
	}
	for (i = 0; i < SDT_SECTION_NUM; i++) {
		memcpy(&len, sg_mib_sdt[sg_si_param.cha][i], 2);
		trace_info("got sdt section #%d, len %d", i, len);
	}

	return 0;
}

static int parse_nit()
{
	unsigned short len;
	int rc;

	if (!had_nit) {
		trace_info("no nit in this channel!");
		return 0;
	}

	sg_si_param.cur_cnt = 0;
	sg_si_param.type = EUV_SECTION;
	sg_si_param.tbl_type = EUV_TBL_NIT;
	sg_si_param.sec[0] = sg_mib_nit[sg_si_param.cha];
	psi_parse_timer_start(20);
	rc = dvbSI_Dec_NIT(&nit, stream, &stream_num);
	psi_parse_timer_stop();
	if (rc) {
		trace_err("nit parse failed! rc %d", rc);
		return -1;
	}
	memcpy(&len, sg_mib_nit[sg_si_param.cha], 2);
	trace_info("got nit section, len %d", len);

	return 0;
}

int uvSI_psi_parse()
{
	int i, k;
	int rc;
	uint16_t ts_status;

	disable_snmp_connection_check();
	clear_section_data();
	sg_si_param.cur_stat = &All_Channel_Psi_Status;
	for (k = 0; k < CHANNEL_MAX_NUM; k++) {
		if (request_stop_parse)
			break;
		if (hfpga_get_ts_status(k, &ts_status) <= 0) {
			trace_warn("channel %d had no ts!", k + 1);
			sg_si_param.cur_stat->ch_s |= 0x01 << k;
			continue;
		}

		trace_info("channel %d start parse psi ...", k + 1);
		sg_si_param.cha = k;
		for (i = 0; i < UV_MAX_SI_SECTION_CNT; i++)
			sg_si_param.sec_len[i] = 0;
		hfpga_dev.cha = sg_si_param.cha;
		dvbSI_Start(&hfpga_dev);

		trace_info("decode PAT ...");
		rc = parse_pat();
		if (rc)
			goto channel_analyse_done;

		trace_info("decode PMT ...");
		rc = parse_pmt();
		if (rc)
			goto channel_analyse_done;

		trace_info("decode CAT ...");
		rc = parse_cat();

		trace_info("decode SDT ...");
		rc = parse_sdt();

		trace_info("decode NIT ...");
		rc = parse_nit();
		if (rc)
			goto channel_analyse_done;

channel_analyse_done:
		sg_si_param.cur_stat->ch_s |= 0x01 << k;

		dvbSI_Stop();

		trace_info("channel %d psi parse finished.", sg_si_param.cha + 1);
	}

	for (; k < CHANNEL_MAX_NUM; k++) {
		sg_si_param.cur_stat->ch_s |= 0x01 << k;
	}

	request_stop_parse = false;
	enable_snmp_connection_check();

	return 0;
}

void uvSI_psi_parse_stop()
{
	request_stop_parse = true;
}

int parse_sdt_section_and_decode(uint8_t chan_idx,
	uv_sdt_data *p_sdt_data, uv_sdt_serv_data *p_sdt_serv_data,
	uint16_t *p_serv_num)
{
	int i, rc;
	uint16_t len;

	/* prepare */
	sg_si_param.cur_stat = &All_Channel_Psi_Status;
	sg_si_param.cha = chan_idx;

	rc = parse_sdt();
	if (rc) {
		return -1;
	}
	dvbpsi_decode_sdt_section_begin(p_sdt_data, p_sdt_serv_data, p_serv_num);
	for (i = 0; i < SDT_SECTION_NUM; i++) {
		memcpy(&len, sg_mib_sdt[chan_idx][i], 2);
		if (len <= 0)
			break;
		dvbpsi_push_sdt_section(&sg_mib_sdt[chan_idx][i][2], len);
	}
	dvbpsi_decode_sdt_section_end();

	return 0;
}

