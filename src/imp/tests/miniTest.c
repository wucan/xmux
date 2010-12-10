#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#include "uvdebug_def.h"

#define UV_HAT24C64_DEV_PATH ("/var/nfs/AT24C64")

//#define UV_HAT24C64_DEV_PATH ("/dev/XC3S")

int test001()
{
	char buf[100];

	int nlenrw=100;
	int hdev = open(UV_HAT24C64_DEV_PATH, O_RDWR);
	int i;
	if(-1 == hdev)
	{
		dbg_prt("[TEST-HFPGA-RW] Cannot open HFPGA dev !!! \n ");
		return -1;	
	}
	
	memset(buf,0x00,sizeof(buf));

#if 0
	i=read(hdev,buf,100);

	dbg_prt("i=%d\n",i);
	for(i=0;i<100;i++)
	{
		dbg_prt("%02X ",buf[i]);
		//buf[i]=i;
	}
	dbg_prt("\n");
#endif

	//strcpy(buf,"12345678900000000000000ABCD");
	//memset(buf,0x97,sizeof(buf));

	for(i=0;i<100;i++)
	{
		//dbg_prt("%02X ",buf[i]);
		buf[i]=100-i;
	}



#if 0
	lseek(hdev, 0x00, SEEK_SET);
	i=write(hdev,buf,nlenrw);

	dbg_prt("write_len_i=%d\n",i);
	for(i=0;i<100;i++)
	{
		dbg_prt("%02X ",buf[i]);
		//buf[i]=i;
	}
	dbg_prt("\n");
#endif

	memset(buf,0x00,sizeof(buf));
	lseek(hdev, 0x00, SEEK_SET);

#if 1
	i=read(hdev,buf,nlenrw);
	dbg_prt("read_len_i=%d\n",i);
	for(i=0;i<100;i++)
	{
		dbg_prt("%02X ",buf[i]);
		//buf[i]=i;
	}
	dbg_prt("\n");
#endif
	
	//write();
	close(hdev);
	dbg_prt("111\n");

}

int main(int argc, char *argv[])
{
	unsigned int addr;
	unsigned short data;
	char *endptr;

	while(1)
	{
		test001();
		return 0;
		sleep(2);
	}

	dbg_prt("[TEST-MUX-RW] Read Write Data \n");

	// 8400 0004
	// 8400 0002

        // argv: 0(read)  84000004(address) 
        //       1(write) 84000004(address) 1234(data)

	addr = 0xFFFFFFFF & strtoll(argv[2], &endptr, 16);
	dbg_prt("[TEST-MUX-RW] Echo, Address is: 0x%02x \n", addr);
	
	if(0 == atoi(argv[1]))
	{
		data = *(unsigned short *)addr;	
	}
	else
	{
		data = 0xFFFF & strtol(argv[3], &endptr, 16);
		dbg_prt("[TEST-MUX-RW] Echo, Input Data is: 0x%02x \n", data);

		*(unsigned short *)addr = data;
		data = *(unsigned short *)addr;
	}
	
	dbg_prt("[TEST-MUX-RW] Result: %02x \n", data);

	return 0;	
}

