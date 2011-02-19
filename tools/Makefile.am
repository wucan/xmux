if BUILD_FOR_ARM
noinst_PROGRAMS = setmac rr wr pcmcia
else
noinst_PROGRAMS = gen-eeprom setmac rr wr
endif

gen_eeprom_CPPFLAGS = \
	-I $(srcdir) \
	-I $(top_srcdir)/include \
	-I $(top_srcdir)/src \
	$(AM_CPPFLAGS)

gen_eeprom_LDADD = \
	../src/libwu/libwu.la

gen_eeprom_SOURCES = gen-eeprom.c \
	../src/pid_map_rule.c

setmac_CPPFLAGS = \
	-I $(srcdir) \
	-I $(top_srcdir)/include \
	-I $(top_srcdir)/src \
	$(AM_CPPFLAGS)

setmac_LDADD =

setmac_SOURCES = setmac.c

rr_SOURCES = rr.c
wr_SOURCES = wr.c
pcmcia_SOURCES = pcmcia.c

