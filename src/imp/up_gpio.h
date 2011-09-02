/*
 * gpio operations 
 *
 * Copyright (C) 2005, wucan <can_wu@cnbvcom.com>
 *
 */

#ifndef _UP_GPIO_H_
#define _UP_GPIO_H_


enum {
	UP_GPIO_TS_PASSTHROUGH = 3,
	UP_GPIO_ASI_NO_INPUT = 8,
	UP_GPIO_RESET_MUX_FPGA = 9,
	UP_GPIO_MUX_BITRATE_OVERRUN = 11,
};

#define IOPMODE1		(*(unsigned int *)0xF0030000)
#define IOPCON1			(*(unsigned int *)0xF0030008)
#define IOPDATA1		(*(unsigned int *)0xF003001C)
#define IOPDRV1			(*(unsigned int *)0xF0030024)

#define IOPMODE2		(*(unsigned int *)0xF0030004)
#define IOPCON2			(*(unsigned int *)0xF003000C)
#define IOPDATA2		(*(unsigned int *)0xF0030020)
#define IOPDRV2			(*(unsigned int *)0xF0030028)

enum {
	TRI_STATE = 0,
	OPEN_DRAIN,
};
#define DRV			OPEN_DRAIN


void gpio_output_low (int gpio);
void gpio_output_high (int gpio);
void gpio_set_input_mode (int gpio);
void gpio_set_output_mode (int gpio);
int gpio_get_input_value (int gpio);
int gpio_get_multi_input_value_high32 (int gpio_mask);
int gpio_get_multi_input_value_low32 (int gpio_mask);


#endif /* _UP_GPIO_H_ */

