#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>

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
/*
 * set new Media Control Address(MAC) through ioctl
 */
int net_ioctl_set_mac(int ethid, char *mac)
{
	int sk, rc;
	struct ifreq ifr;

	sk = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sk < 0) {
		perror("socket");
		return -1;
	}
	sprintf(ifr.ifr_name, "eth%d", ethid);
	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
	memcpy(ifr.ifr_hwaddr.sa_data, mac, 6);
	rc = ioctl(sk, SIOCSIFHWADDR, &ifr);
	if (rc < 0) {
		perror("SIOCSIFHWADDR");
	}
	close(sk);

	return rc;
}
/*
 * get Media Control Address(MAC)
 */
int net_ioctl_get_mac(int ethid, char *mac)
{
	int sk, rc;
	struct ifreq ifr;

	sk = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sk < 0) {
		perror("socket");
		return -1;
	}
	sprintf(ifr.ifr_name, "eth%d", ethid);
	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
	rc = ioctl(sk, SIOCGIFHWADDR, &ifr);
	if (rc < 0) {
		perror("SIOCSIFHWADDR");
	} else {
		memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);
	}
	close(sk);

	return rc;
}
/*
 * Convert Ethernet address string representation to binary data
 * @param    a    string in xx:xx:xx:xx:xx:xx notation
 * @param    e    binary data
 * @return    TRUE if conversion was successful and FALSE otherwise
 */
int ether_atoe(const char *a, unsigned char *e)
{
    char *c = (char *) a;
    int i = 0;

    memset(e, 0, IFHWADDRLEN);
    for (;;) {
        e[i++] = (unsigned char) strtoul(c, &c, 16);
        if (!*c++ || i == IFHWADDRLEN)
            break;
    }

    return (i == IFHWADDRLEN);
}
/*
 * Convert Ethernet address binary data to string representation
 * @param    e    binary data
 * @param    a    string in xx:xx:xx:xx:xx:xx notation
 * @return    a
 */
char *ether_etoa(const unsigned char *e, char *a)
{
    char *c = a;
    int i;

    for (i = 0; i < IFHWADDRLEN; i++) {
        if (i)
            *c++ = ':';
        c += sprintf(c, "%02X", e[i] & 0xff);
    }

    return a;
}

