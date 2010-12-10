/* 
 * linux/drivers/char/i2c_s3c.h
 * 
 * IIC driver for the Samsung s3c2500A & s3c2510X01 builtin IIC controller.
 * Character device interface.
 * Copyright (C) 2002  Oleksandr Zhadan <oleks@arcturusnetworks.com>
 * 
 */

#define I2C_MAJOR	63		/* Device major number		*/

struct i2c_client {
	char name[32];
	unsigned int flags;		/* Curent flag			*/
	void *buf;			/* for the clients		*/
	int page_size;			/* eeprom page size		*/
	int speed;			/* 100000 or 400000		*/
	int usage_count;		/* How many accesses currently  */
	ssize_t (*transfer)(struct i2c_client *, size_t count, loff_t *offset);
	int delay;
	unsigned char addr;		/* chip address - NOTE: 7bit 	*/
	};
					/* Flags */
#define I2C_READ_PAGE	0x01
#define I2C_WRITE_PAGE	0x02


#define defCLK (133*1000000)

#define	Wait_Cycle_ms(x) (x*5000-1)	/* IIC write cycle */

//#define SET_I2C_CLOCK() REG_WRITE(IICPS,((int)((((float)defCLK/(float)client->speed)-3.0)/16.0)-1))
//#define SET_I2C_CLOCK() REG_WRITE(IICPS,((int)((((float)CONFIG_ARM_CLK/(float)client->speed)-3.0)/16.0)-1))

#define SET_I2C_CLOCK() REG_WRITE(IICPS,((int)((((int)defCLK/(int)client->speed)-3)/16)-1))

#if defined(CONFIG_BOARD_SMDK2500)
#   define I2CDEV_MAX_DEV 1
#   define I2CDEV_DELAY   0xffff
#endif

#if defined(CONFIG_BOARD_SMDK2510) || defined(CONFIG_BOARD_DAREDEVIL)
#   define I2CDEV_MAX_DEV 1
#   define I2CDEV_DELAY   0xffff
#endif

#if defined(CONFIG_BOARD_EVS3C4530HEI)
#   define I2CDEV_MAX_DEV 1
#   define I2CDEV_DELAY   0x7fff
#endif

#if defined(CONFIG_BOARD_EVS3C2500RGP)
#   define I2CDEV_MAX_DEV 1
#   define I2CDEV_DELAY   0xffff
#endif

#define Wait_BF() 	client->delay = I2CDEV_DELAY; \
			while(!(REG_READ(IICCON)&IICCON_BF)&&client->delay-- ); \
			asm("nop;nop;nop")
#define Wait_NONBUSY() 	while( REG_READ(IICCON)&IICCON_BUSY)
  
