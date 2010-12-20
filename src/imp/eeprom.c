#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "xmux.h"
#include "eeprom.h"


#ifdef _UCLINUX_
#define UV_HAT24C64_DEV_PATH		"/dev/AT24C64"
#else
#define UV_HAT24C64_DEV_PATH		"./EEPROM"
#endif

static int eep_fd;
int eeprom_open()
{
	eep_fd = open(UV_HAT24C64_DEV_PATH, O_RDWR);
	if (eep_fd < 0) {
#ifdef _UCLINUX_
		return -1;
#else
		/*
		 * create a new file backed eeprom if not exist
		 */
		eep_fd = open(UV_HAT24C64_DEV_PATH, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
		if (eep_fd < 0) {
			perror("open");
			return -1;
		}
		/* init to zero of all size */
		posix_fallocate(eep_fd, 0, EEPROM_SIZE);
#endif
	}

	return 0;
}
void eeprom_write(int offset, void *buf, size_t len)
{
	lseek(eep_fd, offset, SEEK_SET);
	write(eep_fd, buf, len);
}
void eeprom_read(int offset, void *buf, size_t len)
{
	lseek(eep_fd, offset, SEEK_SET);
	read(eep_fd, buf, len);
}

