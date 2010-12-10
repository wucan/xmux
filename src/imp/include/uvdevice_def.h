//uvdevice_def.h

#ifndef _UVDEVICE_DEF_H_
#define _UVDEVICE_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct univ_device_s{
	unsigned short device_no;
	unsigned char comm[64];
	unsigned char i_comm;
	
	//unsigned char ip[4];
	//unsigned char gw[4];
	//unsigned char sb[4];
	//unsigned char mac[6];
	unsigned short device_status;
} univ_device;

#ifdef __cplusplus
}
#endif

#endif