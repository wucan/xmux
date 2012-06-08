#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>

#include "i2cdev.h"
#include "up_gpio.h"


#define TUNER_DEV			"tuner"
#define TUNER_DEV1			"/dev/tuner"
#define TUNER_W_ADDR		0xD0
#define TUNER_R_ADDR		0xD1
#define TUNER_6110_W_ADDR            0xC0
#define TUNER_6110_R_ADDR            0xC1
#define RESET_GPIO			50
#define POL_GPIO			51
#define LNB_GPIO			52
#define F22K_GPIO			53
#define TUNER_SDA 49
#define TUNER_SCL 44
typedef enum
	{
		I2C_OK = 0,		/* I2C transmission OK */
		I2C_NOSDA,		/* SDA line is stucked */
		I2C_NOSCL,		/* SCL line is stucked */
		I2C_BUSERROR,	/* SDA and SCL are stucked */
		I2C_NOACK		/* SDA and SCL lines are OK but the device does not acknowledge */
	} I2C_RESULT;
	
	typedef enum
	{
		I2C_READ,		/* Read from slave */
		I2C_WRITE,		/* Write to slave */
		I2C_WRITENOSTOP /* Write to slave without stop */   
	} I2C_MODE;
void IIC_delay(void)
{
    unsigned int  tmp;
    for(tmp = 0;tmp < 75;tmp++)//72//36//20//5
    ;//  NOP();	
}
void  Start(void)
{
	gpio_set_output_mode(TUNER_SDA);
        gpio_set_output_mode(TUNER_SCL);
        gpio_output_high(TUNER_SDA);
        gpio_output_high(TUNER_SCL);
        IIC_delay();
        gpio_output_low(TUNER_SDA);
        IIC_delay();
        gpio_output_low(TUNER_SCL);
        IIC_delay();
}

//终止I2C总线，当SCL为高电平时使SDA产生一个正跳变
void  Stop(void)
{
	gpio_output_low(TUNER_SDA);
	gpio_output_low(TUNER_SCL);
        IIC_delay();
	gpio_output_high(TUNER_SCL);
	IIC_delay();
	gpio_output_high(TUNER_SDA);
	IIC_delay();
}


void  IIC_ack(void)
{
	unsigned char tmp,m;
	gpio_set_input_mode(TUNER_SDA);
	gpio_output_high(TUNER_SCL);
	for(m=0;m<10;m++)	
	 {
	  IIC_delay();
	  tmp = gpio_get_input_value(TUNER_SDA);
	  if(tmp == 0)
	  break;
	 }
         gpio_output_low(TUNER_SCL);
         gpio_set_output_mode(TUNER_SDA);

}


void write_byte(unsigned char data)
{  
     unsigned char m,tmp;
     gpio_set_output_mode(TUNER_SDA);
     for(m = 0; m < 8; m++)
     {
	   if(data & 0x80)
		 gpio_output_high(TUNER_SDA);
	   else  
		 gpio_output_low(TUNER_SDA);
	   IIC_delay();
	   gpio_output_high(TUNER_SCL);
	   IIC_delay();
	   data = data<<1;
	   gpio_output_low(TUNER_SCL);
     }
     IIC_delay();
     IIC_ack();
     IIC_delay();
}

//读取一字节数据程序

unsigned char read_byte(void)
{
      unsigned char m;
      unsigned char tmp1 = 0;
      unsigned char tmp2 = 0;
      gpio_set_input_mode(TUNER_SDA);
      for(m = 0; m < 8; m++)
      {
	 gpio_output_low(TUNER_SCL);
	 IIC_delay();
	 gpio_output_high(TUNER_SCL);
	 IIC_delay();
	 tmp1 = gpio_get_input_value(TUNER_SDA);
         if(1 == tmp1)
            tmp2 = ((tmp2 << 1) | 1);
         else
            tmp2 = ((tmp2 << 1) & 0xfe);
	gpio_output_low(TUNER_SCL);
      }
	
	IIC_delay();
	IIC_ack();
	return(tmp2);
}
unsigned char Write_Stv0903(unsigned char chipaddr,unsigned int addr,unsigned char data)  
{    
    gpio_set_output_mode(TUNER_SDA);
    gpio_set_output_mode(TUNER_SCL);
    gpio_output_high(TUNER_SDA);
    gpio_output_high(TUNER_SCL);
    Start();
    write_byte(chipaddr);//写芯片地址
    write_byte(addr/256);
    write_byte(addr%256);
    write_byte(data);
    Stop();
    return(1);
}


unsigned char Read_Stv0903(unsigned char chipaddr,unsigned int addr)
{     
    unsigned char temp;
    gpio_set_output_mode(TUNER_SDA);
    gpio_set_output_mode(TUNER_SCL);
    gpio_output_high(TUNER_SDA);
    gpio_output_high(TUNER_SCL);
    Start();
    write_byte(chipaddr);//写芯片地址
    write_byte(addr/256);
    write_byte(addr%256);
    Stop();
  
    Start();
    write_byte(chipaddr+1);//读芯片地址
    temp = read_byte(); 	  
    Stop();  
    return(temp);		 
}
unsigned char Write_6110(unsigned char chipaddr,unsigned char regaddr,unsigned char data)
{
    unsigned char tmp;
    gpio_set_output_mode(TUNER_SDA);
    gpio_set_output_mode(TUNER_SCL);
    gpio_output_high(TUNER_SDA);
    gpio_output_high(TUNER_SCL);    
    Start();
    write_byte(chipaddr);//写芯片地址
    write_byte(regaddr);
    write_byte(data);
    Stop();
    return(1);
}

unsigned char Read_6110(unsigned char chipaddr,unsigned char regaddr )
{
    unsigned char temp;
    gpio_set_output_mode(TUNER_SDA);
    gpio_set_output_mode(TUNER_SCL);
    gpio_output_high(TUNER_SDA);
    gpio_output_high(TUNER_SCL);
	  	
    Start();
    write_byte(chipaddr);//写芯片地址
    write_byte(regaddr);
    Stop();
  
    Start();
    write_byte(chipaddr+1);//读芯片地址 
    temp = read_byte(); 	  
    Stop();
    return(temp);		
}

void tuner_device_reset()
{
	gpio_set_output_mode(RESET_GPIO);
	gpio_output_low(RESET_GPIO);
	usleep(100000);
	gpio_output_high(RESET_GPIO);
	usleep(100000);
}
  
void tuner_device_pol(unsigned char mode)
{
        gpio_set_output_mode(POL_GPIO);
        if(mode==1)
        {
	    printf("POL:V\n");
            gpio_output_low(POL_GPIO);
        }
        else
        {
	    printf("POL:H\n");
            gpio_output_high(POL_GPIO);
        }
}
void tuner_device_lnb(unsigned char mode)
{
        gpio_set_output_mode(LNB_GPIO);
        if(mode==1)
        {
            printf("LNB:1\n");
            gpio_output_high(LNB_GPIO);
        }
        else
        {
	    printf("LNB:0\n");
            gpio_output_low(LNB_GPIO);
        }

}

void tuner_device_f22k(unsigned char mode)
{
        gpio_set_output_mode(F22K_GPIO);
        if(mode==1)
        gpio_output_high(F22K_GPIO);
        else
        gpio_output_low(F22K_GPIO);

}

int tuner_device_open()
{
	tuner_device_reset();

	return 0;
}

void tuner_device_close()
{
}

