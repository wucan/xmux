#include <string.h>

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
uint8_t sg_mib_pat[CHANNEL_MAX_NUM][SECTION_MAX_SIZE];
uint8_t sg_mib_pmt[CHANNEL_MAX_NUM][PROGRAM_MAX_NUM][SECTION_MAX_SIZE];
uint8_t sg_mib_cat[CHANNEL_MAX_NUM][SECTION_MAX_SIZE];
uint8_t sg_mib_nit[CHANNEL_MAX_NUM][SECTION_MAX_SIZE];
uint8_t sg_mib_sdt[CHANNEL_MAX_NUM][SDT_SECTION_NUM][SECTION_MAX_SIZE];
uint8_t sg_mib_eit[CHANNEL_MAX_NUM][EIT_SECTION_NUM][SECTION_MAX_SIZE];


int psi_parsing()
{
	int progs, total_progs = 0;
	uint8_t chan_idx;

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		progs = psi_parse_channel(chan_idx);
		trace_info("channel #%d had %d programs", chan_idx, progs);
		total_progs += progs;
	}
	trace_info("there are total %d programs", total_progs);

	return total_progs;
}

int psi_parse_channel(uint8_t chan_idx)
{
	uint16_t ts_status;
	uint8_t prog_num = 0;

	if (hfpga_get_ts_status(chan_idx, &ts_status) > 0) {
		gen_pmt(&(g_prog_info_table[chan_idx * PROGRAM_MAX_NUM]),
			&prog_num, chan_idx);
		g_chan_num.num[chan_idx] = prog_num;
	}

	return prog_num;
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
	int i;
	unsigned short len;

	pid_num = 0;

	sg_si_param.cur_cnt = 0;
	sg_si_param.type = EUV_BOTH;
	sg_si_param.tbl_type = EUV_TBL_PAT;
	sg_si_param.sec[0] = sg_mib_pat[sg_si_param.cha];
	dvbSI_Dec_PAT(&pat, pid_data, &pid_num);
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
	int i;
	unsigned char sg_mib_curpmt[SECTION_MAX_SIZE];

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
		dvbSI_Dec_PMT(&pmt, es, &es_num);
		memcpy(&len, sg_mib_curpmt, 2);
		printf("[uvSI] pmt pid %#x, got section, len %d\n",
			pmt.i_pmt_pid, len);
		memcpy(sg_mib_pmt[sg_si_param.cha][i - cnt], sg_mib_curpmt, len);
	}

	return 0;
}

static int parse_cat()
{
	unsigned short len;

	sg_si_param.cur_cnt = 0;
	sg_si_param.type = EUV_SECTION;
	sg_si_param.tbl_type = EUV_TBL_CAT;
	sg_si_param.sec[0] = sg_mib_cat[sg_si_param.cha];
	dvbSI_Dec_CAT(cat_descr, &cat_descr_num);

	return 0;
}

static int parse_sdt()
{
	unsigned short len;

	sg_si_param.cur_cnt = 0;
	sg_si_param.type = EUV_SECTION;
	sg_si_param.tbl_type = EUV_TBL_SDT;
	sg_si_param.sec[0] = sg_mib_sdt[sg_si_param.cha][0];
	dvbSI_Dec_SDT(&sdt, serv, &serv_num);
	memcpy(&len, sg_mib_sdt[sg_si_param.cha], 2);
	printf("[uvSI] got sdt section, len %d\n", len);
	printf("[uvSI] there are total %d services\n", serv_num);

	return 0;
}

static int parse_nit()
{
	unsigned short len;

	sg_si_param.cur_cnt = 0;
	sg_si_param.type = EUV_SECTION;
	sg_si_param.tbl_type = EUV_TBL_NIT;
	sg_si_param.sec[0] = sg_mib_nit[sg_si_param.cha];
	dvbSI_Dec_NIT(&nit, stream, &stream_num);
	memcpy(&len, sg_mib_nit[sg_si_param.cha], 2);
	printf("[uvSI] got nit section, len %d\n", len);

	return 0;
}

int uvSI_psi_parse()
{
	int i, k;
	unsigned char ts_status;

	for (k = 0; k < CHANNEL_MAX_NUM; k++) {
		if (hfpga_get_ts_status(k, &ts_status) <= 0) {
			printf("[uvSI] channel %d had no ts!\n", k + 1);
			continue;
		}

		printf("[uvSI] channel %d start parse psi ...\n", k + 1);
		sg_si_param.cha = k;
		for (i = 0; i < UV_MAX_SI_SECTION_CNT; i++)
			sg_si_param.sec_len[i] = 0;
		sg_si_param.cur_stat = &All_Channel_Psi_Status;
		sg_si_param.cur_stat->ch_s = k;
		hfpga_dev.cha = sg_si_param.cha;
		dvbSI_Start(&hfpga_dev);

		printf("[uvSI] decode PAT ...\n");
		parse_pat();

		printf("[uvSI] decode PMT ...\n");
		parse_pmt();

		printf("[uvSI] decode CAT ...\n");
		parse_cat();

		printf("[uvSI] decode SDT ...\n");
		parse_sdt();

		printf("[uvSI] decode NIT ...\n");
		parse_nit();

		dvbSI_Stop();

		sg_si_param.cur_stat->ch_s |= 0x01 << k;
		printf("[uvSI] channel %d psi parse finished.", sg_si_param.cha + 1);
	}

	return 0;
}

