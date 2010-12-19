#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "net.h"


/*
 * set the ethX IP and netmask
 *
 * @ethid: the eth0 is 0, eth1 is 1 ...
 *
 */
int net_set_eth_ip_netmask (int ethid, IPAddress ip, IPAddress netmask)
{
	struct in_addr inaddr;
	char *ipstr, *nmstr;
	char cmd[100];
	int rc;
	int pid;

	inaddr.s_addr = ip;
	ipstr = strdup(inet_ntoa(inaddr));
	inaddr.s_addr = netmask;
	nmstr = inet_ntoa(inaddr);
	sprintf(cmd, "ifconfig eth%d %s netmask %s", ethid, ipstr, nmstr);
	printf("now config net: %s\n", cmd);
	free(ipstr);
	rc = system(cmd);
	if (rc) {
		fprintf(stderr, "exec cmd error: %s\n", cmd);
		return rc;
	}
#if HAD_NIC_1
	/*
	 * reconfig route table, set the eth0 as default
	 */
	pid = fork();
	if (pid > 0) {
		int status;
		printf("wait for ifconfig\n");
		wait(&status);
		printf("ifconfig done\n");
	} else if (pid == 0) {
		/* child */
		if (ethid == 0) {
			system("ifconfig eth1 down");
			system("ifconfig eth1 up");
		}
		/* sleep for system stable */
		rc = system("sleep 5");
		if (rc) {
			printf("system sleep 5 error: %d\n", rc);
		}
		exit(1);
	}
#endif

	return rc;
}

/*
 * set gateway
 */
int net_set_gateway (IPAddress gateway)
{
	struct in_addr inaddr;
	char *gwstr;
	char cmd[100];
	int rc;

	inaddr.s_addr = gateway;
	gwstr = strdup(inet_ntoa(inaddr));
	sprintf(cmd, "route add default gw %s", gwstr);
	printf("now config gateway: %s\n", cmd);
	free(gwstr);
	rc = system(cmd);
	if (rc) {
		fprintf(stderr, "exec cmd error: %s\n", cmd);
		return rc;
	}

	return rc;
}
int net_set_mac (int ethid, char *mac)
{
	char cmd[100];
	int rc = 0;

	/*
	 * check the mac
	 */
	if (strlen(mac) < 17) {
		return -1;
	}
	mac[17] = 0;
	if (mac[0] != '0' || mac[1] != '0') {
		printf("mac %s invalidate!\n", mac);
		return -1;
	}
	/*
	 * frist down the nic
	 */
	sprintf(cmd, "ifconfig eth%d down", ethid);
	system(cmd);
	/*
	 * then set it's mac
	 */
	sprintf(cmd, "ifconfig eth%d hw ether %s", ethid, mac);
	printf("eth%d: set it's mac: %s\n", ethid, mac);
	rc = system(cmd);
	if (rc) {
		printf("failed to run: %s\n", cmd);
	}
	/*
	 * at last, up the nic
	 */
	sprintf(cmd, "ifconfig eth%d up", ethid);
	system(cmd);

	return rc;
}

