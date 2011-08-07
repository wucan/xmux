lib_LTLIBRARIES = libci.la

libci_la_LDFLAGS = -avoid-version

INCLUDES = \
	-I$(srcdir)/../../include \
	-I$(top_srcdir)/include/ci

AM_CFLAGS =

libci_la_SOURCES = \
	ci_ctrl.c \
	ci_io_interface.c \
	cipmt.c \
	libci.c \
	query_ci.c \
	read_ci.c \
	send_xpdu.c \
	xpdu_parser.c \
	fixup.c

libci_la_LIBADD = -lpthread

EXTRA_DIST =

