#ifndef _UP_SUPPORT_H_
#define _UP_SUPPORT_H_

#include "up_type.h"


int up_set_net_param (int ethid, IPAddress ip, IPAddress netmask);
int up_set_gateway (IPAddress gateway);
int up_set_mac (int ethid, char *mac);
const char * up_get_mac (int ethid);
const char * up_get_netmask (int ethid);
const char * up_get_gateway ();


#endif /* _UP_SUPPORT_H_ */

