#include <string.h>

#include "wu/message.h"

#include "fpga_api.h"
#include "gen_dvb_si.h"

#include "front_panel_intstr.h"
#include "front_panel_define.h"
#include "pid_map_rule.h"
#include "psi_parse.h"
#include "ca.h"


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

static void clear_prog_info_table();

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

static int scan_program_pids(uv_pmt_data *pmt,
		uv_pmt_es_data *es, uint16_t es_num, uint16_t *pids)
{
	int nr_pids = 0;
	int i;

	/*
	 * scan pids
	 */
	pids[0] = pmt->i_pcr_pid;
	nr_pids = 1;
	for (i = 0; i < es_num; i++) {
		if (es[i].i_pid == pmt->i_pcr_pid)
			continue;
		pids[nr_pids] = es[i].i_pid;
		nr_pids++;
	}

	return nr_pids;
}

static int do_parse_channel(PROG_INFO_T *chan_prog_info, uint8_t * p_chan_prog_cnt, uint8_t chan_idx)
{
	int i, j, k;
	uint8_t prog_cnt = 0;
	PROG_INFO_T *prog_info;
	int rc = 0;
	uint16_t pids[PROGRAM_PID_MAX_NUM];
	int nr_pids;

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
	sg_si_param.tbl_type = EUV_TBL_PAT;
	psi_parse_timer_start(5);
	rc = dvbSI_Dec_PAT(&pat, pid_data, &pid_num);
	psi_parse_timer_stop();
	if (rc) {
		trace_err("pat parse failed! rc %d\n", rc);
		goto channel_analyse_done;
	}
	trace_info("PAT decode done, TS id %#x, pmt pid num %d",
		pat.i_tran_stream_id, pid_num);
	for (i = 0; i < pid_num; i++) {
		trace_info("  program #%d: program number %#x, PMT %#x",
			i, pid_data[i].i_pg_num, pid_data[i].i_pid);
	}

	trace_info("decode PMT ...");
	sg_si_param.tbl_type = EUV_TBL_PMT;
	for (i = 0; i < pid_num; i++) {
		if (pid_data[i].i_pg_num != 0x00) {
			prog_info = chan_prog_info + prog_cnt;
			prog_cnt++;
			prog_info->info.pmt.in = pid_data[i].i_pid;
			prog_info->info.pmt.out =
				pid_map_rule_map_psi_pid(chan_idx, prog_cnt - 1,
					DSW_PID_PMT, pid_data[i].i_pid, NULL, 0);
			prog_info->info.prog_num = pid_data[i].i_pg_num;

			trace_info("decode program_number %d, PMT %#x ...",
				pid_data[i].i_pg_num, pid_data[i].i_pid);
			pmt.i_pg_num = pid_data[i].i_pg_num;
			pmt.i_pmt_pid = pid_data[i].i_pid;
			psi_parse_timer_start(5);
			rc = dvbSI_Dec_PMT(&pmt, es, &es_num);
			psi_parse_timer_stop();
			if (rc) {
				trace_err("pmt parse #%d failed! rc %d\n", i, rc);
				goto channel_analyse_done;
			}

			/*
			 * pmt descriptors
			 * if we found CA_Descriptor, then it's scrambled!
			 */
			for (j = 0; j < pmt.i_descr_num; j++) {
				trace_info("PMT desc #%d, tag %#x, len %d, data:",
					j, pmt.p_descr[j].i_tag, pmt.p_descr[j].i_length);
				if (pmt.p_descr[j].i_tag == CA_DR_TAG) {
					trace_info("found CA_Descriptor!");
					prog_info->status |= FP_STATUS_SCRAMBLED;
				}
			}

			/*
			 * scan PCR and data PIDs and we'll use them do pid remap
			 */
			nr_pids = scan_program_pids(&pmt, &es, es_num, pids);

			prog_info->info.pcr.type = PID_TYPE_PCR;
			prog_info->info.pcr.in = pmt.i_pcr_pid;
			prog_info->info.pcr.out =
				pid_map_rule_map_psi_pid(chan_idx, prog_cnt - 1,
					DSW_PID_PCR, pmt.i_pcr_pid, pids, nr_pids);
			trace_info("PCR %#x, %d descrs",
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
					prog_info->info.data[j].type = es[j].i_type;
					if (es[j].i_pid != pmt.i_pcr_pid) {
						prog_info->info.data[j].in = es[j].i_pid;
						prog_info->info.data[j].out =
							pid_map_rule_map_psi_pid(chan_idx, prog_cnt - 1,
								DSW_PID_VIDEO, es[j].i_pid, pids, nr_pids);
					} else {
						prog_info->info.data[j].in = es[j].i_pid;
						prog_info->info.data[j].out = prog_info->info.pcr.out;
						/* correct pcr type */
						if (es_is_video(es[j].i_type))
							prog_info->info.pcr.type = PID_TYPE_PCR_VIDEO;
						else
							prog_info->info.pcr.type = PID_TYPE_PCR_AUDIO;
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
	sg_si_param.tbl_type = EUV_TBL_SDT;
	psi_parse_timer_start(20);
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
			if (serv[j].i_serv_id == prog_info->info.prog_num) {
				trace_info("service #%d: service_id %#x",
					j, serv[j].i_serv_id);
				extract_program_name(serv[j].p_descr->p_data,
						(unsigned char *)prog_info->info.prog_name);
				/* set default output program name same with original */
				memcpy(prog_info->info.prog_name[1],
					prog_info->info.prog_name[0], PROGRAM_NAME_SIZE);
				break;
			}
		}
		if (i == prog_cnt) {
			trace_warn("service #%d: service_id %#x, no owner program!",
				j, serv[j].i_serv_id);
		}
	}

#if 0
	trace_info("decode CAT ... ");
	sg_si_param.tbl_type = EUV_TBL_CAT;
	dvbSI_Dec_CAT(cat_descr, &cat_descr_num);

	trace_info("decode NIT ... ");
	sg_si_param.tbl_type = EUV_TBL_NIT;
	dvbSI_Dec_NIT(&nit, stream, &stream_num);

	trace_info("decode EIT ... ");
	sg_si_param.tbl_type = EUV_TBL_EIT;
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

	sg_si_param.type = EUV_DEFAULT;
	if (hfpga_get_ts_status(chan_idx, &ts_status) > 0) {
		do_parse_channel(&(g_prog_info_table[chan_idx * PROGRAM_MAX_NUM]),
			&prog_num, chan_idx);
		g_chan_num.num[chan_idx] = prog_num;
	}

	return prog_num;
}

struct program_attribute g_prog_attr_table[CHANNEL_MAX_NUM * PROGRAM_MAX_NUM];
void fp_build_program_attr_table()
{
	int i;
	uint8_t chan_idx, prog_idx, tmp_prog_idx;
	PROG_INFO_T *prog;
	PROG_INFO_T *chan_prog_table;
	struct program_attribute *chan_prog_attr_table;

	memset(g_prog_attr_table, 0, sizeof(g_prog_attr_table));
	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		uint8_t prog_num = g_chan_num.num[chan_idx];
		if (prog_num == 0)
			continue;

		chan_prog_table = &g_prog_info_table[chan_idx * PROGRAM_MAX_NUM];
		chan_prog_attr_table = &g_prog_attr_table[chan_idx * PROGRAM_MAX_NUM];

		// build pub pcr and solo pcr
		for (prog_idx = 0; prog_idx < prog_num; prog_idx++) {
			prog = &chan_prog_table[prog_idx];
			for (tmp_prog_idx = 0; tmp_prog_idx < prog_num; tmp_prog_idx++) {
				PROG_INFO_T *tmp_prog;
				if (prog_idx == tmp_prog_idx)
					continue;
				tmp_prog = &chan_prog_table[tmp_prog_idx];
				if (prog->info.pcr.in == tmp_prog->info.pcr.in) {
					if (chan_prog_attr_table[tmp_prog_idx].pcr_type == PUB_PCR) {
						chan_prog_attr_table[prog_idx] = chan_prog_attr_table[tmp_prog_idx];
					} else {
						chan_prog_attr_table[prog_idx].pcr_type = PUB_PCR;
						chan_prog_attr_table[prog_idx].pcr_group_id =
							chan_idx * PROGRAM_MAX_NUM + prog_idx;
					}
					break;
				}
			}
			if (tmp_prog_idx == prog_num) {
				chan_prog_attr_table[prog_idx].pcr_type = SOLO_PCR;
				chan_prog_attr_table[prog_idx].pcr_group_id =
					chan_idx * PROGRAM_MAX_NUM + prog_idx;
			}
		}

		// build common pcr
		for (prog_idx = 0; prog_idx < prog_num; prog_idx++) {
			prog = &chan_prog_table[prog_idx];
			if (chan_prog_attr_table[prog_idx].pcr_type == SOLO_PCR) {
				uint8_t d;
				for (d = 0; d < PROGRAM_DATA_PID_MAX_NUM; d++) {
					if (prog->info.data[d].in == prog->info.pcr.in) {
						chan_prog_attr_table[prog_idx].pcr_type = COM_PCR;
						break;
					}
				}
			}
		}
	}

	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		if (g_prog_attr_table[i].pcr_type != NA_PCR) {
			trace_info("program #%d pcr type %s, group_id %d",
				i,
				pcr_type_name(g_prog_attr_table[i].pcr_type),
				g_prog_attr_table[i].pcr_group_id);
		}
	}
}

int fp_psi_parse()
{
	int progs, total_progs = 0;
	uint8_t chan_idx;

	/* clear fp psi_parse state data */
	memset(&g_chan_num, 0, sizeof(g_chan_num));
	clear_prog_info_table();

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		progs = parse_channel(chan_idx);
		trace_info("channel #%d had %d programs", chan_idx, progs);
		total_progs += progs;
	}
	trace_info("there are total %d programs", total_progs);
	hex_dump("g_chan_num", &g_chan_num, sizeof(g_chan_num));

	fp_build_program_attr_table();

	return total_progs;
}

static void clear_prog_info_table()
{
	uint8_t chan_idx;
	uint8_t prog_idx, pid_idx;
	uint16_t in_pid, out_pid;
	struct xmux_program_info *prog;

	memset(g_prog_info_table, 0,
		sizeof(PROG_INFO_T) * CHANNEL_MAX_NUM * PROGRAM_MAX_NUM);
	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		for (prog_idx = 0; prog_idx < PROGRAM_MAX_NUM; prog_idx++) {
			prog = &g_prog_info_table[chan_idx * PROGRAM_MAX_NUM + prog_idx].info;
			prog->pmt.type = PID_TYPE_PMT;
			prog->pmt.in = DATA_PID_PAD_VALUE;
			prog->pmt.out = DATA_PID_PAD_VALUE;
			prog->pcr.type = PID_TYPE_PCR;
			prog->pcr.in = DATA_PID_PAD_VALUE;
			prog->pcr.out = DATA_PID_PAD_VALUE;
			for (pid_idx = 0; pid_idx < PROGRAM_DATA_PID_MAX_NUM; pid_idx++) {
				prog->data[pid_idx].type = PID_TYPE_PAD;
				prog->data[pid_idx].in = DATA_PID_PAD_VALUE;
				prog->data[pid_idx].out = DATA_PID_PAD_VALUE;
			}
		}
	}
}

