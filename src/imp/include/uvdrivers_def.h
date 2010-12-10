// uvdrivers_def.h

#ifndef _UVDRIVERS_DEF_H
#define _UVDRIVERS_DEF_H

#define UV_MODULE

unsigned short READ_REGISTER_USHORT(unsigned short *addr)
{
   return *addr;
}

void WRITE_REGISTER_USHORT(unsigned short *addr, unsigned short data)
{
   *addr = data;
}


#endif
