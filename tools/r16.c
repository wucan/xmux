#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "kernel_type.h"
#include "pci.h"


#define BASE		0x0

#define printk		printf
#define __init

int SMDK2510_CardBus_Setup(void);

static void usage(char *exec_name)
{
	printf("usage: %s <reg offset>\n", exec_name);
}
int main(int argc, char **argv)
{
	unsigned long off, reg;
	unsigned short value;

	if (argc < 2) {
		usage(argv[0]);
		exit(1);
	}
	off = strtoul(argv[1], NULL, 16);
	reg = BASE + off;
	value = *(volatile unsigned short *)reg;
	printf("read 0x%08x -> 0x%04x\n", reg, value);

	return 0;
}

void __init PCI_Delay(unsigned long x)
{
	while(--x);
}

/* 
 * CardBus related functions. 
 */

#define DEASSERT_LOGIC_RESET    (sPCIRCC.RSL=1)
#define DEASSERT_RESET  (sPCIRCC.RSB=1)

#define CHECK_CCD   ((sPCCARDPRS.CD1 && sPCCARDPRS.CD2)? 0:1) 
#define     TYPE_CARDBUS    0x1

#define     THREE_VOLTAGE   0x33
#define     FIVE_VOLTAGE    0x50

#define PROVIDE_VCC(n) { sPCCARDCON.VCC=0; if(n==THREE_VOLTAGE) sPCCARDCON.VCC=3; else if (n==FIVE_VOLTAGE) sPCCARDCON.VCC=2;}

int SMDK2510_CardBus_Setup(void)
{
    unsigned int flags;

    /* STEP 1. internal arbiter ON, auto-adress translation ON */
    sPCICON.ARB=1;
    sPCICON.ATS=1;
    sPCICON.RDY =1;

    /* STEP 2. clock UN-mask, auto-clkrun signal */
    //sPCIDIAG0.EXC=1;    /* use external clock */
    sPCIDIAG0.EXC=0;    /* use internal clock */
    sPCIDIAG0.NPU=1;
    sPCIDIAG0.X16=1;

    sPCIRCC.MSK=0;
    sPCIRCC.M33=0;
    sPCIRCC.ACC=1;

    /* STEP 3 assert pci logic reset */
    sPCIRCC.RSL=1;

    /*
     * STEP 4 setting interrupt-related registers 
     * event mask (0->mask, 1->Unmask)
     * all is unmasked
     * and then pc-card event interrupt enable, all pci interrupt enable also.
     */
    sPCCARDEVM.STC=1;
    sPCCARDEVM.CD1=1;
    sPCCARDEVM.CD2=1;
    sPCCARDEVM.PWC=1;

    sPCIINTEN.PME=1;

	/* mask value to cover SDRAM size (32M == 0x2000000) */
    rPCIBAM0 = 0xfe000000;

    /*
     * 1. test whether cards are already present or not
     * 2. apply Vcc
     * 3. release bus & logic reset
     * 4. enable pc-card interrupts
     * 5. set configuration done
     */


    if(!CHECK_CCD || !sPCCARDPRS.C32) {
        printk( "No card, or not CardBus card.\n");
        if (!sPCCARDPRS.C16)
            return -1;
        printk("16BIT PC Card(PCMCIA) found.\n");
    }

    printk("CardBus Card is detected: ");

    /* apply appropriate Vcc for card */
    printk("apply Vcc for card\n");
    if(sPCCARDPRS.C3V)
    {
    	printk("3V\n");
        PROVIDE_VCC(THREE_VOLTAGE);
        printk("3 Voltage Card\n");
    }
    else if(sPCCARDPRS.C5V)
    {
    	printk("5V\n");
        PROVIDE_VCC(FIVE_VOLTAGE);
        printk("5 Voltage Card\n");
    }


    /* wait for card active state */
    printk("wait for card activate\n");
    while(!sPCCARDCON.ACT)
        ;

    /* deassert logic & bus reset */
    printk("deassert logic & bus reset\n");
    DEASSERT_LOGIC_RESET;
    DEASSERT_RESET;

    printk("wait for target ready...\n");
	PCI_Delay(13300000);    // to wait for target to ready  
    printk("target ready.\n");

    /* all pc-card interrupts enable */
    printk("enable all pc-card interrupts.\n");
    rPCIINTST = 0xffffffff;
    rPCIINTEN = 0xffffffff;

    /* configuration done */
    printk("configuration done, mark it\n");
    sPCICON.CFD=1;

    /* initialize BARs */
    printk("initialize BARS\n");
	// comment this to remove the kernel boot ooops!
    //sPCIHSC.CMD=0x7;

    printk("%s done\n", __func__);
	return 0;
}


