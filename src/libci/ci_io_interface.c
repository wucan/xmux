/*
 * create by why @ 2009 06 10 13:30
 * read & write ci io space & 1021
 */
#include <stdio.h>
#include "ci_define.h"
extern bool ci_card_actived();
extern uint16_t ci_attr_mem_read_u16(int off);
extern bool ci_io_write_u8(int off, uint8_t v);
extern uint8_t ci_io_read_u8(int off);
extern bool ci_mem_write_u16(int off, uint16_t v);
extern uint16_t ci_mem_read_u16;


unsigned char wait_for_io_bit(unsigned int  cardtype,unsigned char mask,unsigned int wait_time)
{
    int i = 0;
    unsigned char ret = 0;
    unsigned short buff_size = 0;

    for(i=0; i<wait_time; i++)
	{
		if(ci_io_read_u8(0x01) & mask) {			
            ret = 1;
            break;
		}
        //usleep(1000);
	 usleep(50000);
	}

    return ret;
}

unsigned int test_read_bit(uint32_t cardtype)
{
    if(ci_io_read_u8(0x01) & 0x80)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

unsigned int test_write_bit(uint32_t cardtype)
{
    if(ci_io_read_u8(0x01) & 0x40)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

unsigned int read_card_io(uint32_t cardtype,unsigned char* buff)
{
    unsigned int i = 0;
    unsigned short Status_Reg = 0x00;
    unsigned char Data_Reg = 0x00;
    unsigned short Size_Reg[2] = {0x00};
    unsigned short bsize = 0x00;
    unsigned char flag;
   // if(0 == wait_for_io_bit(cardtype,0x80, 50))
    //{
    //	printf("read_card_io 01reg is no 0x80\n");
     //   return 0;
    //}
 while(1)
	{
		flag=ci_io_read_u8(0x01) ;
		//printf("wait 01reg=0x%x\n",flag);
		if((flag&0x80)==0x80)
	        break;
	}
 
    Status_Reg = ci_io_read_u8(0x01);
 // printf("read_card_io Status_Reg=0x%x\n",Status_Reg);
    if (Status_Reg & 0x80) {
        Size_Reg[0] = ci_io_read_u8(0x02);
        Size_Reg[1] = ci_io_read_u8(0x03);

        bsize = Size_Reg[0] | Size_Reg[1] << 8;
       printf("bsize=%d\n",bsize);
		//bsize=2;
        for (i=0; i<bsize; i++)
            buff[i] = (ci_io_read_u8(0x00)&0x00FF);
    }

    return bsize;
}
unsigned int read_card_io_2(uint32_t cardtype,unsigned char* buff)
{
    unsigned int i = 0;
    unsigned short Status_Reg = 0x00;
    unsigned char Data_Reg = 0x00;
    unsigned short Size_Reg[2] = {0x00};
    unsigned short bsize = 0x00;
    unsigned char flag;
    if(0 == wait_for_io_bit(cardtype,0x80, 50))
    {
    	printf("read_card_io 01reg is no 0x80\n");
        return 0;
    }
 
 
    Status_Reg = ci_io_read_u8(0x01);
 // printf("read_card_io Status_Reg=0x%x\n",Status_Reg);
    if (Status_Reg & 0x80) {
        Size_Reg[0] = ci_io_read_u8(0x02);
        Size_Reg[1] = ci_io_read_u8(0x03);

        bsize = Size_Reg[0] | Size_Reg[1] << 8;
      // printf("bsize=%d,",bsize); 
		//bsize=2;
        for (i=0; i<bsize; i++)
        {
            buff[i] = (ci_io_read_u8(0x00)&0x00FF);
	  //  printf("0x%x,", buff[i]);
        }
	//printf("\n");
    }

    return bsize;
}

unsigned short write_card_io(uint32_t cardtype,unsigned char* buff, 
                             unsigned short len)
{
    int i = 0;
    int wait_cnt = 0;
    unsigned char read_buff[0x100];

    ci_io_write_u8(0x01, 0x01);

    while(!(ci_io_read_u8(0x01)&0x40)){
        ci_io_write_u8(0x01, 0x01);
        usleep(1000);
		//usleep(10000);
        wait_cnt++;
        if(wait_cnt>10)
        {
            break;
        }
    }

    if(wait_cnt>10)
    {
    	printf("write_card_io timeout\n");
        return 0;
    }

    ci_io_write_u8(0x02, len&0xFF);
    ci_io_write_u8(0x03, len>>8);

    for (i=0; i<len; i++){
        ci_io_write_u8(0x00, buff[i]);
	ci_io_read_u8(0x01);
    }

    ci_io_write_u8(0x01, 0x00);
    return len;
}
