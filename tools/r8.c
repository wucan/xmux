#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>


#define BASE		0x0

static void usage(char *exec_name)
{
	printf("usage: %s <reg offset> <bytes>\n", exec_name);
}
int main(int argc, char **argv)
{
	unsigned long off, reg;
	unsigned char value;
	int nbytes = 0, dst_bytes = 1;

	if (argc < 2) {
		usage(argv[0]);
		exit(1);
	}
	off = strtoul(argv[1], NULL, 16);
	if (argc == 3)
		dst_bytes = strtoul(argv[2], NULL, 10);

	printf("dump %d bytes @%#x:\n");
	reg = BASE + off;
	while (nbytes < dst_bytes) {
		nbytes++;
		value = *(volatile unsigned char *)reg;
		printf("%02x ", value);
		if (nbytes % 16 == 0)
			printf("\n");
		reg++;
	}
	if (nbytes % 16)
		printf("\n");

	return 0;
}

