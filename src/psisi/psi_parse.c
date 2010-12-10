#include "xmux.h"

#include "fpga_api_def.h"
#include "psi.h"
#include "uvSI_def.h"
#include "gen_dvb_si.h"
#include "psi_mcu_panel.h"

#include "psi_imp.h"


#define MAX_CHANNEL_NUM 8
#define UV_PROG_NUM_MAX (32)
#define TYPE_VPID 0x2000
#define TYPE_APID 0x4000
#define TYPE_PRIVATEPID 0x5000
#define TYPE_OTHERPID 0x8000

static uv_dvb_io hfpga_dev = {
#if 0
	hfpga_open,	
	hfpga_close,
	hfpga_readn,	
	hfpga_writen,
        hfpga_ioctl,
	"/dev/XC3S",	
	O_RDWR,
	-1,
	0x00 //cha number
#endif
};


static uv_pat_data pat;
static uv_pat_pid_data pid_data[UV_PROG_NUM_MAX+1];
static uint16_t pid_num = 0;

static uv_pmt_data pmt;

static uint16_t es_num = 0;

static uv_pmt_es_data es[UV_PROG_NUM_MAX];

static uv_descriptor cat_descr[5];
static uint16_t cat_descr_num;

static uv_sdt_data sdt;

static uv_nit_data nit;

static uv_sdt_serv_data serv[UV_PROG_NUM_MAX];
static uint16_t serv_num = 0;

static uv_nit_stream_data stream[UV_PROG_NUM_MAX];
static uint16_t stream_num = 0;

static uv_cha_si_stat All_Channel_Psi_Status;

static CHN_PROG_TO_MCU_T All_Channel_Info_Mcu[MAX_CHANNEL_NUM];


int psi_parsing()
{
	int i, j, k;
	unsigned char Tstype = 0;
	unsigned short pid;

	for (k = 0; k < CHANNEL_MAX_NUM; k++) {
		sg_si_param.cha = k;
		for (i = 0; i < UV_MAX_SI_SECTION_CNT; i++)
			sg_si_param.sec_len[i] = 0;
		sg_si_param.cur_stat = &All_Channel_Psi_Status;
		sg_si_param.cur_stat->ch_s = 0;
		dvbSI_Start(&hfpga_dev);

		uv_test_printf(printf("[UV-SI-SNMP] dvbSI_Dec_PAT starts. \n "));

		sg_si_param.cur_cnt = 0;
		sg_si_param.type = EUV_BOTH;
		sg_si_param.tbl_type = EUV_TBL_PAT;
		sg_si_param.sec[0] = sg_mib_pat[sg_si_param.cha];
		dvbSI_Dec_PAT(&pat, pid_data, &pid_num);


		uv_test_printf(printf("[UV-SI-SNMP] dvbSI_Dec_PAT returned. \n "));

		uv_test_printf(printf
					   ("[UV-SI-SNMP] TS id is: %02x, pid num is: %d. \n ",
						pat.i_tran_stream_id, pid_num));
		for (i = 0; i < pid_num; i++) {
			uv_test_printf(printf
						   ("++++ prog num is: %02x. pmt pid is: %02x \n ",
							pid_data[i].i_pg_num, pid_data[i].i_pid));
		}

		uv_test_printf(printf("[UV-SI-SNMP] Begin to decode PMTs. \n "));

		All_Channel_Info_Mcu[i].prognum = pid_num;
		for (i = 0; i < pid_num; i++) {
			if (pid_data[i].i_pg_num) {
				pmt.i_pg_num = pid_data[i].i_pg_num;
				pmt.i_pmt_pid = pid_data[i].i_pid;
				sg_si_param.cur_cnt = 0;
				sg_si_param.type = EUV_BOTH;
				sg_si_param.tbl_type = EUV_TBL_PMT;
				sg_si_param.sec[i] = sg_mib_pmt[sg_si_param.cha][i];

				dvbSI_Dec_PMT(&pmt, es, &es_num);
				All_Channel_Info_Mcu[k].progitem[i].prognum = pmt.i_pg_num;
				All_Channel_Info_Mcu[k].progitem[i].PMT_PID_IN =
					pmt.i_pmt_pid;
				All_Channel_Info_Mcu[k].progitem[i].PMT_PID_OUT =
					pmt.i_pmt_pid;
				All_Channel_Info_Mcu[k].progitem[i].PCR_PID_IN =
					pmt.i_pcr_pid;
				All_Channel_Info_Mcu[k].progitem[i].PCR_PID_OUT =
					pmt.i_pcr_pid;
				if (pmt.i_descr_num > 6)
					pmt.i_descr_num = 6;
				for (j = 0; j < 6; j++) {
					if (j < pmt.i_descr_num) {
						Tstype = pmt.p_descr[j].p_data[0];
						switch (Tstype) {
						case 1:
						case 2:
							pid =
								(pmt.p_descr[j].p_data[1] << 8 | pmt.
								 p_descr[j].p_data[2]) & 0x1fff;
							pid |= TYPE_VPID;
							All_Channel_Info_Mcu[k].progitem[i].
								PIDS[j][0] = pid;
							All_Channel_Info_Mcu[k].progitem[i].
								PIDS[j][1] = pid;
							break;
						case 3:
						case 4:
							pid =
								(pmt.p_descr[j].p_data[1] << 8 | pmt.
								 p_descr[j].p_data[2]) & 0x1fff;
							pid |= TYPE_APID;
							All_Channel_Info_Mcu[k].progitem[i].
								PIDS[j][0] = pid;
							All_Channel_Info_Mcu[k].progitem[i].
								PIDS[j][1] = pid;
							break;
						case 6:
							pid =
								(pmt.p_descr[j].p_data[1] << 8 | pmt.
								 p_descr[j].p_data[2]) & 0x1fff;
							pid |= TYPE_PRIVATEPID;
							All_Channel_Info_Mcu[k].progitem[i].
								PIDS[j][0] = pid;
							All_Channel_Info_Mcu[k].progitem[i].
								PIDS[j][1] = pid;
							break;
						default:
							pid =
								(pmt.p_descr[j].p_data[1] << 8 | pmt.
								 p_descr[j].p_data[2]) & 0x1fff;
							pid |= TYPE_OTHERPID;
							All_Channel_Info_Mcu[k].progitem[i].
								PIDS[j][0] = pid;
							All_Channel_Info_Mcu[k].progitem[i].
								PIDS[j][1] = pid;
							break;
						}
					} else {
						All_Channel_Info_Mcu[k].progitem[i].PIDS[j][0] =
							0x000f;
						All_Channel_Info_Mcu[k].progitem[i].PIDS[j][1] =
							0x000f;
					}
				}
				uv_test_printf(printf
							   ("--[%d]-- PCR pid is: %02x. descr num is: %02x \n ",
								i + 1, pmt.i_pcr_pid, pmt.i_descr_num));
				for (j = 0; j < pmt.i_descr_num; j++) {
					uv_test_printf(printf
								   ("-----------------[PMT descriptor %d--tag: %02x len: 0x%02x(%d)]---------------\n",
									j, pmt.p_descr[j].i_tag,
									pmt.p_descr[j].i_length,
									pmt.p_descr[j].i_length));
					outputHex(pmt.p_descr[j].p_data,
							  pmt.p_descr[j].i_length, 16);
				}
				for (j = 0; j < es_num; j++) {
					//uv_test_printf(printf("########[PMT es %d]########\n", j));
					//uv_test_printf(printf("type is: %02x. pid is: %02x \n ", es[j].i_type, es[j].i_pid));

					for (k = 0; k < es[i].i_descr_num; k++) {
						uv_test_printf(printf
									   ("---****---[PMT es descriptor %d--tag: %02x len: 0x%02x(%d)]---****---\n",
										k, es[j].p_descr[k].i_tag,
										es[j].p_descr[k].i_length,
										es[j].p_descr[k].i_length));
						outputHex(es[j].p_descr[k].p_data,
								  es[j].p_descr[k].i_length, 16);
					}
				}
			}

		}

		uv_test_printf(printf("[UV-SI-SNMP] Begin to decode CAT. \n "));
		sg_si_param.cur_cnt = 0;
		sg_si_param.type = EUV_BOTH;
		sg_si_param.tbl_type = EUV_TBL_CAT;
		sg_si_param.sec[0] = sg_mib_cat[sg_si_param.cha];
		dvbSI_Dec_CAT(cat_descr, &cat_descr_num);

		uv_test_printf(printf("[UV-SI-SNMP] Begin to decode SDT. \n "));
		sg_si_param.cur_cnt = 0;
		sg_si_param.type = EUV_BOTH;
		sg_si_param.tbl_type = EUV_TBL_SDT;
		sg_si_param.sec[0] = sg_mib_sdt[sg_si_param.cha][0];
		dvbSI_Dec_SDT(&sdt, serv, &serv_num);
		for (i = 0; i < All_Channel_Info_Mcu[k].progitem[i].prognum; i++) {
			for (j = 0; j < serv_num; j++) {
				if (All_Channel_Info_Mcu[k].progitem[i].prognum ==
					serv[j].i_serv_id) {
					//dvbpsi_dump_program_name(serv[j].p_descr->p_data,All_Channel_Info_Mcu[k].progitem[i].progname[0]);
					All_Channel_Info_Mcu[k].progitem[i].progname[1][0] = 5;
					//dvbpsi_dault_program_name(All_Channel_Info_Mcu[k].progitem[i].progname[1]);
				}

			}
		}
		uv_test_printf(printf("[UV-SI-SNMP] Begin to decode NIT. \n "));
		sg_si_param.cur_cnt = 0;
		sg_si_param.type = EUV_BOTH;
		sg_si_param.tbl_type = EUV_TBL_NIT;
		sg_si_param.sec[0] = sg_mib_nit[sg_si_param.cha];
		dvbSI_Dec_NIT(&nit, stream, &stream_num);
#if 0
		uv_test_printf(printf("[UV-SI-SNMP] Begin to decode EIT. \n "));
		sg_si_param.cur_cnt = 0;
		sg_si_param.type = EUV_BOTH;
		sg_si_param.tbl_type = EUV_TBL_EIT;
		sg_si_param.sec[0] = sg_mib_eit[sg_si_param.cha];
		dvbSI_Dec_EIT(&eit, event, &event_num);
#endif

		dvbSI_Stop();
		sg_si_param.cur_stat->ch_s |= 0x01 << k;
	}

	return 0;
}

