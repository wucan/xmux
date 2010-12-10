
/*##########################################################################
## dvb_io.h
## grepo@163.com
## 2010-09-05
## 
##########################################################################*/

#ifndef __DVB_IO_H__ 
#define __DVB_IO_H__ 

#include <stdio.h>
#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

typedef int (*fcn_open)(const char *, int);
typedef void (*fcn_close)(int);
typedef int (*fcn_read)(unsigned char *p_buf, unsigned int len, void *p_param);
typedef int (*fcn_write)(unsigned char *p_data, unsigned int len, void *p_param);

typedef struct _dvb_io{
	fcn_open open;	
	fcn_close close;
	fcn_read read;	
	fcn_write write;
	char devname[64];	
	int flags;
	int hdev;
	unsigned char cha;
} uv_dvb_io;

static uv_dvb_io dvb_io_dev;

#endif
