#ifndef _GEN_DVB_SI_API_H_
#define _GEN_DVB_SI_API_H_

#include <stdio.h>
#include <stdint.h>

#include "xmux.h"


// define TS analyse operation type
// ---- default is decode the content,
// ---- section denotes to only input or output section
typedef enum {
	EUV_DEFAULT = 0, 
	EUV_SECTION,
	EUV_BOTH,
	EUV_DEC_FROM_SECTION
} UV_SI_OPS_TYPE;

typedef enum {
	EUV_TBL_PAT = 0,
	EUV_TBL_PMT,
	EUV_TBL_CAT,
	EUV_TBL_NIT,
	EUV_TBL_SDT,
	EUV_TBL_EIT,
} UV_SI_TBL_TYPE;

#define UV_MAX_SI_SECTION_CNT (6)

/*Í¨µÀ×´Ì¬£ºÍ¨µÀ1-8ÊÇ·ñ·ÖÎöÍê³ÉÖ¸Ê¾±êÖ¾£¬±êÖ¾0Î´·ÖÎö£¬1Îª·ÖÎöÍê³É¡£MSBÎª8Í¨µÀ£¬LSBÎª1Í¨µÀ¡£
Í¨µÀ±í×´Ì¬£ºÏàÓ¦±íÊÇ·ñ·ÖÎöÍê³É±êÖ¾£¬ºÍMIBµÄ±í·ÖÎöSECTION½ÚµãÖ±½Ó¶ÔÓ¦£¬PAT£¬CAT£¬NIT¶ÔÓ¦1¸ö½Úµã£¬
SDT¶ÔÓ¦5¸ö½Úµã£¬PMT¶ÔÓ¦32¸ö½Úµã£¨Ã¿¸ö½Úµã¶ÔÓ¦Ò»¸ösection£©£¬
½ÚµãSECTION·ÖÎöÍê³É¶ÔÓ¦Î»ÖÃ1£¬
Î´Íê³ÉÖÃ0£»5BÖÐµÄ×î¸ß×Ö½Ú±íÊ¾PAT(bit7)£¬CAT(bit6)£¬NIT(bit5)£¬SDT£¨bit4-bit0£©;
µÍ4×Ö½ÚÎªPMT£¨MIB½Úµã1¶ÔÓ¦×îµÍÎ»£¬MIB½Úµã32¶ÔÓ¦×î¸ßÎ»£©¡£
*/
typedef struct {
	uint8_t ch_s; 				// MSB is ch8
	uint8_t tbl_s[8][(PROGRAM_MAX_NUM / 8) + 1];  
} __attribute__((packed)) uv_cha_si_stat;

typedef struct _uv_param{
	// ²ÎÊýÖ¸Õë
	void *p1;
	void *p2;
	void *p3;
	void *p4;
	unsigned int pcnt;

  // Õë¶ÔÍ¨µÀcha£¬±ítbl_typeµÄ section£¬
  // ¶ÔÓÚPMT·ÖÎö»¹ÒªÖ¸¶¨¾ßÌåµÄPMT´ÎÐòtbl_which_pmt
	UV_SI_OPS_TYPE type;														//  ³õÊ¼»¯Îª½âÎö²ã£¨EUV_SECTION»òÕßEUV_DEFAULT£©
  	uint8_t *sec[UV_MAX_SI_SECTION_CNT];//  ·ÖÎöÊ±£¬ÈôÖ»µÃµ½section£¬ÔòÒ»¶¨Òª½«Ö¸ÕëÖ¸ÏòÓÐÐ§ÄÚ´æ,´æ´¢section¾²Ì¬±äÁ¿Ö¸Õë
	unsigned short sec_len[UV_MAX_SI_SECTION_CNT];  //  ³õÊ¼»¯Îª0£¬Ã¿¸ösection³¤¶È£¬psi·ÖÎö¿ªÊ¼Ê±³õÊ¼»¯Ê±Ó¦Îª0
	unsigned char cur_cnt;			// section¸öÊý£¬¬psi·ÖÎö¿ªÊ¼Ê± ³õÊ¼»¯Îª0
	unsigned char cha; 															//  Í¨µÀ£¬±íÊ¾0 - 7
	UV_SI_TBL_TYPE tbl_type;		//  Ö¸Ã÷·ÖÎöµÄtable ÀàÐÍ0Îªpid£¬1Îªsection
	unsigned char tbl_which_pmt;	//  Èç¹ûÊÇpmt»¹Òª°´ÐòÖ¸Ã÷ÊÇÄÄÒ»¸öPMT
	// È«¾ÖÍ¨µÀ×´Ì¬ºÍÍ¨µÀ±íÐÅÏ¢
	uv_cha_si_stat *cur_stat;		//ËùÓÐÍ¨µÀ×´Ì¬
} uv_param;

uv_param sg_si_param;


struct _dvb_io;

int dvbSI_Start(struct _dvb_io *io);
int dvbSI_ChangCha(unsigned char cha);
void dvbSI_Stop();

/*
 * PAT  
 */
typedef struct {
	uint16_t i_tran_stream_id;
} uv_pat_data;

typedef struct {
	uint16_t i_pg_num;
	uint16_t i_pid;
} uv_pat_pid_data;

int dvbSI_Gen_PAT(uv_pat_data *p_pat_data, uv_pat_pid_data *p_pid_data, uint16_t i_pid_num);
int dvbSI_Dec_PAT(uv_pat_data *p_pat_data, uv_pat_pid_data *p_pid_data, uint16_t *p_pid_num);

/*
 * PMT
 */
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

/*
 * SDT
 */
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

/*
 * CAT
 */
int dvbSI_Gen_CAT(uv_descriptor *p_descr, uint16_t i_descr_num);
int dvbSI_Dec_CAT(uv_descriptor *p_descr, uint16_t *i_descr_num);

/*
 * NIT
 */
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

/*
 * EIT
 */
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

/*
 * enc section to ts and send to mux
 * @howto - parameter for sending period, such as 400ms(howto=0),
 *          2s(howto=1) and so on.
 */
void dvbSI_Send_Pack(uint8_t howto, unsigned short pid, unsigned char* p_section, unsigned int len);

/*
 * Start/Stop SI Send
 */
int dvbSI_GenSS(unsigned int ss);

/*
 * hook up timer check func for Dec_Xxx()
 */
void uvPSI_SetTimeoutFunc(int (*func)());


#endif /* _GEN_DVB_SI_API_H_ */

