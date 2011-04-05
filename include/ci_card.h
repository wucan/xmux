#ifndef _CI_H_
#define _CI_H_

#include "wu/wu_base_type.h"


bool ci_card_actived();
uint16_t ci_attr_mem_read_u16(int off);
bool ci_io_write_u8(int off, uint8_t v);
uint8_t ci_io_read_u8(int off);
bool ci_mem_write_u16(int off, uint16_t v);
uint16_t ci_mem_read_u16(int off);


#endif /* _CI_H_ */

