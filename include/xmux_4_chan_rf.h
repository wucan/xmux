#ifndef _MXU_4_CHAN_RF_H_
#define _MXU_4_CHAN_RF_H_

#include "xmux_tunner.h"
#include "xmux_bcm3033.h"


struct xmux_4_chan_rf_param {
	struct tunner_info {
		struct tunner_param param;
		struct tunner_status status;
	} tunner[TUNNER_MAX_NUM];
	struct bcm3033_param bcm;
};

extern struct xmux_4_chan_rf_param g_4_chan_rf_param;


#endif /* _MXU_4_CHAN_RF_H_ */

