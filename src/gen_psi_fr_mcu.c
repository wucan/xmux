#include <fcntl.h>

#include "wu/message.h"
#include "wu/wu_converter.h"

#include "fpga_api.h"
#include "gen_dvb_si.h"

#include "front_panel_intstr.h"
#include "front_panel_define.h"


#define UV_PROG_NUM_MAX (6)
#define UV_DESCR_LEN    (33)

extern uv_dvb_io hfpga_dev;

static msgobj mo = {MSG_INFO, ENCOLOR, "fp-psi-gen"};

static uint8_t sdbuf[32][5][UV_DESCR_LEN];

static int GenSDT(void)
{
	uv_sdt_data sdt_data;
	uv_sdt_serv_data sdt_serv_data[32];
	uint16_t serv_num = 32;
	int i, j;
	uv_descriptor *p_descr;
	int num = 2;
	char sztmp[10];

	trace_info("generate SDT ...");

	sdt_data.i_table_id = 0x42;
	sdt_data.i_tran_stream_id = 0x1234;
	sdt_data.i_orig_net_id = 0xEAEA;

	for (i = 0; i < serv_num; i++) {
		sdt_serv_data[i].i_serv_id = i + 1;
		sdt_serv_data[i].i_eit_pres_foll_flag = 0;
		sdt_serv_data[i].i_eit_sched_flag = 0;
		sdt_serv_data[i].i_free_ca_mode = 0;
		sdt_serv_data[i].i_running_status = 1;
		sdt_serv_data[i].i_descr_num = num;
		sdt_serv_data[i].p_descr =
			(uv_descriptor *) malloc(sdt_serv_data[i].i_descr_num *
									 sizeof(uv_descriptor));
		p_descr = sdt_serv_data[i].p_descr;
		for (j = 0; j < num; j++) {
			p_descr[j].i_tag = 0x48;
			p_descr[j].i_length = 14;
			p_descr[j].p_data = sdbuf[i][j];
			p_descr[j].p_data[0] = 0x01;	// serivece type DTV
			p_descr[j].p_data[1] = 4;	// 
			p_descr[j].p_data[6] = 7;
			if (j == 0) {
				sprintf(sztmp, "CCTV-%02d", i);
				memcpy(p_descr[j].p_data + 2, "CCTV", 4);
				memcpy(p_descr[j].p_data + 7, sztmp, 7);
			} else {
				sprintf(sztmp, "SZTV-%02d", i);
				memcpy(p_descr[j].p_data + 2, "SZTV", 4);
				memcpy(p_descr[j].p_data + 7, sztmp, 7);
			}
		}
	}

	dvbSI_Gen_SDT(&sdt_data, sdt_serv_data, serv_num);

	// free mem
	for (i = 0; i < serv_num; i++) {
		free(sdt_serv_data[i].p_descr);
	}

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
static uv_pat_pid_data tpid_data[UV_PROG_NUM_MAX + 1];
static uint16_t tpid_num = UV_PROG_NUM_MAX;

// ------ PAT End

// ------ PMT Start
static uv_pmt_data tpmt;
static uv_descriptor tpmt_descr[5];
static unsigned char tp_pmt_data[5][UV_DESCR_LEN];

static uv_pmt_es_data tes[UV_PROG_NUM_MAX];
static uv_descriptor tes_descr[UV_PROG_NUM_MAX][5];
static unsigned char tp_es_data[UV_PROG_NUM_MAX][5][UV_DESCR_LEN];

static uint16_t tes_num = 0;

static int get_dsw_provider_len(void)
{
	return strlen(defProviderDsw);
}

// ------ PMT End
int gen_sdt_fr_mcu(uint8_t * packpara, const PROG_INFO_T * pProgpara)
{
#if 1
	uv_sdt_data sdt_data;
	uv_sdt_serv_data sdt_serv_data[PROGRAM_MAX_NUM];
	int ncount;
	uv_descriptor *p_descr;
	int num = 1;				//2;

	int nProgSel = 0;
	PROG_INFO_T *pProg = (PROG_INFO_T *) pProgpara;

	trace_info("generate SDT ...");

	sdt_data.i_table_id = 0x42;
	sdt_data.i_tran_stream_id = 0x1234;
	sdt_data.i_orig_net_id = 0xEAEA;

	for (ncount = 0; ncount < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; ncount++) {
		int j;
		// 4 is PMT_PID_IN,PMT_PID_OUT,PCR_PID_IN,PCR_PID_OUT
		// defProgPidNum*2 is PIDS
		pProg = (PROG_INFO_T *) pProgpara + ncount;
		if (pProg->status == 1) {
			sdt_serv_data[nProgSel].i_serv_id = nProgSel + 1;
			sdt_serv_data[nProgSel].i_eit_pres_foll_flag = 0;
			sdt_serv_data[nProgSel].i_eit_sched_flag = 0;
			sdt_serv_data[nProgSel].i_free_ca_mode = 0;
			sdt_serv_data[nProgSel].i_running_status = 1;
			sdt_serv_data[nProgSel].i_descr_num = num;
			sdt_serv_data[nProgSel].p_descr =
				(uv_descriptor *) calloc(sdt_serv_data[nProgSel].
										 i_descr_num *
										 sizeof(uv_descriptor),
										 sizeof(uint8_t));
			p_descr = sdt_serv_data[nProgSel].p_descr;
			for (j = 0; j < num; j++) {
				p_descr[j].i_tag = 0x48;
				p_descr[j].i_length =
					4 + get_dsw_provider_len() + pProg->progname[1][0];
				// 4 is service_type(1) + service_provider_length(1) + service_name_length(1) + progname_len(1)
				//p_descr[j].p_data = malloc(p_descr[j].i_length);  //sdbuf[nProgSel][j];
				p_descr[j].p_data = sdbuf[nProgSel][j];



				p_descr[j].p_data[0] = 0x01;	// serivece type DTV
				p_descr[j].p_data[1] = get_dsw_provider_len();	// 
				memcpy(&(p_descr[j].p_data[2]), defProviderDsw,
					   get_dsw_provider_len());
				p_descr[j].p_data[2 + get_dsw_provider_len()] =
					pProg->progname[1][0] + 1;

			}

			nProgSel++;
			if (nProgSel >= PROGRAM_MAX_NUM)
				break;
		}
	}
	dvbSI_Gen_SDT(&sdt_data, sdt_serv_data, nProgSel);

#if 0
	// free mem
	for (ncount = 0; ncount < nProgSel; ncount++) {

#if 0
		int j;
		p_descr = sdt_serv_data[ncount].p_descr + ncount;
		for (j = 0; j < num; j++) {
			printf("p_descr_005=0x%08X;count=%d;j=%d\n", p_descr, ncount,
				   j);
			if (p_descr[j].p_data)
				free(p_descr[j].p_data);
		}

		//p_descr=sdt_serv_data[ncount].p_descr
#endif
		free(sdt_serv_data[ncount].p_descr);
	}
#endif
	// free mem

	for (ncount = 0; ncount < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; ncount++) {
		if (pProg->status == 1)
			free(sdt_serv_data[ncount].p_descr);
	}

#endif
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
	for (i = 0; i < UV_PROG_NUM_MAX; i++) {
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

			tpid_data[nProgSel].i_pid = pProg->PMT_PID_OUT;

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

			tpmt.i_pmt_pid = pProg->PMT_PID_OUT;
			tpmt.i_pcr_pid = pProg->PCR_PID_OUT;
			tpmt.i_descr_num = 0;

			tes_num = 0;
			for (j = 0; j < PROGRAM_DATA_PID_MAX_NUM; j++)	// Video Audio
			{
				if (pProg->pids[j].out != 0x00
					&& pProg->pids[j].out != 0x0F) {
					tes[j].i_type = pProg->pids[j].type;
					tes[j].i_pid = pProg->pids[j].out;
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
	for (i = 0; i < UV_PROG_NUM_MAX; i++) {
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

