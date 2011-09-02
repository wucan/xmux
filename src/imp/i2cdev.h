#ifndef _I2CDEV_H_
#define _I2CDEV_H_

#include <linux/ioctl.h>


#define TUNER_MAX_RW_BYTES		24

struct i2cdev_ioc_data_param {
	unsigned short nbytes_write;
	unsigned short nbytes_expect_read;
	unsigned char bytes[TUNER_MAX_RW_BYTES];

	unsigned short read_head_nbytes;
	unsigned char read_head_bytes[4];
};

#define I2CDEV_IOC_WRITE_DATA	_IOW('I', 0, struct i2cdev_ioc_data_param)
#define I2CDEV_IOC_WREAD_DATA	_IOWR('I', 1, struct i2cdev_ioc_data_param)


#endif /* _I2CDEV_H_ */

