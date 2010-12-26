#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>


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
int main(int argc, char **argv)
{
	char mac[6];

	if (argc != 2)
		return -1;
	ether_atoe(argv[1], mac);
	net_ioctl_set_mac(0, mac);

	return 0;
}

