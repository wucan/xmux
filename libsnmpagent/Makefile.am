#base on libXi/src/Makefile.am

lib_LTLIBRARIES = libsnmpagent.la

libsnmpagent_la_SOURCES = \
	snmp_agent_register.c \
	snmp_data_oidops.c \
	up_mib_wrap.c \
	snmp_agent.c \
	snmp_agent_entry.c \
	up_mib.c \
	oid_mib_load.c \
	none_leaf_oidops.c

libsnmpagent_la_LIBADD = $(NETSNMP_LIBS)

AM_CFLAGS = -I$(top_srcdir)/include \
            $(NETSNMP_CFLAGS)


libsnmpagent_la_LDFLAGS = -version-number 0:0:1 -no-undefined

libsnmpagentincludedir = 
libsnmpagentinclude_HEADERS = snmpagent.h

EXTRA_DIST =
