#ifndef _NET_H_
#define _NET_H_

#include "up_type.h"


int net_set_eth_ip_netmask (int ethid, IPAddress ip, IPAddress netmask);
int net_set_gateway (IPAddress gateway);
int net_set_mac (int ethid, char *mac);
int net_ioctl_set_mac(int ethid, char *mac);
int net_ioctl_get_mac(int ethid, char *mac);

char *ether_etoa(const unsigned char *e, char *a);
int ether_atoe(const char *a, unsigned char *e);


#endif /* _NET_H_ */

