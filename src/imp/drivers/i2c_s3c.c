/*
 * linux/drivers/char/i2c_s3c.c
 *
 * IIC driver for the Samsung s3c2510A builtin IIC controller.
 * Character device interface.
 * I2C Read/Write in pulling mode.
 *
 * Copyright (C) 2002  Oleksandr G. Zhadan <oleks@arcturusnetworks.com>
 * Based on: drivers/i2c/i2c-dev.c
 *  Copyright (C) 1995-97 Simon G. Vogl
 *  Copyright (C) 1998-99 Frodo Looijaard <frodol@dds.nl>
 *
 */

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/smp_lock.h>

#if 0
#ifdef CONFIG_DEVFS_FS
#include <linux/devfs_fs_kernel.h>
#endif
#endif

/* If you want debugging uncomment */
/* #define DEBUG */

#include <linux/init.h>
#include <asm/uaccess.h>

#include <asm/arch/hardware.h>
#include "i2c_s3c.h"

static ssize_t transfer_slave0 (struct i2c_client *, size_t, loff_t *);
static ssize_t transfer_slave1 (struct i2c_client *, size_t, loff_t *);

static struct i2c_client i2c_slave0 = {
    name:	"AT24C64",
    page_size:	64,

#if 0	
#if defined(CONFIG_CPU_S3C4530)   
	speed:	100000,
#else
    speed:	40000,
#endif    
#endif

#if defined(CONFIG_CPU_S3C4530)   
	speed:	10000,
#else
    speed:	40000,
#endif    


    transfer:	transfer_slave0,
    addr:	0xa4,		/* ((Slave address | 0x50) << 1) */
    };

static struct i2c_client i2c_slave1 = {
    name:	"AT24C02",
    page_size:	8,

#if 0
#if defined(CONFIG_CPU_S3C4530)    
    speed:	100000,
#else
    speed:	400000,
#endif    
#endif

#if defined(CONFIG_CPU_S3C4530)    
    speed:	10000,
#else
    speed:	40000,
#endif    


    transfer:	transfer_slave1,
    addr:	0xa0,		/* ((Slave address | 0x50) << 1) */
    };


static struct i2c_client  *i2cdev_slave[I2CDEV_MAX_DEV] = { &i2c_slave0 };

#ifdef MODULE
int init_module(void);
int cleanup_module(void);
int i2cdev_cleanup(void);
#endif

static int	i2cdev_open	(struct inode *, struct file *);
static int	i2cdev_release	(struct inode *, struct file *);
static ssize_t	i2cdev_read	(struct file *, char *, size_t, loff_t *);
static ssize_t	i2cdev_write	(struct file *, const char *, size_t, loff_t *);
static loff_t	i2cdev_lseek	(struct file *file, loff_t offset, int origin);

static struct file_operations i2cdev_fops = {
    owner:	THIS_MODULE,
    open:	i2cdev_open,
    release:	i2cdev_release,
    read:	i2cdev_read,
    write:	i2cdev_write,
    llseek:	i2cdev_lseek,
    };

#if 0
#ifdef CONFIG_DEVFS_FS
static devfs_handle_t devfs_i2c[I2CDEV_MAX_DEV];
static devfs_handle_t devfs_handle = NULL;
#endif
#endif

static DECLARE_MUTEX (I2C_sem);

static ssize_t
transfer_slave0 (struct i2c_client *client, size_t count, loff_t *offset)
{
    unsigned int i, j;
    unsigned int Page_Number;
    unsigned int Curent_Page_Size;
    unsigned int Last_Page_Size=0;
    char *tmp = client->buf;

    Curent_Page_Size = client->page_size / 2;
    Page_Number      = count / Curent_Page_Size;
    Last_Page_Size   = count % Curent_Page_Size;

    for ( i = 0; i <= Page_Number; i++) {
	if  ( (i == Page_Number) && (Last_Page_Size != 0) )
	    Curent_Page_Size = Last_Page_Size;

	REG_WRITE ( IICCON, IICCON_RESET );
	SET_I2C_CLOCK();
    	Wait_NONBUSY();	REG_WRITE(IICCON,(IICCON_START|IICCON_BF));

	cli();
	printk("AT24C64_I2C_OPERATE_PAGE_client->flags=%d;%d\n",client->flags,*offset);
	if	( client->flags == I2C_READ_PAGE ) {
		printk("Read Offset is: %02x \n", *offset);
		for ( j = 0; j < 7; j++ ) {
    		    Wait_BF();
    		    switch ( j ) {
			case 0 : REG_WRITE(IICBUF, (client->addr|IICBUF_WRITE));	break;
			case 1 : REG_WRITE(IICBUF, ((*offset >> 8) & 0xff));		break;
			case 2 : REG_WRITE(IICBUF, (*offset & 0xff));			break;
			case 3 : REG_WRITE(IICCON, (IICCON_RESTART|IICCON_ACK|IICCON_BF));break;
			case 4 : REG_WRITE(IICCON, (IICCON_START|IICCON_ACK|IICCON_BF));break;
			case 5 : REG_WRITE(IICBUF, (client->addr|IICBUF_READ));		break;
    	case 6 : REG_WRITE(IICCON, (REG_READ(IICCON)&0xfc));		break;
			}
    		    }
		for ( j = 0; j < Curent_Page_Size; j++ ) {
		    Wait_BF();	*tmp++ = REG_READ(IICBUF);
		    
		    printk("[Curent_Page_Size %d ] %02x ", Curent_Page_Size, *(tmp-1));
		    	
		    }
		sti();

		REG_WRITE(IICCON,(REG_READ(IICCON)&0xf4));
    		Wait_BF();	REG_WRITE(IICCON,IICCON_STOP);
		}
	else if ( client->flags == I2C_WRITE_PAGE ) {
		printk("Write Offset is: %02x \n", *offset);

		for ( j = 0; j < 3; j++ ) {
    		    Wait_BF();
    		    switch ( j ) {
    			case 0 : REG_WRITE(IICBUF, (client->addr|IICBUF_WRITE));	break;
    			case 1 : REG_WRITE(IICBUF, ((*offset >> 8) & 0xff));		break;
    			case 2 : REG_WRITE(IICBUF, (*offset & 0xff));			break;
    			}
    		    }

		for ( j = 0; j < Curent_Page_Size; j++ ) {
    		    Wait_BF();    REG_WRITE(IICBUF, *tmp++);
		    }
		sti();

		for ( j = 0; j < (int)Wait_Cycle_ms(5); j++);
		REG_WRITE(IICCON,IICCON_STOP);
		}
	for ( j = 0; j < (int)Wait_Cycle_ms(5); j++);
	*offset += Curent_Page_Size;
	}
    return ( count );
}

static ssize_t
transfer_slave1 (struct i2c_client *client, size_t count, loff_t *offset)
{
    unsigned int i, j;
    unsigned int Page_Number;
    unsigned int Curent_Page_Size;
    unsigned int Last_Page_Size=0;
    char *tmp = client->buf;

    Curent_Page_Size = client->page_size / 2;
    Page_Number      = count / Curent_Page_Size;
    Last_Page_Size   = count % Curent_Page_Size;

    for ( i = 0; i <= Page_Number; i++) {
	if  ( (i == Page_Number) && (Last_Page_Size != 0) )
	    Curent_Page_Size = Last_Page_Size;

	REG_WRITE ( IICCON, IICCON_RESET );
	SET_I2C_CLOCK();
    	Wait_NONBUSY();	REG_WRITE(IICCON,(IICCON_START|IICCON_BF));
	printk("AT24C02_I2C_OPERATE_PAGE_client->flags=%d\n",client->flags);
	cli();
	if	( client->flags == I2C_READ_PAGE ) {
		printk("Slave1 reading: I2C_READ_PAGE_client->flags=%d\n",client->flags);

		for ( j = 0; j < 7; j++ ) {
    		    Wait_BF();
    		    switch ( j ) {
			case 0 : REG_WRITE(IICBUF, (client->addr|IICBUF_WRITE));	break;
			case 1 : REG_WRITE(IICBUF, (*offset & 0xff));			break;
			case 2 : REG_WRITE(IICCON, (IICCON_RESTART|IICCON_ACK|IICCON_BF));break;
			case 3 : REG_WRITE(IICCON, (IICCON_START|IICCON_ACK|IICCON_BF));break;
			case 4 : REG_WRITE(IICBUF, (client->addr|IICBUF_READ));		break;
    			case 5 : REG_WRITE(IICCON, (REG_READ(IICCON)&0xfc));		break;
			}
    		    }
		for ( j = 0; j < Curent_Page_Size; j++ ) {
		    Wait_BF();	*tmp++ = REG_READ(IICBUF);
		    }
		sti();

		REG_WRITE(IICCON,(REG_READ(IICCON)&0xf4));
    		Wait_BF();	REG_WRITE(IICCON,IICCON_STOP);
		}
	else if ( client->flags == I2C_WRITE_PAGE ) {
		printk("I2C_WRITE_PAGE_client->flags=%d\n",client->flags);

		for ( j = 0; j < 3; j++ ) {
    		    Wait_BF();
    		    switch ( j ) {
    			case 0 : REG_WRITE(IICBUF, (client->addr|IICBUF_WRITE));	break;
    			case 1 : REG_WRITE(IICBUF, (*offset & 0xff));			break;
    			}
    		    }

		for ( j = 0; j < Curent_Page_Size; j++ ) {
    		    Wait_BF();    REG_WRITE(IICBUF, *tmp++);
		    }
		sti();

		for ( j = 0; j < (int)Wait_Cycle_ms(5); j++);
		REG_WRITE(IICCON,IICCON_STOP);
		}
	for ( j = 0; j < (int)Wait_Cycle_ms(5); j++);
	*offset += Curent_Page_Size;
	}
    return ( count );
}

static ssize_t
i2cdev_read (struct file *file, char *buf, size_t count, loff_t *offset)
{
    struct i2c_client *client;
	int i;

    if  ( count <= 0 )
	return 0;

    down (&I2C_sem);

	printk("i2cdev_read_i=%d\n",i);

    client     = (struct i2c_client *)file->private_data;
    client->buf = kmalloc(count, GFP_KERNEL);
    if  ( client->buf == NULL )
	return -ENOMEM;

	printk("i2cdev_read_i=%d\n",i);


    client->flags |= I2C_READ_PAGE;
    client->transfer(client, count, offset);
    client->flags &= (~I2C_READ_PAGE);

    /* copy temporary kernel buffer to user space. */
    cli();
    copy_to_user(buf, client->buf, count);
    sti();

    kfree(client->buf);

    up (&I2C_sem);
    return count;
}

static ssize_t
i2cdev_write (struct file *file, const char *buf, size_t count, loff_t *offset)
{
	int i;
    struct i2c_client *client;

    if  ( count <= 0 )
	return 0;

    down (&I2C_sem);

    client = (struct i2c_client *)file->private_data;
    client->buf = kmalloc(count,GFP_KERNEL);
    if  (client->buf == NULL)
	{
		printk("i2cdev_write_ENOMEM=%d\n",ENOMEM);
		return -ENOMEM;
	}

    /* copy user space data to kernel space. */
    cli();
    copy_from_user(client->buf, buf, count);
    sti();
	
	printk("i2cdev_write_i=%d\n",i);
	for(i=0;i<100;i++)
	{
		printk("%02X ",buf[i]);
		//buf[i]=i;
	}
	printk("\n");

    client->flags |= I2C_WRITE_PAGE;
    client->transfer(client, count, offset);
    client->flags &= (~I2C_WRITE_PAGE);

    kfree(client->buf);

    up (&I2C_sem);

	printk("i2cdev_write_count=%d\n",count);
    return count;
}

loff_t
i2cdev_lseek (struct file *file, loff_t offset, int origin)
{
    file->f_pos = offset;
    return offset;
}

int i2cdev_open (struct inode *inode, struct file *file)
{
    unsigned int minor = MINOR(inode->i_rdev);
    struct i2c_client *client;

    if	( (minor >= I2CDEV_MAX_DEV)) {
#ifdef DEBUG
	printk("i2c_s3c.o: Trying to open unattached adapter i2c-%d\n", minor);
#endif
	return -ENODEV;
	}

    if	(! (client = kmalloc(sizeof(struct i2c_client),GFP_KERNEL)))
	return -ENOMEM;

    memcpy(client, i2cdev_slave[minor], sizeof(struct i2c_client));
    file->private_data = client;

#ifdef DEBUG
    printk("i2c_s3c.o: opened i2c-%d\n",minor);
#endif
    return 0;
}

static int
i2cdev_release (struct inode *inode, struct file *file)
{
    kfree(file->private_data);
    file->private_data=NULL;
#ifdef DEBUG
    printk("i2c_s3c.o: Closed: i2c-%d\n", minor);
#endif
    return 0;
}

static int 
__init i2cdev_init(void)
{
    int i;
    printk("Simple I2C bus character device driver. (c) 2002 <Oleks@ArcturusNetworks.com>\n");
#if 0
#ifdef CONFIG_DEVFS_FS
    if	(devfs_register_chrdev(I2C_MAJOR, "i2c", &i2cdev_fops)) {
#else

#endif
#endif
    if	(register_chrdev(I2C_MAJOR,"AT24C64",&i2cdev_fops)) {
	printk("i2c_s3c.o: unable to get major %d for i2c bus\n", I2C_MAJOR);
	return -EIO;
	}
#if 0
#ifdef CONFIG_DEVFS_FS
    devfs_handle = devfs_mk_dir(NULL, "i2c", NULL);
#endif
#endif

    printk("i2c master: at 0x%08x is a builtin Samsung ",IICCON);
#if defined(CONFIG_CPU_S3C4530)
    printk("s3c4530");
#endif
#if defined(CONFIG_CPU_S3C2500)
    printk("s3c2500");
#endif
#if defined(CONFIG_CPU_S3C2510)
    printk("s3c2510"); /* choish */
#endif

	printk(" i2c bus controller\n");

    for ( i = 0; i < I2CDEV_MAX_DEV; i++ )
	printk("i2c-%d: at %02x is a slave I2C eeprom (%s)\n", i,
		((i2cdev_slave[i]->addr & 0x0f)>>1), i2cdev_slave[i]->name);

    return 0;
}

#ifdef MODULE
MODULE_AUTHOR("Oleksandr G. Zhadan <oleks@arcturusnetworks.com>");
MODULE_DESCRIPTION("i2c bus character device driver");

int
init_module(void)
{
    return i2cdev_init();
}

int
cleanup_module(void)
{
    return i2cdev_cleanup();
}

int 
i2cdev_cleanup(void)
{
    int res;

#if 0
#ifdef CONFIG_DEVFS_FS
    devfs_unregister(devfs_handle);
    if  ((res = devfs_unregister_chrdev(I2C_MAJOR, "i2c"))) {
#else
#endif

#endif
    if  ((res = unregister_chrdev(I2C_MAJOR,"AT24C64"))) {
        printk("i2c_s3c.o: unable to release major %d for i2c bus\n", I2C_MAJOR);
        return res;
	}
    return 0;
}

#else
int
__init i2c_s3c_init_all(void)
{
    i2cdev_init();
    return 0;
}
#endif
