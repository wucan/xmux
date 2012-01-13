lib_LTLIBRARIES = libtunner.la

libtunner_la_LDFLAGS = -avoid-version

INCLUDES = \
	-I$(srcdir)/../../include \
	-I$(top_srcdir)/include/ci

AM_CFLAGS =

libtunner_la_SOURCES = \
	FE_Sat_Tuner.c SAT_NULL_Tuner.c STFE_utilities.c \
	STV0903_Drv.c STV0903_Init.c STV0903_tst.c \
	STV6110_Tuner.c STV0903_Drv._c tunerinterface.c \
	chip.c

libtunner_la_LIBADD = -lpthread

EXTRA_DIST =

