#include <stdio.h>
#include <fcntl.h>

#include "eeprom.h"


#define UV_HAT24C64_DEV_PATH		"/dev/AT24C64"

static int eep_fd;
int eeprom_open()
{
	eep_fd = open(UV_HAT24C64_DEV_PATH, O_RDWR);
	if (eep_fd < 0) {
		return -1;
	}

	return 0;
}
void eeprom_write(int offset, uint8_t *buf, size_t len)
{
	lseek(eep_fd, offset, SEEK_SET);
	write(eep_fd, buf, len);
}
void eeprom_read(int offset, uint8_t *buf, size_t len)
{
	lseek(eep_fd, offset, SEEK_SET);
	read(eep_fd, buf, len);
}

