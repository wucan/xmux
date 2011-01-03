#include "wu/message.h"
#include "wu/wu_csc.h"

#include "xmux.h"
#include "xmux_config.h"
#include "up_support.h"
#include "net.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "net"};

bool xmux_net_param_validate(struct xmux_net_param *net)
{
	uint8_t csc;
	uint8_t *p;
	int i;

	csc = wu_csc(net, offsetof(struct xmux_net_param, csc));
	if (net->csc != csc) {
		trace_err("csc invalidate!");
		return false;
	}
	/* check netmask */
	p = (uint8_t *)&net->netmask;
	for (i = 0; i < 4; i++) {
		if (p[i] != 0x00 && p[i] != 0xFF) {
			trace_err("netmask %#x invalidate!", net->netmask);
			return false;
		}
	}
	/* check mac */
	if (net->mac[0] != 0x00) {
		trace_err("mac invalidate!");
		hex_dump("mac", net->mac, sizeof(net->mac));
		return false;
	}

	return true;
}

void xmux_net_param_init_default(struct xmux_net_param *net)
{
	uint8_t default_mac[6] = {0x00, 0x09, 0x30, 0x28, 0x12, 0x22};

	net->ip = inet_addr("192.168.0.125");
	net->netmask = inet_addr("255.255.255.0");
	net->server_ip = inet_addr("192.168.0.100");
	net->gateway = inet_addr("192.168.0.1");
	memcpy(net->mac, default_mac, 6);
	net->csc = wu_csc(net, offsetof(struct xmux_net_param, csc));
}

void xmux_net_param_dump(struct xmux_net_param *net)
{
	char *ip = strdup(inet_ntoa(net->ip));
	char *netmask = strdup(inet_ntoa(net->netmask));
	char *gw = strdup(inet_ntoa(net->gateway));
	char *server_ip = strdup(inet_ntoa(net->server_ip));

	trace_info("net: ip %s, netmask %s, gw %s, mac %s, server ip %s",
		ip, netmask, gw, mac_string(net->mac), server_ip);

	free(ip);
	free(netmask);
	free(gw);
	free(server_ip);
}

int xmux_net_set(struct xmux_net_param *net)
{
	up_set_net_param(0, net->ip, net->netmask);
	up_set_gateway(net->gateway);
	net_ioctl_set_mac(0, net->mac);

	xmux_config_save_net_param(net);

	return 0;
}

int xmux_net_restore()
{
	struct xmux_net_param *net = &g_eeprom_param.net;

	if (!xmux_net_param_validate(net)) {
		trace_err("net param invalidate, no restore!");
		return -1;
	}
	up_set_net_param(0, net->ip, net->netmask);
	up_set_gateway(net->gateway);
	net_ioctl_set_mac(0, net->mac);

	return 0;
}

