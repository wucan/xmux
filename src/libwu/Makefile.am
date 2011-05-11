lib_LTLIBRARIES = libwu.la

libwu_la_LDFLAGS = -avoid-version

INCLUDES = \
	-I$(srcdir)/../../include \
	-I$(top_srcdir)/include/wu

AM_CFLAGS =

libwu_la_SOURCES = \
	lock.c \
	wuswait.c \
	message.c \
	bget.c \
	mem.c \
	wulist.c \
	wutree.c \
	wufs.c \
	thread.c \
	message_queue.c \
	dict.c \
	wu_udp.c

libwu_la_LIBADD = -lpthread -lm

EXTRA_DIST =

