#include "wu/message.h"
#include "wu/wu_csc.h"

#include "xmux.h"
#include "xmux_config.h"
#include "up_support.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "net"};

bool xmux_net_param_validate(struct xmux_net_param *net)
{
	uint8_t csc;

	csc = wu_csc(net, offsetof(struct xmux_net_param, csc));
	if (net->csc != csc)
		return false;

	return true;
}

void xmux_net_param_init_default(struct xmux_net_param *net)
{
	net->csc = wu_csc(net, offsetof(struct xmux_net_param, csc));
}

void xmux_net_param_dump(struct xmux_net_param *net)
{
	trace_info("net: ");
}

int xmux_net_set(struct xmux_net_param *net)
{
	up_set_net_param(0, net->ip, net->netmask);
	up_set_gateway(net->gateway);
	up_set_mac(0, net->mac);

	xmux_config_save_net_param(net);

	return 0;
}

