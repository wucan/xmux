#ifndef _XMUX_TUNNER_H_
#define _XMUX_TUNNER_H_

#include "wu/wu_base_type.h"


#define TUNNER_MAX_NUM		2

struct tunner_param {
	uint32_t frequency; // MHZ
	uint32_t symbol_rate;
	uint32_t lof; // local frequency, MHZ
	uint8_t polarization; // H = 0, V = 1
	uint8_t lnb; // off = 0,  on = 1
	uint8_t f22k; // off = 0, on = 1

	uint8_t csc;
} __attribute__ ((__packed__));


struct tunner_status {
	uint8_t lock; // unlock = 0, lock = 1
	uint8_t standard; // dss = 0, dvbs1 = 1, dvbs2 = 2
	uint8_t level;
	uint32_t cn;
	uint8_t puncture_rate; // 1/2=0, 2/3=1, 3/4=2, 5/6=3, 6/7=4, 7/8=5
	uint32_t ber;
} __attribute__ ((__packed__));

int tunner_set_param(int id, struct tunner_param *param);
int tunner_get_status(int id, struct tunner_status *status);


#endif /* _XMUX_TUNNER_H_ */

