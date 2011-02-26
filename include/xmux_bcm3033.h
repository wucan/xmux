#ifndef _XMUX_BCM3033_H_
#define _XMUX_BCM3033_H_

#include "wu/wu_base_type.h"


struct bcm3033_param {
	uint16_t annex: 2;
	uint16_t mode: 3;
	uint16_t alpha: 2;
	uint16_t iq_inv: 1;
	uint16_t _if: 2;
	uint8_t interlace[3];
	uint8_t baud[2];
	uint8_t rf_freq[3];
	uint8_t att;
} __attribute__ ((__packed__));


int bcm3033_set_param(struct bcm3033_param *param);
int bcm3033_get_param(struct bcm3033_param *param);


#endif /* _XMUX_BCM3033_H_ */

