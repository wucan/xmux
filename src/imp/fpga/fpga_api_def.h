/*##########################################################################
## fpga_api_def.h
## grepo@163.com
## 2010-09-05
## 
##########################################################################*/

#ifndef __FPGA_API_DEF_H__ 
#define __FPGA_API_DEF_H__ 

typedef struct _io_param{
	unsigned short pid;
	unsigned char cha;
	unsigned char type; // 0  表示packet，1表示变长度section
	unsigned short len;
} uv_io_param;


typedef int (*fcn_open)(const char *, int);
typedef void (*fcn_close)(int);
typedef int (*fcn_read)(unsigned char *p_buf, unsigned int len, void *p_param,int nbgn);
typedef int (*fcn_write)(unsigned char *p_data, unsigned int len, void *p_param);
typedef int (*fcn_ioctl)(unsigned int cmd, void *p_param);

typedef struct _dvb_io{
	fcn_open open;	
	fcn_close close;
	fcn_read read;	
	fcn_write write;
	fcn_ioctl ioctl;
	char devname[64];	
	int flags;
	int hdev;
	unsigned char cha;
} uv_dvb_io;

extern uv_dvb_io dvb_io_dev;

#endif


