#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>


//#define REG_BASE		0xF0110000
#define REG_BASE		0x0

static void usage(char *exec_name)
{
	printf("usage: %s <reg offset> <value>\n", exec_name);
}
int main(int argc, char **argv)
{
	unsigned int reg, off;
	unsigned short value;

	if (argc < 3) {
		usage(argv[0]);
		exit(1);
	}
	off = strtoul(argv[1], NULL, 16);
	reg = REG_BASE + off;
	value = strtoul(argv[2], NULL, 16);
	*(volatile unsigned short *)reg = value;
	printf("@%#x : %#x\n", reg, value);

	return 0;
}

