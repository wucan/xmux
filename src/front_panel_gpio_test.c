#include <stdio.h>
#include <stdlib.h>


static void listitem()
{
	printf("\n== GPIO test ==\n");
	printf("1. OPEN SOUND AND LIGHT ALARM \n");
	printf("2. CLOSE SOUND AND LIGHT ALARM \n");
	printf("3. CLOSE MCU WATCHDOG  \n");
	printf("4. Exit Test\n");
	printf("Please enter the choice>>\n");
}
void front_panel_gpio_test(int fdttyS1)
{
	int test_over = 0;

	while (!test_over) {
		unsigned char rbuf[256];
		int i;
		listitem();

		if (scanf("%d", &i) == 0) {
			scanf("%*[\n]");
			getchar();
			continue;
		}

		switch (i) {
		case 1:
			rbuf[0] = 0xC0;
			rbuf[1] = 0xFA;
			rbuf[2] = 0x04;
			rbuf[3] = 0xA1;
			rbuf[4] = 0xA2;
			rbuf[5] = 0xA3;
			rbuf[6] = 0xA4;

			rbuf[0] = 0x30;
			rbuf[1] = 0x31;
			rbuf[2] = 0x32;
			rbuf[3] = 0x33;
			rbuf[4] = 0x34;
			rbuf[5] = 0x35;
			rbuf[6] = 0x36;

			write(fdttyS1, rbuf, 7);
			break;
		case 2:
			rbuf[0] = 0xC0;
			rbuf[1] = 0xFC;
			rbuf[2] = 0x04;
			rbuf[3] = 0xA1;
			rbuf[4] = 0xA2;
			rbuf[5] = 0xA3;
			rbuf[6] = 0xA4;

			write(fdttyS1, rbuf, 7);
			break;
		case 3:				//Set Pin Direction
			rbuf[0] = 0xC0;
			rbuf[1] = 0x13;
			rbuf[2] = 0x00;
			write(fdttyS1, rbuf, 3);
			break;
		case 4:
			test_over = 1;
			break;
		case 5:
			break;
		default:
			break;
		}
	}
}

