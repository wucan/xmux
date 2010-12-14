#include <string.h>

#include "wu/message.h"
#include "wu/wu_converter.h"

#include "xmux.h"
#include "xmux_config.h"
#include "psi_parse.h"
#include "psi_gen.h"
#include "section.h"
#include "gen_dvb_si.h"
#include "front_panel_intstr.h"
#include "hfpga.h"


extern uv_dvb_io hfpga_dev;

static msgobj mo = {MSG_INFO, ENCOLOR, "psi_gen"};

static int pkt_offset;

void fill_output_psi_data(int psi_type, uint8_t *ts_buf, int ts_len)
{
	struct xmux_output_psi_data *psi_data = &g_eeprom_param.output_psi_area.output_psi;

	if (psi_type == 0) {
		memset(psi_data->psi_ents, 0, sizeof(psi_data->psi_ents));
		pkt_offset = 0;
	}

	if (!psi_data->psi_ents[psi_type].nr_ts_pkts) {
		psi_data->psi_ents[psi_type].offset = pkt_offset;
	}
	psi_data->psi_ents[psi_type].nr_ts_pkts += ts_len / TS_PACKET_BYTES;
	memcpy(&psi_data->ts_pkts[pkt_offset], ts_buf, ts_len);
	
	pkt_offset += ts_len / TS_PACKET_BYTES;
}

int psi_gen_output_psi_from_sections()
{
	int sec_len, ts_len;
	uint8_t ts_buf[188 * 7];
	int cc;
	uint8_t sec_idx;

	/*
	 * PAT
	 */
	cc = 0;
	sec_len = sg_mib_xxx_len(sg_mib_pat[CHANNEL_MAX_NUM]);
	ts_len = section_to_ts_length(sec_len);
	ts_len = section_to_ts(sg_mib_pat[CHANNEL_MAX_NUM] + 2, sec_len, ts_buf, PAT_PID, &cc);
	fill_output_psi_data(0, ts_buf, ts_len);

	/*
	 * PMT
	 */
	{
	struct pid_trans_info_snmp_data *pid_trans_info;
	struct xmux_program_info *prog;
	uint8_t chan_idx, prog_idx;
	int sel_nprogs = 0;

	cc = 0;
	pid_trans_info = g_eeprom_param.pid_trans_info_area.pid_trans_info;
	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		for (prog_idx = 0; prog_idx < pid_trans_info[chan_idx].nprogs; prog_idx++) {
			if (!(pid_trans_info[chan_idx].status & (1 << prog_idx)))
				continue;
			printf("pid_gen: gen pmt, use secion chan #%d, prog #%d\n",
				chan_idx, prog_idx);
			prog = &pid_trans_info[chan_idx].programs[prog_idx];
			sec_len = sg_mib_xxx_len(sg_mib_pmt[CHANNEL_MAX_NUM][sel_nprogs]);
			ts_len = section_to_ts_length(sec_len);
			ts_len = section_to_ts(sg_mib_pmt[CHANNEL_MAX_NUM][sel_nprogs] + 2,
				sec_len, ts_buf, prog->pmt.out, &cc);
			fill_output_psi_data(1, ts_buf, ts_len);
			sel_nprogs++;
			if (sel_nprogs >= PROGRAM_MAX_NUM) {
				printf("pid_gen: gen pmt, max program reached, can't add more!",
					sel_nprogs);
				goto gen_pmt_done;
			}
		}
	}
gen_pmt_done:
		;
	}

	/*
	 * SDT
	 */
	cc = 0;
	for (sec_idx = 0; sec_idx < SDT_SECTION_NUM; sec_idx++) {
		sec_len = sg_mib_xxx_len(sg_mib_sdt[CHANNEL_MAX_NUM][sec_idx]);
		ts_len = section_to_ts_length(sec_len);
		ts_len = section_to_ts(sg_mib_sdt[CHANNEL_MAX_NUM][sec_idx] + 2,
			sec_len, ts_buf, SDT_PID, &cc);
		fill_output_psi_data(2, ts_buf, ts_len);
	}

	/*
	 * NIT
	 */
	cc = 0;
	sec_len = sg_mib_xxx_len(sg_mib_nit[CHANNEL_MAX_NUM]);
	ts_len = section_to_ts_length(sec_len);
	ts_len = section_to_ts(sg_mib_nit[CHANNEL_MAX_NUM] + 2,
		sec_len, ts_buf, NIT_PID, &cc);
	fill_output_psi_data(3, ts_buf, ts_len);

	/*
	 * CAT
	 */
	cc = 0;
	sec_len = sg_mib_xxx_len(sg_mib_cat[CHANNEL_MAX_NUM]);
	ts_len = section_to_ts_length(sec_len);
	ts_len = section_to_ts(sg_mib_cat[CHANNEL_MAX_NUM] + 2,
		sec_len, ts_buf, CAT_PID, &cc);
	fill_output_psi_data(4, ts_buf, ts_len);

	/*
	 * at last save it to eeprom
	 */
	xmux_config_save_output_psi_data();

	return 0;
}

int psi_apply_from_output_psi()
{
	struct xmux_output_psi_data *psi_data = &g_eeprom_param.output_psi_area.output_psi;
	struct output_psi_data_entry *ent;
	uint8_t psi_type, howto = 0;

	/*
	 * FIXME: PAT, PMT, CAT: howto = 0; SDT, NIT: howto = 1
	 */
	dvbSI_Start(&hfpga_dev);
	for (psi_type = 0; psi_type < OUTPUT_PSI_TYPE_MAX_NUM; psi_type++) {
		ent = &psi_data->psi_ents[psi_type];
		if (ent->nr_ts_pkts) {
			hfpga_dev.write(&psi_data->ts_pkts[ent->offset], 188 * ent->nr_ts_pkts, howto);
		}
	}
	dvbSI_Stop(&hfpga_dev);

	return 0;
}

/*
 * SDT
 */
void sdt_gen_context_init(struct sdt_gen_context *ctx)
{
	ctx->serv_num = 0;
	ctx->desc_num = 1;

	ctx->sdt_data.i_table_id = 0x42;
	ctx->sdt_data.i_tran_stream_id = 0x1234;
	ctx->sdt_data.i_orig_net_id = 0xEAEA;
}
void sdt_gen_context_pack(struct sdt_gen_context *ctx)
{
	int i, j;
	uv_descriptor *p_descr;
	uv_sdt_serv_data *sdt_serv_data = ctx->sdt_serv_data;
	static uint8_t sdbuf[PROGRAM_MAX_NUM][5][UV_DESCR_LEN];

	for (i = 0; i < ctx->serv_num; i++) {
		sdt_serv_data[i].i_serv_id = ctx->serv_info[i].service_id;
		sdt_serv_data[i].i_eit_pres_foll_flag = 0;
		sdt_serv_data[i].i_eit_sched_flag = 0;
		sdt_serv_data[i].i_free_ca_mode = 0;
		sdt_serv_data[i].i_running_status = 1;
		sdt_serv_data[i].i_descr_num = ctx->desc_num;
		sdt_serv_data[i].p_descr = (uv_descriptor *)malloc(
			sdt_serv_data[i].i_descr_num * sizeof(uv_descriptor));
		p_descr = sdt_serv_data[i].p_descr;
		for (j = 0; j < sdt_serv_data[i].i_descr_num; j++) {
			char name[32];
			p_descr[j].i_tag = 0x48;
			p_descr[j].i_length = 3 + ctx->serv_info[i].provider_len + ctx->serv_info[i].name_len;
			p_descr[j].p_data = sdbuf[i][j];
			p_descr[j].p_data[0] = 0x01; // serivece type: DTV
			p_descr[j].p_data[1] = ctx->serv_info[i].provider_len;
			memcpy(p_descr[j].p_data + 2, ctx->serv_info[i].provider, ctx->serv_info[i].provider_len);
			p_descr[j].p_data[6] = ctx->serv_info[i].name_len;
			memcpy(p_descr[j].p_data + 3 + ctx->serv_info[i].provider_len, ctx->serv_info[i].name, ctx->serv_info[i].name_len);
		}
	}
}
void sdt_gen_context_add_service(struct sdt_gen_context *ctx,
		const char *serv_name, int service_id, const char *provider)
{
	struct sdt_service_info *info = &ctx->serv_info[ctx->serv_num];

	info->service_id = service_id;
	info->name_len = strlen(serv_name);
	memcpy(info->name, serv_name, info->name_len);
	info->provider_len = strlen(provider);
	memcpy(info->provider, provider, info->provider_len);
	ctx->serv_num++;
}
void sdt_gen_context_free(struct sdt_gen_context *ctx)
{
	int i;

	for (i = 0; i < ctx->serv_num; i++) {
		free(ctx->sdt_serv_data[i].p_descr);
	}
}
/*
 * PAT
 */
void pat_gen_context_init(struct pat_gen_context *ctx)
{
	ctx->nprogs = 0;
	ctx->tpat.i_tran_stream_id = 0x123;
}
void pat_gen_context_pack(struct pat_gen_context *ctx)
{
}
void pat_gen_context_add_program(struct pat_gen_context *ctx,
		uint16_t prog_num, uint16_t pmt_pid)
{
	ctx->programs[ctx->nprogs].prog_num = prog_num;
	ctx->programs[ctx->nprogs].pmt_pid = pmt_pid;
	ctx->nprogs++;
}
void pat_gen_context_free(struct pat_gen_context *ctx)
{
}
/*
 * PMT
 */
static uv_descriptor tpmt_descr[5];
static unsigned char tp_pmt_data[5][UV_DESCR_LEN];
static uv_descriptor tes_descr[PROGRAM_MAX_NUM][5];
static unsigned char tp_es_data[PROGRAM_MAX_NUM][5][UV_DESCR_LEN];
void pmt_gen_context_init(struct pmt_gen_context *ctx)
{
	int i, j;

	ctx->tpmt.p_descr = tpmt_descr;
	for (i = 0; i < 5; i++) {
		tpmt_descr[i].p_data = tp_pmt_data[i];
	}
	for (i = 0; i < PROGRAM_MAX_NUM; i++) {
		ctx->tes[i].p_descr = tes_descr[i];
		for (j = 0; j < 5; j++) {
			tes_descr[i][j].p_data = tp_es_data[i][j];
		}
	}

	ctx->nes = 0;
}
void pmt_gen_context_pack(struct pmt_gen_context *ctx)
{
}
void pmt_gen_context_add_program_info(struct pmt_gen_context *ctx,
		uint16_t prog_num, uint16_t pmt_pid, uint16_t pcr_pid)
{
	uv_pmt_data *tpmt = &ctx->tpmt;

	tpmt->i_pg_num = prog_num;
	tpmt->i_pmt_pid = pmt_pid;
	tpmt->i_pcr_pid = pcr_pid;

	tpmt->i_descr_num = 0;
}
void pmt_gen_context_add_es(struct pmt_gen_context *ctx,
		uint16_t pid, uint8_t type)
{
	uv_pmt_es_data *tes = &ctx->tes[ctx->nes];

	tes->i_pid = pid;
	tes->i_type = type;
	tes->i_descr_num = 0;

	ctx->nes++;
}
void pmt_gen_context_free(struct pmt_gen_context *ctx)
{
}
/*
 * NIT
 */
static void formdescr_cable(unsigned char *buf, unsigned int frequency,
		unsigned char FEC_outer, unsigned char modulation,
		unsigned int sym_rate, unsigned char FEC_inner)
{
	unsigned int freqH = wu_bcd2hex(frequency);
	unsigned int rateH = wu_bcd2hex(sym_rate);

	buf[0] = freqH >> 24;		// from MSB byte to LSB byte
	buf[1] = freqH >> 16;
	buf[2] = freqH >> 8;
	buf[3] = freqH;
	buf[4] = 0xFF;
	buf[5] = 0xF0 | FEC_outer;	//fec_puter
	buf[6] = modulation;		// mod 

	buf[7] = rateH >> 20;		// mod 
	buf[8] = rateH >> 12;		// mod 
	buf[9] = rateH >> 4;		// mod 
	buf[10] = ((0x0F & rateH) << 4) | (0x0F & FEC_inner);	// mod 
}
static uint8_t nit_buf[5][UV_DESCR_LEN];
static uint8_t nit_buf_stream[5][5][UV_DESCR_LEN];
void nit_gen_context_init(struct nit_gen_context *ctx)
{
	int i;
	uv_nit_data *nit_data = &ctx->nit_data;

	ctx->desc_num = 1;

	nit_data->i_table_id = 0x40;
	nit_data->i_net_id = 0x1234;
	nit_data->i_descr_num = ctx->desc_num;
	nit_data->p_descr = (uv_descriptor *)malloc(ctx->desc_num * sizeof(uv_descriptor));

	for (i = 0; i < ctx->desc_num; i++) {
		nit_data->p_descr[i].i_tag = 0x44;	// cable_delivery_system_descriptor
		nit_data->p_descr[i].i_length = 11;
		nit_data->p_descr[i].p_data = nit_buf[i];
		formdescr_cable(nit_buf[i], 03100000, 1, 0x02, 0274500, 0xF);	//310MHz
	}
}
void nit_gen_context_pack(struct nit_gen_context *ctx)
{
}
void nit_gen_context_add_stream(struct nit_gen_context *ctx,
		uint32_t tsid, uint8_t stream_desc_num)
{
	uv_nit_stream_data *nit_stream_data = &ctx->nit_stream_data[ctx->stream_num];
	uv_descriptor *p_stream_descr;
	uint32_t freqCr;
	int j;

	nit_stream_data->i_tran_stream_id = tsid;
	nit_stream_data->i_orig_net_id = 0x123;
	nit_stream_data->i_descr_num = stream_desc_num;
	nit_stream_data->p_descr = (uv_descriptor *)malloc(stream_desc_num * sizeof(uv_descriptor));

	p_stream_descr = nit_stream_data->p_descr;
	for (j = 0; j < stream_desc_num; j++) {
		uint8_t *buf_stream = nit_buf_stream[ctx->stream_num][j];

		p_stream_descr[j].i_tag = 0x62;	// frequency_list_descriptor
		p_stream_descr[j].i_length = 5;
		p_stream_descr[j].p_data = buf_stream;
		buf_stream[0] = 0xFE;	// coding type( calble or wireless and so on)
		freqCr = wu_bcd2hex(280 + 10 * ctx->stream_num + j);
		buf_stream[1] = freqCr >> 24;
		buf_stream[2] = freqCr >> 16;
		buf_stream[3] = freqCr >> 8;
		buf_stream[4] = freqCr;
	}

	ctx->stream_num++;
}
void nit_gen_context_free(struct nit_gen_context *ctx)
{
	int i;

	free(ctx->nit_data.p_descr);
	for (i = 0; i < ctx->stream_num; i++) {
		free(ctx->nit_stream_data[i].p_descr);
	}
}
/*
 * CAT
 */
static uint8_t cat_desc_data[5][UV_DESCR_LEN];
void cat_gen_context_init(struct cat_gen_context *ctx)
{
	ctx->desc_num = 0;
}
void cat_gen_context_pack(struct cat_gen_context *ctx)
{
}
void cat_gen_context_add_ca_system(struct cat_gen_context *ctx,
		uint16_t ca_system_id, uint16_t ca_pid)
{
	uv_descriptor *desc = &ctx->cat_desc[ctx->desc_num];

	desc->i_tag = 0x09;
	desc->i_length = 4;
	desc->p_data = cat_desc_data[ctx->desc_num];

	desc->p_data[0] = ca_system_id >> 8;
	desc->p_data[1] = ca_system_id & 0xFF;
	desc->p_data[2] = ca_pid >> 8;
	desc->p_data[3] = ca_pid & 0xFF;

	ctx->desc_num++;
}
void cat_gen_context_free(struct cat_gen_context *ctx)
{
}

/*
 * generate psi and download to fpga
 */
int psi_gen_and_apply_from_fp()
{
	uint8_t *packpara[8192];

	dvbSI_Start(&hfpga_dev);

	trace_info("stop gen si");
	dvbSI_GenSS(HFPGA_CMD_SI_STOP);

	gen_pat_pmt_from_fp(packpara, g_prog_info_table);
	gen_sdt_from_fp(packpara, g_prog_info_table);
	gen_nit_from_fp();
	gen_cat_from_fp();

	trace_info("start gen si");
	dvbSI_GenSS(HFPGA_CMD_SI_START);

	dvbSI_Stop();

	return 0;
}

