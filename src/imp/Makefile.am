lib_LTLIBRARIES = libimp.la

libimp_la_LDFLAGS = -avoid-version

INCLUDES = \
	-I$(srcdir)/../../include \
	-I$(srcdir)/include \
	-I$(srcdir)/fpga \
	-I$(srcdir)/drivers

AM_CFLAGS =

libimp_la_SOURCES = \
	eeprom.c \
	rs232.c \
	hfpga.c \
	fake_hfpga.c \
	ci.c

libimp_la_LIBADD =

EXTRA_DIST =

SUBDIRS = fpga

