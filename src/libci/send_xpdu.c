/*
 * create by why @ 2009 06 10 14:40
 * send l_pdu t_pdu s_pdu a_pdu and so on
 */
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "ci_define.h"

#define VLC_SUCCESS   0
#define VLC_EGENERIC -1
#define SIZE_INDICATOR 0x80
extern cards_status all_card_status;


static uint8_t *SetLength( uint8_t *p_data, int i_length )
{
    uint8_t *p = p_data;

    if ( i_length < 128 )
    {
        *p++ = i_length;
    }
    else if ( i_length < 256 )
    {
        *p++ = SIZE_INDICATOR | 0x1;
        *p++ = i_length;
    }
    else if ( i_length < 65536 )
    {
        *p++ = SIZE_INDICATOR | 0x2;
        *p++ = i_length >> 8;
        *p++ = i_length & 0xff;
    }
    else if ( i_length < 16777216 )
    {
        *p++ = SIZE_INDICATOR | 0x3;
        *p++ = i_length >> 16;
        *p++ = (i_length >> 8) & 0xff;
        *p++ = i_length & 0xff;
    }
    else
    {
        *p++ = SIZE_INDICATOR | 0x4;
        *p++ = i_length >> 24;
        *p++ = (i_length >> 16) & 0xff;
        *p++ = (i_length >> 8) & 0xff;
        *p++ = i_length & 0xff;
    }

    return p;
}

#define MAX_TPDU_SIZE  2048
#define MAX_TPDU_DATA  (MAX_TPDU_SIZE - 4)
#define DATA_INDICATOR 0x80

#define T_SB           0x80
#define T_RCV          0x81
#define T_CREATE_TC    0x82
#define T_CTC_REPLY    0x83
#define T_DELETE_TC    0x84
#define T_DTC_REPLY    0x85
#define T_REQUEST_TC   0x86
#define T_NEW_TC       0x87
#define T_TC_ERROR     0x88
#define T_DATA_LAST    0xA0
#define T_DATA_MORE    0xA1

static int TPDUSend( uint32_t cardtype,uint8_t tpdu_id, uint8_t i_tag,
                     const uint8_t *p_content, int i_length )
{
   	uint8_t i_tcid = tpdu_id;
    	uint8_t p_data[MAX_TPDU_SIZE];
 	uint8_t p_data1[512];
    	int i_size;
	int i,j;
	int len;
	unsigned char dat;
 	unsigned char buff[0x100] = {0};
	unsigned char capmt[128]=
	{0x01,0x80,0xa0,0x81,0x8e,0x01,0x90,0x02,0x00,0x04,0x9f,0x80,0x32,0x81,0x84,0x03,0x00,0x68,0xf3,0xf0,0x00,0x02,
  	0xe4,0x10,0x00,0x3a,0x01,0x09,0x11,0x01,0x00,0xe4,0x16,0x00,0x93,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,
  	0x2a,0x53,0x09,0x11,0x01,0x00,0xe4,0x17,0x01,0x2d,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x2a,0x53,0x09,
  	0x11,0x01,0x00,0xe4,0x18,0x01,0x30,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x2a,0x53,0x04,0xe4,0x11,0x00,
  	0x3a,0x01,0x09,0x11,0x01,0x00,0xe4,0x16,0x00,0x93,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x2a,0x53,0x09,
  	0x11,0x01,0x00,0xe4,0x17,0x01,0x2d,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x2a,0x53};
	unsigned char capmtother[21]=
	{
		0x01,0x00,0x09,0x11,0x01,0x00,0xe4,0x18,0x01,0x30,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x2a,0x53
	};
    	i_size = 0;
    	p_data[0] = i_tcid;
  	//p_data[1] = 0x80;
     	p_data[1] = 0x00;
   	p_data[2] = i_tag;

    	switch ( i_tag )
    	{
    		case T_RCV:
    		case T_CREATE_TC:
   	 	case T_CTC_REPLY:
    		case T_DELETE_TC:
    		case T_DTC_REPLY:
    		case T_REQUEST_TC:
       	 	p_data[3] = 1; /* length */
        	p_data[4] = i_tcid;
        	i_size = 5;
        	break;

    		case T_NEW_TC:
    		case T_TC_ERROR:
        	p_data[3] = 2; /* length */
        	p_data[4] = i_tcid;
        	p_data[5] = p_content[0];
        	i_size = 6;
        	break;

    		case T_DATA_LAST:
    		case T_DATA_MORE:
    		{
        /* i_length <= MAX_TPDU_DATA */
        uint8_t *p = p_data + 3;
        p = SetLength( p, i_length + 1 );
        *p++ = i_tcid;

        if ( i_length )
            memcpy( p, p_content, i_length );
            i_size = i_length + (p - p_data);
        }
        break;

    default:
        break;
    }
#if 0
    for(i=0; i<i_size; i++)
    {
        printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 0x%02X\n", p_data[i]);
    }
#endif
    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&size= %d\n", i_size);

// for(i=0;i<i_size;i++)
// printf("0x%x,",p_data[i]);
// printf("\n");
//
//p_data[9]=0x04;
//p_data[18]|=0xc0;
//p_data[19]=0xf0; 
//len=read_ci(IO_CARDB,buff);
   //     parse_readci(IO_CARDB,buff,len);
      //  len=read_ci(IO_CARDB,buff);
        //parse_readci(IO_CARDB,buff,len);
#if 1
 read_ci(IO_CARDB,buff); 

    if(i_size>128)
    {
    	write_card_io(cardtype,p_data, 128);
	p_data1[0]=0x01;
	p_data1[1]=0x00;
	memcpy(p_data1+2,p_data+128,i_size-128+2);
	for(i=0;i<4;i++)
		{
       			 dat=ci_io_read_u8(0x01);	
		 	//printf("read  reg01:0x%x\n ",dat);
		}
	write_card_io(cardtype,p_data1, i_size-128+2);
	
    }
    else
    {
    	if(write_card_io(cardtype,p_data, i_size) != i_size)
    	{
        	printf("cannot write to CAM device\n");
    	}
     }
	
	#endif
	//if(write_card_io(cardtype,p_data, i_size) != i_size)
    	//{
        //	printf("cannot write to CAM device\n");
    	//}
	//write_card_io(cardtype,capmt, 128);
	//write_card_io(cardtype,capmtother, 21);
	//read_ci(cardtype,buff);
	//write_card_io(cardtype,capmt, 128);
	//write_card_io(cardtype,capmtother, 21);
/*
    if ( write( p_sys->i_ca_handle, p_data, i_size ) != i_size )
    {
        msg_Err( p_access, "cannot write to CAM device (%s)",
                 strerror(errno) );
        return VLC_EGENERIC;
    }
*/
    return VLC_SUCCESS;
}

#define ST_SESSION_NUMBER           0x90
#define ST_OPEN_SESSION_REQUEST     0x91
#define ST_OPEN_SESSION_RESPONSE    0x92
#define ST_CREATE_SESSION           0x93
#define ST_CREATE_SESSION_RESPONSE  0x94
#define ST_CLOSE_SESSION_REQUEST    0x95
#define ST_CLOSE_SESSION_RESPONSE   0x96

#define SS_OK             0x00
#define SS_NOT_ALLOCATED  0xF0

#define RI_RESOURCE_MANAGER            0x00010041
#define RI_APPLICATION_INFORMATION     0x00020041
#define RI_CONDITIONAL_ACCESS_SUPPORT  0x00030041
#define RI_HOST_CONTROL                0x00200041
#define RI_DATE_TIME                   0x00240041
#define RI_MMI                         0x00400041
static int SPDUSend( uint32_t cardtype,uint8_t tpdu_id, int i_session_id,
                     uint8_t *p_data, int i_size )
{
    uint8_t *p_spdu = malloc( i_size + 4 );
    uint8_t *p = p_spdu;
    uint8_t i_tag;

    *p++ = ST_SESSION_NUMBER;
    *p++ = 0x02;
    *p++ = (i_session_id >> 8);
    *p++ = i_session_id & 0xff;

    memcpy( p, p_data, i_size );

    i_size += 4;
    p = p_spdu;

    while ( i_size > 0 )
    {
        if ( i_size > MAX_TPDU_DATA )
        {
            if ( TPDUSend(cardtype, tpdu_id, T_DATA_MORE, p,
                           MAX_TPDU_DATA ) != VLC_SUCCESS )
            {
               printf( "couldn't send TPDU on session %d",
                       i_session_id );
                free( p_spdu );
                return VLC_EGENERIC;
            }
            p += MAX_TPDU_DATA;
            i_size -= MAX_TPDU_DATA;
        }
        else
        {
            if ( TPDUSend( cardtype,tpdu_id, T_DATA_LAST, p, i_size )
                    != VLC_SUCCESS )
            {
                printf( "couldn't send TPDU on session %d",
                        i_session_id );
                free( p_spdu );
                return VLC_EGENERIC;
            }
            i_size = 0;
        }
/*
        if ( TPDURecv( tpdu_id, &i_tag, NULL, NULL ) != VLC_SUCCESS
               || i_tag != T_SB )
        {
            msg_Err( p_access, "couldn't recv TPDU on session %d",
                     i_session_id );
            free( p_spdu );
            return VLC_EGENERIC;
        }
*/
    }

    free( p_spdu );
    return VLC_SUCCESS;
}

/*****************************************************************************
 * APDUSend
 *****************************************************************************/
int APDUSend( uint32_t cardtype,uint8_t tpdu_id, int i_session_id, int i_tag,
              uint8_t *p_data, int i_size )
{
    uint8_t *p_apdu = malloc( i_size + 12 );
    uint8_t *p = p_apdu;
    int i_ret;

    *p++ = (i_tag >> 16);
    *p++ = (i_tag >> 8) & 0xff;
    *p++ = i_tag & 0xff;
    p = SetLength( p, i_size );
    if ( i_size )
        memcpy( p, p_data, i_size );
    i_ret = SPDUSend(cardtype, tpdu_id, i_session_id, p_apdu, i_size + p - p_apdu );

    free( p_apdu );
    return i_ret;
}


//send t_create_pdu
unsigned int send_t_create_pdu(uint32_t cardtype,unsigned char conn_id)
{
    unsigned char buff[5] = {conn_id, 
                             0x00, 
                             T_CREATE_PDU, 
                             0x01, 
                             conn_id};
	//if(all_card_status.card_exist==CARD_ALL)
     //    	return write_card_io_delay(cardtype,buff, 5);
	//else
    		return write_card_io(cardtype,buff, 5);
}
unsigned int send_t_create_pdu_nodelay(uint32_t cardtype,unsigned char conn_id)
{
    unsigned char buff[5] = {conn_id, 
                             0x00, 
                             T_CREATE_PDU, 
                             0x01, 
                             conn_id};
    		return write_card_io(cardtype,buff, 5);
}

//send t_recv_pdu
unsigned int send_t_recv_pdu(uint32_t cardtype,unsigned char conn_id)
{
    unsigned char buff[5] = {conn_id, 
                             0x00, 
                             T_RCV_PDU, 
                             0x01, 
                             0x01};

    return write_card_io(cardtype,buff, 5);
}
//send t_recv_pdu
unsigned int send_t_data_last(uint32_t cardtype,unsigned char conn_id)
{
    unsigned char buff[5] = {conn_id, 
                             0x00, 
                             T_DATA_LAST_PDU, 
                             0x01, 
                             0x01};

    return write_card_io(cardtype,buff, 5);
}
//send session open response
unsigned int send_s_open_response(uint32_t cardtype,unsigned char conn_id, 
                                  unsigned short session_id, 
                                  unsigned int res_id)
{
    unsigned char buff[14] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x0A, 
                              0x01, 
                              S_OPEN_RESPONSE, 
                              0x07, 0x00, 
                              (res_id & 0xFF000000)>>24, 
                              (res_id & 0x00FF0000)>>16, 
                              (res_id & 0x0000FF00)>>8, 
                              (res_id & 0x000000FF), 
                              (session_id&0xFF00)>>8, session_id&0x00FF
                              };

    return write_card_io(cardtype,buff, 14);
}

//send query session
unsigned int send_s_query(uint32_t cardtype,unsigned char conn_id, 
                                  unsigned short session_id, 
                                  unsigned int apdu_tag)
{
    unsigned char buff[13] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x09, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                              (apdu_tag&0x00FF0000)>>16, 
                              (apdu_tag&0x0000FF00)>>8, 
                              (apdu_tag&0x000000FF),
                              0x00};

    if(0 == apdu_tag)
    {
        return 0;
    }

    return write_card_io(cardtype,buff, 13);
}

unsigned int send_s_profile_connect(uint32_t cardtype,unsigned char conn_id, 
                                  unsigned short session_id, 
                                  unsigned int apdu_tag)
{
    unsigned char buff[13] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x09, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                             0x9f, 
                             0x80, 
                              0x10,
                              0x00};

    if(0 == apdu_tag)
    {
        return 0;
    }

    return write_card_io(cardtype,buff, 13);
}

unsigned int send_s_appinfo_connect(uint32_t cardtype,unsigned char conn_id, 
                                  unsigned short session_id, 
                                  unsigned int apdu_tag)
{
    unsigned char buff[13] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x09, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                             0x9f, 
                             0x80, 
                              0x20,
                              0x00};

    if(0 == apdu_tag)
    {
        return 0;
    }

    return write_card_io(cardtype,buff, 13);
}

unsigned int send_s_cainfo_connect(uint32_t cardtype,unsigned char conn_id, 
                                  unsigned short session_id, 
                                  unsigned int apdu_tag)
{
    unsigned char buff[13] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x09, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                             0x9f, 
                             0x80, 
                              0x30,
                              0x00};

    if(0 == apdu_tag)
    {
        return 0;
    }

    return write_card_io(cardtype,buff, 13);
}

unsigned int send_s_timeinfo_connect(uint32_t cardtype,unsigned char conn_id, 
                                  unsigned short session_id, 
                                  unsigned int apdu_tag)
{
    unsigned char buff[13] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x09, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                             0x9f, 
                             0x84, 
                              0x40,
                              0x00};

    if(0 == apdu_tag)
    {
        return 0;
    }

    return write_card_io(cardtype,buff, 13);
}
unsigned int send_s_mmiinfo_connect(uint32_t cardtype,unsigned char conn_id, 
                                  unsigned short session_id, 
                                  unsigned int apdu_tag)
{
    unsigned char buff[13] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x09, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                             0x9f, 
                             0x88, 
                              0x00,
                              0x00};

    if(0 == apdu_tag)
    {
        return 0;
    }

    return write_card_io(cardtype,buff, 13);
}

unsigned int send_s_menu_connect(uint32_t cardtype,unsigned char conn_id, 
                                  unsigned short session_id, 
                                  unsigned int apdu_tag)
{
    unsigned char buff[13] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x09, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                             0x9f, 
                             0x84, 
                              0x40,
                              0x00};

    if(0 == apdu_tag)
    {
        return 0;
    }

    return write_card_io(cardtype,buff, 13);
}
unsigned int send_datetime_response(uint32_t cardtype,unsigned char conn_id, 
                                   unsigned short session_id)
{
    unsigned char buff[18] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x0e, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF, 
                              0x9F, 0x84, 0x41, 0x5,
                              0xd0, 0x7b, 0x00, 0x00,
                              0x02
                              };

    return write_card_io(cardtype,buff, 18);
}
//send change profile
unsigned int send_s_change_profile(uint32_t cardtype,unsigned char conn_id, 
                                   unsigned short session_id)
{
    unsigned char buff[13] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x09, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                              0x9F, 0x80, 0x12, 0x00
                              };

    return write_card_io(cardtype,buff, 13);
}

//send profile query response
unsigned int send_profile_response(uint32_t cardtype,unsigned char conn_id, 
                                   unsigned short session_id)
{
    unsigned char buff[33] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x1D, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF, 
                              0x9F, 0x80, 0x11, 0x14,
                              0x00, 0x01, 0x00, 0x41,
                              0x00, 0x02, 0x00, 0x41,
                              0x00, 0x03, 0x00, 0x41,
                              0x00, 0x24, 0x00, 0x41,
                              0x00, 0x40, 0x00, 0x41
                              };

    return write_card_io(cardtype,buff, 33);
}

//
unsigned int send_s_display_ctrl_response(uint32_t cardtype,unsigned char conn_id, 
                                          unsigned short session_id)
{
    unsigned char buff[15] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x0B, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                              0x9F, 0x88, 0x02, 0x02, 0x01, 0x01};

    return write_card_io(cardtype,buff, 15);
}

//send menu asnw
unsigned int send_s_menu_answ(uint32_t cardtype,unsigned char conn_id, 
                              unsigned short session_id)
{
    unsigned char buff[14] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x0A, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                              0x9F, 0x88, 0x0B, 0x01, 0x01};

    return write_card_io(cardtype,buff, 14);
}

//send open session
unsigned int send_open_session_response(uint32_t cardtype,unsigned char conn_id)
{
    unsigned char buff[14] = {conn_id, 
                              0, 
                              T_DATA_LAST_PDU, 
                              0x08, 
                              conn_id, 
                              S_OPEN_RESPONSE, 
                              0x07, 
                              0x00, 0x00, 0x01, 0x00,
                              0x41, 0x01};

    return write_card_io(cardtype,buff, 14);
}

//send create session
unsigned int send_create_session(uint32_t cardtype,unsigned char conn_id, 
                                 unsigned short session_id)
{
    unsigned char buff[13] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x09, 
                              0x01, 
                              S_CREATE_REQUEST, 
                              0x06, 
                              0x00, 0x03, 0x00, 0x41, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                              };

    return write_card_io(cardtype,buff, 13);
}

//send close session
unsigned int send_s_close_session(uint32_t cardtype,unsigned char conn_id, 
                                  unsigned short session_id)
{
    unsigned char buff[10] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x06, 
                              0x01, 
                              S_CLOSE_RESPONSE, 
                              0x03, 
                              0x00, 
                              (session_id&0xFF00)>>8, session_id&0x00FF};

    return write_card_io(cardtype,buff, 10);
}
//send menu enter
unsigned int MenuEnterSend(uint32_t cardtype,unsigned char conn_id, 
                              unsigned short session_id)
{
    unsigned char buff[13] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x09, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                              0x9F, 0x80, 0x22, 0x00};   
     printf("MenuEnterSend conn_id=%d,session_id=%d\n",conn_id,session_id);
    return write_card_io(cardtype,buff, 13);
}
//send menu enter
unsigned int MenuCloseSend(uint32_t cardtype,unsigned char conn_id, 
                              unsigned short session_id)
{
    unsigned char buff[13] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x09, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                              0x9F, 0x88, 0x00, 0x00};
    printf("MenuCloseSend\n");
    return write_card_io(cardtype,buff, 13);
}
//send menu control
unsigned int MenuControlSend(uint32_t cardtype,unsigned char conn_id, 
                              unsigned short session_id,unsigned char cmd)
{
    unsigned char buff[14] = {conn_id, 
                              0x00, 
                              T_DATA_LAST_PDU, 
                              0x0a, 
                              0x01, 
                              S_NUMBER, 
                              0x02, 
                              (session_id&0xFF00)>>8, session_id&0x00FF,
                              0x9F, 0x88, 0x0b, 0x01,cmd};
printf("MenuControlSend\n");
    return write_card_io(cardtype,buff, 14);
}
//send menu control
unsigned int MenuStringSend(uint32_t cardtype,unsigned char conn_id, 
                              unsigned short session_id,char*str, uint32_t len)
{
	 uint8_t *buff = malloc( len + 14 );
printf("MenuStringSend\n");
     buff[0] = conn_id;
    buff[1]  = 0x00;
   buff[2]  =T_DATA_LAST_PDU;
    buff[3]  =0x0a+len;
   buff[4]  =  0x01;
   buff[5]  = S_NUMBER;
    buff[6]  = 0x02;
    buff[7]  = (session_id&0xFF00)>>8;
    buff[8]  =  session_id&0x00FF;
    buff[9]  = 0x9F;
    buff[10]  = 0x88;
   buff[11]  = 0x08;
   buff[12]  = len+1;
    buff[13]  = 0x01;
     memcpy( buff+14, str, len );
     write_card_io(cardtype,buff, 14+len);
     free(buff);
}


