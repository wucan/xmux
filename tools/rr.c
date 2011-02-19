#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>


#define BASE		0xF0110000

static void read_range(unsigned long start_off, unsigned long end_off);

static void usage(char *exec_name)
{
	printf("usage: %s <reg offset>\n", exec_name);
}
int main(int argc, char **argv)
{
	/*
	 * read range register
	 */
	if (argc == 3) {
		unsigned long start_off, end_off;
		start_off = strtoul(argv[1], NULL, 16);
		end_off = strtoul(argv[2], NULL, 16);
		read_range(start_off, end_off);
	}

	/*
	 * read single register
	 */
	if (argc == 2) {
		unsigned long off, reg, value;
		off = strtoul(argv[1], NULL, 16);
		reg = BASE + off;
		value = *(volatile unsigned long *)reg;
		printf("read 0x%08x -> 0x%08x\n", reg, value);
	}

	usage(argv[0]);

	return 0;
}
static void read_range(unsigned long start_off, unsigned long end_off)
{
	unsigned long off, reg, value;

	for (off = start_off; off < end_off; off += 4) {
		reg = BASE + off;
		value = *(volatile unsigned long *)reg;
		if (value != 0xffffffff)
			printf("read 0x%08x -> 0x%08x\n", reg, value);
	}
}

