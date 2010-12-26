noinst_PROGRAMS = gen-eeprom setmac

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

