#ifndef _PSI_PARSE_H_
#define _PSI_PARSE_H_

#include "xmux.h"


extern uint8_t sg_mib_pat[CHANNEL_MAX_NUM][SECTION_MAX_SIZE];
extern uint8_t sg_mib_pmt[CHANNEL_MAX_NUM][PROGRAM_MAX_NUM][SECTION_MAX_SIZE];
extern uint8_t sg_mib_cat[CHANNEL_MAX_NUM][SECTION_MAX_SIZE];
extern uint8_t sg_mib_nit[CHANNEL_MAX_NUM][SECTION_MAX_SIZE];
extern uint8_t sg_mib_sdt[CHANNEL_MAX_NUM][SDT_SECTION_NUM][SECTION_MAX_SIZE];
extern uint8_t sg_mib_eit[CHANNEL_MAX_NUM][EIT_SECTION_NUM][SECTION_MAX_SIZE];

int fp_psi_parse();

int uvSI_psi_parse();
void uvSI_psi_parse_stop();


#endif /* _PSI_PARSE_H_ */

