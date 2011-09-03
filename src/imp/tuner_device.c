#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>

#include "i2cdev.h"
#include "up_gpio.h"


#define TUNER_DEV			"tuner"
#define TUNER_DEV1			"/dev/tuner"
#define TUNER_W_ADDR		0xD0
#define TUNER_R_ADDR		0xD1
#define RESET_GPIO			50

static int fd;

void tuner_device_reset()
{
	gpio_set_output_mode(RESET_GPIO);
	gpio_output_low(RESET_GPIO);
	usleep(100000);
	gpio_output_high(RESET_GPIO);
	usleep(100000);
}

int tuner_device_open()
{
	tuner_device_reset();

	fd = open(TUNER_DEV, O_RDWR);
	if (fd < 0) {
		printf("can't open device %s! try %s\n", TUNER_DEV, TUNER_DEV1);
		fd = open(TUNER_DEV1, O_RDWR);
		if (fd < 0) {
			printf("can't open device %s!\n", TUNER_DEV1);
			return -1;
		}
	}

	return 0;
}

void tuner_device_close()
{
	if (fd > 0) {
		close(fd);
		fd = 0;
	}
}

int tuner_device_write_register(uint16_t reg, uint8_t *buf, size_t size)
{
	struct i2cdev_ioc_data_param param;
	int rc;

	if (fd <= 0)
		return -1;

	if (size > (TUNER_MAX_RW_BYTES - 3)) {
		fprintf(stderr, "tuner: write exceed! %d > %d\n",
			size, TUNER_MAX_RW_BYTES - 3);
		return -1;
	}

	param.nbytes_write = 3 + size;
	param.bytes[0] = TUNER_W_ADDR;
	param.bytes[1] = reg > 8;
	param.bytes[2] = reg & 0xFF;
	memcpy(&param.bytes[3], buf, size);
	rc = ioctl(fd, I2CDEV_IOC_WRITE_DATA, &param);

	return 0;
}

int tuner_device_read_register(uint16_t reg, uint8_t *buf, size_t size)
{
	struct i2cdev_ioc_data_param param;
	int rc;

	if (fd <= 0)
		return -1;

	if (size > (TUNER_MAX_RW_BYTES - 3)) {
		fprintf(stderr, "tuner: read exceed! %d > %d\n",
			size, TUNER_MAX_RW_BYTES - 3);
		return -1;
	}

	param.nbytes_write = 3;
	param.nbytes_expect_read = size;
	param.bytes[0] = TUNER_W_ADDR;
	param.bytes[1] = reg > 8;
	param.bytes[2] = reg & 0xFF;
	param.read_head_nbytes = 1;
	param.read_head_bytes[0] = TUNER_R_ADDR;
	rc = ioctl(fd, I2CDEV_IOC_WREAD_DATA, &param);
	memcpy(buf, param.bytes, size);

	return 0;
}

