#ifndef _XMUX_NET_H_
#define _XMUX_NET_H_

#include "wu/wu_base_type.h"


struct xmux_net_param;

bool xmux_net_param_validate(struct xmux_net_param *net);
void xmux_net_param_init_default(struct xmux_net_param *net);
void xmux_net_param_dump(struct xmux_net_param *net);
int xmux_net_set(struct xmux_net_param *net);
int xmux_net_restore();


#endif /* _XMUX_NET_H_ */

