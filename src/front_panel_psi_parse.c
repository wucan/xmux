#include <string.h>

#include "wu/message.h"

#include "fpga_api.h"
#include "gen_dvb_si.h"

#include "front_panel_intstr.h"
#include "front_panel_define.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "fp_psi_parse"};

extern uv_dvb_io hfpga_dev;

typedef enum {
	DSW_PID_PMT = 0,
	DSW_PID_PCR = 1,
	DSW_PID_VIDEO = 2,
	DSW_PID_AUDIO = 3,

	DSW_PID_MAX = 0x0F
} enmDswPsiPid;

typedef enum {
	DSW_PSI_TYPE_VIDEO = 2,
	DSW_PSI_TYPE_AUDIO = 4,

	DSW_PSI_TYPE_MAX = 0x0F
} enmDswPsiType;

#define UV_PROG_NUM_MAX PROGRAM_MAX_NUM
#define UV_DESCR_LEN    (32)

static uv_pat_data pat;
static uv_pat_pid_data pid_data[UV_PROG_NUM_MAX + 1];
static uint16_t pid_num = 0;

static uv_pmt_data pmt;
static uv_descriptor pmt_descr[5];
static unsigned char p_pmt_data[5][UV_DESCR_LEN];
static uv_pmt_es_data es[UV_PROG_NUM_MAX];
static uv_descriptor es_descr[UV_PROG_NUM_MAX][5];
static unsigned char p_es_data[UV_PROG_NUM_MAX][5][UV_DESCR_LEN];

static uint16_t es_num = 0;

static uv_nit_data nit;
static uv_descriptor nit_descr[5];
static unsigned char p_nit_data[5][UV_DESCR_LEN];
static uv_nit_stream_data stream[UV_PROG_NUM_MAX];
static uv_descriptor stream_descr[UV_PROG_NUM_MAX][5];
static unsigned char p_stream_data[UV_PROG_NUM_MAX][5][UV_DESCR_LEN];

static uint16_t stream_num = 0;

static uv_sdt_data sdt;
static uv_sdt_serv_data serv[UV_PROG_NUM_MAX];
static uv_descriptor serv_descr[UV_PROG_NUM_MAX][5];
static unsigned char p_serv_data[UV_PROG_NUM_MAX][5][UV_DESCR_LEN];

static uint16_t serv_num = 0;

static uv_eit_data eit;
static uv_eit_event_data event[UV_PROG_NUM_MAX];
static uv_descriptor event_descr[UV_PROG_NUM_MAX][5];
static unsigned char p_event_data[UV_PROG_NUM_MAX][5][UV_DESCR_LEN];

static uint16_t event_num = 0;

static uv_descriptor cat_descr[5];
static unsigned char p_cat_descr_data[5][UV_DESCR_LEN];
static uint16_t cat_descr_num;


static void extract_program_name(unsigned char *desc_content,
									 unsigned char *prog_name)
{
	unsigned char prog_name_len;
	unsigned char prog_name_idx;

	prog_name_idx = 2 + desc_content[1] + 1;
	prog_name_len = desc_content[prog_name_idx - 1];
	prog_name[0] = prog_name_len;
	memcpy(prog_name + 1, desc_content + prog_name_idx, prog_name_len);
	trace_info("program name size %d, name %s",
		   prog_name_len, desc_content + prog_name_idx);
}

static uint16_t get_dsw_psi_pid(uint8_t chan_idx, uint8_t prog_idx, enmDswPsiPid psipid)
{
	return defProgPidBgn + chan_idx * defChnProgPidNum +
		defProgPidNum * (prog_idx - 1) + psipid;
}

static int do_parse_channel(PROG_INFO_T *chan_prog_info, uint8_t * p_chan_prog_cnt, uint8_t chan_idx)
{
	int i, j, k;
	uint8_t prog_cnt = 0;
	PROG_INFO_T *prog_info;

	pmt.p_descr = pmt_descr;
	nit.p_descr = nit_descr;
	for (i = 0; i < 5; i++) {
		pmt_descr[i].p_data = p_pmt_data[i];
		nit_descr[i].p_data = p_nit_data[i];
		cat_descr[i].p_data = p_cat_descr_data[i];
	}

	for (i = 0; i < UV_PROG_NUM_MAX; i++) {
		es[i].p_descr = es_descr[i];
		stream[i].p_descr = stream_descr[i];
		serv[i].p_descr = serv_descr[i];
		event[i].p_descr = event_descr[i];
		for (j = 0; j < 5; j++) {
			es_descr[i][j].p_data = p_es_data[i][j];
			stream_descr[i][j].p_data = p_stream_data[i][j];
			serv_descr[i][j].p_data = p_serv_data[i][j];
			event_descr[i][j].p_data = p_event_data[i][j];
		}
	}

	hfpga_dev.cha = chan_idx;
	dvbSI_Start(&hfpga_dev);

	trace_info("decode PAT ...");
	dvbSI_Dec_PAT(&pat, pid_data, &pid_num);
	trace_info("PAT decode done, TS id %02x, pmt pid num %d",
		pat.i_tran_stream_id, pid_num);
	for (i = 0; i < pid_num; i++) {
		trace_info("  program #%d: program number %02x, PMT %02x",
			i, pid_data[i].i_pg_num, pid_data[i].i_pid);
	}

	trace_info("decode PMT ...");
	for (i = 0; i < pid_num; i++) {
		if (pid_data[i].i_pid != NIT_PID) {
			prog_info = chan_prog_info + prog_cnt;
			prog_cnt++;
			prog_info->PMT_PID_IN = pid_data[i].i_pid;
			prog_info->PMT_PID_OUT =
				get_dsw_psi_pid(chan_idx, prog_cnt, DSW_PID_PMT);
			prog_info->prognum = pid_data[i].i_pg_num;

			trace_info("decode PMT %#x ...", pid_data[i].i_pid);
			pmt.i_pg_num = pid_data[i].i_pg_num;
			pmt.i_pmt_pid = pid_data[i].i_pid;
			dvbSI_Dec_PMT(&pmt, es, &es_num);

			prog_info->PCR_PID_IN = pmt.i_pcr_pid;
			prog_info->PCR_PID_OUT =
				get_dsw_psi_pid(chan_idx, prog_cnt, DSW_PID_PCR);
			trace_info("PCR %#x, %s descrs",
					pmt.i_pcr_pid, pmt.i_descr_num);

			for (j = 0; j < pmt.i_descr_num; j++) {
				trace_info("desc %d, tag %#x, len %d",	
					 j, pmt.p_descr[j].i_tag, pmt.p_descr[j].i_length);
				hex_dump("desc", pmt.p_descr[j].p_data,
					pmt.p_descr[j].i_length);
			}
			for (j = 0; j < es_num; j++) {
				trace_info("es %d, type %#x, pid %#x",
					j, es[j].i_type, es[j].i_pid);
				if (j < PROGRAM_DATA_PID_MAX_NUM) {
					prog_info->pids[j].type = es[j].i_type;
					if (es[j].i_pid != pmt.i_pcr_pid) {
						prog_info->pids[j].in = es[j].i_pid;
						prog_info->pids[j].out =
							get_dsw_psi_pid(chan_idx, prog_cnt,
											DSW_PID_VIDEO + j);
					} else {
						prog_info->pids[j].in = es[j].i_pid;
						prog_info->pids[j].out = get_dsw_psi_pid(chan_idx, prog_cnt, DSW_PID_PCR);
					}
				}

				for (k = 0; k < es[i].i_descr_num; k++) {
					trace_info("es descriptor %d, tag %#x, len %d",
						 k, es[j].p_descr[k].i_tag, es[j].p_descr[k].i_length);
					hex_dump("desc", es[j].p_descr[k].p_data,
							  es[j].p_descr[k].i_length);
				}
			}
		}
	}
	*p_chan_prog_cnt = prog_cnt;

	trace_info("decode SDT ...");
	dvbSI_Dec_SDT(&sdt, serv, &serv_num);
	trace_info("there are total %d services", serv_num);
	for (i = 0; i < prog_cnt; i++) {
		prog_info = chan_prog_info + i;
		for (j = 0; j < serv_num; j++) {
			if (serv[j].i_serv_id == prog_info->prognum) {
				trace_info("service #%d: service_id %#x",
					j, serv[j].i_serv_id);
				extract_program_name((unsigned char *)prog_info->progname,
						serv[j].p_descr->p_data);
			}
		}
	}

#if 0
	trace_info("decode CAT ... ");
	dvbSI_Dec_CAT(cat_descr, &cat_descr_num);

	trace_info("decode NIT ... ");
	dvbSI_Dec_NIT(&nit, stream, &stream_num);

	trace_info("decode EIT ... ");
	dvbSI_Dec_EIT(&eit, event, &event_num);
#endif

	dvbSI_Stop();

	return 0;
}

static int parse_channel(uint8_t chan_idx)
{
	uint16_t ts_status;
	uint8_t prog_num = 0;

	if (hfpga_get_ts_status(chan_idx, &ts_status) > 0) {
		do_parse_channel(&(g_prog_info_table[chan_idx * PROGRAM_MAX_NUM]),
			&prog_num, chan_idx);
		g_chan_num.num[chan_idx] = prog_num;
	}

	return prog_num;
}

int fp_psi_parse()
{
	int progs, total_progs = 0;
	uint8_t chan_idx;

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		progs = parse_channel(chan_idx);
		trace_info("channel #%d had %d programs", chan_idx, progs);
		total_progs += progs;
	}
	trace_info("there are total %d programs", total_progs);

	return total_progs;
}

