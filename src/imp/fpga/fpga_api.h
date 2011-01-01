/*##########################################################################
## fpga_api.h
## grepo@163.com
## 2010-09-05
## 
##########################################################################*/

#ifndef __FPGA_API_H__ 
#define __FPGA_API_H__ 

#include "uvdebug_def.h"
#include "host_fpga.h"

#include "fpga_api_def.h"

int hfpga_open(const char *name, int flags);
void hfpga_close(int hdev);
int hfpga_pid_map(void* pid_map);
int hfpga_readn(unsigned char *p_buf, unsigned int len, void *p_param,int nbgn);
int hfpga_writen(unsigned char *p_data, unsigned int len, void *p_param);
int hfpga_ioctl(unsigned int cmd, void *p_param);

typedef void (*FPGAWriteHook)(void *data, int len);
void fpga_set_write_hook(FPGAWriteHook hook);


#endif
