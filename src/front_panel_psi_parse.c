#include <string.h>

#include "wu/message.h"

#include "fpga_api.h"
#include "gen_dvb_si.h"

#include "front_panel_intstr.h"
#include "front_panel_define.h"
#include "pid_map_rule.h"
#include "psi_parse.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "fp_psi_parse"};

extern uv_dvb_io hfpga_dev;

#define UV_DESCR_LEN    (32)

static uv_pat_data pat;
static uv_pat_pid_data pid_data[PROGRAM_MAX_NUM + 1];
static uint16_t pid_num = 0;

static uv_pmt_data pmt;
static uv_descriptor pmt_descr[5];
static unsigned char p_pmt_data[5][UV_DESCR_LEN];
static uv_pmt_es_data es[PROGRAM_MAX_NUM];
static uv_descriptor es_descr[PROGRAM_MAX_NUM][5];
static unsigned char p_es_data[PROGRAM_MAX_NUM][5][UV_DESCR_LEN];

static uint16_t es_num = 0;

static uv_nit_data nit;
static uv_descriptor nit_descr[5];
static unsigned char p_nit_data[5][UV_DESCR_LEN];
static uv_nit_stream_data stream[PROGRAM_MAX_NUM];
static uv_descriptor stream_descr[PROGRAM_MAX_NUM][5];
static unsigned char p_stream_data[PROGRAM_MAX_NUM][5][UV_DESCR_LEN];

static uint16_t stream_num = 0;

static uv_sdt_data sdt;
static uv_sdt_serv_data serv[PROGRAM_MAX_NUM];
static uv_descriptor serv_descr[PROGRAM_MAX_NUM][5];
static unsigned char p_serv_data[PROGRAM_MAX_NUM][5][UV_DESCR_LEN];

static uint16_t serv_num = 0;

static uv_eit_data eit;
static uv_eit_event_data event[PROGRAM_MAX_NUM];
static uv_descriptor event_descr[PROGRAM_MAX_NUM][5];
static unsigned char p_event_data[PROGRAM_MAX_NUM][5][UV_DESCR_LEN];

static uint16_t event_num = 0;

static uv_descriptor cat_descr[5];
static unsigned char p_cat_descr_data[5][UV_DESCR_LEN];
static uint16_t cat_descr_num;


static void extract_program_name(unsigned char *desc_content,
									 unsigned char *prog_name)
{
	unsigned char prog_name_len;
	unsigned char prog_name_idx;
	uint8_t provid_name_len;
	uint8_t provide[256];

	/* provider name */
	provid_name_len = desc_content[1];
	memcpy(provide, &desc_content[1], provid_name_len);
	provide[provid_name_len] = 0;
	/* program name */
	prog_name_len = desc_content[1 + 1 + provid_name_len];
	prog_name[0] = prog_name_len;
	memcpy(prog_name + 1, &desc_content[3 + provid_name_len], prog_name_len);
	prog_name[1 + prog_name_len] = 0;
	trace_info("provid name %s, program name %s",
		provide, prog_name + 1);
}

static int do_parse_channel(PROG_INFO_T *chan_prog_info, uint8_t * p_chan_prog_cnt, uint8_t chan_idx)
{
	int i, j, k;
	uint8_t prog_cnt = 0;
	PROG_INFO_T *prog_info;
	int rc = 0;

	pmt.p_descr = pmt_descr;
	nit.p_descr = nit_descr;
	for (i = 0; i < 5; i++) {
		pmt_descr[i].p_data = p_pmt_data[i];
		nit_descr[i].p_data = p_nit_data[i];
		cat_descr[i].p_data = p_cat_descr_data[i];
	}

	for (i = 0; i < PROGRAM_MAX_NUM; i++) {
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
	psi_parse_timer_start();
	rc = dvbSI_Dec_PAT(&pat, pid_data, &pid_num);
	psi_parse_timer_stop();
	if (rc) {
		trace_err("pat parse failed! rc %d\n", rc);
		goto channel_analyse_done;
	}
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
				pid_map_rule_map_psi_pid(chan_idx, prog_cnt - 1, DSW_PID_PMT);
			prog_info->prognum = pid_data[i].i_pg_num;

			trace_info("decode PMT %#x ...", pid_data[i].i_pid);
			pmt.i_pg_num = pid_data[i].i_pg_num;
			pmt.i_pmt_pid = pid_data[i].i_pid;
			psi_parse_timer_start();
			rc = dvbSI_Dec_PMT(&pmt, es, &es_num);
			psi_parse_timer_stop();
			if (rc) {
				trace_err("pmt parse #%d failed! rc %d\n", i, rc);
				goto channel_analyse_done;
			}
			prog_info->PCR_PID_IN = pmt.i_pcr_pid;
			prog_info->PCR_PID_OUT =
				pid_map_rule_map_psi_pid(chan_idx, prog_cnt - 1, DSW_PID_PCR);
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
							pid_map_rule_map_psi_pid(chan_idx, prog_cnt - 1,
											DSW_PID_VIDEO + j);
					} else {
						prog_info->pids[j].in = es[j].i_pid;
						prog_info->pids[j].out = pid_map_rule_map_psi_pid(chan_idx, prog_cnt - 1, DSW_PID_PCR);
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
	psi_parse_timer_start();
	rc = dvbSI_Dec_SDT(&sdt, serv, &serv_num);
	psi_parse_timer_stop();
	if (rc) {
		trace_err("sdt parse failed! rc %d\n", rc);
		goto channel_analyse_done;
	}
	trace_info("there are total %d services", serv_num);
	for (j = 0; j < serv_num; j++) {
		for (i = 0; i < prog_cnt; i++) {
			prog_info = chan_prog_info + i;
			if (serv[j].i_serv_id == prog_info->prognum) {
				trace_info("service #%d: service_id %#x",
					j, serv[j].i_serv_id);
				extract_program_name(serv[j].p_descr->p_data,
						(unsigned char *)prog_info->progname);
			}
		}
		if (i == prog_cnt) {
			trace_warn("service #%d: service_id %#x, no owner program!",
				j, serv[j].i_serv_id);
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

channel_analyse_done:
	dvbSI_Stop();

	return rc;
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

