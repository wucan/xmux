#include <fcntl.h>

#include "wu/message.h"
#include "wu/wu_converter.h"

#include "xmux.h"
#include "fpga_api.h"
#include "psi_gen.h"

#include "front_panel_intstr.h"
#include "front_panel_define.h"


extern uv_dvb_io hfpga_dev;

static msgobj mo = {MSG_INFO, ENCOLOR, "fp-psi-gen"};

static uint8_t sdbuf[PROGRAM_MAX_NUM][5][UV_DESCR_LEN];

static int GenSDT(void)
{
	struct sdt_gen_context gen_ctx;

	trace_info("generate SDT ...");
	sdt_gen_context_init(&gen_ctx);
	sdt_gen_context_add_service(&gen_ctx, "CCTV1", 1, "CCTV");
	sdt_gen_context_add_service(&gen_ctx, "CCTV2", 2, "CCTV");
	sdt_gen_context_pack(&gen_ctx);

	dvbSI_Gen_SDT(&gen_ctx.sdt_data, gen_ctx.sdt_serv_data, gen_ctx.serv_num);

	sdt_gen_context_free(&gen_ctx);

	return 0;
}

void formdescr_cable(unsigned char *buf, unsigned int frequency,
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

static int GenNIT(void)
{
	uv_nit_data nit_data;
	uv_nit_stream_data nit_stream_data[5];
	uint16_t stream_num = 2;
	uv_descriptor *p_descr;
	uint16_t nit_descr_num = 1;
	uv_descriptor *p_stream_descr;
	int stream_descr_num = 1;

	int i, j;
	unsigned int freqCr;

	uint8_t buf[5][UV_DESCR_LEN];
	uint8_t buf_stream[5][5][UV_DESCR_LEN];

	trace_info("generate NIT ...");

	nit_data.i_table_id = 0x40;
	nit_data.i_net_id = 0x1234;
	nit_data.i_descr_num = nit_descr_num;

	nit_data.p_descr =
		(uv_descriptor *) malloc(nit_data.i_descr_num *
								 sizeof(uv_descriptor));
	for (i = 0; i < nit_descr_num; i++) {
		nit_data.p_descr[i].i_tag = 0x44;	// cable_delivery_system_descriptor
		nit_data.p_descr[i].i_length = 11;
		nit_data.p_descr[i].p_data = buf[i];
		formdescr_cable((uint8_t *) buf, 03100000, 1, 0x02, 0274500, 0xF);	//310MHz
	}
	for (i = 0; i < stream_num; i++) {
		nit_stream_data[i].i_tran_stream_id = i + 1;
		nit_stream_data[i].i_orig_net_id = 0x123;
		nit_stream_data[i].i_descr_num = stream_descr_num;
		nit_stream_data[i].p_descr =
			(uv_descriptor *) malloc(nit_stream_data[i].i_descr_num *
									 sizeof(uv_descriptor));
		p_stream_descr = nit_stream_data[i].p_descr;

		for (j = 0; j < nit_stream_data[i].i_descr_num; j++) {
			p_stream_descr[j].i_tag = 0x62;	// frequency_list_descriptor
			p_stream_descr[j].i_length = 5;
			p_stream_descr[j].p_data = buf_stream[i][j];
			buf_stream[i][j][0] = 0xFE;	// coding type( calble or wireless and so on)
			freqCr = wu_bcd2hex(280 + 10 * i + j);
			buf_stream[i][j][1] = freqCr >> 24;
			buf_stream[i][j][2] = freqCr >> 16;
			buf_stream[i][j][3] = freqCr >> 8;
			buf_stream[i][j][4] = freqCr;
		}
	}

	dvbSI_Gen_NIT(&nit_data, nit_stream_data, stream_num);

	// free mem
	free(nit_data.p_descr);
	for (i = 0; i < stream_num; i++) {
		free(nit_stream_data[i].p_descr);
	}

	return 0;
}

static int GenCAT(void)
{
	int i, j;
	uv_descriptor cat_descr[5];
	unsigned char p_cat_descr_data[5][UV_DESCR_LEN];

	uint16_t cat_descr_num = 1;

	trace_info("generate CAT ...");

	for (i = 0; i < cat_descr_num; i++) {
		cat_descr[i].i_tag = 0x09;
		cat_descr[i].i_length = 4;
		cat_descr[i].p_data = p_cat_descr_data[i];
		for (j = 0; j < 4; j++) {
			p_cat_descr_data[i][0] = i;
			p_cat_descr_data[i][1] = 0x10 + j;	// CA_System_ID
			p_cat_descr_data[i][2] = 0;
			p_cat_descr_data[i][3] = 0x20 + j;	// CA_PID
		}
	}

	dvbSI_Gen_CAT(cat_descr, cat_descr_num);
}

// ------ PAT Start
static uv_pat_data tpat;
static uv_pat_pid_data tpid_data[PROGRAM_MAX_NUM + 1];
static uint16_t tpid_num = PROGRAM_MAX_NUM;

// ------ PAT End

// ------ PMT Start
static uv_pmt_data tpmt;
static uv_descriptor tpmt_descr[5];
static unsigned char tp_pmt_data[5][UV_DESCR_LEN];

static uv_pmt_es_data tes[PROGRAM_MAX_NUM];
static uv_descriptor tes_descr[PROGRAM_MAX_NUM][5];
static unsigned char tp_es_data[PROGRAM_MAX_NUM][5][UV_DESCR_LEN];

static uint16_t tes_num = 0;

static int get_dsw_provider_len(void)
{
	return strlen(defProviderDsw);
}

// ------ PMT End
int gen_sdt_fr_mcu(uint8_t * packpara, const PROG_INFO_T * pProgpara)
{
	struct sdt_gen_context gen_ctx;
	int ncount;
	int nProgSel = 0;
	PROG_INFO_T *pProg;

	trace_info("generate SDT ...");
	sdt_gen_context_init(&gen_ctx);
	for (ncount = 0; ncount < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; ncount++) {
		pProg = (PROG_INFO_T *) pProgpara + ncount;
		if (pProg->status == 1) {
			pProg->info.prog_name[1][pProg->info.prog_name[1][0]] = 0;
			sdt_gen_context_add_service(&gen_ctx, pProg->info.prog_name[1][1], nProgSel + 1, defProviderDsw);
			nProgSel++;
			if (nProgSel >= PROGRAM_MAX_NUM)
				break;
		}
	}
	sdt_gen_context_pack(&gen_ctx);
	dvbSI_Gen_SDT(&gen_ctx.sdt_data, gen_ctx.sdt_serv_data, gen_ctx.serv_num);
	sdt_gen_context_pack(&gen_ctx);

	return 0;
}


int gen_pat_pmt_fr_mcu(uint8_t * packpara, const PROG_INFO_T * pProgpara)
{
	int i, j, k;
	uint8_t packet[188];

	uint8_t buf[PROGRAM_MAX_NUM * (4 + PROGRAM_DATA_PID_MAX_NUM * 2) * sizeof(uint16_t)];
	int nProgSel = 0;
	int nProgNum = 0;
	PROG_INFO_T *pProg = (PROG_INFO_T *) pProgpara;

	trace_info("generate PAT&PMT ...");
	tpmt.p_descr = tpmt_descr;
	for (i = 0; i < 5; i++) {
		tpmt_descr[i].p_data = tp_pmt_data[i];
	}
	for (i = 0; i < PROGRAM_MAX_NUM; i++) {
		tes[i].p_descr = tes_descr[i];
		for (j = 0; j < 5; j++) {
			tes_descr[i][j].p_data = tp_es_data[i][j];
		}
	}

	// Begin Set Values
	tpat.i_tran_stream_id = 0x123;

	nProgSel = 0;
	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		int j;
		// 4 is PMT_PID_IN,PMT_PID_OUT,PCR_PID_IN,PCR_PID_OUT
		// defProgPidNum*2 is PIDS
		pProg = (PROG_INFO_T *) pProgpara + i;

		if (pProg->status == 1) {

			tpid_data[nProgSel].i_pg_num = nProgSel + 1;

			tpid_data[nProgSel].i_pid = pProg->info.pmt.out;

			nProgSel++;
			if (nProgSel >= PROGRAM_MAX_NUM)
				break;
		}
	}

	trace_info("generate PAT ...");
	dvbSI_Gen_PAT(&tpat, tpid_data, tpid_num);

	nProgSel = 0;
	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		int j;
		// 4 is PMT_PID_IN,PMT_PID_OUT,PCR_PID_IN,PCR_PID_OUT
		// defProgPidNum*2 is PIDS
		pProg = (PROG_INFO_T *) pProgpara + i;
		if (pProg->status == 1) {
			tpmt.i_pg_num = nProgSel + 1;

			tpmt.i_pmt_pid = pProg->info.pmt.out;
			tpmt.i_pcr_pid = pProg->info.pcr.out;
			tpmt.i_descr_num = 0;

			tes_num = 0;
			for (j = 0; j < PROGRAM_DATA_PID_MAX_NUM; j++)	// Video Audio
			{
				uint16_t out_pid = pProg->info.data[j].out;
				if (out_pid != 0x00 && out_pid != 0x0F) {
					tes[j].i_type = pProg->info.data[j].type;
					tes[j].i_pid = out_pid;
					tes[j].i_descr_num = 0;	//
					tes_num++;
				}
			}
			trace_info("generate PMT of program $#%d ...", i);
			dvbSI_Gen_PMT(&tpmt, tes, tes_num);
			nProgSel++;

			if (nProgSel >= PROGRAM_MAX_NUM)
				break;
		}
	}

	return 0;
}



static int GenPAT_and_PMT(void)
{
	int i, j, k;
	uint8_t packet[188];

	trace_info("generate PAT&PMT ...");
	tpmt.p_descr = tpmt_descr;
	for (i = 0; i < 5; i++) {
		tpmt_descr[i].p_data = tp_pmt_data[i];
	}
	for (i = 0; i < PROGRAM_MAX_NUM; i++) {
		tes[i].p_descr = tes_descr[i];
		for (j = 0; j < 5; j++) {
			tes_descr[i][j].p_data = tp_es_data[i][j];
		}
	}

	// Begin Set Values
	tpat.i_tran_stream_id = 0x123;

	for (i = 0; i < tpid_num; i++) {
		tpid_data[i].i_pg_num = 0x10 + i;
		tpid_data[i].i_pid = 0x250 + i;
	}
	trace_info("generate PAT ...");
	dvbSI_Gen_PAT(&tpat, tpid_data, tpid_num);

	for (i = 0; i < tpid_num; i++) {
		tpmt.i_pg_num = tpid_data[i].i_pg_num;
		tpmt.i_pmt_pid = tpid_data[i].i_pid;
		tpmt.i_pcr_pid = 0x270 + i;
		tpmt.i_descr_num = 0;

		tes_num = 2;

		for (j = 0; j < tes_num; j++)	// Video Audio
		{
			tes[j].i_type = 0x3 + j;
			tes[j].i_pid = 0x300 + 32 * i + j;
			tes[j].i_descr_num = 0;	//
		}

		trace_info("generate PMT of program $#%d ...", i);
		dvbSI_Gen_PMT(&tpmt, tes, tes_num);
	}

	return 0;
}

void test_gen_psi()
{
	dvbSI_Start(&hfpga_dev);

	dvbSI_GenSS(HFPGA_CMD_SI_STOP);

	GenPAT_and_PMT();

	sleep(2);
	GenSDT();

	sleep(2);
	GenNIT();

	sleep(2);
	GenCAT();

	sleep(1);
	dvbSI_GenSS(HFPGA_CMD_SI_START);

	dvbSI_Stop();
}

