#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "wu/wu_base_type.h"


int eeprom_open();
void eeprom_write(int offset, void *buf, size_t len);
void eeprom_read(int offset, void *buf, size_t len);


#endif /* _EEPROM_H_ */

