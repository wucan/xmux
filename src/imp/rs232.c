#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>


#include "rs232.h"


static int setport(int fd, int baud, int databits, int stopbits, int parity) 
{
    int baudrate; //, status;
    struct termios newtio;
	int nCount;
	int i;

	int arrbps[]=
	{
		300,B300,600,B600,1200,B1200,2400,B2400,4800,B4800,9600,B9600,19200,B19200,38400,B38400,115200,B115200,
	};

	nCount=sizeof(arrbps)/sizeof(int);

	baudrate=baud;
	for(i=0;i<nCount/2;i++)
	{
		if(baud!=arrbps[i*2]) continue;
		baudrate=arrbps[i*2+1];
		break;
	}

	baudrate=baud;

    tcgetattr(fd, &newtio);
    bzero(&newtio, sizeof (newtio));
    //setting   c_cflag
    newtio.c_cflag &= ~CSIZE;
    switch (databits) /*set data bits*/ {
        case 7:
            newtio.c_cflag |= CS7; //7bits' data
            break;
        case 8:
            newtio.c_cflag |= CS8; //8bits'
            break;
        default:
            newtio.c_cflag |= CS8;
            break;
    }
    switch (parity) //set parity
    {
        case 'n':
        case 'N':
            newtio.c_cflag &= ~PARENB; // Clear parity enable
            newtio.c_iflag &= ~INPCK; // Enable parity checking
            break;
        case 'o':
        case 'O':
            newtio.c_cflag |= (PARODD | PARENB); //set odd check
            newtio.c_iflag |= INPCK; //Disnable parity checking
            break;
        case 'e':
        case 'E':
            newtio.c_cflag |= PARENB; //Enable parity
            newtio.c_cflag &= ~PARODD; //change to odd
            newtio.c_iflag |= INPCK; //Disnable parity checking
            break;
        case 'S':
        case 's': //as no parity
            newtio.c_cflag &= ~PARENB;
            newtio.c_cflag &= ~CSTOPB;
            break;
        default:
            newtio.c_cflag &= ~PARENB; // Clear parity enable */
            newtio.c_iflag &= ~INPCK; // Enable parity checking */
            break;
    }
    switch (stopbits)//set stop bits
    {
        case 1:
            newtio.c_cflag &= ~CSTOPB; //1bit
            break;
        case 2:
            newtio.c_cflag |= CSTOPB; //2bits
            break;
        default:
            newtio.c_cflag &= ~CSTOPB;
            break;
    }
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    newtio.c_cflag |= (CLOCAL | CREAD);
    newtio.c_oflag |= OPOST;
    newtio.c_iflag &= ~(IXON | IXOFF | IXANY);
    cfsetispeed(&newtio, baudrate);
    cfsetospeed(&newtio, baudrate);
    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &newtio) != 0) {
        perror("SetupSerial 3");
        return -1;
    }


    return 0;
}

static void clearport(int fd) //if found data error, call the function to refresh com
{
    tcflush(fd, TCIOFLUSH);
}

int openport(const char *dev_path, int baud)
{
	int fdttyS1 = -1;
	if(fdttyS1<=0)
	{
		fdttyS1 = open(dev_path,O_RDWR );
		if (-1 == fdttyS1) 
		{
			perror("Can't Open Serial Port");
			return -1;
		}
		printf("init rs232 for rf successfully.\n");

		//set serial port:baud,databits,stopbits,parity
		if (setport(fdttyS1, baud, 8, 1, 'n') < 0)
		{
			printf("rf: Can't Set Serial Port!\n");
			return -1;
		}
		printf("set rs232 for rf successfully.\n");
	}

	return fdttyS1;
}



int ttyS1_recv(int fdttyS1, char* rbuf,int* nlen)
{
	int nret=-1;
	int  rc;

	int nstatus=0;
	//memset( rbuf,0x00,sizeof(rbuf) );

	usleep(500);
	rc = read(fdttyS1, rbuf, 3);
	if ( rc== 3 && rbuf[0] ==0xC0 && rbuf[2]<=0x08 )
	{
		int len=0;
		int i;
		while( len<rbuf[2] )
		{
			usleep(500);
			rc = read(fdttyS1, &rbuf[3+len], rbuf[2]-len);
			if( rc<0 )
			{
				nstatus=-1;
				break;
			}
			else
			{
				len+=rc;
			}
		}
		*nlen=len+3;

		printf("recv %d \nrfbuf=%s\n",rc,rbuf);
		for(i=0;i<rbuf[2]+3;i++) printf("0x%02x ",rbuf[i]);
		printf("\nrecv end\n");
	}

	clearport(fdttyS1);
	
	if(nstatus!=0)
	{
		printf("recv err rc=%d\n",rc);
	}
	else
	{
		nret=0;
	}

	return nret;

}


////////////////////////////////////////////////////////////////////////////////
//cht ver2
//io15 is the switch of io2-io5, LOW enable
//io0:  not used;
//io1:  restore to default values, HIGH enable
//io2:  LED1's red led,High enable
//io3:  LED1's green led, High enable
//io4:  LED2's red led, High enable
//io5:  LED2's green led, High enable
//io8:  PIR input pin, LOW has pir input
//io10: beep, High beep
//io12: reset pin, LOW has input

/*int InitHomeVersion2Gpio() {
    int ret;
    ret = GpioEnable(1);
    ret |= SetGpio(8, SIO_DIR_IN, 0);
    ret |= SetGpio(8, SIO_INTR_SET_DOUBLEEDGE, 0);
    return ret;
}*/
