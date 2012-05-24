#include "wu/wu_base_type.h"

#include "kernel_type.h"
#include "pci.h"


#define CI_ATTR_MEM_BASE	0xC0000000
#define CI_MEM_BASE			0xC4000000
#define CI_IO_BASE			0xC8000000

static inline void delay()
{
	int i;

	for (i = 0; i < 1024; i++) {
		;;;
	}
}

bool ci_card_actived()
{
#ifndef _UCLINUX_
	return false;
#endif

	if (sPCCARDCON.ACT)
		return true;

	return false;
}

uint16_t ci_attr_mem_read_u16(int off)
{
	delay();
	if (!ci_card_actived())
		return 0;

	return *(volatile uint16_t *)(CI_ATTR_MEM_BASE + off);
}

bool ci_io_write_u8(int off, uint8_t v)
{
	delay();
	if (!ci_card_actived())
		return false;

	*(volatile uint8_t *)(CI_IO_BASE + off) = v;

	return true;
}

uint8_t ci_io_read_u8(int off)
{
	delay();
	if (!ci_card_actived())
		return 0;

	return *(volatile uint8_t *)(CI_IO_BASE + off);
}

bool ci_mem_write_u16(int off, uint16_t v)
{
	delay();
	if (!ci_card_actived())
		return false;

	*(volatile uint16_t *)(CI_MEM_BASE + off) = v;

	return true;
}

uint16_t ci_mem_read_u16(int off)
{
	delay();
	if (!ci_card_actived())
		return 0;

	return *(volatile uint16_t *)(CI_MEM_BASE + off);
}

