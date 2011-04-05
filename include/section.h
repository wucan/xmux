#ifndef _SECTION_H
#define _SECTION_H

#include "wu/wu_base_type.h"


int section_to_ts_length(int sec_len);
int section_to_ts(uint8_t *sec_data, int sec_len, uint8_t *ts_buf,
					 uint16_t pid, int *p_cc);
int ts_to_section(uint8_t *ts, uint8_t *sec);


#endif /* _SECTION_H */

