
#ifndef _rs232_H_
#define _rs232_H_

int openport(const char *dev_path);

int ttyS1_recv(int fdttyS1, char* buf,int* nlen);

#endif


