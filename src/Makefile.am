bin_PROGRAMS = xmux

#NETSNMP_LIBS = -lnetsnmpagent -lnetsnmphelpers -lnetsnmp
NETSNMP_LIBS =

#NETSNMP_SRC = netsnmp.c
NETSNMP_SRC =

AM_CFLAGS = $(CFLAGS) $(DVBPSI_CFLAGS) \
	-I $(top_srcdir)/include \
	-I $(srcdir)/imp \
	-I $(srcdir)/imp/include \
	-I $(srcdir)/imp/fpga \
	-I $(srcdir)/imp/drivers \
	-I $(srcdir)/imp/uvPSI/include

if BUILD_FOR_ARM
xmux_LDADD = \
	libci/libci.la \
	libtunner/libtunner.la \
	imp/libimp.la \
	imp/fpga/libimpfpga.la \
	$(top_srcdir)/extern-arm-libs/libuvpsi.a \
	$(NETSNMP_LIBS) \
	libwu/libwu.la \
	-lm -lpthread
else
xmux_LDADD = $(NETSNMP_LIBS) -lm \
	libtunner/libtunner.la \
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
	ctrl_mcu.c \
	xmux_4_chan_rf.c \
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
	io_table.c \
	xmux_snmp.c \
	wu_snmp_agent.c \
	wu_snmp_lib.c \
	$(NETSNMP_SRC) \
	oids.c \
	xmux.c \
	up_sys.c \
	up_support.c \
	net.c \
	xmux_net.c \
	xmux_misc.c \
	xmux_tunner.c \
	xmux_bcm3033.c \
	xmux_ci.c \
	$(SIM_SRC) \
	$(NULL_FILE)

if CHANNEL_1
xmux_SOURCES += \
	xmux_tunner_device.c \
	$(NULL_FILE)
endif

SUBDIRS = imp libwu libci libtunner

