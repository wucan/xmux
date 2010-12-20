#include <string.h>
#include <signal.h>

#include "wu/message.h"

#include "xmux.h"
#include "front_panel_intstr.h"
#include "front_panel_define.h"
#include "psi_parse.h"

#include "hfpga.h"
#include "gen_dvb_si.h"


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

/*
 * psi parse timeout manangement
 */
static int psi_parse_timeout_sec = 20;
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
void psi_parse_timer_start()
{
	uvPSI_SetTimeoutFunc(psi_parse_timer_is_timeouted);
	signal(SIGALRM, sig_alarm);
	psi_parse_timeouted = 0;
	alarm(psi_parse_timeout_sec);
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
static uint16_t serv_num;

static uv_nit_data nit;

static uv_nit_stream_data stream[PROGRAM_MAX_NUM];
static uint16_t stream_num;

static int parse_pat()
{
	int i, rc;
	unsigned short len;

	pid_num = 0;

	sg_si_param.cur_cnt = 0;
	sg_si_param.type = EUV_BOTH;
	sg_si_param.tbl_type = EUV_TBL_PAT;
	sg_si_param.sec[0] = sg_mib_pat[sg_si_param.cha];
	psi_parse_timer_start();
	rc = dvbSI_Dec_PAT(&pat, pid_data, &pid_num);
	psi_parse_timer_stop();
	if (rc) {
		printf("pat parse failed! rc %d\n", rc);
		return -1;
	}
	memcpy(&len, sg_mib_pat[sg_si_param.cha], 2);
	printf("[uvSI] channel %d got pat section, len %d\n",
		sg_si_param.cha + 1, len);
	printf("[uvSI] TS id %#x, %d programs\n",
		pat.i_tran_stream_id, pid_num);
	for (i = 0; i < pid_num; i++) {
		printf("program number %#x, PMT pid %#x\n",
			pid_data[i].i_pg_num, pid_data[i].i_pid);
	}

	return 0;
}

static int parse_pmt()
{
	unsigned int cnt = 0;
	unsigned short len;
	int i, rc;
	unsigned char sg_mib_curpmt[SECTION_MAX_SIZE];
	uint8_t chan_idx = sg_si_param.cha;
	uint32_t pmt_state = 0;

	/* skip NIT */
	if (pid_data[0].i_pid == 0x10)
		cnt = 1;
	else
		cnt = 0;
	for (i = cnt; i < pid_num; i++) {
		pmt.i_pg_num = pid_data[i].i_pg_num;
		pmt.i_pmt_pid = pid_data[i].i_pid;
		sg_si_param.cur_cnt = 0;
		sg_si_param.type = EUV_SECTION;
		sg_si_param.tbl_type = EUV_TBL_PMT;
		sg_si_param.sec[0] = sg_mib_curpmt;
		psi_parse_timer_start();
		rc = dvbSI_Dec_PMT(&pmt, es, &es_num);
		psi_parse_timer_stop();
		if (rc) {
			printf("pmt parse failed! rc %d\n", rc);
			memcpy(&sg_si_param.cur_stat->tbl_s[chan_idx][1], &pmt_state, 4);
			return -1;
		}
		memcpy(&len, sg_mib_curpmt, 2);
		printf("[uvSI] pmt pid %#x, got section, len %d\n",
			pmt.i_pmt_pid, len);
		if (len > 0) {
			memcpy(sg_mib_pmt[sg_si_param.cha][i - cnt], sg_mib_curpmt, len + 2);
			pmt_state |= 1 << (i - cnt);
		}
	}
	memcpy(&sg_si_param.cur_stat->tbl_s[chan_idx][1], &pmt_state, 4);

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
	psi_parse_timer_start();
	rc = dvbSI_Dec_CAT(cat_descr, &cat_descr_num);
	psi_parse_timer_stop();
	if (rc) {
		printf("cat parse failed! rc %d\n", rc);
		return -1;
	}

	return 0;
}

static int parse_sdt()
{
	unsigned short len;
	int rc;
	int i;

	sg_si_param.cur_cnt = 0;
	sg_si_param.type = EUV_SECTION;
	sg_si_param.tbl_type = EUV_TBL_SDT;
	for (i = 0; i < 5; i++)
		sg_si_param.sec[i] = sg_mib_sdt[sg_si_param.cha][i];
	psi_parse_timer_start();
	rc = dvbSI_Dec_SDT(&sdt, serv, &serv_num);
	psi_parse_timer_stop();
	if (rc) {
		printf("sdt parse failed! rc %d\n", rc);
		return -1;
	}
	for (i = 0; i < 5; i++) {
		memcpy(&len, sg_mib_sdt[sg_si_param.cha][i], 2);
		printf("[uvSI] got sdt section #%d, len %d\n", i, len);
	}
	printf("[uvSI] there are total %d services\n", serv_num);

	return 0;
}

static int parse_nit()
{
	unsigned short len;
	int rc;

	sg_si_param.cur_cnt = 0;
	sg_si_param.type = EUV_SECTION;
	sg_si_param.tbl_type = EUV_TBL_NIT;
	sg_si_param.sec[0] = sg_mib_nit[sg_si_param.cha];
	psi_parse_timer_start();
	rc = dvbSI_Dec_NIT(&nit, stream, &stream_num);
	psi_parse_timer_stop();
	if (rc) {
		printf("nit parse failed! rc %d\n", rc);
		return -1;
	}
	memcpy(&len, sg_mib_nit[sg_si_param.cha], 2);
	printf("[uvSI] got nit section, len %d\n", len);

	return 0;
}

int uvSI_psi_parse()
{
	int i, k;
	int rc;
	uint16_t ts_status;

	memset(&All_Channel_Psi_Status, 0, sizeof(All_Channel_Psi_Status));
	sg_si_param.cur_stat = &All_Channel_Psi_Status;
	for (k = 0; k < CHANNEL_MAX_NUM; k++) {
		if (request_stop_parse)
			break;
		if (hfpga_get_ts_status(k, &ts_status) <= 0) {
			printf("[uvSI] channel %d had no ts!\n", k + 1);
			sg_si_param.cur_stat->ch_s |= 0x01 << k;
			continue;
		}

		printf("[uvSI] channel %d start parse psi ...\n", k + 1);
		sg_si_param.cha = k;
		for (i = 0; i < UV_MAX_SI_SECTION_CNT; i++)
			sg_si_param.sec_len[i] = 0;
		hfpga_dev.cha = sg_si_param.cha;
		dvbSI_Start(&hfpga_dev);

		printf("[uvSI] decode PAT ...\n");
		rc = parse_pat();
		if (rc)
			goto channel_analyse_done;

		printf("[uvSI] decode PMT ...\n");
		rc = parse_pmt();
		if (rc)
			goto channel_analyse_done;

		printf("[uvSI] decode CAT ...\n");
		rc = parse_cat();
		if (rc)
			goto channel_analyse_done;

		printf("[uvSI] decode SDT ...\n");
		rc = parse_sdt();
		if (rc)
			goto channel_analyse_done;

		printf("[uvSI] decode NIT ...\n");
		rc = parse_nit();
		if (rc)
			goto channel_analyse_done;

channel_analyse_done:
		sg_si_param.cur_stat->ch_s |= 0x01 << k;

		dvbSI_Stop();

		printf("[uvSI] channel %d psi parse finished.", sg_si_param.cha + 1);
	}

	for (; k < CHANNEL_MAX_NUM; k++) {
		sg_si_param.cur_stat->ch_s |= 0x01 << k;
	}

	request_stop_parse = false;

	return 0;
}

void uvSI_psi_parse_stop()
{
	request_stop_parse = true;
}

