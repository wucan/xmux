#ifndef _TUNER_DEVICE_H_
#define _TUNER_DEVICE_H_

#include <stdint.h>


void tuner_device_reset();
int tuner_device_open();
void tuner_device_close();
int tuner_device_write_register(uint16_t reg, uint8_t *buf, size_t size);
int tuner_device_read_register(uint16_t reg, uint8_t *buf, size_t size);
int tuner_device_6110_write_register(uint16_t reg, uint8_t *buf, size_t size);
int tuner_device_6110_read_register(uint16_t reg, uint8_t *buf, size_t size);



#endif /* _TUNER_DEVICE_H_ */

