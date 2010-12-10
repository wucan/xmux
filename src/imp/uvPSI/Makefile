# Linux based PSI/SI Gen and Anly


CEXTRAFLAGS = -DDVBPSI_DIST

CINC = -I ./include -I ./../fpga
CLINC =


SCR_INC = 
SCR_LINC =

//CFLAGS := -O2 -g -Wall -I${TOPDIR}/include -I${TOPSNAPGEAR}/lib/uClibc/include $(CINC)
CFLAGS := -O2 -g -Wall -I$(SNAPGEAR_DIR)/linux-2.4.x/include  -I$(SNAPGEAR_DIR)/lib/uClibc/include $(CINC)


DESCRIPTOR_SRC =  descriptors/dr_02.c \
                  descriptors/dr_03.c \
                  descriptors/dr_04.c \
                  descriptors/dr_05.c \
                  descriptors/dr_06.c \
                  descriptors/dr_07.c \
                  descriptors/dr_08.c \
                  descriptors/dr_09.c \
                  descriptors/dr_0a.c \
                  descriptors/dr_0b.c \
                  descriptors/dr_0c.c \
                  descriptors/dr_0d.c \
                  descriptors/dr_0e.c \
                  descriptors/dr_0f.c \
                  descriptors/dr_42.c \
                  descriptors/dr_47.c \
                  descriptors/dr_48.c \
                  descriptors/dr_59.c

DESCRIPTOR_OBJ =  descriptors/dr_02.o \
                  descriptors/dr_03.o \
                  descriptors/dr_04.o \
                  descriptors/dr_05.o \
                  descriptors/dr_06.o \
                  descriptors/dr_07.o \
                  descriptors/dr_08.o \
                  descriptors/dr_09.o \
                  descriptors/dr_0a.o \
                  descriptors/dr_0b.o \
                  descriptors/dr_0c.o \
                  descriptors/dr_0d.o \
                  descriptors/dr_0e.o \
                  descriptors/dr_0f.o \
                  descriptors/dr_42.o \
                  descriptors/dr_47.o \
                  descriptors/dr_48.o \
                  descriptors/dr_59.o

TABLE_SRC  =   	tables/pat.c  \
               	tables/pmt.c  \
	       	tables/sdt.c  \
		tables/cat.c  \
		tables/nit.c  \
		tables/eit.c  

TABLE_OBJ  =   	tables/pat.o  \
               	tables/pmt.o  \
	       	tables/sdt.o  \
		tables/cat.o  \
		tables/nit.o  \
		tables/eit.o

PSI_SRC  =   psi/descriptor.c  	 \
             psi/demux.c  	 \
	     psi/dvbpsi.c        \
             psi/psi.c           \
	     psi/gen_dvb_si.c

PSI_OBJ  =   psi/descriptor.o  	 \
             psi/demux.o  	 \
	     psi/dvbpsi.o        \
             psi/psi.o           \
	     psi/gen_dvb_si.o

SRCS = ${DESCRIPTOR_SRC} ${TABLE_SRC}  ${PSI_SRC}
SRCS_OBJ = ${DESCRIPTOR_OBJ} ${TABLE_OBJ}  ${PSI_OBJ}

#.o.c:
#	$(CC) $(CFLAGS) -c -o $@ $<

LIB_UVPSI = libuvpsi.a
$(LIB_UVPSI): $(SRCS_OBJ)
	$(AR) rc $@ $^
	cp -f $(LIB_UVPSI) ../output

#.c.a:
#	$(CC) $(CFLAGS) -c $< -o $*.o 
#	$(AR) rc $@ $*.o

#APP_DPAT = examples/decode_pat.c
#APP_EXE_DPAT = decode_pat
#${APP_EXE_DPAT}: ${APP_DPAT} $(LIB_UVPSI)
#	$(CC) $(LDFLAGS) -lc -L./  ${CEXTRAFLAGS} ${CINC} -o ${APP_EXE_DPAT} $< -luvpsi
	
#APP_TEST_DR = examples/test.c
#APP_EXE_TEST_DR = test
#${APP_EXE_TEST_DR}: ${APP_TEST_DR} $(LIB_UVPSI)
#	$(CC) $(LDFLAGS) -lc -L./  ${CEXTRAFLAGS} ${CINC} -o ${APP_EXE_TEST_DR} $< -luvpsi
	
#all: ${APP_EXE_DPAT} ${APP_EXE_TEST_DR}
all: $(LIB_UVPSI)		

clean:
	rm -fr *.o *.lo
	rm -fr *.a *.la
	rm -fr *.so  *.gdb 
	rm -fr ${APP_EXE_DPAT} ${APP_EXE_DPMT} ${APP_EXE_DSDT} 		\
			${APP_EXE_GPAT} ${APP_EXE_GPMT} ${APP_EXE_GCRC}		\
			 ${APP_EXE_TEST_DR}
	find . -name "*.o" | xargs rm -f

 
