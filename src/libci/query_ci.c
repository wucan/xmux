/*
 * create by why @ 2009 06 10 14:40
 * loop query ci if thire is have something to read
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include "ci_define.h"
#include "dvbpsi/dvbpsi.h"
#include "dvbpsi/descriptor.h"
#include "dvbpsi/pat.h"
#include "dvbpsi/pmt.h"
//#include "dvbpsi/dr.h"
#include "dvbpsi/psi.h"
typedef struct xpmtsectin_s
{
	unsigned int len;
	unsigned char buf[1024];
}pmtinfo;

#if 1
typedef struct pmtsectin_s
{
	unsigned int  programtotal;
	unsigned int index;
	pmtinfo section[64];
}pmtsectin;
#endif
pmtsectin 	cur_section;
extern unsigned char  conn_id;
extern unsigned short res_mng_sid;
extern unsigned short app_inf_sid;
extern unsigned short ca_support_sid;
extern unsigned short host_ctrl_sid;
extern unsigned short date_time_sid;
extern unsigned short mmi_sid;
extern unsigned short low_speed_comm_sid;
extern cards_status all_card_status;
static unsigned char ci_work_status;

system_ids_t sys_ids;
static unsigned char send_pmt_status0=0;
static unsigned char send_pmt_en0=0;
unsigned int fpgaswitchmode=0;
unsigned int ci_max_cnt;
unsigned char curpmt[1024];
#define status ((cards_status*)(&all_card_status))

static pthread_t query_ci_thread;
static pthread_t send_pmt_thread;


extern cards_status* get_cards_status();
extern unsigned int GetCardMaxChannel(void);
extern void Set_Ci_CPLD_Mode(uint32_t mode);
extern unsigned int MenuEnterSend(uint32_t cardtype,unsigned char conn_id, 
                              unsigned short session_id);

extern unsigned int MenuCloseSend(uint32_t cardtype,unsigned char conn_id, 
                              unsigned short session_id);

extern unsigned int MenuControlSend(uint32_t cardtype,unsigned char conn_id, 
                              unsigned short session_id,unsigned char cmd);

extern unsigned int MenuStringSend(uint32_t cardtype,unsigned char conn_id, 
                              unsigned short session_id,char*str, uint32_t len);

void SendCIPMT(char* pmt,unsigned int pmt_index,unsigned int len);
extern unsigned char CA_SEND_PMT(unsigned char *pBuf, unsigned int dwLen,int type);
unsigned int init_pmt_setA=0;
static void start_send_pmt();
static void stop_send_pmt();
void clean_ci_dcas_list();
unsigned char ciname[20]={0};
void  get_ci_name(char* str)
{
	memcpy(str,ciname,20);
}
void  parse_readci(uint32_t cardtype,unsigned char* buff,unsigned int len)
{
	int ret=0;
	xpdu_content  xpdu = {0};
	parser_xpdu(&xpdu, buff, len);
        if(xpdu.t_pdu_tag==T_SB_PDU)
        {
        	if(buff[5]==0x00)
		send_t_data_last(cardtype,conn_id);
		else
		send_t_recv_pdu(cardtype,conn_id);
        }
}
void set_ci_max_cnt(unsigned int cnt)
{
	ci_max_cnt=cnt;
}
void loadPMT(uint8_t *sec[], int sec_len[], int num)
{
      int i,j;
#if 1
      printf("loadPMT:prog total:%d\n",num);
      stop_send_pmt();
      clean_ci_dcas_list();
      cur_section.index=0;
      cur_section.programtotal=num;
      printf("loadPMT:programnum=%d\n", cur_section.programtotal);
	 status->dcas_cnt = 0;
       for(i=0;i<num;i++)
       {
        	memcpy(cur_section.section[i].buf,sec[i],sec_len[i]);
                cur_section.section[i].len=sec_len[i];
                printf("program[%d]\n");
                for(j=0;j<sec_len[i];j++)
		{
                   printf("0x%x,",cur_section.section[i].buf[j]);
		}
                printf("\n");
       }
	   start_send_pmt();
#endif
}
 void PrintMenuInfo(unsigned char*str,int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		printf("0x%02X,",str[i]);
		if(((i%10)==0)&&(i!=0))
		printf("\n");
	}
	printf("\n");
}
 int casystemidflag0=0;
  int casystemidflag1=0;
static void handle_apdu(uint32_t cardtype,xpdu_content* xpdu)
{
    unsigned char buff[0x100];
    unsigned int capmt_size;
    
    int i;
   int len;
    switch(xpdu->apdu.apdu_tag)
    {
        case PROFILE_ENQ:
            send_profile_response(cardtype,conn_id, res_mng_sid);
           // len=read_ci(cardtype,buff, sizeof(buff));
           //     parse_readci(cardtype,buff,len);
            break;

        case PROFILE:
            send_s_change_profile(cardtype,conn_id, res_mng_sid);
          // len=read_ci(cardtype,buff, sizeof(buff));
           //     parse_readci(cardtype,buff,len);
            break;

        case PROFILE_CHG://TODO:need fix it.
            break;

        case APPLACATION:
           PrintMenuInfo(xpdu->apdu_buff, xpdu->apdu.apdu_len);
		send_t_data_last(cardtype,conn_id);
            if(xpdu->apdu_buff[5]>20)
            xpdu->apdu_buff[5]=20;
          // printf("%s\n",xpdu->apdu_buff+6);
            memcpy(ciname,xpdu->apdu_buff+6,xpdu->apdu_buff[5]);
             printf("%s\n",ciname);

	  // MenuEnterSend(0,conn_id,app_inf_sid);
	  //read_ci(cardtype,buff, sizeof(buff));
            break;

        case CA_INFO:
            //TODO:set ca pmt table here
            get_system_id(xpdu->apdu_buff, &sys_ids, xpdu->apdu.apdu_len);
            status->card_status = CARD_IS_READY;
	     status->card_munu_status = CARD_NOMENU;

	     printf("card A ready!\n");
		send_t_data_last(cardtype,conn_id); 
             send_pmt_en0=1;
            break;
      case DATETIMEINFO:
	//printf("DATETIMEINFO\n");
        send_t_data_last(cardtype,conn_id);
	//send_datetime_response(cardtype,conn_id, date_time_sid);
    //    len=read_ci(cardtype,buff, sizeof(buff));
     //           parse_readci(cardtype,buff,len);
		// MenuEnterSend(0,conn_id,app_inf_sid);
	   //read_ci(cardtype,buff, sizeof(buff));
	break;
        case CA_PMT_REPLY:
            break;

        case DISPLAY_CTRL:
            send_s_display_ctrl_response(cardtype,conn_id, mmi_sid);
       //   len=read_ci(cardtype,buff, sizeof(buff));
      //          parse_readci(cardtype,buff,len);
            break;

        case MENU_LAST:
	    if(status->card_munu_status != CARD_MENU)
	    status->card_munu_status =CARD_MENU;
	     memcpy(status->card_munu_info, xpdu->apdu_buff, xpdu->apdu.apdu_len);
		 status->card_munu_info[xpdu->apdu.apdu_len]=0;
		 printf("MENU_LAST:\n");
		 PrintMenuInfo(xpdu->apdu_buff, xpdu->apdu.apdu_len);
		 send_t_data_last(cardtype,conn_id);
            break;
	 case MENU_INPUT:
	  if(status->card_munu_status != CARD_MENU_INPUT)
	    status->card_munu_status =CARD_MENU_INPUT;
	     memcpy(status->card_munu_info, xpdu->apdu_buff, xpdu->apdu.apdu_len);
		status->card_munu_info[xpdu->apdu.apdu_len]=0;
		 printf("MENU_INPUT:\n");
		 PrintMenuInfo(xpdu->apdu_buff, xpdu->apdu.apdu_len);
		break;
        default:
        if((xpdu->apdu.apdu_tag>>8)&0xffff==0x9f88)
        	{
        		//send_s_menu_answ(cardtype,conn_id, mmi_sid);
            		//read_ci(cardtype,buff, sizeof(buff));
        		 memcpy(status->card_munu_info, xpdu->apdu_buff, xpdu->apdu.apdu_len);
		       status->card_munu_info[xpdu->apdu.apdu_len]=0;
		        printf("MENU OTHER:\n");
 			PrintMenuInfo(xpdu->apdu_buff, xpdu->apdu.apdu_len);
        	}
            break;
    }
}
static void handle_open_session(uint32_t cardtype,xpdu_content* xpdu)
{
    unsigned int res_id = xpdu->spdu.res_id;
    unsigned char sid = 0;
    unsigned query_apdu_tag = 0;
    unsigned char buff[0x100];
    unsigned int len;
	unsigned char dat;
	int i;
  printf("handle_open_session res_id=0x%04x\n ",res_id);
    switch(res_id)
    {
        case RESOURCE_MANAGER:
            sid = res_mng_sid;
            query_apdu_tag = PROFILE_ENQ;
		//	usleep(10000);
		send_s_open_response(cardtype,conn_id, sid, xpdu->spdu.res_id); 
    		len=read_ci(cardtype,buff, sizeof(buff));
                //parse_readci(cardtype,buff,len);
             
		for(i=0;i<17;i++)
		{
       			 dat=ci_io_read_u8(0x01);	
		 	//printf("read  reg01:0x%x\n ",dat);
		}
    		send_s_profile_connect(cardtype,conn_id, sid, query_apdu_tag);
    		//len=read_ci(cardtype,buff, sizeof(buff));
                //parse_readci(cardtype,buff,len);
		
            break;

        case APPLACATION_INFO:
            sid = app_inf_sid;
            query_apdu_tag = APPLACATION_ENQ;
		send_s_open_response(cardtype,conn_id, sid, xpdu->spdu.res_id);
    		 len=read_ci(cardtype,buff, sizeof(buff));
                //parse_readci(cardtype,buff,len);
            	
                for(i=0;i<17;i++)
		{
       			 dat=ci_io_read_u8(0x01);	
		 	//printf("read  reg01:0x%x\n ",dat);
		}
    		send_s_appinfo_connect(cardtype,conn_id, sid, query_apdu_tag);
    		//len=read_ci(cardtype,buff, sizeof(buff));
               // parse_readci(cardtype,buff,len);
            	
            break;

        case CA_SUPPORT:
            	sid = ca_support_sid;
            	query_apdu_tag = CA_INFO_ENQ;
		send_s_open_response(cardtype,conn_id, sid, xpdu->spdu.res_id);
    		len=read_ci(cardtype,buff, sizeof(buff));
              //  parse_readci(cardtype,buff,len);
            	
                 for(i=0;i<17;i++)
		{
       			 dat=ci_io_read_u8(0x01);	
		 	//printf("read  reg01:0x%x\n ",dat);
		}
    		send_s_cainfo_connect(cardtype,conn_id, sid, query_apdu_tag);
    		//len=read_ci(cardtype,buff, sizeof(buff));
                //parse_readci(cardtype,buff,len);
            	
            break;

        case HOST_CONTROL:
            sid = host_ctrl_sid;
			send_s_open_response(cardtype,conn_id, sid, xpdu->spdu.res_id);
    		len=read_ci(cardtype,buff, sizeof(buff));
                parse_readci(cardtype,buff,len);
            	
    		send_s_query(cardtype,conn_id, sid, query_apdu_tag);
    		len=read_ci(cardtype,buff, sizeof(buff));
                parse_readci(cardtype,buff,len);
            	
            break;

        case DATE_TIME:
            sid = date_time_sid;
			sid=sid-1;
		send_s_open_response(cardtype,conn_id, sid, xpdu->spdu.res_id);
    		len=read_ci(cardtype,buff, sizeof(buff));
                parse_readci(cardtype,buff,len);
            	
    		send_s_timeinfo_connect(cardtype,conn_id, sid, query_apdu_tag);
    		//read_ci(cardtype,buff, sizeof(buff));
            break;

        case MMI:
            sid = mmi_sid;
		send_s_open_response(cardtype,conn_id, sid, xpdu->spdu.res_id);
//    		len=read_ci(cardtype,buff, sizeof(buff));
  //              parse_readci(cardtype,buff,len);
            	

    //		send_s_mmiinfo_connect(cardtype,conn_id, sid, query_apdu_tag);
    		//len=read_ci(cardtype,buff, sizeof(buff));
            	
            break;

        default:
           send_s_close_session(cardtype,conn_id,xpdu->spdu.session_number);
            break;
    }

    printf("conn_id: 0x%02X, session_id: 0x%02X, res_id: 0x%08X, sid: 0x%02X\n", 
           conn_id, sid, xpdu->spdu.res_id,sid);

    		
	
}

static void handle_spdu(uint32_t cardtype,xpdu_content* xpdu)
{
    unsigned char buff[0x100];
int len;
    switch(xpdu->spdu.spdu_tag)
    {
        case S_OPEN_REQUEST:
            handle_open_session(cardtype,xpdu);
            break;

        case S_CREATE_RESPONSE:
            break;

        case S_CLOSE_REQUEST:
            send_s_close_session(cardtype,conn_id, xpdu->spdu.session_number);
           len=read_ci(cardtype,buff, sizeof(buff));
                parse_readci(cardtype,buff,len);
            break;

        case S_CLOSE_RESPONSE:
            break;

        case S_NUMBER:
            handle_apdu(cardtype,xpdu);
            break;
    }
}

static void handle_xpdu(uint32_t cardtype,xpdu_content* xpdu)
{
    switch(xpdu->t_pdu_tag)
    {
	case T_SB_PDU:
		//send_t_recv_pdu(0,conn_id);
		break;
	case T_REPLY_PDU:
            break;

        case T_DEL_PDU://TODO:need fix it;
            break;

        case T_DEL_REPLY_PDU://TODO:need fix it;
            break;

        case T_REQUEST_PDU://TODO:need fix it;
            break;

        case T_DATA_LAST_PDU:
        case T_DATA_MORE_PDU:
            handle_spdu(cardtype,xpdu);
            break;

        default:
            break;
    }
}

static void dump_xpdu(xpdu_content* xpdu)
{
    printf("[tpdu tag is:]0x%02X\n", xpdu->t_pdu_tag);
    printf("[spdu tag is:]0x%02X\n", xpdu->spdu.spdu_tag);
    printf("[spdu res id is:]0x%08X\n", xpdu->spdu.res_id);
    printf("[apdu tag is:]0x%08X\n", xpdu->apdu.apdu_tag);
}

int cardasend=0;  
int get_pmt(void)
{
	if(cur_section.index<cur_section.programtotal)
	{
	        memcpy(curpmt,cur_section.section[cur_section.index].buf,cur_section.section[cur_section.index].len);
                cur_section.index+=1;
		return cur_section.index-1;
	}
	else
	{
		return -1;
	}
	
}
static void loop_query_ci(void* param)
{
    unsigned char buff[0x100] = {0};
    unsigned int  read_byte = 0;
    xpdu_content  xpdu = {0};

    unsigned int card_ready = 0;
    char* pmtA;
    unsigned int pmt_indexA;
    unsigned int ret = 0;
    send_pmt_en0=0;	
    while(ci_work_status)
    {
     	
	if(init_pmt_setA==0)
    	 {

    		if(cur_section.programtotal>0)
    		{
                        printf("card0 first set pmt\n");	  		
			  init_pmt_setA=1;

    		}
    	 }
	  if((init_pmt_setA==1)&&(send_pmt_en0==1))
	  {
	  		send_pmt_status0=1;
			status->dcas_cnt = 0;
	  }

	  if((send_pmt_status0==1)&&(send_pmt_en0==1))
         {

	        	pmt_indexA = get_pmt();
	 		if(pmt_indexA==-1)
                        {
				send_pmt_status0=0;
			}
                        else
			{
				printf("pmt_indexA:%d\n",pmt_indexA);
			}
	  }
	  else
	  {
		pmt_indexA=-1;
	  }

	  
	  
	if((CARD_IS_EXIST == status->card_status)
	    ||(CARD_IS_READY == status->card_status))
      {
		if(cardasend==0)
		{
        		send_t_data_last(IO_CARDA,conn_id);
			cardasend=1;
		}
        	read_byte = read_ci(IO_CARDA,buff, sizeof(buff));
        	if(0 != read_byte)
        	{
           		 parser_xpdu(&xpdu, buff, read_byte);
                         if(xpdu.t_pdu_tag==T_SB_PDU)
        		   {	
        			 if(buff[5]==0x00)
				send_t_data_last(IO_CARDA,conn_id);
				else
				send_t_recv_pdu(IO_CARDA,conn_id);
        		  }
           		 handle_xpdu(IO_CARDA,&xpdu);
            		memset(&xpdu, 0, XPDU_CONTENT_LEN);
        	}
                if(send_pmt_en0==1)
                {
			if(pmt_indexA!=-1)
			{
				if(CARD_IS_READY == status->card_status)
				{
					printf("card A start work, index %d!\n", pmt_indexA);
					if((pmt_indexA<ci_max_cnt)&&(pmt_indexA<cur_section.programtotal))
					{
						printf("pmtA:0x%x,0x%x,0x%x,0x%x,0x%x\n",curpmt[0],curpmt[1],curpmt[2],curpmt[3],curpmt[4]);
						SendCIPMT(curpmt,pmt_indexA,cur_section.section[pmt_indexA].len);}
					}
			}
		}
               else
               {
			usleep(100000);
			continue;
		 }
	}
       usleep(100000);
    }
}

static unsigned int try_to_dcas( 
                                char* pmt,unsigned int pmt_index,unsigned int len)
{

    unsigned int capmt_size;
    unsigned char capmt_list_mg;
    unsigned char buff[0x100];
    unsigned int ret = 0;

    printf("%s: pmt index %d, section len %d\n",
        __func__, pmt_index, len);
  	 if(send_pmt_en0==0 )
   	{
		printf("pmt set is interrupt!\n");
    		return 5;
   	}

printf("program total:%d,index:%d\n",cur_section.programtotal,pmt_index);
	if(CARD_IS_READY == status->card_status)
    {
        	if(0 == pmt_index)
        	{
	   		if(cur_section.programtotal==1)
	   		{
		 		printf("nr_program=1.CA_PMT_LIST_ONLY\n ");
            			capmt_list_mg = CA_PMT_LIST_ONLY;
           		}
          		else  if(ci_max_cnt==1)
	   		{
		 		printf("nr_program=1.CA_PMT_LIST_ONLY\n ");
            			capmt_list_mg = CA_PMT_LIST_ONLY;
           		}
           		else
           		{
	    			printf("CA_PMT_LIST_FIRST\n ");
            			capmt_list_mg = CA_PMT_LIST_FIRST;
           		}
        	}
        	else if(pmt_index+1 ==ci_max_cnt)
        	{
	     		printf("CA_PMT_LIST_LAST\n ");
            		capmt_list_mg = CA_PMT_LIST_LAST;
        	}
		else if(pmt_index+1==cur_section.programtotal)
        	{
			printf("CA_PMT_LIST_LAST\n ");
            		capmt_list_mg = CA_PMT_LIST_LAST;
		}
        	else
        	{
	    		printf("CA_PMT_LIST_MORE\n ");
            		capmt_list_mg = CA_PMT_LIST_ADD;
        	}

        	if(pmt_index+1 <ci_max_cnt)
        	{
               	CA_SEND_PMT(pmt,len,capmt_list_mg);
                	sleep(1);
                	status->dcas_cnt++;
            	}
        	else
        	{
            		ret = 2;
        	}
    }
    else
    {
        ret = 3;
    }

    return ret;
}
void SendCIPMT(char* pmt,unsigned int pmt_index,unsigned int len)
{
	unsigned int ret = 0;
 	 ret = try_to_dcas(pmt,pmt_index,len);
        usleep(1000);
}
 
static void start_send_pmt()
{
   send_pmt_en0=1;
}

static void stop_send_pmt()
{
   send_pmt_en0=0;
}

int get_cicard_maxcnt()
{
   int ret;
   ret=ci_max_cnt;
   return ret;
}
void get_ciname(char*name)
{
}

void set_cicard_maxcnt(int num)
{
    ci_max_cnt=num;
}
unsigned int start_query_ci(void)
{
    unsigned ret = 0;
    unsigned char buf[44]={
0x02,0xb0,0x29,0x03,0x7a,0xcf,0x00,0x00,0xe7,0xcf,
0xf0,0x06,0x09,0x04,0x4a,0x02,0xf7,0xdf,0x02,0xe3,
0xea,0xf0,0x06,0x09,0x04,0x4a,0x02,0xf7,0xdf,0x04,
0xe3,0xeb,0xf0,0x06,0x09,0x04,0x4a,0x02,0xf7,0xdf,
0xa7,0xc2,0x1b,0xd2
};
    ci_work_status = 1;
  printf("\n\nci thread start\n\n"); 
     cur_section.index=0;
    printf("pmt init 0x%x,0x%x,0x%x\n",cur_section.section[0].buf[0],cur_section.section[0].buf[1],cur_section.section[0].buf[2]);
    ret = pthread_create(&query_ci_thread, 
                         NULL, 
                         (void *)loop_query_ci, 
                         NULL);

    if(0 != ret)
    {
        return ret;
    }

    return ret;
}

void stop_query_ci()
{
    ci_work_status = 0;

    pthread_join(query_ci_thread, NULL);
    pthread_join(send_pmt_thread, NULL);
}
void clean_ci_dcas_list()
{
   cur_section.index=0;
   cur_section.programtotal=0;
    status->dcas_cnt = 0;

}
void update_ci_dcas_list(pmtsectin* pmtinfo )
{
    memcpy(&cur_section,pmtinfo,sizeof(pmtsectin)); 
    cur_section.index=0;
     status->dcas_cnt = 0;
   
}

static void ci_set_pmt_section(void *sec, int len)
{
	status->dcas_cnt = 0;
	cur_section.programtotal = 1;
	cur_section.index = 0;
	cur_section.section[0].len = len;
	memcpy(&cur_section.section[0].buf, sec, len);
}

void libci_test()
{
	CIStart();
}

