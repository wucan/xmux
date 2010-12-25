bin_PROGRAMS = xmux

NETSNMP_LIBS = -lnetsnmpagent -lnetsnmphelpers -lnetsnmp

AM_CFLAGS = $(CFLAGS) $(DVBPSI_CFLAGS) \
	-I $(top_srcdir)/include \
	-I $(srcdir)/imp \
	-I $(srcdir)/imp/include \
	-I $(srcdir)/imp/fpga \
	-I $(srcdir)/imp/drivers \
	-I $(srcdir)/imp/uvPSI/include

if BUILD_FOR_ARM
xmux_LDADD = \
	imp/libimp.la \
	imp/fpga/libimpfpga.la \
	$(top_srcdir)/extern-arm-libs/libuvpsi.a \
	$(NETSNMP_LIBS) \
	libwu/libwu.la \
	-lm -lpthread
else
xmux_LDADD = $(DVBPSI_LIBS) $(NETSNMP_LIBS) -lm \
	imp/libimp.la \
	imp/fpga/libimpfpga.la \
	libwu/libwu.la \
	imp/uvPSI/libuvpsi.a \
	$(gtk_LIBS)

AM_CFLAGS += $(gtk_CFLAGS)

SIM_SRC = sim_fp.c
endif


NULL_FILE =

if HAVE_DVBPSI
dvbpsi_files = 
endif

xmux_SOURCES = \
	main.c \
	$(dvbpsi_files) \
	front_panel.c \
	front_panel_gpio_test.c \
	front_panel_process.c \
	front_panel_data_churning.c \
	front_panel_psi_gen.c \
	front_panel_psi_parse.c \
	section.c \
	psi_worker.c \
	psi_parse.c \
	psi_gen.c \
	output_psi_data.c \
	pid_trans_info.c \
	xmux_config.c \
	pid_map_table.c \
	pid_map_rule.c \
	program_pid_validater.c \
	xmux_snmp.c \
	wu_snmp_agent.c \
	xmux.c \
	up_sys.c \
	up_support.c \
	net.c \
	xmux_net.c \
	xmux_misc.c \
	$(SIM_SRC) \
	$(NULL_FILE)

SUBDIRS = imp libwu

