//uvcheck_def.h

#ifndef _UVCHECK_DEF_H_
#define _UVCHECK_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

bool check_csc_byte(unsigned char *data, unsigned int len);
unsigned char gen_csc_byte(unsigned char *data, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif