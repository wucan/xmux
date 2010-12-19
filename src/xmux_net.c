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

static const char *mac_string(struct xmux_net_param *net)
{
	static char mac_str[64];

	sprintf(mac_str, "%#x:%#x:%#x:%#x:%#x:%#x",
		net->mac[0], net->mac[1], net->mac[2],
		net->mac[3], net->mac[4], net->mac[5]);

	return mac_str;
}

void xmux_net_param_dump(struct xmux_net_param *net)
{
	trace_info("net: ip %s, netmask %s, gw %s, mac %s, server ip %s",
		inet_ntoa(net->ip), inet_ntoa(net->netmask), inet_ntoa(net->gateway),
		mac_string(net), inet_ntoa(net->server_ip));
}

int xmux_net_set(struct xmux_net_param *net)
{
	up_set_net_param(0, net->ip, net->netmask);
	up_set_gateway(net->gateway);
	up_set_mac(0, net->mac);

	xmux_config_save_net_param(net);

	return 0;
}

