#include "wu/wu_base_type.h"

#include "kernel_type.h"
#include "pci.h"


#define CI_IO_BASE			0xC8000000


bool ci_card_actived()
{
	if (sPCCARDCON.ACT)
		return true;

	return false;
}

bool ci_io_write_u8(int off, uint8_t v)
{
	if (!ci_card_actived())
		return -1;

	*(volatile uint8_t *)(CI_IO_BASE + off) = v;

	return false;
}

uint8_t ci_io_read_u8(int off)
{
	if (!ci_card_actived())
		return 0;

	return *(volatile uint8_t *)(CI_IO_BASE + off);
}

