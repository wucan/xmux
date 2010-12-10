#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifdef CONFIG_SMP
#define __SMP__
#endif

#include <linux/config.h>
#include <linux/version.h> 
#include <linux/module.h>
#include <linux/slab.h>

#if defined(CONFIG_MODVERSIONS) && !defined(MODVERSIONS)
#define MODVERSIONS	// force it on 
#endif 

/* include version definitions for kernel symbols and our symbol, unless
 * we are generating checksums (__GENKSYMS__) defined */

#if defined(MODVERSIONS) && !defined(__GENKSYMS__)
#include <linux/modversions.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <linux/autoconf.h>	
#endif

#ifdef MODVERSIONS	
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <config/modversions.h>
#else
#include <linux/modversions.h>
#endif
#endif

#include <linux/kernel.h>  // printk()
#include <linux/fs.h>         // struct file_operations, register_chrdev()
#include <asm/uaccess.h> // copy_to_use() and copy_from_use()

#include <linux/init.h>       // __init  & __exit
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/unistd.h>
#include <linux/spinlock.h>
#include <linux/vmalloc.h>

#include <linux/ioctl.h>
#include <asm/arch/hardware.h>
#include <linux/delay.h>

/*
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/arch/hardware.h>
#include <linux/ioctl.h>
*/

#include "uvdebug_def.h"
#include "uvdrivers_def.h"
#include "host_fpga.h"

#ifdef MODULE
EXPORT_NO_SYMBOLS;
MODULE_AUTHOR("grepo@163.com");
MODULE_DESCRIPTION("Driver For HOST MUX FPGA");
#endif

//#define DRV_PRT

#ifdef DRV_PRT
#ifndef drv_prt
#define drv_prt printk
#endif
#else
#ifndef drv_prt
#define drv_prt(...)
#endif

#endif

static const char hfpga_name[] = "XC3S"; // Xilinx Spartan-3
static const unsigned int hfpga_ram_size = 256;

static unsigned short cur_40ms_offset = HFPGA_MAP_MEM_ADDR_SI_40MS_BASE;
static unsigned short cur_2s_offset = HFPGA_MAP_MEM_ADDR_SI_2S_BASE;

static DECLARE_MUTEX(hfpga_ioctl_sem);

static int hfpga_open(struct inode * inode, struct file * file);
static int hfpga_ioctl(struct inode * inode, struct file * filp,unsigned int cmd, unsigned long arg);  
static int hfpga_close(struct inode * inode, struct file * file);

static ssize_t hfpga_read(struct file * file, char * buf, size_t count, loff_t *off);
static ssize_t hfpga_write(struct file * file, const char * buf, size_t count,loff_t *off);
static loff_t hfpga_lseek(struct file * file, loff_t offset, int orig);

struct file_operations hfpga_fops =
{
	.ioctl   = hfpga_ioctl,
	.open    = hfpga_open,
	//.read    = hfpga_read,
	//.write   = hfpga_write,
	//.llseek   = hfpga_lseek,
	.release = hfpga_close
};

MODULE_LICENSE("GPL");

//int __init hfpga_init(void)
static int __init init_module()
{
	if (register_chrdev(UV_HFPGA_MAJOR_NR, hfpga_name, &hfpga_fops))
	{
		drv_prt(KERN_ERR "[UV-DRIVER] register_chrdev fails \n");
		return -1;
	}
	drv_prt(KERN_INFO "[UV-DRIVER] start  HOST FPGA \n");
	return 0;
}

//void __exit hfpga_exit(void)
void __exit cleanup_module(void)
{
	//
	unregister_chrdev(UV_HFPGA_MAJOR_NR, hfpga_name);
	drv_prt(KERN_INFO "[UV-DRIVER] remove HOST FPGA \n");
	return;
}

static void UV_MEM_FREE(void *p)
{
	if(p)
	{
		kfree(p);
		p = 0;
	}	
}

static int hfpga_open(struct inode * inode, struct file * file)
{
	//	drv_prt(KERN_INFO "HOST FPGA hfpga_open(): Open device major number # %d #!\n", MAJOR(inode->i_rdev));
	//	drv_prt(KERN_INFO "HOST FPGA hfpga_open(): Open device minor number # %d #!\n", MINOR(inode->i_rdev));

	/* the version is too low, imajor and iminor are not supported
	   if(imajor(inode) != UV_HFPGA_MAJOR_NR)
	   {
	   drv_prt(KERN_ERR "HOST FPGA hfpga_open(): Invalid device minor number # %d #!\n", MAJOR(inode->i_rdev));
	   return -ENXIO;
	   }
	   if(iminor(inode) != UV_HFPGA_MINOR_NR)
	   {
	   drv_prt(KERN_ERR "HOST FPGA hfpga_open(): Invalid device minor number # %d #!\n", MINOR(inode->i_rdev));
	   return -ENXIO;
	   }
	   */

	file->f_op = &hfpga_fops;

	MOD_INC_USE_COUNT; 

	return 0;
}

void read_hfpga_register(unsigned long addr, unsigned short *pdata)
{
	*pdata = READ_REGISTER_USHORT((unsigned short * )addr);	   
}

void write_hfpga_register(unsigned long addr, unsigned short data)
{
	WRITE_REGISTER_USHORT((unsigned short * )addr, data);
}

void read_hfpga_ts_pack(ACCESS_HFPGA_TS_PACK *pdata)
{

}

void write_hfpga_ts_pack(ACCESS_HFPGA_TS_PACK *pdata)
{

}

void read_hfpga_all_pids(ACCESS_HFPGA_ALL_PIDS *pdata)
{

}

void read_hfpga_pid_map(ACCESS_HFPGA_PID_MAP *pdata)
{
	unsigned short offset = HFPGA_MAP_MEM_ADDR_BASE;
	unsigned char cha = pdata->cha;
	int i = 0, j = 0;

	while(cha > 0)
	{
		if(0x01 & cha)
		{
			for(j = 0; j < 32; j++)
			{
				write_hfpga_register(HFPGA_REG_ADDR_MAP_MEM, offset+j);

				read_hfpga_register(HFPGA_REG_ADDR_MEM_DATA_L16, &(pdata->pid_map[i*32+j].out_pid));
				read_hfpga_register(HFPGA_REG_ADDR_MEM_DATA_H16, &(pdata->pid_map[i*32+j].in_pid));
			} 	
		}
		cha >>= 1;
		offset += 32; // 
		i++;
	}
}

void write_hfpga_pid_map(ACCESS_HFPGA_PID_MAP *pdata)
{
	unsigned short offset = HFPGA_MAP_MEM_ADDR_BASE;
	unsigned char cha = pdata->cha;
	int i = 0, j = 0;

	while(cha > 0)
	{
		if(0x01 & cha)
		{
			for(j = 0; j < 32; j++)
			{
				if(pdata->pid_map[i*32+j].in_pid != UV_HFPGA_INVALID_PID_MAP && pdata->pid_map[i*32+j].out_pid != UV_HFPGA_INVALID_PID_MAP)
				{
					write_hfpga_register(HFPGA_REG_ADDR_MAP_MEM_WCON, 0x0000); // low level signal

					write_hfpga_register(HFPGA_REG_ADDR_MAP_MEM, offset+j);

					write_hfpga_register(HFPGA_REG_ADDR_MEM_DATA_L16, pdata->pid_map[i*32+j].out_pid);
					write_hfpga_register(HFPGA_REG_ADDR_MEM_DATA_H16, pdata->pid_map[i*32+j].in_pid);

					write_hfpga_register(HFPGA_REG_ADDR_MAP_MEM_WCON, 0x55AA); // high level signal
				}
			} 
		}
		offset += 32; // 
		i++;
		cha >>= 1;
	}
}

/*
   struct timer_list str_poll_timer;

   init_timer(&str_poll_timer);
   str_poll_timer.data = (unsigned long)1;
   str_poll_timer.function = str_poll_handler;
   str_poll_timer.expires = jiffies + 3*HZ; // HZ is interrupt frequency, each second, jiffies will increase one HZ. 
   add_timer(&str_poll_timer);

   void str_poll_handler(unsigned long data)
   {
   unsigned short state;
   read_hfpga_register(HFPGA_REG_ADDR_FIFO_STATUS, &state);
   str_poll_timer.expires = jiffies + 3*HZ; 
   add_timer(&str_poll_timer);
   }

*/

// Read data of length % len % from addr(cha: 0-7) of fifo
unsigned int read_hfpga_str_data(unsigned char *pdata, unsigned int addr, unsigned int len,int nbgn)
{
	unsigned short data;
	unsigned int pos = 0;
	unsigned int delay = UV_HFPGA_TS_FIFO_RD_MAX_DELAY;
	int i;

	if(nbgn)
	{
		write_hfpga_register(HFPGA_REG_ADDR_RST_FIFO, 0x0000);
		write_hfpga_register(HFPGA_REG_ADDR_RST_FIFO, 0x55AA);
		write_hfpga_register(HFPGA_REG_ADDR_RST_FIFO, 0x0000);

		write_hfpga_register(HFPGA_REG_ADDR_CHA_SEL, addr);
	}



	while(len--){
		for(; delay>0; delay--)
		{
			read_hfpga_register(HFPGA_REG_ADDR_FIFO_STATUS, &data);

			//drv_prt(KERN_INFO "_%02x_", data);

			if(data>>15)// whether MSB is 1 ?
			{
				delay = UV_HFPGA_TS_FIFO_RD_MAX_DELAY;
				break;	
			}
		}
		if(!delay)
		{
			drv_prt(KERN_INFO "HFPGA hfpga_ioctl()-> read_hfpga_str_data(hhhh) time out! \n");
			return 0;
			break; // time out
		}

		write_hfpga_register(HFPGA_REG_ADDR_FIFO_RCON, 0x0000);
		write_hfpga_register(HFPGA_REG_ADDR_FIFO_RCON, 0x55AA);
		write_hfpga_register(HFPGA_REG_ADDR_FIFO_RCON, 0x0000);

		read_hfpga_register(HFPGA_REG_ADDR_FIFO_STATUS, &data);
		/*
		printk("0x%04X;",data);

		i++;
		if(i>=16)
		{
			printk("\n");
			i=0;
		}
		*/
		


		/*	
		// data is 16bits, for little endian
		// LSB 8bits are x th
		// MSB 8bits are x+1 th 
		pdata[pos++] = 0xFF&data;
		if(pos<len) // for odd bytes
		{
		pdata[pos++] = data>>8;
		len--;
		}
		*/
		// make sure the first byte in pdata is 0x47
		//
#if 1
		if((!pos) && (!((data>>8)&0x01)))
		//if((!pos) && ( (data &0x7FFF) ==0x147)  )  //0x147
		{
			len++;
			continue;// 0x47
		}
		else
#endif
		{
			pdata[pos++] = 0xFF&data;

			if(!(data&0x8000))
			{
				//delay = UV_HFPGA_TS_FIFO_RD_MAX_DELAY;
			}

		}

		//else delay = 0;

	}

	return pos; 
}

// the 500ms 2s mode flag is set in "addr" param
void write_hfpga_str_data(unsigned char *pdata, unsigned int addr, unsigned int len)
{
	int i;
	unsigned char mode = addr;
	unsigned short pos = 0;  

	if(mode == 0) // 400ms
	{
		pos = cur_40ms_offset;
	}
	else // 2s
	{
		pos = cur_2s_offset;	
	}
	for(i = 0; i < len; i++, pos++)
	{
		write_hfpga_register(HFPGA_REG_ADDR_MAP_MEM_WCON, 0x0000);   				//  low
		write_hfpga_register(HFPGA_REG_ADDR_MAP_MEM, pos+HFPGA_MAP_MEM_ADDR_SI_PHY_BASE);       //  addr
		write_hfpga_register(HFPGA_REG_ADDR_MEM_DATA_L16, pdata[i]); 				//  data
		write_hfpga_register(HFPGA_REG_ADDR_MAP_MEM_WCON, 0x55AA);   				//  high
	}
	write_hfpga_register(HFPGA_REG_ADDR_MAP_MEM_WCON, 0x0000);     					//  low

	if(mode == 0)
	{
		cur_40ms_offset = pos;
	}
	else
	{
		cur_2s_offset = pos;
	}
}

static void outputhex_si(unsigned short addr40ms, unsigned short len40ms, unsigned short addr2s, unsigned short len2s)
{
	int i, col;
	int width = 16;
	unsigned short val = 0;

	drv_prt(KERN_INFO "[HOST FPGA] Print SI- 40ms Length is: %d \n", len40ms);

	for(i = 0; i < len40ms;)
	{
		for(col = 0; col < width && i < len40ms; col++, i++)
		{
			write_hfpga_register(HFPGA_REG_ADDR_MAP_MEM, HFPGA_MAP_MEM_ADDR_SI_PHY_BASE+addr40ms+i);
			read_hfpga_register(HFPGA_REG_ADDR_MEM_DATA_L16, &val);

			drv_prt("%02x", 0xFF&val);
		}
		drv_prt("\n");
	}

	drv_prt(KERN_INFO "\n[HOST FPGA] Print SI- 2s Length is: %d \n", len2s);

	for(i = 0; i < len2s; )
	{
		for(col = 0; col < width && i < len2s; col++, i++)
		{
			write_hfpga_register(HFPGA_REG_ADDR_MAP_MEM, HFPGA_MAP_MEM_ADDR_SI_PHY_BASE+addr2s+i);
			read_hfpga_register(HFPGA_REG_ADDR_MEM_DATA_L16, &val);

			drv_prt("%02x", 0xFF&val);
		}
		drv_prt("\n");
	}
}


#if 0
static int hfpga_ioctl(struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg)
{
	int retval = (-ENOTTY);
	//unsigned char *pdata = NULL;
	unsigned int nread;

	//up(&hfpga_ioctl_sem);

	ACCESS_HFPGA_REGS	hfpga_regs;
#if 1
	if(UV_HFPGA_MAGIC != _IOC_TYPE(cmd)) 
	{
		drv_prt(KERN_ERR "HFPGA hfpga_ioctl(): IOC_TYPE(%x)!=MAGIC(%x)\n", _IOC_TYPE(cmd), UV_HFPGA_MAGIC);
		return (-EFAULT);
	}
	// check key number(nr)
	if(UV_KEY_IOCTL_MAX_KEY <= _IOC_NR(cmd)) {
		drv_prt(KERN_ERR "HFPGA hfpga_ioctl(): Failure: IOC_NR(%02x) invalid\n", _IOC_NR(cmd));
		return (-EFAULT);
	}
#endif
	//drv_prt(KERN_INFO "HFPGA hfpga_ioctl() CMD \n");

	switch(cmd)
	{	
		// -------------------------------------------------------------
		// Read and Write Registers
		case UV_HFPGA_IOCTL_CMD_READ_REGS: 
			drv_prt("UV_HFPGA_IOCTL_CMD_READ_REGS_printk\n");
			retval = copy_from_user( &hfpga_regs, (void*)arg, sizeof(ACCESS_HFPGA_REGS));
			if (0 != retval) {
				drv_prt(KERN_ERR "HFPGA hfpga_ioctl()->copy_from_user() failed\n");
				return (-EFAULT);
			}
			// Read data from the address
			if(hfpga_regs.reg < 0x840000FF)
			{
				read_hfpga_register(hfpga_regs.reg, &hfpga_regs.data);
				copy_to_user((PACCESS_HFPGA_REGS)arg, &hfpga_regs, sizeof(ACCESS_HFPGA_REGS));
				return 0;
			}
			else
			{
				drv_prt(KERN_ERR "HFPGA hfpga_ioctl(): Failure: Invalid address!\n");
				return (-EINVAL);
			}
			break;


		default:
			drv_prt(KERN_WARNING "HFPGA hfpga_ioctl(): Failure: Invalid commands!\n");
			return (-EFAULT);
			break;
	}
	return (0);

}
#endif


#if 1
static int hfpga_ioctl(struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg)
{
	//int retval = (-ENOTTY);
	//unsigned char *pdata = NULL;
	//unsigned int nread;
       unsigned short noff;

	//down(&hfpga_ioctl_sem);

	//drv_prt(KERN_INFO "HFPGA hfpga_ioctl() Enter... \n");


	//up(&hfpga_ioctl_sem);
      
	// check type
	if(UV_HFPGA_MAGIC != _IOC_TYPE(cmd)) 
	{
		drv_prt(KERN_ERR "HFPGA hfpga_ioctl(): IOC_TYPE(%x)!=MAGIC(%x)\n", _IOC_TYPE(cmd), UV_HFPGA_MAGIC);
		return (-EFAULT);	
	}

	// check key number(nr)
	if(UV_KEY_IOCTL_MAX_KEY <= _IOC_NR(cmd)) {
		drv_prt(KERN_ERR "HFPGA hfpga_ioctl(): Failure: IOC_NR(%02x) invalid\n", _IOC_NR(cmd));
		return (-EFAULT);	
	}

	//drv_prt(KERN_INFO "HFPGA hfpga_ioctl() CMD \n");

	switch(cmd)
	{	
		// -------------------------------------------------------------
		// Read and Write Registers
		case UV_HFPGA_IOCTL_CMD_READ_REGS: 
			{
				int retval = (-ENOTTY);
				ACCESS_HFPGA_REGS* pfpga=(ACCESS_HFPGA_REGS*)kmalloc(sizeof(ACCESS_HFPGA_REGS), GFP_KERNEL);
				if(pfpga==NULL) return (-EFAULT);
				retval = copy_from_user( pfpga, (PACCESS_HFPGA_REGS)arg, sizeof(ACCESS_HFPGA_REGS));
				if (0 != retval) 
				{
					drv_prt(KERN_ERR "HFPGA hfpga_ioctl()->copy_from_user() failed\n");
					retval=(-EFAULT);
				}
				else
				{
					// Read data from the address
					if(pfpga->reg < 0x840000FF)
					{
						read_hfpga_register(pfpga->reg, &pfpga->data);
						copy_to_user((PACCESS_HFPGA_REGS)arg, pfpga, sizeof(ACCESS_HFPGA_REGS));
						retval=0;
					}
					else
					{
						drv_prt(KERN_ERR "HFPGA hfpga_ioctl(): Failure: Invalid address!\n");
						retval= (-EINVAL);
					}
				}
				kfree(pfpga);
				return retval;
			}
			break;

		case UV_HFPGA_IOCTL_CMD_WRITE_REGS:
			{	
				int retval = (-ENOTTY);
				ACCESS_HFPGA_REGS* pfpga=(ACCESS_HFPGA_REGS*)kmalloc(sizeof(ACCESS_HFPGA_REGS), GFP_KERNEL);
				if(pfpga==NULL) return  (-EFAULT);
				retval = copy_from_user( pfpga, (PACCESS_HFPGA_REGS)arg, sizeof(ACCESS_HFPGA_REGS));
				if (0 != retval) {
					drv_prt(KERN_ERR "HFPGA hfpga_ioctl()->copy_from_user() failed\n");
					retval= (-EFAULT);
				}
				// Write data to the address
				if(pfpga->reg < 0x840000FF)
				{
					write_hfpga_register(pfpga->reg, pfpga->data);
					retval = 0;
				}
				else
				{
					drv_prt(KERN_ERR "HFPGA hfpga_ioctl(): Failure: Invalid address!\n");
					retval = (-EINVAL);
				}
				copy_to_user((PACCESS_HFPGA_REGS)arg, pfpga, sizeof(ACCESS_HFPGA_REGS));
				kfree(pfpga);
				return retval;
			}
			break;		

			// -------------------------------------------------------------
			// Read and Write TS Packets
		case UV_HFPGA_IOCTL_CMD_READ_TS_PACK: // Read TS Packets
			{
				int retval = (-ENOTTY);
				ACCESS_HFPGA_TS_PACK* pfpga=(ACCESS_HFPGA_TS_PACK*)kmalloc(sizeof(ACCESS_HFPGA_TS_PACK), GFP_KERNEL);
				if(pfpga==NULL) return  (-EFAULT);

				retval = copy_from_user( pfpga, (PACCESS_HFPGA_TS_PACK)arg, sizeof(ACCESS_HFPGA_TS_PACK));
				if (0 != retval) {
					drv_prt(KERN_ERR "HFPGA hfpga_ioctl()->copy_from_user() failed\n");
					retval = (-EFAULT);
				}
				else
				{
					read_hfpga_ts_pack(pfpga);
					copy_to_user((PACCESS_HFPGA_TS_PACK)arg, pfpga, sizeof(ACCESS_HFPGA_TS_PACK));
					retval = 0;
				}
				kfree(pfpga);
				return retval;
			}
			break;

		case UV_HFPGA_IOCTL_CMD_WRITE_TS_PACK: // Write Ts Packets
			{
				int retval = (-ENOTTY);
				ACCESS_HFPGA_TS_PACK* pfpga=(ACCESS_HFPGA_TS_PACK*)kmalloc(sizeof(ACCESS_HFPGA_TS_PACK), GFP_KERNEL);
				if(pfpga==NULL) return  (-EFAULT);

				retval = copy_from_user( pfpga, (PACCESS_HFPGA_TS_PACK)arg, sizeof(ACCESS_HFPGA_TS_PACK) );
				if (0 != retval) {
					drv_prt(KERN_ERR "HFPGA hfpga_ioctl(): copy_from_user () failed\n");
					retval = (-EFAULT);
				}
				else
				{
					write_hfpga_ts_pack(pfpga);
					retval = 0;
				}
				kfree(pfpga);
				return retval;
			}
			break;			

			// -------------------------------------------------------------
			// Read All PIDS in all input channels
		case UV_HFPGA_IOCTL_CMD_READ_ALL_PIDS:
			{
				int retval = (-ENOTTY);
				ACCESS_HFPGA_ALL_PIDS* pfpga=(ACCESS_HFPGA_ALL_PIDS*)kmalloc(sizeof(ACCESS_HFPGA_ALL_PIDS), GFP_KERNEL);
				if(pfpga==NULL) return  (-EFAULT);

				retval = copy_from_user( pfpga, (PACCESS_HFPGA_ALL_PIDS)arg, sizeof(ACCESS_HFPGA_ALL_PIDS));
				if (0 != retval) {
					drv_prt(KERN_ERR "HFPGA hfpga_ioctl()->copy_from_user() failed\n");
					retval = (-EFAULT);
				}
				else
				{
					read_hfpga_all_pids(pfpga);
					copy_to_user((PACCESS_HFPGA_ALL_PIDS)arg, pfpga, sizeof(ACCESS_HFPGA_ALL_PIDS));
					retval=0;
				}
				kfree(pfpga);
				return retval;
			}
			break;

			// -------------------------------------------------------------
			// Write PID Maps
		case UV_HFPGA_IOCTL_CMD_WRITE_PID_MAP: // Write PID filter registers
			{
				int retval = (-ENOTTY);
				ACCESS_HFPGA_PID_MAP* pfpga=(ACCESS_HFPGA_PID_MAP*)kmalloc(sizeof(ACCESS_HFPGA_PID_MAP), GFP_KERNEL);
				if(pfpga==NULL) return  (-EFAULT);

				retval = copy_from_user( pfpga, (PACCESS_HFPGA_PID_MAP)arg, sizeof(ACCESS_HFPGA_PID_MAP)); 
				if (0 != retval) {
					drv_prt(KERN_ERR "HFPGA hfpga_ioctl()->copy_from_user () failed\n");
					return (-EFAULT);
				}
				else
				{
					write_hfpga_pid_map(pfpga);
					retval=0;
				}
				kfree(pfpga);
				return retval;
			}
			break;

			// Read PID Maps
		case UV_HFPGA_IOCTL_CMD_READ_PID_MAP: // Read PID filter registers
			{
				int retval = (-ENOTTY);
				ACCESS_HFPGA_PID_MAP* pfpga=(ACCESS_HFPGA_PID_MAP*)kmalloc(sizeof(ACCESS_HFPGA_PID_MAP), GFP_KERNEL);
				if(pfpga==NULL) return  (-EFAULT);

				retval = copy_from_user( pfpga, (PACCESS_HFPGA_PID_MAP)arg, sizeof(ACCESS_HFPGA_PID_MAP));
				if (0 != retval) {
					drv_prt(KERN_ERR "HFPGA hfpga_ioctl()->copy_from_user () failed\n");
					return (-EFAULT);
				}
				else
				{
					read_hfpga_pid_map(pfpga);
					copy_to_user((PACCESS_HFPGA_PID_MAP)arg, pfpga, sizeof(ACCESS_HFPGA_PID_MAP));
					retval=0;
				}
				kfree(pfpga);
				return retval;
			}
			break;

			// -------------------------------------------------------------
			// Capture/Output stream data
		case UV_HFPGA_IOCTL_CMD_READ_STR_DATA:
			{
				int retval = (-ENOTTY);
				ACCESS_HFPGA_STR_DATA* pfpga=(ACCESS_HFPGA_STR_DATA*)kmalloc(sizeof(ACCESS_HFPGA_STR_DATA), GFP_KERNEL);
				if(pfpga==NULL) return  (-EFAULT);

				retval = copy_from_user( pfpga, (PACCESS_HFPGA_STR_DATA)arg, sizeof(ACCESS_HFPGA_STR_DATA));
				if (0 != retval) {
					drv_prt(KERN_ERR "HFPGA hfpga_ioctl()->copy_from_user() failed\n");
					retval = (-EFAULT);
				}
				else
				{
					unsigned char *pdata = kmalloc(pfpga->len, GFP_KERNEL);
					if(NULL == pdata)
					{
						drv_prt(KERN_ERR "HFPGA hfpga_ioctl kmalloc failed\n");
						retval = (-EFAULT);
					}
					else
					{
						unsigned int nread = read_hfpga_str_data(pdata, pfpga->addr, pfpga->len,pfpga->nbgn);
						drv_prt("nread=%02x %02x %02x %02x +++ \n", nread, pdata[0], pdata[1], pdata[2]);
						pfpga->len = nread;
						if(nread>0)
						{
							copy_to_user(pfpga->pdata, pdata, nread);
							copy_to_user((PACCESS_HFPGA_STR_DATA)arg, pfpga, sizeof(ACCESS_HFPGA_STR_DATA));
							retval=0;
						}
						else retval = (-EFAULT);

						kfree(pdata);
						//retval=0;
					}
				}
				kfree(pfpga);
				return retval;
			}
			break;

		case UV_HFPGA_IOCTL_CMD_WRITE_STR_DATA:
			{
				int retval = (-ENOTTY);
				ACCESS_HFPGA_STR_DATA* pfpga=(ACCESS_HFPGA_STR_DATA*)kmalloc(sizeof(ACCESS_HFPGA_STR_DATA), GFP_KERNEL);
				if(pfpga==NULL) return  (-EFAULT);

				retval = copy_from_user( pfpga, (PACCESS_HFPGA_STR_DATA)arg, sizeof(ACCESS_HFPGA_STR_DATA));
				if (0 != retval) {
					drv_prt(KERN_ERR "HFPGA hfpga_ioctl()->copy_from_user() failed\n");
					retval = (-EFAULT);
				}
				else
				{
					unsigned char* pdata = kmalloc(pfpga->len, GFP_KERNEL);
					if(NULL == pdata)
					{
						drv_prt(KERN_ERR "HFPGA hfpga_ioctl kmalloc failed\n");
						retval = (-EFAULT);
					}
					else
					{
						retval = copy_from_user(pdata, pfpga->pdata, pfpga->len);
						write_hfpga_str_data(pdata, pfpga->addr, pfpga->len); 
						kfree(pdata);
						retval = 0;
					}
				}
				kfree(pfpga);
				return retval;
			}
			break;

		case UV_HFPGA_IOCTL_CMD_CMDS:
			{
				int retval = (-ENOTTY);
				ACCESS_HFPGA_CMD* pfpga=(ACCESS_HFPGA_CMD*)kmalloc(sizeof(ACCESS_HFPGA_CMD), GFP_KERNEL);
				if(pfpga==NULL) return  (-EFAULT);
				
				retval = copy_from_user( pfpga, (PACCESS_HFPGA_CMD)arg, sizeof(ACCESS_HFPGA_CMD));
				if (0 != retval) {
					drv_prt(KERN_ERR "HFPGA hfpga_ioctl()->copy_from_user() failed\n");
					retval = (-EFAULT);
				}
				else
				{
					if(HFPGA_CMD_SI_START == (0x0FF&(pfpga->cmd)))
					{
						// for debugging { 
						//outputhex_si(HFPGA_MAP_MEM_ADDR_SI_40MS_BASE, cur_40ms_offset-HFPGA_MAP_MEM_ADDR_SI_40MS_BASE, HFPGA_MAP_MEM_ADDR_SI_2S_BASE, cur_2s_offset - HFPGA_MAP_MEM_ADDR_SI_2S_BASE);	
						// for debugging }
						if(cur_40ms_offset <= HFPGA_MAP_MEM_ADDR_SI_40MS_BASE)
							noff=0;
						else noff = cur_40ms_offset - HFPGA_MAP_MEM_ADDR_SI_40MS_BASE - 1;
						write_hfpga_register(HFPGA_REG_ADDR_SI_40MS_PACK_LEN, noff);

						if(cur_2s_offset <= HFPGA_MAP_MEM_ADDR_SI_2S_BASE)
							noff=0;
						else noff = cur_2s_offset - HFPGA_MAP_MEM_ADDR_SI_2S_BASE - 1;

						write_hfpga_register(HFPGA_REG_ADDR_SI_2S_PACK_LEN, noff );
						
			#ifdef 0
						write_hfpga_register(HFPGA_REG_ADDR_SI_40MS_PACK_LEN, cur_40ms_offset - HFPGA_MAP_MEM_ADDR_SI_40MS_BASE - 1);
						write_hfpga_register(HFPGA_REG_ADDR_SI_2S_PACK_LEN, cur_2s_offset - HFPGA_MAP_MEM_ADDR_SI_2S_BASE - 1);
			#endif
						// start send SI
						write_hfpga_register(HFPGA_REG_ADDR_SI_SS, 0x55AA);
			
						
					}
					else if(HFPGA_CMD_SI_STOP == (0x0FF&(pfpga->cmd)))
					{
						// stop send SI
						write_hfpga_register(HFPGA_REG_ADDR_SI_SS, 0x0000);

						write_hfpga_register(HFPGA_REG_ADDR_SI_40MS_PACK_ADDR, HFPGA_MAP_MEM_ADDR_SI_40MS_BASE);
						write_hfpga_register(HFPGA_REG_ADDR_SI_2S_PACK_ADDR, HFPGA_MAP_MEM_ADDR_SI_2S_BASE);

						cur_40ms_offset = HFPGA_MAP_MEM_ADDR_SI_40MS_BASE;
						cur_2s_offset = HFPGA_MAP_MEM_ADDR_SI_2S_BASE;
					}
					copy_to_user((PACCESS_HFPGA_CMD)arg, pfpga, sizeof(ACCESS_HFPGA_CMD));
					retval = 0;
				}
				kfree(pfpga);
				return retval;
			}
			break;

		default:
			drv_prt(KERN_WARNING "HFPGA hfpga_ioctl(): Failure: Invalid commands!\n");
			return (-EINVAL);
			break;
	}
	//up(&hfpga_ioctl_sem);
	return (0);

}
#endif


static loff_t hfpga_lseek(struct file * file, loff_t offset, int orig)
{
	/*
	 *  orig 0: position from begning of eeprom
	 *  orig 1: relative from current position
	 *  orig 2: position from last eeprom address
	 */

	switch (orig)
	{
		case 0:
			file->f_pos = offset;
			break;
		case 1:
			file->f_pos += offset;
			break;
		case 2:
			file->f_pos = hfpga_ram_size - offset;
			break;
		default:
			return -EINVAL;
	}

	/* truncate position */
	if (file->f_pos < 0)
	{
		file->f_pos = 0;    
		return(-EOVERFLOW);
	}

	if (file->f_pos >= hfpga_ram_size)
	{
		file->f_pos = hfpga_ram_size - 1;
		return(-EOVERFLOW);
	}

	return ( file->f_pos );
}

static int read_from_hfpga_ram(unsigned long addr, char * buf, int count)
{
	int size = 0;

	return size;
}

static int write_to_hfpga_ram(unsigned long addr, char * buf, int count)
{
	int size = 0;

	return size;
}

static ssize_t hfpga_read(struct file * file, char * buf, size_t count, loff_t *off)
{
	int read=0;
	unsigned long p = file->f_pos;

	if(p >= hfpga_ram_size || (p + count) > hfpga_ram_size) 
	{
		drv_prt(KERN_ERR "HFPGA hfpga_read(): Failure: Invalid addr and counts!\n");
		return -EFAULT;
	}

	down(&hfpga_ioctl_sem);

	/* go on with the actual read */
	read = read_from_hfpga_ram(p, buf, count);

	if(read > 0)
	{
		file->f_pos += read;
	}

	up(&hfpga_ioctl_sem);

	return read;
}

static ssize_t hfpga_write(struct file * file, const char * buf, size_t count,loff_t *off)
{
	int  written = 0;
	unsigned long p = file->f_pos;

	if (verify_area(VERIFY_READ, buf, count))
	{
		return -EFAULT;
	}

	down(&hfpga_ioctl_sem);

	/* go on with the actual write */
	written = write_to_hfpga_ram(p, buf, count);

	up(&hfpga_ioctl_sem);

	if (written == 0 && file->f_pos >= hfpga_ram_size){
		return -ENOSPC;
	}

	file->f_pos += written;

	return written;
}

static int hfpga_close(struct inode * inode, struct file * file)
{
	MOD_DEC_USE_COUNT;
	return 0;
}
