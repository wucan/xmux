/*
 * create by why @ 2009 06 10 14:40
 * parser l_pdu t_pdu s_pdu a_pdu and so on
 */
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "ci_define.h"

#define get_res_id(buff, pos) (buff[pos]<<24) |\
                              (buff[pos+1]<<16) |\
                              (buff[pos+2]<<8)  |\
                              (buff[pos+3])

#define get_session_num(buff, pos) (buff[pos]<<8)|(buff[pos+1])

#define get_apdu_tag(buff, pos) (buff[pos]<<16) | \
                                (buff[pos+1]<<8)  | \
                                (buff[pos+2])
extern void PrintMenuInfo(unsigned char*str,int len);
#if 1
static unsigned int get_xpdu_lenfiled(unsigned char* buff)
{
    unsigned int ret = 0;
    //unsigned int lenpos = 1;
    unsigned int lenfield = 0;

    lenfield = buff[0]&0xFF;

    if(0x80 & lenfield)
    {
        lenfield = lenfield&0x7F;
	if(lenfield==0 || (lenfield>2))
	{
		printf("Invalid TPDU Length (%d) \n",lenfield);
		return 0;
	}
	if(lenfield == 1)
	ret = buff[1];
      else
	ret = (buff[1]<<8|buff[2]);    
	  
    }
    else
    {
        ret = lenfield;
    }

    return ret;
}
#endif
#if 0
static unsigned int get_xpdu_lenfiled(unsigned char* buff)
{
    unsigned int ret = 0;
    unsigned int lenpos = 1;
    unsigned int lenfield = 0;

    lenfield = buff[0]&0xFF;

    if(0x80 & lenfield)
    {
        lenfield = lenfield&0x7F;
        while(lenfield)
        {
            ret |= (buff[lenpos]<<(lenfield*8));
            lenfield--;
            lenpos++;
        }
    }
    else
    {
        ret = lenfield;
    }

    return ret;
}
#endif


static unsigned int parser_spdu(xpdu_content* xpdu, 
                                unsigned char* buff)
{
    unsigned int ret = 0;

    xpdu->spdu_cnt++;
    xpdu->spdu.spdu_tag = buff[0];
    xpdu->spdu.s_pdu_len = get_xpdu_lenfiled(buff+1);
    switch(xpdu->spdu.spdu_tag)
    {
        case S_OPEN_REQUEST:
            xpdu->spdu.res_id = get_res_id(buff, 2);
            ret = 6;
            break;

        case S_CREATE_RESPONSE:
            xpdu->spdu.res_id = get_res_id(buff, 3);
            xpdu->spdu.session_number = get_session_num(buff, 7);
            ret = 9;
            break;

        case S_CLOSE_REQUEST:
            xpdu->spdu.session_number = get_session_num(buff, 2);
            ret = 4;
            break;

        case S_CLOSE_RESPONSE:
            xpdu->spdu.session_number = get_session_num(buff, 3);
            ret = 5;
            break;

        case S_NUMBER:
            xpdu->spdu.session_number = get_session_num(buff, 2);
            ret = 4;
            break;

        default:
            break;
    }

    return ret;
}

static unsigned int parser_apdu(xpdu_content* xpdu, 
                                unsigned char* buff)
{
    unsigned int ret = 0;
    unsigned int head_len = 4;

    xpdu->apdu_cnt++;
    xpdu->apdu.apdu_tag = get_apdu_tag(buff, 0);
    xpdu->apdu.apdu_len = get_xpdu_lenfiled(buff+3);

    ret = xpdu->apdu.apdu_len;

    if(0x80 & buff[3])
    {
        head_len += (0x7F & buff[3]);
    }
    ret += head_len;
    
    memcpy(xpdu->apdu_buff, buff+head_len, xpdu->apdu.apdu_len);

    return ret;
}

static unsigned int parser_s_number_spdu(xpdu_content* xpdu, 
                                         unsigned char* buff,
                                         unsigned int len)
{
    unsigned ret = 0;
    unsigned int spdu_idx = xpdu->spdu_cnt;
    unsigned int apdu_idx = 0;

    while(len>0)
    {
        if(0x9F != buff[0])
        {
            ret += parser_spdu(xpdu, buff+ret);
        }
        else
        {
            ret += parser_apdu(xpdu, buff+ret);
        }
        
        len-=ret;
        break;
    }
    
    return ret;
}

unsigned int parser_xpdu(xpdu_content* xpdu,
                         unsigned char* buff, 
                         unsigned int len)
{
    unsigned int ret = 0;
    unsigned int offset=0;
    if(NULL == buff)
    {
        return -1;
    }

    if(0 == len)
    {
        return -1;
    }

    if(NULL == xpdu)
    {
        return -1;
    }

    xpdu->t_pdu_tag = buff[2];
    xpdu->t_pdu_len = get_xpdu_lenfiled(buff+3);
    if(xpdu->t_pdu_len==0)
    return -1;
    xpdu->sb_value = buff[len-1];
    offset= buff[3]&0xFF;
    if(0x80 & offset)
    {
      offset=offset&0x7f;
     // printf("offset=%d\n",offset);

     }
     else
     offset=0;
    if(T_DATA_LAST_PDU == xpdu->t_pdu_tag ||
       T_DATA_MORE_PDU == xpdu->t_pdu_tag)
    {
        ret = parser_spdu(xpdu, buff+5+offset);
    }
    else
    {
        return ret;
    }
      //printf("spdu_tag=%d\n",xpdu->spdu.spdu_tag);
    if(S_NUMBER == xpdu->spdu.spdu_tag)
    {
    	//PrintMenuInfo(buff,len);
        ret += parser_s_number_spdu(xpdu, buff+5+ret+offset, len-5-ret-offset);
    }

    return ret;
}

unsigned int get_system_id(unsigned char* buff, 
                           system_ids_t* sys_ids, 
                           int len)
{
    int i=0;
    int j=0;

    memset(sys_ids, 0, SYSTEM_IDS_T_LEN);
    for(i=0;i<len;i+=2)
    {
        sys_ids->pi_system_ids[j] = (buff[i]<<8) | buff[i+1];
        
        printf("[--------------------system id is : 0x%04X]\n", sys_ids->pi_system_ids[j]);
        printf("[--------------------len is : 0x%04X]\n", len);
        printf("[--------------------buff 1 is : 0x%02X]\n", buff[i]);
        printf("[--------------------buff 2 is : 0x%02X]\n", buff[i+1]);

        j++;
    }
}
/*
void free_spdu_content(spdu_content* xpdu)
{
    spdu_content* p;

    if(NULL == xpdu)
    {
        return;
    }

    p = xpdu->next;

    while(NULL != xpdu)
    {
        free(xpdu);
        xpdu = p;
        p = p->next;
    }
}
*/
