noinst_PROGRAMS = gen-eeprom

gen_eeprom_CPPFLAGS = \
	-I $(srcdir) \
	-I $(top_srcdir)/include \
	-I $(top_srcdir)/src \
	$(AM_CPPFLAGS)

gen_eeprom_LDADD = \
	../src/libwu/libwu.la

gen_eeprom_SOURCES = gen-eeprom.c \
	../src/pid_map_rule.c

