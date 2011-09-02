/*
 * 2510 gpio operations
 *
 * Copyright (C) 2008, wucan <can_wu@cnbvcom.com>
 *
 */

#include "up_gpio.h"


void gpio_output_low (int gpio)
{
#ifndef _USE_GTK_
	if (gpio >= 64) {
		printf("gpio %d out of ragne, should be 0 -> 63.\n", gpio);
		return;
	}
	if (gpio >= 32) {
		gpio -= 32;
		IOPDRV2 |= (DRV << gpio);
		/* write 0 */
		IOPDATA2 &= ~(1 << gpio);
	} else {
		IOPDRV1 |= (DRV << gpio);
		/* write 0 */
		IOPDATA1 &= ~(1 << gpio);
	}
#endif
}
void gpio_output_high (int gpio)
{
#ifndef _USE_GTK_
	if (gpio >= 64) {
		printf("gpio %d out of ragne, should be 0 -> 63.\n", gpio);
		return;
	}
	if (gpio >= 32) {
		gpio -= 32;
		IOPMODE2 &= ~(1 << gpio);
		IOPDRV2 |= (DRV << gpio);
		IOPCON2 |= (1 << gpio);
		/* write 1 */
		IOPDATA2 |= 1 << gpio;
	} else {
		IOPMODE1 &= ~(1 << gpio);
		IOPDRV1 |= (DRV << gpio);
		IOPCON1 |= (1 << gpio);
		/* write 1 */
		IOPDATA1 |= 1 << gpio;
	}
#endif
}
void gpio_set_input_mode (int gpio)
{
#ifndef _USE_GTK_
	if (gpio >= 64) {
		printf("gpio %d out of ragne, should be 0 -> 63.\n", gpio);
		return;
	}
	if (gpio >= 32) {
		gpio -= 32;
		IOPMODE2 |= (1 << gpio);
		IOPCON2 |= (1 << gpio);
	} else {
		IOPMODE1 |= (1 << gpio);
		IOPCON1 |= (1 << gpio);
	}
#endif
}
void gpio_set_output_mode (int gpio)
{
#ifndef _USE_GTK_
	if (gpio >= 64) {
		printf("gpio %d out of ragne, should be 0 -> 63.\n", gpio);
		return;
	}
	if (gpio >= 32) {
		gpio -= 32;
		IOPMODE2 &= ~(1 << gpio);
		IOPCON2 |= (1 << gpio);
	} else {
		IOPMODE1 &= ~(1 << gpio);
		IOPCON1 |= (1 << gpio);
	}
#endif
}
int gpio_get_input_value (int gpio)
{
#ifndef _USE_GTK_
	int v;

	if (gpio >= 32) {
		gpio -= 32;
		v = IOPDATA2 & (1 << gpio);
	} else {
		v = IOPDATA1 & (1 << gpio);
	}
	if (v) {
		return 1;
	}
#endif

	return 0;
}
/*
 * get multi gpio value
 */
int gpio_get_multi_input_value_high32 (int gpio_mask)
{
	return IOPDATA2 & gpio_mask;
}
int gpio_get_multi_input_value_low32 (int gpio_mask)
{
	return IOPDATA1 & gpio_mask;
}

