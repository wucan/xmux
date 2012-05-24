/*
 * create by why @ 2009 06 10 14:44
 * struct and macro defines
 */
#ifndef CI_DEFINE_HEAD
#define CI_DEFINE_HEAD

#include <stdint.h>


#ifndef bool
typedef int bool;
#endif

#define TRUE 1
#define FALSE 0

typedef unsigned char BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

//TPDU tag defines
#define T_SB_PDU        0x80 // host <----------- modules
#define T_RCV_PDU       0x81 // host -----------> modules
#define T_CREATE_PDU    0x82 // host -----------> modules
#define T_REPLY_PDU     0x83 // host <----------- modules
#define T_DEL_PDU       0x84 // host <----------> modules
#define T_DEL_REPLY_PDU 0x85 // host <----------> modules
#define T_REQUEST_PDU   0x86 // host <----------- modules
#define T_NEW_PDU       0x87 // host -----------> modules
#define T_ERROR_PDU     0x88 // host -----------> modules
#define T_DATA_LAST_PDU 0xA0 // host <----------> modules
#define T_DATA_MORE_PDU 0xA1 // host <----------> modules

//SPDU tag defines
#define S_OPEN_REQUEST    0x91 // host <----------- modules
#define S_OPEN_RESPONSE   0x92 // host -----------> modules
#define S_CREATE_REQUEST  0x93 // host -----------> modules
#define S_CREATE_RESPONSE 0x94 // host <----------- modules
#define S_CLOSE_REQUEST   0x95 // host <----------> modules
#define S_CLOSE_RESPONSE  0x96 // host <----------> modules
#define S_NUMBER          0x90 // host <----------> modules

//Resource Identifier values
#define RESOURCE_MANAGER 0x00010041
#define APPLACATION_INFO 0x00020041
#define CA_SUPPORT       0x00030041
#define HOST_CONTROL     0x00200041
#define DATE_TIME        0x00240041
#define MMI              0x00400041

//apdu tags
#define PROFILE_ENQ     0x009F8010 // host <----------> modules
#define PROFILE         0x009F8011 // host <----------> modules
#define PROFILE_CHG     0x009F8012 // host <----------> modules
#define APPLACATION_ENQ 0x009F8020 // host -----------> modules
#define APPLACATION     0x009F8021 // host <----------- modules
#define ENTER_MENU      0x009F8022 // host -----------> modules
#define DATETIMEINFO      0x009F8440 
#define CA_INFO_ENQ     0x009F8030 // host -----------> modules
#define CA_INFO         0x009F8031 // host <----------- modules
#define CA_PMT          0x009F8032 // host -----------> modules
#define CA_PMT_REPLY    0x009F8033 // host <----------- modules
#define DISPLAY_CTRL    0x009F8801 // host <----------- modules
#define MENU_LAST       0x009F8809 // host <----------- modules
#define MENU_INPUT       0x009F8807 // host <----------- modules
/*#define PROFILE_ENQ 0x009F8010 // host <----------- modules
#define PROFILE_ENQ 0x009F8010 // host <----------- modules
#define PROFILE_ENQ 0x009F8010 // host <----------- modules
#define PROFILE_ENQ 0x009F8010 // host <----------- modules
#define PROFILE_ENQ 0x009F8010 // host <----------- modules
#define PROFILE_ENQ 0x009F8010 // host <----------- modules
#define PROFILE_ENQ 0x009F8010 // host <----------- modules
#define PROFILE_ENQ 0x009F8010 // host <----------- modules
#define PROFILE_ENQ 0x009F8010 // host <----------- modules
#define PROFILE_ENQ 0x009F8010 // host <----------- modules
#define PROFILE_ENQ 0x009F8010 // host <----------- modules
*/

//define about ca_pmt
#define CA_PMT_LIST_MORE   0x00
#define CA_PMT_LIST_FIRST  0x01
#define CA_PMT_LIST_LAST   0x02
#define CA_PMT_LIST_ONLY   0x03
#define CA_PMT_LIST_ADD    0x04
#define CA_PMT_LIST_MODIFY 0x05

#define CA_PMT_CMD_ID_OKDES 0x01
#define CA_PMT_CMD_ID_OKMMI 0x02
#define CA_PMT_CMD_ID_QUERY 0x03
#define CA_PMT_CMD_ID_NOSEL 0x04

#define CONN_ID 0x01

//card status defines
#define CARD_NOT_EXIST  0x00
#define CARD_IS_EXIST   0x01
#define CARD_IS_READY   0x03

#define CARD_NO 0x00
#define CARDA_ONLY 0x01
#define CARDB_ONLY 0x02
#define CARD_ALL 0x03

/*  card status defines end  */

//ci bord status
#define CI_INIT_OVER   0x00
#define CI_NOT_EXIST   0x01
#define CI_IS_EXIST    0x02
#define CI_INIT_FAILED 0x03
/*  ci bord status  */
//ci address
#define	CI_INT	 	0x00000
#define	CI_CPLD 	0x100000

/*  ci address  */
//ci cpld register
#define CPLD_CIBOARDID  0x00
#define CPLD_CARDSTATUS  0x02
#define CPLD_CIENABLE  0x04
#define CPLD_CONTROL  0x06
#define CPLD_CIRESET  0x08
#define CPLD_CIRDADDR  0x10
#define CPLD_CIRDDAT  0x12
#define CPLD_CIWRADDR  0x14
#define CPLD_CIWRDAT  0x16
#define CPLD_CIWR  0x20
#define CI_EN 0x01
#define IO_CARDA 0x00
#define MEM_CARDA 0x01
#define IO_CARDB 0x02
#define MEM_CARDB 0x03
#define RST_CARDA 0x01
#define RST_CARDB 0x02
#define CPLD_WR 0x00
#define CPLD_RD 0x01
/*ci cpld register*/
//ci menu
#define CARD_NOMENU 0
#define CARD_MENU 1
#define CARD_MENU_INPUT 2
/*ci address*/
//fpga switch mode
#define SWITCH_FPGA_PARALLEL             0
#define SWITCH_FPGA_SERIES_INPUT1   1   
#define SWITCH_FPGA_SERIES_INPUT2   2 
/*fpga switch mode*/

#define MAX_CASYSTEM_IDS 64

//program dcas flag
#define PROGRAM_DCAS_OK     1
#define PROGRAM_NOT_DCAS    0
#define PROGRAM_NOT_INVALID 2

#define PROGRAM_PMT_UPDATE     1
#define PROGRAM_PMT_NOUPDATE    0


//fpga switch
#define SWITCH_PARALLEL_INPUT 0x0000
#define SWITCH_SERIES_INPUT 0x0004 
#define SWITCH_PARALLEL_CARDA 0x0001
#define SWITCH_PARALLEL_CARDB 0x0002
#define SWITCH_SERIES_CARDA 0x0001
#define SWITCH_SERIES_CARDB 0x0002

#define APDU_CONTENT_LEN sizeof(apdu_content)
typedef struct apdu_content_s
{
unsigned int apdu_tag;
unsigned int apdu_len;
}apdu_content;

#define SPDU_CONTENT_LEN sizeof(spdu_content)
typedef struct spdu_content_s
{
unsigned char  spdu_tag;
unsigned short session_number;
unsigned int   s_pdu_len;
unsigned int   res_id;
}spdu_content;

#define XPDU_CONTENT_LEN sizeof(xpdu_content)
typedef struct xpdu_content_s
{
unsigned char t_pdu_tag;
unsigned char sb_value;
unsigned char spdu_cnt;
unsigned char apdu_cnt;
unsigned char t_pdu_id;
unsigned int  t_pdu_len;
spdu_content  spdu;
apdu_content  apdu;
unsigned char apdu_buff[0x100];
}xpdu_content;

#define SYSTEM_IDS_T_LEN sizeof(system_ids_t)
typedef struct system_ids_t_s
{
unsigned short pi_system_ids[MAX_CASYSTEM_IDS + 1];
} system_ids_t;

//card status
#define CARDS_STATUS_LEN sizeof(cards_status)
typedef struct cards_status_s
{
unsigned char card_exist;
unsigned char dcas_cnt;
unsigned char max_dcas;
unsigned char card_status;
unsigned char card_munu_status;
unsigned char card_munu_info[0x100];
} cards_status;

#endif

