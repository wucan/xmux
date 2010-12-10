#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include "up_config.h"
#include "net.h"
#include "up_support.h"
#include "up_sys.h"


int up_set_net_param (int ethid, IPAddress ip, IPAddress netmask)
{
	FILE *file;
	char tmpbuf[100];
	struct in_addr inaddr;
	char *ipstr, *nmstr;
	int rc = -1;

#if !HAD_NIC_1
	if (ethid >= 1) {
		return 0;
	}
#endif
	rc = net_set_eth_ip_netmask(ethid, ip, netmask);
	if (rc) {
		fprintf(stderr, "net: set eth%d ip error\n", ethid);
		return rc;
	}
#if SAVE_NET_PARAM_TO_BOOTROM
	/* save ip */
	inaddr.s_addr = ip;
	ipstr = inet_ntoa(inaddr);
	sprintf(tmpbuf, "ipaddr%d", ethid);
	up_sys_set_benv(tmpbuf, ipstr);
	/* save netmask */
	inaddr.s_addr = netmask;
	nmstr = inet_ntoa(inaddr);
	sprintf(tmpbuf, "netmask%d", ethid);
	up_sys_set_benv(tmpbuf, nmstr);
#endif

	return 0;
}

/*
 * set gateway
 */
int up_set_gateway (IPAddress gateway)
{
	struct in_addr inaddr;
	char *gwstr;
	int rc;
	FILE *file;

	rc = net_set_gateway(gateway);
	if (rc) {
		fprintf(stderr, "net: set gateway error\n");
		return rc;
	}
#if SAVE_NET_PARAM_TO_BOOTROM
	/* save ip */
	inaddr.s_addr = gateway;
	gwstr = inet_ntoa(inaddr);
	up_sys_set_benv("GATEWAY", gwstr);

	/*
	 * use the gateway as our dns
	 */
	file = fopen("/var/resolv.conf", "w+");
	if (!file) {
		fprintf(stderr, "couldn't open /var/resolv.conf!\n");
		return -1;
	}
	fprintf(file, "nameserver %s", gwstr);
	fclose(file);
#endif

	return 0;
}
/*
 * set mac
 */
int up_set_mac (int ethid, char *mac)
{
	static char *name[2] = {"HWADDR0", "HWADDR1"};

#if !HAD_NIC_1
	if (ethid >= 1) {
		return 0;
	}
#endif
	if (ethid >= 2) {
		return -1;
	}
	if (net_set_mac(ethid, mac)) {
		return -1;
	}
	up_sys_set_benv(name[ethid], mac);

	return 0;
}
const char * up_get_mac (int ethid)
{
	static char mac_buf[20];
	static char *name[2] = {"HWADDR0", "HWADDR1"};
	char *mac = NULL;

	if (ethid < 2) {
		mac = up_sys_get_benv(name[ethid]);
	}
	if (mac) {
		strncpy(mac_buf, mac, 17);
	} else {
		strcpy(mac_buf, "00:00:00:00:00:00");
	}
	mac_buf[17] = 0;

	return mac_buf;
}
const char * up_get_netmask (int ethid)
{
	static char *name[2] = {"netmask0", "netmask1"};
	char *netmask = NULL;

	if (ethid < 2) {
		netmask = up_sys_get_benv(name[ethid]);
	}
	if (!netmask) {
		netmask = "255.255.255.0";
	}

	return netmask;
}
const char * up_get_gateway ()
{
	char *gateway;

	gateway = up_sys_get_benv("GATEWAY");
	if (!gateway) {
		gateway = "127.0.0.1";
	}

	return gateway;
}

