
EXTRA_INC = -I./../include

EXEC_FPGA = thfpga
DRV_OBJ_FPGA = host_fpga.o
MINI_EXEC = miniTest

all: $(DRV_OBJ_FPGA) $(EXEC_FPGA) $(MINI_EXEC)

$(DRV_OBJ_FPGA): host_fpga.c host_fpga.h 
	$(CC) $(MODFLAGS) $(CFLAGS) $(EXTRA_INC) -DKBUILD_BASENAME=host_fpga  -c -o  host_fpga.o host_fpga.c
	cp -f $(DRV_OBJ_FPGA) ../output

$(EXEC_FPGA): test_hfpga.c 
	$(CC) $(LDFLAGS) $(EXTRA_INC) -lc -o $(EXEC_FPGA) test_hfpga.c
	cp -f $(EXEC_FPGA) ../output
	
$(MINI_EXEC): miniTest.c
	#$(HOSTCC) $(HOSTCFLAGS) $(EXTRA_INC) -lc -o $(MINI_EXEC) miniTest.c
	$(CC) $(LDFLAGS) $(EXTRA_INC) -lc -o $(MINI_EXEC) miniTest.c
	cp -f $(MINI_EXEC) ../output
	
clean:
	rm -fr *.o *.gdb
	rm -f $(EXEC_FPGA) $(MINI_EXEC) $(DRV_OBJ_FPGA)
