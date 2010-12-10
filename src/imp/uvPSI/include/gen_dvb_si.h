/*##########################################################################
## gen_dvb_si.h
## grepo@163.com
## 2010-07-06
## Interface for generating and decoding SI
##########################################################################*/

#ifndef __GEN_DVB_SI_H__ 
#define __GEN_DVB_SI_H__

#include "config.h"

#include <stdio.h>

#include <stdint.h>

/* the libdvbpsi distribution defines DVBPSI_DIST */
#include "dvbpsi_private.h"
#include "descriptor.h"
#include "dvbpsi.h"
#include "demux.h"
#include "pmt.h"
#include "pmt_private.h"
#include "psi.h"
#include "pat.h"
#include "pat_private.h"
#include "sdt.h"
#include "sdt_private.h"
#include "eit.h"
#include "eit_private.h"
#include "nit.h"
#include "nit_private.h"
#include "cat.h"
#include "cat_private.h"

#include "fpga_api_def.h"

#define DEBUG_PRT

#ifdef DEBUG_PRT

#ifndef dbg_prt
#define dbg_prt printf
#endif

#else

#ifndef dbg_prt
#define dbg_prt(...)
#endif

#endif


/***********************************************************************************************

 SI Generator APIs  &   SI Decoder APIs

***********************************************************************************************/

/*---------------------------------------------------------------------------------------------*/
//	Utilities
/*---------------------------------------------------------------------------------------------*/

int dvbSI_Start(uv_dvb_io *io);
int dvbSI_ChangCha(unsigned char cha);
void dvbSI_Stop();

/*
struct uv_err_info {
	int i;
	char *msg;
};
static  struct uv_err_info si_err_info = {
	{0, "success"},
	{-1, "error"},
	{-2, "warning"}
};
*/
/*---------------------------------------------------------------------------------------------*/
//	PAT  
/*---------------------------------------------------------------------------------------------*/

typedef struct {
	uint16_t i_tran_stream_id;
} uv_pat_data;

typedef struct {
	uint16_t i_pg_num;
	uint16_t i_pid; // 	
} uv_pat_pid_data;

int dvbSI_Gen_PAT(uv_pat_data *p_pat_data, uv_pat_pid_data *p_pid_data, uint16_t i_pid_num);
int dvbSI_Dec_PAT(uv_pat_data *p_pat_data, uv_pat_pid_data *p_pid_data, uint16_t *p_pid_num);

/*---------------------------------------------------------------------------------------------*/
//	PMT
/*---------------------------------------------------------------------------------------------*/

typedef struct {
	uint8_t i_tag;
	uint16_t i_length;
	uint8_t* p_data;
} uv_descriptor;

typedef struct {
	uint16_t i_pmt_pid;
	uint16_t i_pg_num;
	uint16_t i_pcr_pid;
	uint16_t i_descr_num;
	uv_descriptor *p_descr;
} uv_pmt_data;

typedef struct {
	uint8_t i_type;
	uint16_t i_pid; 	
	uint16_t i_descr_num;
	uv_descriptor *p_descr;
} uv_pmt_es_data;

int dvbSI_Gen_PMT(uv_pmt_data *p_pmt_data, uv_pmt_es_data *p_pmt_es_data, uint16_t i_es_num);
int dvbSI_Dec_PMT(uv_pmt_data *p_pmt_data, uv_pmt_es_data *p_pmt_es_data, uint16_t *p_es_num);

/*---------------------------------------------------------------------------------------------*/
//	SDT
/*---------------------------------------------------------------------------------------------*/

typedef struct {
	uint8_t i_table_id; 
	uint16_t i_tran_stream_id; 	
	uint16_t i_orig_net_id;
} uv_sdt_data;

typedef struct {
	uint16_t i_serv_id;
	uint8_t i_eit_sched_flag; 	
	uint8_t i_eit_pres_foll_flag; 
	uint8_t i_running_status; 
	uint8_t i_free_ca_mode; 
	uint16_t i_descr_num;
	uv_descriptor *p_descr;
} uv_sdt_serv_data;

int dvbSI_Gen_SDT(uv_sdt_data *p_sdt_data, uv_sdt_serv_data *p_sdt_serv_data, uint16_t i_serv_num);
int dvbSI_Dec_SDT(uv_sdt_data *p_sdt_data, uv_sdt_serv_data *p_sdt_serv_data, uint16_t *p_serv_num);

/*---------------------------------------------------------------------------------------------*/
//	CAT
/*---------------------------------------------------------------------------------------------*/

int dvbSI_Gen_CAT(uv_descriptor *p_descr, uint16_t i_descr_num);
int dvbSI_Dec_CAT(uv_descriptor *p_descr, uint16_t *i_descr_num);

/*---------------------------------------------------------------------------------------------*/
//	NIT
/*---------------------------------------------------------------------------------------------*/

typedef struct {
	uint8_t i_table_id; 
	uint16_t i_net_id; 	
	uint16_t i_descr_num;
	uv_descriptor *p_descr;
} uv_nit_data;

typedef struct {
	uint16_t i_tran_stream_id;
	uint16_t i_orig_net_id;	
	uint16_t i_descr_num;
	uv_descriptor *p_descr;
} uv_nit_stream_data;

int dvbSI_Gen_NIT(uv_nit_data *p_nit_data, uv_nit_stream_data *p_stream_data, uint16_t i_stream_num);
int dvbSI_Dec_NIT(uv_nit_data *p_nit_data, uv_nit_stream_data *p_stream_data, uint16_t *p_stream_num);

/*---------------------------------------------------------------------------------------------*/
//	EIT
/*---------------------------------------------------------------------------------------------*/

typedef struct {
	uint8_t i_table_id; 
	uint16_t i_serv_id; 	
	uint16_t i_tran_stream_id;
	uint16_t i_orig_net_id;
	uint16_t i_last_table_id;
} uv_eit_data;

typedef struct {
	uint16_t i_event_id;
	uint32_t i_start_time;
	uint32_t i_duration; // the MSB 8bits will be stored in low bits of start_time.  
	uint8_t i_running_status;	
	uint16_t i_free_ca_mode;
	uint16_t i_descr_num;
	uv_descriptor *p_descr;
} uv_eit_event_data;

int dvbSI_Gen_EIT(uv_eit_data *p_eit_data, uv_eit_event_data *p_event_data,  uint16_t i_event_num);
int dvbSI_Dec_EIT(uv_eit_data *p_eit_data, uv_eit_event_data *p_event_data,  uint16_t *p_event_num);

/*---------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------*/
// enc section to ts and send to mux
/*---------------------------------------------------------------------------------------------*/
// howto is parameter for sending period, such as 400ms(howto=0), 2s(howto=1) and so on.
void dvbSI_Send_Pack(uint8_t howto, unsigned short pid, unsigned char* p_section, unsigned int len);
/*---------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------*/
// Start/Stop SI Send
/*---------------------------------------------------------------------------------------------*/
int dvbSI_GenSS(unsigned int ss);

/*---------------------------------------------------------------------------------------------*/

#endif

