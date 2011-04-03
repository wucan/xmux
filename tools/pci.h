
/*
 * ./include/asm-armnommu/arch-samsung/SMDK2510/pci.h
 *   pci header file  for SMDK2510
 *  edited by Roh you-chang
 *  
 *  Copyright (C) 2002-2004 SAMSUNG ELECTRONICS
 * 
 */

            
// Title	: PCI Header File
// Logic	: PCI Host or Agent Interface
//		  PC Card Host or Agent Interface
//		  - CardBus Host or Agent Interface
//		  - 16-bit PC Card (PCMCIA) Host Interface
// Engineer	: LECOS (Lee, Cheon-su)
//		  E-mail: lecos@samsung.co.kr or lecos@hanmir.com

#ifndef __ASM_ARCH_PCI_H
#define __ASM_ARCH_PCI_H
/*********************************************************************
 * typedefs
 *********************************************************************/
#ifndef CONFIG_PCI
typedef unsigned int	UINT;
typedef	unsigned long	ULONG;
typedef unsigned long	UINT32,U32;
typedef unsigned short	UINT16,U16;
typedef unsigned char	UINT8,U8;
typedef	unsigned char	UCHAR;
#endif
//----- Definition of PC Card Standard

// PCCARDCON[VPP, VCC] 
#define	VCCPP_OFF	0	// 3'b000 = Power off
#define	VCCPP_12V	1	// 3'b001 = 12V (only VPP)
#define	VCCPP_5V	2	// 3'b010 = 5V
#define	VCCPP_3_3V	3	// 3'b011 = 3.3V
#define	VCCPP_X_XV	4	// 3'b100 = X.XV
#define	VCCPP_Y_YV	5	// 3'b101 = Y.YV

#ifdef INT
#undef INT
#endif

//----- Basic Macro Definitions

#define	r32(addr)	(*(volatile unsigned long*)(addr))
#define	r16(addr)	(*(volatile unsigned short*)(addr))
#define	r8(addr)	(*(volatile unsigned char* )(addr))

#define	co_r32(addr)	(*(unsigned long*)(addr))
#define	co_r16(addr)	(*(unsigned short*)(addr))
#define	co_r8(addr)	(*(unsigned char* )(addr))



//----------------------------------------------------------------------------
//					Declarations for PCI SFR (Address)
#define	MEM_OFF		0x00000000	// bit [28:0]
#define	CFG0_OFF	0x10000000
#define	CFG1_OFF	0x14000000
#define	SPEC_OFF	0x18000000
#define	IO_OFF		0x1C000000

#define	BIF_OFF		0x100		// bit [8:0]
#define	PDMA0_OFF	0x180
#define	PDMA1_OFF	0x1A0
#define	PDMACH_OFF	0x020		// (PDMA1_OFF-PDMA0_OFF)

#define	PCIHID_OFF	0x00
#define	PCIHVID_OFF 0x00
#define	PCIHDID_OFF 0x02
#define	PCIHSC_OFF	0x04
#define	PCIHCMD_OFF	0x04
#define	PCIHST_OFF	0x06
#define	PCIHCODE_OFF	0x8
#define	PCIHREV_OFF	0x08
#define	PCIHCLASS_OFF	0x09
#define	PCIHLINE_OFF	0x0C
#define	PCIHLATTIMER_OFF	0x0d
#define	PCIHHEADER_OFF 0x0e
#define	PCIHBIST_OFF	0x0f
#define	PCIHBAR0_OFF	0x10
#define	PCIHBAR1_OFF	0x14
#define	PCIHBAR2_OFF	0x18
#define	PCIHBAR3_OFF	0x1C
#define	PCIHBAR4_OFF	0x20
#define	PCIHBAR5_OFF	0x24
#define	PCIHCISP_OFF	0x28
#define	PCIHSSID_OFF	0x2C
#define	PCIHSSVID_OFF	0x2e
#define	PCIHEXPR_OFF	0x30	// reserved region
#define	PCIHCAP_OFF	0x34
#define	PCIHLTIT_OFF	0x3C
#define	PCIHINTLINE_OFF	0x3c
#define	PCIHINTPIN_OFF	0x3D
#define	PCIHMINGNT_OFF	0x3E
#define	PCIHMAXLAT_OFF	0x3F
#define	PCIHTIMER_OFF	0x40
#define	PCIHPMR0_OFF	0xDC
#define	PCIHPMR1_OFF	0xE0

#define	PCICON_OFF	0x00
#define	PCISET_OFF	0x04
#define	PCIINTEN_OFF	0x08
#define	PCIINTST_OFF	0x0C
#define	PCIINTAD_OFF	0x10
#define	PCIBATAPM_OFF	0x14
#define	PCIBATAPI_OFF	0x18
#define	PCIRCC_OFF	0x1C
#define	PCIDIAG0_OFF	0x20
#define	PCIDIAG1_OFF	0x24
#define	PCIBELAP_OFF	0x28
#define	PCIBELPA_OFF	0x2C
#define	PCIMAIL0_OFF	0x30
#define	PCIMAIL1_OFF	0x34
#define	PCIMAIL2_OFF	0x38
#define	PCIMAIL3_OFF	0x3C
#define	PCIBATPA0_OFF	0x40
#define	PCIBAM0_OFF	0x44
#define	PCIBATPA1_OFF	0x48
#define	PCIBAM1_OFF	0x4C
#define	PCIBATPA2_OFF	0x50
#define	PCIBAM2_OFF	0x54
#define	PCISWAP_OFF	0x58
#define	PCCARDEVT_OFF	0x60
#define	PCCARDEVM_OFF	0x64
#define	PCCARDPRS_OFF	0x68
#define	PCCARDFEV_OFF	0x6C
#define	PCCARDCON_OFF	0x70
#define	PCCARD16C_OFF	0x74
#define	PCCARD16M_OFF	0x78
#define	PCCARD16I_OFF	0x7C

#define	PDMACON_OFF	0x00
#define	PDMASRC_OFF	0x04
#define	PDMADST_OFF	0x08
#define	PDMACNT_OFF	0x0C
#define	PDMARUN_OFF	0x10

//----- PCMCIA(16-bit PC Card) Host AHB Address Offsets
#define	PCMCIAAM_OFF	0x00000000	
#define	PCMCIACM_OFF	0x04000000	
#define	PCMCIAIO_OFF	0x08000000	


//----- Real Address (AHB or PCI or PCMCIA)
#define	BS_FIELD 	0xe0000000	//refer to User's Manual
#define	CS 	0x1c000000	//refer to User's Manual
#define	PCI_ADDR	0xC0000000 // Address Space as Master 
#define	PCISFR_ADDR	0xF0110000 // Special Register Space
#define	PCIMEM_ADDR	( PCI_ADDR	+ MEM_OFF	)
#define	PCIMEM_ADDR_SIZE 0x10000000
#define	PCICFG0_ADDR	( PCI_ADDR	+ CFG0_OFF	)
#define	PCICFG1_ADDR	( PCI_ADDR	+ CFG1_OFF	)
#define	PCISPEC_ADDR	( PCI_ADDR	+ SPEC_OFF	)
#define	PCIIO_ADDR	( PCI_ADDR	+ IO_OFF	)
#define	PCIIO_ADDR_SIZE 0x04000000
#define	PCMCIAAM_ADDR	( PCI_ADDR	+ PCMCIAAM_OFF	)
#define	PCMCIACM_ADDR	( PCI_ADDR	+ PCMCIACM_OFF	)
#define	PCMCIAIO_ADDR	( PCI_ADDR	+ PCMCIAIO_OFF	)
#define	BIFSFR_ADDR	( PCISFR_ADDR	+ BIF_OFF	)
#define	PDMA0SFR_ADDR	( PCISFR_ADDR	+ PDMA0_OFF	)
#define	PDMA1SFR_ADDR	( PCISFR_ADDR	+ PDMA1_OFF	)

#define	PCIHID		( PCISFR_ADDR	+ PCIHID_OFF	)
#define	PCIHSC		( PCISFR_ADDR	+ PCIHSC_OFF	)
#define	PCIHCODE	( PCISFR_ADDR	+ PCIHCODE_OFF	)
#define	PCIHLINE	( PCISFR_ADDR	+ PCIHLINE_OFF	)
#define	PCIHBAR0	( PCISFR_ADDR	+ PCIHBAR0_OFF	)
#define	PCIHBAR1	( PCISFR_ADDR	+ PCIHBAR1_OFF	)
#define	PCIHBAR2	( PCISFR_ADDR	+ PCIHBAR2_OFF	)
#define	PCIHCISP	( PCISFR_ADDR	+ PCIHCISP_OFF	)
#define	PCIHSSID	( PCISFR_ADDR	+ PCIHSSID_OFF	)
#define	PCIHCAP	( PCISFR_ADDR	+ PCIHCAP_OFF	)
#define	PCIHLTIT	( PCISFR_ADDR	+ PCIHLTIT_OFF	)
#define	PCIHTIMER	( PCISFR_ADDR	+ PCIHTIMER_OFF	)
#define	PCIHPMR0	( PCISFR_ADDR	+ PCIHPMR0_OFF	)
#define	PCIHPMR1	( PCISFR_ADDR	+ PCIHPMR1_OFF	)

#define	PCICON		( BIFSFR_ADDR	+ PCICON_OFF	)
#define	PCISET		( BIFSFR_ADDR	+ PCISET_OFF	)
#define	PCIINTEN	( BIFSFR_ADDR	+ PCIINTEN_OFF	)
#define	PCIINTST	( BIFSFR_ADDR	+ PCIINTST_OFF	)
#define	PCIINTAD	( BIFSFR_ADDR	+ PCIINTAD_OFF	)
#define	PCIBATAPM	( BIFSFR_ADDR	+ PCIBATAPM_OFF	)
#define	PCIBATAPI	( BIFSFR_ADDR	+ PCIBATAPI_OFF	)
#define	PCIRCC		( BIFSFR_ADDR	+ PCIRCC_OFF	)
#define	PCIDIAG0	( BIFSFR_ADDR	+ PCIDIAG0_OFF	)
#define	PCIDIAG1	( BIFSFR_ADDR	+ PCIDIAG1_OFF	)
#define	PCIBELAP	( BIFSFR_ADDR	+ PCIBELAP_OFF	)
#define	PCIBELPA	( BIFSFR_ADDR	+ PCIBELPA_OFF	)
#define	PCIMAIL0	( BIFSFR_ADDR	+ PCIMAIL0_OFF	)
#define	PCIMAIL1	( BIFSFR_ADDR	+ PCIMAIL1_OFF	)
#define	PCIMAIL2	( BIFSFR_ADDR	+ PCIMAIL2_OFF	)
#define	PCIMAIL3	( BIFSFR_ADDR	+ PCIMAIL3_OFF	)
#define	PCIBATPA0	( BIFSFR_ADDR	+ PCIBATPA0_OFF	)
#define	PCIBAM0 	( BIFSFR_ADDR	+ PCIBAM0_OFF	)
#define	PCIBATPA1	( BIFSFR_ADDR	+ PCIBATPA1_OFF	)
#define	PCIBAM1 	( BIFSFR_ADDR	+ PCIBAM1_OFF	)
#define	PCIBATPA2	( BIFSFR_ADDR	+ PCIBATPA2_OFF	)
#define	PCIBAM2 	( BIFSFR_ADDR	+ PCIBAM2_OFF	)
#define	PCISWAP 	( BIFSFR_ADDR	+ PCISWAP_OFF	)
#define	PCCARDEVT	( BIFSFR_ADDR	+ PCCARDEVT_OFF	)
#define	PCCARDEVM	( BIFSFR_ADDR	+ PCCARDEVM_OFF	)
#define	PCCARDPRS	( BIFSFR_ADDR	+ PCCARDPRS_OFF	)
#define	PCCARDFEV	( BIFSFR_ADDR	+ PCCARDFEV_OFF	)
#define	PCCARDCON	( BIFSFR_ADDR	+ PCCARDCON_OFF	)
#define	PCCARD16C	( BIFSFR_ADDR	+ PCCARD16C_OFF	)
#define	PCCARD16M	( BIFSFR_ADDR	+ PCCARD16M_OFF	)
#define	PCCARD16I	( BIFSFR_ADDR	+ PCCARD16I_OFF	)

#define	PDMACON0	( PDMA0SFR_ADDR	+ PDMACON_OFF	)
#define	PDMASRC0	( PDMA0SFR_ADDR	+ PDMASRC_OFF	)
#define	PDMADST0	( PDMA0SFR_ADDR	+ PDMADST_OFF	)
#define	PDMACNT0	( PDMA0SFR_ADDR	+ PDMACNT_OFF	)
#define	PDMARUN0	( PDMA0SFR_ADDR	+ PDMARUN_OFF	)
#define	PDMACON1	( PDMA1SFR_ADDR	+ PDMACON_OFF	)
#define	PDMASRC1	( PDMA1SFR_ADDR	+ PDMASRC_OFF	)
#define	PDMADST1	( PDMA1SFR_ADDR	+ PDMADST_OFF	)
#define	PDMACNT1	( PDMA1SFR_ADDR	+ PDMACNT_OFF	)
#define	PDMARUN1	( PDMA1SFR_ADDR	+ PDMARUN_OFF	)

#define	PDMACON(ch)	( PDMACON0 + PDMACH_OFF*(ch)	)
#define	PDMASRC(ch)	( PDMASRC0 + PDMACH_OFF*(ch)	)
#define	PDMADST(ch)	( PDMADST0 + PDMACH_OFF*(ch)	)
#define	PDMACNT(ch)	( PDMACNT0 + PDMACH_OFF*(ch)	)
#define	PDMARUN(ch)	( PDMARUN0 + PDMACH_OFF*(ch)	)


//----------------------------------------------------------------------------
//					Declarations for PCI SFR (Register)


#define	rPCIHID		r32( PCIHID	) //00h Cfg Reg: Device ID, Vendor ID
#define	rPCIHSC		r32( PCIHSC	) //04h Cfg Reg: Status, Command
#define	rPCIHCODE	r32( PCIHCODE	) //08h Cfg Reg: Class code, Rev
#define	rPCIHLINE	r32( PCIHLINE	) //0Ch Cfg Reg: Header, Lat, CacheLine
#define	rPCIHBAR0	r32( PCIHBAR0	) //10h Cfg Reg: Mem base address 0
#define	rPCIHBAR1	r32( PCIHBAR1	) //14h Cfg Reg: Mem base address 1
#define	rPCIHBAR2	r32( PCIHBAR2	) //18h Cfg Reg: I/O base address
#define	rPCIHCISP	r32( PCIHCISP	) //28h Cfg Reg: CIS Pointer
#define	rPCIHSSID	r32( PCIHSSID	) //2Ch Cfg Reg: SSID, SVID
#define	rPCIHCAP	r32( PCIHCAP	) // 34h Cfg Reg: Capability List
#define	rPCIHLTIT	r32( PCIHLTIT	) // 3Ch Cfg Reg: Lat,IntPin,IntLine
#define	rPCIHTIMER	r32( PCIHTIMER	) //40h Cfg Reg: Master Timer
#define	rPCIHPMR0	r32( PCIHPMR0	) //DCh Cfg Reg: PMR0 (PMC)
#define	rPCIHPMR1	r32( PCIHPMR1	) //E0h Cfg Reg: PMR1 (PMCSR)

#define	rPCIHCMD	r16( PCIHSC	) //04h Cfg Reg: Command
#define	rPCIHSTS	r16( PCIHSC + 2	) //06h Cfg Reg: Status
#define	rPCIHHDTYPE	r8 ( PCIHLINE+2	) //0Eh Cfg Reg: Header Type
#define	rPCIHBIST	r8 ( PCIHLINE+3	) //0Fh Cfg Reg: BIST
#define	rPCIHPMC	r16( PCIHPMR0+2	) //DEh Cfg Reg: PMC
#define	rPCIHPMCSR	r16( PCIHPMR1	) //E0h Cfg Reg: PMCSR
#define	rPCIHPMBSE	r8 ( PCIHPMR1+2	) //E2h Cfg Reg: PMCSR_BSE

#define	rPCICON		r32( PCICON	) //100 Control & Status Register
#define	rPCISET		r32( PCISET	) //104 Command,Read Count & DAC Addr
#define	rPCIINTEN	r32( PCIINTEN	) //108 Interrupt Enable Register
#define	rPCIINTST	r32( PCIINTST	) //10C Interrupt Status Register
#define	rPCIINTAD	r32( PCIINTAD	) //110 Interrupted Address Register
#define	rPCIBATAPM	r32( PCIBATAPM	) //114 Base Address to PCI bus (Mem)
#define	rPCIBATAPI	r32( PCIBATAPI	) //118 Base Address to PCI bus (I/O)
#define	rPCIRCC		r32( PCIRCC	) //11C Host Mode Reset & Clock Control
#define	rPCIDIAG0	r32( PCIDIAG0	) //120 Diagnostic Register 0
#define	rPCIDIAG1	r32( PCIDIAG1	) //124 Diagnostic Register 1
#define	rPCIBELAP	r32( PCIBELAP	) //128 Doorbell Register to PCI bus
#define	rPCIBELPA	r32( PCIBELPA	) //12C Doorbell Register to AHB bus
#define	rPCIMAIL0	r32( PCIMAIL0	) //130 Mailbox Register 0
#define	rPCIMAIL1	r32( PCIMAIL1	) //134 Mailbox Register 1
#define	rPCIMAIL2	r32( PCIMAIL2	) //138 Mailbox Register 2
#define	rPCIMAIL3	r32( PCIMAIL3	) //13C Mailbox Register 3
#define	rPCIBATPA0	r32( PCIBATPA0	) //140 Base Address (Memory BAR 0)
#define	rPCIBAM0 	r32( PCIBAM0	) //144 Base Address BAR Mask 0
#define	rPCIBATPA1	r32( PCIBATPA1	) //148 Base Address (Memory BAR 1)
#define	rPCIBAM1 	r32( PCIBAM1	) //14C Base Address BAR Mask 1
#define	rPCIBATPA2	r32( PCIBATPA2	) //150 Base Address (I/O BAR 2)
#define	rPCIBAM2 	r32( PCIBAM2	) //154 Base Address BAR Mask 2
#define	rPCISWAP 	r32( PCISWAP	) //158 Byte Swap Register
#define	rPCCARDEVT	r32( PCCARDEVT	) //160 Cardbus: Event Register
#define	rPCCARDEVM	r32( PCCARDEVM	) //164 Cardbus: Event Mask Register
#define	rPCCARDPRS	r32( PCCARDPRS	) //168 Cardbus: Present State Register
#define	rPCCARDFEV	r32( PCCARDFEV	) //16C Cardbus: Force Event Register
#define	rPCCARDCON	r32( PCCARDCON	) //170 Cardbus: Socket Control Register
#define	rPCCARD16C	r32( PCCARD16C	) //174 16-bit Host: Control Register
#define	rPCCARD16M	r32( PCCARD16M	) //178 16-bit Host: Mem Timing Register
#define	rPCCARD16I	r32( PCCARD16I	) //17C 16-bit Host: I/O Timing Register

#define	rPDMACON0	r32( PDMACON0	) //180 PMDA CH0 Control & Status
#define	rPDMASRC0	r32( PDMASRC0	) //184 PDMA CH0 Source Address
#define	rPDMADST0	r32( PDMADST0	) //188 PDMA CH0 Destination Address
#define	rPDMACNT0	r32( PDMACNT0	) //18C PDMA CH0 Transfer Byte Count
#define	rPDMARUN0	r32( PDMARUN0	) //190 PDMA CH0 Run Enable Alias
#define	rPDMACON1	r32( PDMACON1	) //1A0 PMDA CH1 Control & Status
#define	rPDMASRC1	r32( PDMASRC1	) //1A4 PDMA CH1 Source Address
#define	rPDMADST1	r32( PDMADST1	) //1A8 PDMA CH1 Destination Address
#define	rPDMACNT1	r32( PDMACNT1	) //1AC PDMA CH1 Transfer Byte Count
#define	rPDMARUN1	r32( PDMARUN1	) //1B0 PDMA CH1 Run Enable Alias

#define	rPDMACON(ch)	r32( PDMACON(ch) )
#define	rPDMASRC(ch)	r32( PDMASRC(ch) )
#define	rPDMADST(ch)	r32( PDMADST(ch) )
#define	rPDMACNT(ch)	r32( PDMACNT(ch) )
#define	rPDMARUN(ch)	r32( PDMARUN(ch) )

//----------------------------------------------------------------------------
//					PCI SFR Bit Descriptions Declarations

//----------------------
//	PCIHID		(000h) PCI_H : Vendor ID & Device ID
//----------------------
typedef struct {
	unsigned int	VID:16;		// 15 - 0 (VID) : Vendor ID
	unsigned int	DID:16;		// 31 -16 (DID) : Device ID
}s_PCIHID;

//----------------------
//	PCIHSC		(004h) PCI_H : Status & Command Register
//----------------------
typedef struct {
	unsigned int	CMD:16;		// 15 - 0 (CMD) : Command (Device Control) Reg
	unsigned int	STS:16;		// 31 -16 (STS) : (Device) Status Register
}s_PCIHSC;
typedef struct {
	unsigned int	IOE:1;		// Bit  0 (IOE) : I/O Space Decoding Enable
	unsigned int	MME:1;		// Bit  1 (MME) : Memory Space Decoding Enable
	unsigned int	BME:1;		// Bit  2 (BME) : Bus Master Enable
	unsigned int	SCE:1;		// Bit  3 (SCE) : Special Cycle Enable
	unsigned int	MWI:1;		// Bit  4 (MWI) : MemoryWrite&Invalidate Enable
	unsigned int	VGA:1;		// Bit  5 (VGA) : VGA Palette Snoop Enable
	unsigned int	PEE:1;		// Bit  6 (PEE) : Parity Error Response Enable
	unsigned int	STC:1;		// Bit  7 (STC) : Stepping Control Enable
	unsigned int	SER:1;		// Bit  8 (SER) : SERR Enable
	unsigned int	FBE:1;		// Bit  9 (FBE) : Fast Back-to-Back Enable
	unsigned int	rs0:6;		// 15 -10 (rs0)
	unsigned int	rs1:4;		// 19 -16 (rs1)
	unsigned int	CAP:1;		// Bit 20 (CAP) : Capabilities List
	unsigned int	M66:1;		// Bit 21 (M66) : 66MHz Capable
	unsigned int	rs2:1;		// Bit 22 (rs2)
	unsigned int	FBC:1;		// Bit 23 (FBB) : Fast Back-to-Back Capable
	unsigned int	MPE:1;		// Bit 24 (MPE) : Master Data Parity Error
	unsigned int	DST:2;		// 26 -25 (DST) : DEVSEL# Timing
	unsigned int	STA:1;		// Bit 27 (STA) : Signaled Target Abort bit
	unsigned int	RTA:1;		// Bit 28 (RTA) : Received Target Abort bit
	unsigned int	RMA:1;		// Bit 29 (RMA) : Received Master Abort bit
	unsigned int	SSE:1;		// Bit 30 (SSE) : Signaled System Error bit
	unsigned int	DPE:1;		// Bit 31 (DPE) : Detected Parity Error bit
}s_PCIHSC_D;

#define PCIBUSMASTER_ENABLE (1<<2)
#define PCIMWI_ENABLE (1<<4)
#define PCIPERR_RESPONSE_ENABLE (1<<6)
#define PCISERR_ENABLE (1<<8)
#define PCISTATUS_ALL (0xff<<16)
#define	DEVSEL_FAST	0x0	// s_PCIHSTS.DST == 00
#define	DEVSEL_MEDIUM	0x1	// s_PCIHSTS.DST == 01
#define	DEVSEL_SLOW	0x2	// s_PCIHSTS.DST == 10
//efine	DEVSEL_res	0x3	// s_PCIHSTS.DST == 11

//----------------------
//	PCIHCODE	(008h) PCI_H : Class code, Revision ID
//----------------------
typedef struct {
	unsigned int	REV:8;		//  7 - 0 (REV) : Revision ID
	unsigned int	PGI:8;		// 15 - 8 (PGI) : Class Code - Programming I/F
	unsigned int	SCC:8;		// 23 -16 (SCC) : Class Code - Sub-class code
	unsigned int	BCC:8;		// 31 -24 (BCC) : Class Code - Base-class code
}s_PCIHCODE;

//----------------------
//	PCIHLINE	(00Ch) PCI_H : BIST, Header, Lat_Timer, CacheLineSize
//----------------------
typedef struct {
	unsigned int	CLS:8;		//  7 - 0 (CLS) : Cache Line Size
	unsigned int	LTT:8;		// 15 - 8 (LTT) : Latency Timer
	unsigned int	HDT:8;		// 23 -16 (HDT) : Header Type
	unsigned int	BIS:8;		// 31 -24 (BIS) : BIST(Built-in Self Test)
}s_PCIHLINE;
typedef struct {
	unsigned int	CLS:8;		//  7 - 0 (CLS) : Cache Line Size
	unsigned int	LTT:8;		// 15 - 8 (LTT) : Latency Timer
	unsigned int	HTP:6;		// 22 -16 (HTP) : Header Type
	unsigned int	SMF:2;		// Bit 23 (SMF) : Single/Multi Function
	unsigned int	CMP:4;		// 27 -24 (CMP) : Completion Code
	unsigned int	rs0:2;		// 29 -28 (rs0)
	unsigned int	STB:1;		// Bit 30 (STB) : Start BIST
	unsigned int	CAP:1;		// Bit 31 (CAP) : BIST Capable
}s_PCIHLINE_D;

//----------------------
//	PCIHBAR0	(010h) PCI_H : Mem Base Address Register 0
//	PCIHBAR1	(014h) PCI_H : Mem Base Address Register 1
//	PCIHBAR2	(018h) PCI_H : I/O Base Address Register 2
//----------------------
typedef struct {
	unsigned int	IND:1;		// Bit  0 (IND) : Space Indicator(0=Mem,1=IO)
	unsigned int	BAR:31;		// 31 - 1 (BAR) : Base Address Register
}s_PCIHBAR;
typedef struct {
	unsigned int	IND:1;		// Bit  0 (IND) : Space Indicator(0=Mem)
	unsigned int	TYP:2;		//  2 - 1 (TYP) : Memory I/F Type
	unsigned int	PRF:1;		// Bit  3 (PRF) : Prefetchable
	unsigned int	BAR:28;		// 31 - 4 (BAR) : Base Address Register
}s_PCIHMBAR;
typedef struct {
	unsigned int	IND:1;		// Bit  0 (IND) : Space Indicator(1=IO)
	unsigned int	rs0:1;		// Bit  1 (rs0)
	unsigned int	BAR:30;		// 31 - 2 (BAR) : Base Address Register
}s_PCIHIBAR;
typedef struct {
	unsigned int	ADE:1;		// Bit  0 (ADE) : Address Decode Enable
	unsigned int	rs0:10;		// 10 - 1 (rs0)
	unsigned int	BAR:21;		// 31 -11 (BAR) : Expansion ROM Base Address
}s_PCIHRBAR;

#define	MEM_BAR		0	// s_PCIHBAR.IND
#define	IO_BAR		1	// s_PCIHBAR.IND

#define	MEM_TYPE_32	0	// s_PCIHMBAR.TYP : in 32bit address (4G) region
#define	MEM_TYPE_20	1	// s_PCIHMBAR.TYP : not PCI2.2 but CardBus
#define	MEM_TYPE_64	2	// s_PCIHMBAR.TYP : in 64bit address region
//efine	MEM_TYPE_res	3	// s_PCIHMBAR.TYP

//----------------------
//	PCIHCISP	(028h) PCI_H : CIS(CardBusInformationStructure) Pointer
//----------------------
typedef struct {
	unsigned int	IND:3;		//  2 - 0 (IND) : Address Space Indicator
	unsigned int	OFF:29;		// 31 - 3 (OFF) : Address Space Offset(ROM Img)
}s_PCIHCISP;

#define	CIS_IN_CFG	0	// s_PCIHCISP.IND : Configuration Space
#define	CIS_IN_BAR0	1	// s_PCIHCISP.IND : BAR0 (10h)
#define	CIS_IN_BAR1	2	// s_PCIHCISP.IND : BAR1 (14h)
#define	CIS_IN_BAR2	3	// s_PCIHCISP.IND : BAR2 (18h)
#define	CIS_IN_BAR3	4	// s_PCIHCISP.IND : BAR3 (1Ch)
#define	CIS_IN_BAR4	5	// s_PCIHCISP.IND : BAR4 (20h)
#define	CIS_IN_BAR5	6	// s_PCIHCISP.IND : BAR5 (24h)
#define	CIS_IN_EXPR	7	// s_PCIHCISP.IND : Expansion Rom (30h)

//----------------------
//	PCIHSSID	(02Ch) PCI_H : SubSystem ID, SubSystem Vendor ID
//----------------------
typedef struct {
	unsigned int	SVD:16;		// 15 - 0 (SVD) : SubSystem Vendor ID
	unsigned int	SID:16;		// 31 -16 (SID) : SubSystem ID
}s_PCIHSSID;

//----------------------
//	PCIHCAP		(034h) PCI_H : Capabilities List Pointer
//----------------------
typedef struct {
	unsigned int	CAP:8;		//  7 - 0 (CAP) : Capabilities Pointer
	unsigned int	rs0:24;		// 31 - 8 (rs0)
}s_PCIHCAP;

//----------------------
//	PCIHLTIT	(03Ch) PCI_H : Max_Lat, Min_Gnt, Int_Pin, Int_Line
//----------------------
typedef struct {
	unsigned int	INL:8;		//  7 - 0 (INL) : Int_Line(0=IRQ0,1=IRQ1,255=no)
	unsigned int	INP:8;		// 15 - 8 (INP) : Int_Pin(0=no,1=INTA#,2=INTB#)
	unsigned int	MGT:8;		// 23 -16 (MGT) : Min_Gnt(How long occupy bus)
	unsigned int	MLT:8;		// 31 -24 (MLT) : Max_Lat(How much request bus)
}s_PCIHLTIT;

//----------------------
//	PCIHTIMER	(040h) PCI_H : Master Timer Register
//----------------------
typedef struct {
	unsigned int	TRT:8;		//  7 - 0 (TRT) : Master TRDY# Wait Timer
	unsigned int	RTC:8;		// 15 - 8 (RTC) : Master Retry Counter
	unsigned int	rs0:16;		// 31 -16 (rs0)
}s_PCIHTIMER;

//----------------------
//	PCIHPMR0	(0DCh) PCI_H : Power Management Register 0 (PMC)
//----------------------
typedef struct {
	unsigned int	CID:8;		//  7 - 0 (CID) : Capability ID (1=PowerMngReg)
	unsigned int	NIP:8;		// 15 - 8 (NIP) : Next Item Pointer
	unsigned int	PMC:16;		// 31 -16 (PMC) : Power Management Capabilities
}s_PCIHPMR0;
typedef struct {
	unsigned int	CID:8;		//  7 - 0 (CID) : Capability ID (1=PowerMngReg)
	unsigned int	NIP:8;		// 15 - 8 (NIP) : Next Item Pointer
	unsigned int	VER:3;		// 18 -16 (VER) : Power Mng I/F Spec. Version
	unsigned int	CLK:1;		// Bit 19 (CLK) : PCI Clock Necessity for PME#
	unsigned int	rs0:1;		// Bit 20 (rs0)
	unsigned int	DSI:1;		// Bit 21 (DSI) : Device Specific Initial.
	unsigned int	AUX:3;		// 24 -22 (AUX) : Aux Current
	unsigned int	D1S:1;		// Bit 25 (D1S) : D1 State Support
	unsigned int	D2S:1;		// Bit 26 (D2S) : D2 State Support
	unsigned int	PD0:1;		// Bit 27 (PD0) : PME# Support from D0
	unsigned int	PD1:1;		// Bit 28 (PD1) : PME# Support from D1
	unsigned int	PD2:1;		// Bit 29 (PD2) : PME# Support from D2
	unsigned int	PD3:1;		// Bit 30 (PD3) : PME# Support from D3hot
	unsigned int	PDC:1;		// Bit 31 (PDC) : PME# Support from D3cold
}s_PCIHPMR0_D;

#define	AUX_375mA	7	// s_PCIHPMR0_D.AUX : 3.3Vaux Max Current
#define	AUX_320mA	6	// s_PCIHPMR0_D.AUX : 3.3Vaux Max Current
#define	AUX_270mA	5	// s_PCIHPMR0_D.AUX : 3.3Vaux Max Current
#define	AUX_220mA	4	// s_PCIHPMR0_D.AUX : 3.3Vaux Max Current
#define	AUX_160mA	3	// s_PCIHPMR0_D.AUX : 3.3Vaux Max Current
#define	AUX_100mA	2	// s_PCIHPMR0_D.AUX : 3.3Vaux Max Current
#define	AUX_55mA	1	// s_PCIHPMR0_D.AUX : 3.3Vaux Max Current
#define	AUX_SelfPower	0	// s_PCIHPMR0_D.AUX : 3.3Vaux Max Current

//----------------------
//	PCIHPMR1	(0E0h) PCI_H : Power Management Register 1 (PMCSR)
//----------------------
typedef struct {
	unsigned int	CSR:16;		// 15 - 0 (CSR) : PMCSR (Control/Status Reg)
	unsigned int	BSE:8;		// 23 -16 (BSE) : PMCSR_BSE(Bridge Support Ext)
	unsigned int	DAT:8;		// 31 -24 (DAT) : Data Register
}s_PCIHPMR1;
typedef struct {
	unsigned int	PST:2;		//  1 - 0 (PST) : Power State
	unsigned int	rs0:6;		//  7 - 2 (rs0)
	unsigned int	PME:1;		// Bit  8 (PME) : PME_Enable bit
	unsigned int	DSL:4;		// 12 - 9 (DSL) : Data Register Select
	unsigned int	DSC:2;		// 14 -13 (DSC) : Data Register Scale
	unsigned int	PMS:1;		// Bit 15 (PMS) : PME_Status bit
	unsigned int	rs1:6;		// 21 -16 (rs1)
	unsigned int	B23:1;		// Bit 22 (B23) : B2_B3# support for D3hot
	unsigned int	BPE:1;		// Bit 23 (BPE) : BPCC_En (Bus Power/Clock Con)
	unsigned int	DAT:8;		// 31 -24 (DAT) : Data Register
}s_PCIHPMR1_D;

#define	PMSTATE_D0	0	// s_PCIHPMR1_D.PST == 0
#define	PMSTATE_D1	1	// s_PCIHPMR1_D.PST == 1
#define	PMSTATE_D2	2	// s_PCIHPMR1_D.PST == 2
#define	PMSTATE_D3	3	// s_PCIHPMR1_D.PST == 3 : D3hot

//----------------------
//	PCICON		(100h) PCI Control & Status Register
//----------------------
typedef struct {
	unsigned int	HST:1;		// Bit  0 (HST) : Host Mode			
	unsigned int	ARB:1;		// Bit  1 (ARB) : Internal PCI Arbiter Enable	
	unsigned int	rs0:2;		//  3 - 2 (rs0)
	unsigned int	ATS:1;		// Bit  4 (ATS) : Address Translation to PCI	
	unsigned int	SPL:1;		// Bit  5 (SPL) : Split Response Mode		
	unsigned int	IOP:1;		// Bit  6 (IOP) : PCI I/O Space Prefetchable	
	unsigned int	MMP:1;		// Bit  7 (MMP) : PCI Memory Space Prefetchable	
	unsigned int	CFD:1;		// Bit  8 (CFD) : Configuration Done		
	unsigned int	RDY:1;		// Bit  9 (RDY) : PCI Target Ready to Read	
	unsigned int	rs1:6;		// 15 -10 (rs1)
	unsigned int	WKI:1;		// Bit 16 (WKI)	: PME# Assert by ARM interrupt	
	unsigned int	WKA:1;		// Bit 17 (WKA) : PME_Status(PME#) Assert	
	unsigned int	rs2:6;		// 23 -18 (rs2)
	unsigned int	CAR:1;		// Bit 24 (CAR) : Cardbus Interface Mode 	
	unsigned int	rs3:3;		// 27 -25 (rs3)
	unsigned int	TBS:1;		// Bit 28 (TBS) : PCI Target Busy		
	unsigned int	MBS:1;		// Bit 29 (MBS) : PCI Master Busy		
	unsigned int	DBS:1;		// Bit 30 (DBS) : PCI DMA Busy			
	unsigned int	INT:1;		// Bit 31 (INT) : Internal Interrupt Signaled	
}s_PCICON;
#define PCIARBITOR_INT (1<<1)
#define PCIARBITOR_EXT (~PCIARBITOR_INT)
#define PCICONFIG_DONE (1<<8)
#define PCISYSTEMREAD_READY (1<<9)

//----------------------
//	PCISET		(104h) PCI Command, Read Count & DAC Address Register
//----------------------
typedef struct {
	unsigned int	DAC:8;		//  7 - 0 (DAC) : DAC Upper 8 Bit Address	
	unsigned int	RDC:8;		// 15 - 8 (RDC) : Read Word Count 		
	unsigned int	CMD:4;		// 19 -17 (CMD) : PCI Command if PCICON[ATS]=0	
	unsigned int	rs0:12;		// 31 -20 (rs0)
}s_PCISET;

//----------------------
//	PCIINTEN	(108h) PCI Interrupt Enable Register
//----------------------
typedef struct {
	unsigned int	PRD:1;		// Bit  0 (PRD) : Enable of PCIINTST[PRD]
	unsigned int	PRA:1;		// Bit  1 (PRA) : Enable of PCIINTST[PRA]
	unsigned int	MFE:1;		// Bit  2 (MFE) : Enable of PCIINTST[MFE]
	unsigned int	MPE:1;		// Bit  3 (MPE) : Enable of PCIINTST[MPE]
	unsigned int	TPE:1;		// Bit  4 (TPE) : Enable of PCIINTST[TPE]
	unsigned int	PME:1;		// Bit  5 (PME) : Enable of PCIINTST[PME]
	unsigned int	PMC:1;		// Bit  6 (PMC) : Enable of PCIINTST[PMC]
	unsigned int	PSC:1;		// Bit  7 (PSC) : Enable of PCIINTST[PSC]
	unsigned int	BPA:1;		// Bit  8 (BPA) : Enable of PCIBELPA[BEL]
	unsigned int	SER:1;		// Bit  9 (SER) : Enable of PCIINTST[SER]
	unsigned int	INA:1;		// Bit 10 (INA) : Enable of PCIINTST[INA]
	unsigned int	rs0:1;		// Bit 11 (rs0)
	unsigned int	DM0:1;		// Bit 12 (DM0) : Enable of PCIINTST[DM0]
	unsigned int	DE0:1;		// Bit 13 (DE0) : Enable of PCIINTST[DE0]
	unsigned int	DM1:1;		// Bit 14 (DM1) : Enable of PCIINTST[DM1]
	unsigned int	DE1:1;		// Bit 15 (DE1) : Enable of PCIINTST[DE1]
	unsigned int	AER:1;		// Bit 16 (AER) : Enable of PCIINTST[AER]
	unsigned int	rs1:14;		// 30 -17 (rs1)
	unsigned int	BAP:1;		// Bit 31 (BAP) : Enable of PCIBELAP[BEL]
}s_PCIINTEN;

//----------------------
//	PCIINTST	(10Ch) PCI Interrupt Status Register
//----------------------
typedef struct {
	unsigned int	PRD:1;		// Bit  0 (PRD) : PCI Reset Deasserted
	unsigned int	PRA:1;		// Bit  1 (PRA) : PCI Reset Asserted
	unsigned int	MFE:1;		// Bit  2 (MFE) : PCI Master Fatal Error
	unsigned int	MPE:1;		// Bit  3 (MPE) : PCI Master Parity Error
	unsigned int	TPE:1;		// Bit  4 (TPE) : PCI Target Parity Error
	unsigned int	PME:1;		// Bit  5 (PME) : PME Enable Asserted or PME#
	unsigned int	PMC:1;		// Bit  6 (PMC) : PME Clear  Asserted
	unsigned int	PSC:1;		// Bit  7 (PSC) : Power State Changed
	unsigned int	BPA:1;		// Bit  8 (BPA) : PCI Doorbell Register to AHB
	unsigned int	SER:1;		// Bit  9 (SER) : PCI bus SERR# if PCICON[HST]=1
	unsigned int	INA:1;		// Bit 10 (INA) : PCI bus INTA# if PCICON[HST]=1
	unsigned int	rs0:1;		// Bit 11 (rs0)
	unsigned int	DM0:1;		// Bit 12 (DM0) : PDMA Channel 0 Done
	unsigned int	DE0:1;		// Bit 13 (DE0) : PDMA Channel 0 Error
	unsigned int	DM1:1;		// Bit 14 (DM1) : PDMA Channel 1 Done
	unsigned int	DE1:1;		// Bit 15 (DE1) : PDMA Channel 1 Error
	unsigned int	AER:1;		// Bit 16 (AER) : AHB ERROR response
	unsigned int	rs1:12;		// 28- 17 (rs1)
	unsigned int	DME:1;		// Bit 29 (DME) : DMA   Err of PCIINTST[MFE,MPE]
	unsigned int	RDE:1;		// Bit 30 (RDE) : Write Err of PCIINTST[MFE,MPE]
	unsigned int	WRE:1;		// Bit 31 (WRE) : Read  Err of PCIINTST[MFE,MPE]
}s_PCIINTST;
typedef	union {
	s_PCIINTST	sfr;
	unsigned int		u32;
}	u_PCIINTST;

//----------------------
//	PCIINTAD	(110h) PCI Interrupted Address Register
//----------------------
//typedef struct s_PCIINTAD {
//	unsigned int	:32;		
//};

//----------------------
//	PCIBATAPM	(114h) PCI Base Address to PCI bus (Memory region)
//----------------------
typedef struct {
	unsigned int	rs0:28;		// 27 - 0 (rs0)
	unsigned int	APM:4;		// 31 -28 (APM) : Base Address
}s_PCIBATAPM;

//----------------------
//	PCIBATAPI	(118h) PCI Base Address to PCI bus (I/O region)
//----------------------
typedef struct {
	unsigned int	rs0:26;		// 25 - 0 (rs0)
	unsigned int	API:6;		// 31 -26 (APM) : Base Address
}s_PCIBATAPI;

//----------------------
//	PCIRCC		(11C) Host Mode Reset & Clock Control Register
//----------------------
typedef struct {
	unsigned int	RSB:1;		// Bit  0 (RSB)	: P&C Bus   Reset (low active)	
	unsigned int	RSL:1;		// Bit  1 (RSL)	: P&C Logic Reset (low active)	
	unsigned int	MSK:1;		// Bit  2 (MSK) : P&C Clock/Bus Reset Mask	
	unsigned int	M33:1;		// Bit  3 (33M) : PCI Clock 33MHz Mode
	unsigned int	SLW:1;		// Bit  4 (SLW) : Slow Clock Mode(if CLKRUN#=1)	
	unsigned int	rs0:3;		//  7 - 5 (rs0)
	unsigned int	KCK:1;		// Bit  8 (KCK)	: Keep Clock Mode(no stop)	
	unsigned int	ACC:1;		// Bit  9 (ACC) : Automatic CLKRUN# Control	
	unsigned int	CKI:1;		// Bit 10 (CKI) : CLKRUN# Event by ARM intrpt	
	unsigned int	CKR:1;		// Bit 11 (CKR) : CLKRUN# Run by Event [CKI]	
	unsigned int	CKA:1;		// Bit 12 (CKA) : CLKRUN# Assert (Run Clock)	
	unsigned int	CKD:1;		// Bit 13 (CKD) : CLKRUN# Deassert (Stop Clock)	
	unsigned int	rs1:14;		// 27- 14 (rs1)
	unsigned int	BRS:1;		// Bit 28 (BRS) : P&C Bus Reset Status		
	unsigned int	LRS:1;		// Bit 29 (LRS) : Internal Logic Reset Status	
	unsigned int	CKS:1;		// Bit 30 (CKS) : P&C Clock Status		
	unsigned int	CKN:1;		// Bit 31 (CKN) : CLKRUN# Status of last clock	
}s_PCIRCC;

#define PCIBUS_RESET (1<<0)
#define PCILOG_RESET (1<<1)
#define PCIRESETCLK_MASK (1<<2)
#define PCICLK_33M (1<<3)

// Warning: DONOT USE following code
//	sPCIRCC.MSK = 1;	// Disable Input/Output
//	--> Be careful! This may harm chip (I/O cell) by electrical short.
//	Only use as following code (from reset value = 1).
//	sPCIRCC.MSK = 0;	// Enable Input/Output

//----------------------
//	PCIDIAG0	(120h) PCI Diagnostic Register 0
//----------------------
typedef struct {
	unsigned int	CMR:1;		// Bit  0 (CMR) : Use Common P&C Reset	
	unsigned int	EXR:1;		// Bit  1 (EXR) : Use external P&C reset
	unsigned int	EXC:1;		// Bit  2 (EXC) : Use external P&C clock
	unsigned int	IGB:1;		// Bit  3 (IGB) : Ignore byte enable for burst
	unsigned int	WPR:1;		// Bit  4 (WPR) : Write data regardless of perr
	unsigned int	NPU:1;		// Bit  5 (NPU) : Disable Pull-Up resistors
	unsigned int	FAS:1;		// Bit  6 (FAS) : Enable Fast Decoder		
	unsigned int	SPC:1;		// Bit  7 (SPC) : Short Power-cycle mode	
	unsigned int	FTR:1;		// Bit  8 (FTR) : Force to flush the TRCV FIFO	
	unsigned int	FTT:1;		// Bit  9 (FTT) : Force to flush the TXMT FIFO	
	unsigned int	FMR:1;		// Bit 10 (FMR) : Force to flush the MRCV FIFO	
	unsigned int	FMT:1;		// Bit 11 (FMT) : Force to flush the MXMT FIFO	
	unsigned int	DI4:1;		// Bit 12 (DI4) : Disable INCR4 on AHB master 
	unsigned int	DIR:1;		// Bit 13 (DIR) : Disable Internal Retry Mech
	unsigned int	OBE:1;		// Bit 14 (OBE) : Original PCI Read BE Output
	unsigned int	X15:1;		// Bit 15 (X15) : Extra Diagnostic Control 
	unsigned int	X16:1;		// Bit 16 (X16) : Extra Diagnostic Control 
	unsigned int	X17:1;		// Bit 17 (X17) : Extra Diagnostic Control 
	unsigned int	X18:1;		// Bit 18 (X18) : Extra Diagnostic Control 
	unsigned int	X19:1;		// Bit 19 (X19) : Extra Diagnostic Control 
	unsigned int	rs0:12;		// 31 -20 (rs0)
}s_PCIDIAG0;
#define PCIRESET_EXT (1<<1)
#define PCICLK_EXT (1<<2)
#define PCIPULLUP_EXT (1<<5)
//----------------------
//	PCIDIAG1	(124h) PCI Diagnostic Register 1
//----------------------
typedef struct {
	unsigned int	rs0:15;		// 14 - 0 (rs0)
	unsigned int	RSL:1;		// Bit 15 (RSL)	: PCIRCC[RSL] Value Reset
	unsigned int	BID:2;		// 17 -16 (BID) : Last Accessed BAR Number	
	unsigned int	REQ:1;		// Bit 18 (REQ) : PCI Master Request Remains	
	unsigned int	MAS:5;		// 23 -19 (MAS) : PCI Master State 		
	unsigned int	TAR:4;		// 27 -24 (TAR) : PCI Target State 		
	unsigned int	REV:4;		// 31 -28 (REV) : Revision No.			
}s_PCIDIAG1;

//----------------------
//	PCIBELAP	(128h) PCI Doorbell Register to PCI bus
//	PCIBELPA	(12Ch) PCI Doorbell Register to AHB bus
//----------------------
typedef struct {
	unsigned int	UIF:31;		// 30 - 0 (UIF) : User Information
	unsigned int	BEL:1;		// Bit 31 (BEL) : Force interrupt
}s_PCIBEL;

//----------------------
//	PCIMAIL0	(130h) PCI Mailbox Register 0
//	PCIMAIL1	(134h) PCI Mailbox Register 1
//	PCIMAIL2	(138h) PCI Mailbox Register 2
//	PCIMAIL3	(13Ch) PCI Mailbox Register 3
//----------------------
//typedef struct {
//	unsigned int	:32;		
//}	s_PCIMAIL;

//----------------------
//	PCISWAP		(158h) Byte Swap Register
//----------------------
typedef struct {
	unsigned int	TRB:2;		//  1 - 0 (TRB) : Swap of Writing Byte to   AHB
	unsigned int	TRH:2;		//  3 - 2 (TRH) : Swap of Writing Half to   AHB
	unsigned int	TRW:2;		//  5 - 4 (TRW) : Swap of Writing Word to   AHB
	unsigned int	rs0:2;		//  7 - 6 (rs0)
	unsigned int	TTB:2;		//  9 - 8 (TTB) : Swap of Reading Byte from AHB
	unsigned int	TTH:2;		// 11 -10 (TTH) : Swap of Reading Half from AHB
	unsigned int	TTW:2;		// 13 -12 (TTW) : Swap of Reading Word from AHB
	unsigned int	rs1:2;		// 15 -14 (rs1)
	unsigned int	MRB:2;		// 17 -16 (MRB) : Swap of ARM CPU's Reading Byte
	unsigned int	MRH:2;		// 19 -18 (MRH) : Swap of ARM CPU's Reading Half
	unsigned int	MRW:2;		// 21 -20 (MRW) : Swap of ARM CPU's Reading Word
	unsigned int	rs2:2;		// 23 -22 (rs2)
	unsigned int	MTB:2;		// 25 -24 (MTB) : Swap of ARM CPU's Writing Byte
	unsigned int	MTH:2;		// 27 -26 (MTH) : Swap of ARM CPU's Writing Half
	unsigned int	MTW:2;		// 29 -28 (MTW) : Swap of ARM CPU's Writing Word
	unsigned int	rs3:2;		// 31 -30 (rs3)
}s_PCISWAP;

//----------------------
//	PCCARDEVT	(160h) Cardbus: Socket/Function Event Register
//----------------------
typedef struct {		//-.-\\ Socket(host)
	unsigned int	STC:1;		// Bit  0 (STC) : CSTSCHG(BVD[1],STSCHG#)Changed
	unsigned int	CD1:1;		// Bit  1 (CD1) : CCD#[1] Changed
	unsigned int	CD2:1;		// Bit  2 (CD2) : CCD#[2] Changed
	unsigned int	PWC:1;		// Bit  3 (PWC) : PCCARDPRS[PWC] Changed
	unsigned int	rs0:12;		// 15 - 4 (rs0)
	unsigned int	WPC:1;		// Bit 16 (WPC) : PCMCIA WP Signal Changed	
	unsigned int	IRC:1;		// Bit 17 (IRC) : PCMCIA READY(or IREQ#) Changed
	unsigned int	SPC:1;		// Bit 18 (SPC) : PCMCIA BVD[2](SPKR#) Changed	
	unsigned int	WPW:1;		// Bit 19 (WPW) : PCMCIA Memory Write But WP
	unsigned int	RDA:1;		// Bit 20 (RDA) : PCMCIA Access When Not Ready	
	unsigned int	INA:1;		// Bit 21 (INA) : PCMCIA INPACK# No Ack(IO Read)
	unsigned int	rs1:10;		// 31 -22 (rs1)
}s_PCCARDEVT;
typedef struct {		//-.-\\ Function(agent)
	unsigned int	rs0:4;		//  3 - 0 (rs0)
	unsigned int	GWA:1;		// Bit  4 (GWA) : General Wakeup Event
	unsigned int	rs1:10;		// 14 - 5 (rs1)
	unsigned int	INT:1;		// Bit 15 (INT) : Function Event Interrupt Event
	unsigned int	rs2:16;		// 31 -16 (rs2)
}f_PCCARDEVT;

//----------------------
//	PCCARDEVM	(164h) Cardbus: Socket/Function Event Mask Register
//----------------------
typedef struct {		//-.-\\ Socket(host)
	unsigned int	STC:1;		// Bit  0 (STC) : PCCARDEVT[STC] Intrpt Enable
	unsigned int	CD1:1;		// Bit  1 (CD1) : PCCARDEVT[CD1] Intrpt Enable
	unsigned int	CD2:1;		// Bit  2 (CD2) : PCCARDEVT[CD2] Intrpt Enable
	unsigned int	PWC:1;		// Bit  3 (PWC) : PCCARDEVT[PWC] Intrpt Enable
	unsigned int	rs0:12;		// 15 - 4 (rs0)
	unsigned int	WPC:1;		// Bit 16 (WPC) : PCCARDEVT[WPC] Intrpt Enable 
	unsigned int	IRC:1;		// Bit 17 (IRC) : PCCARDEVT[IRC] Intrpt Enable 
	unsigned int	SPC:1;		// Bit 18 (SPC) : PCCARDEVT[SPC] Intrpt Enable 
	unsigned int	WPW:1;		// Bit 19 (WPW) : PCCARDEVT[WPW] Intrpt Enable 
	unsigned int	RDA:1;		// Bit 20 (RDA) : PCCARDEVT[RDA] Intrpt Enable 
	unsigned int	INA:1;		// Bit 21 (INA) : PCCARDEVT[INA] Intrpt Enable 
	unsigned int	rs1:10;		// 31 -22 (rs1)
}s_PCCARDEVM;
typedef struct {		//-.-\\ Function(agent)
	unsigned int	rs0:4;		//  3 - 0 (rs0)
	unsigned int	GWA:1;		// Bit  4 (GWA) : General Wakeup Event Mask
	unsigned int	rs1:9;		// 13 - 5 (rs1)
	unsigned int	WKU:1;		// Bit 14 (WKU) : Wakeup Mask to CSTSCHG Signal
	unsigned int	INT:1;		// Bit 15 (INT) : Function Event Interrupt Mask
	unsigned int	rs2:16;		// 31 -16 (rs2)
}f_PCCARDEVM;

//----------------------
//	PCCARDPRS	(168h) Cardbus: Socket/Function Present State Register
//----------------------
typedef struct {		//-.-\\ Socket(host)
	unsigned int	STC:1;		// Bit  0 (STC) : CSTSCHGorBVD[1](STSCHG#) Pin
	unsigned int	CD1:1;		// Bit  1 (CD1) : CCD#[1] Pin Status
	unsigned int	CD2:1;		// Bit  2 (CD2) : CCD#[2] Pin Status
	unsigned int	PWC:1;		// Bit  3 (PWC) : Power Up Cycle Done Status
	unsigned int	C16:1;		// Bit  4 (C16) : 16-bit PC Card Inserted
	unsigned int	C32:1;		// Bit  5 (C32) : CardBus PC Card Inserted
	unsigned int	rs0:1;		// Bit  6 (rs0)
	unsigned int	CNO:1;		// Bit  7 (CNO) : Not a card (Not Determined)
	unsigned int	DTL:1;		// Bit  8 (DTL) : Data Lost or Left in FIFO
	unsigned int	BVR:1;		// Bit  9 (BVR) : Bad Vcc Request from Software
	unsigned int	C5V:1;		// Bit 10 (C5V) : Card Activated at Vcc=5V
	unsigned int	C3V:1;		// Bit 11 (C3V) : Card Activated at Vcc=3V
	unsigned int	CXV:1;		// Bit 12 (CXV) : Card Activated at Vcc=X.X V
	unsigned int	CYV:1;		// Bit 13 (CYV) : Card Activated at Vcc=Y.Y V
	unsigned int	CVS:1;		// Bit 14 (CVS) : Now in CVS Test (Not in Spec.)
	unsigned int	rs1:1;		// Bit 15 (rs1)
	unsigned int	WPS:1;		// Bit 16 (WPS) : PCMCIA WP Signal Status
	unsigned int	RDY:1;		// Bit 17 (RDY) : PCMCIA READY(IREQ#) Signal
	unsigned int	BVD:1;		// Bit 18 (BVD) : PCMCIA BVD[2](SPKR#) Status
	unsigned int	rs2:2;		// 20- 19 (rs2)
	unsigned int	INP:1;		// Bit 21 (INP) : PCMCIA INPACK# Status
	unsigned int	rs3:6;		// 27- 22 (rs3)
	unsigned int	S5V:1;		// Bit 28 (S5V) : 1 = Socket Can Apply Vcc=5V
	unsigned int	S3V:1;		// Bit 29 (S3V) : 1 = Socket Can Apply Vcc=3V
	unsigned int	SXV:1;		// Bit 30 (SXV) : 0 = Socket NOT Apply Vcc=X.X V
	unsigned int	SYV:1;		// Bit 31 (SYV) : 0 = Socket NOT Apply Vcc=Y.Y V
}s_PCCARDPRS;
typedef struct {		//-.-\\ Function(agent)
	unsigned int	WRP:1;		// Bit  0 (WRP) : 0 = Not write protected
	unsigned int	RED:1;		// Bit  1 (RED) : 1 = Ready
	unsigned int	BVD:2;		//  3 - 2 (BVD) : 11 = Battery operational
	unsigned int	GWA:1;		// Bit  4 (GWA) : General Wakeup Event Status
	unsigned int	rs0:10;		// 14 - 5 (rs0)
	unsigned int	INT:1;		// Bit 15 (INT) : Function Event Status
	unsigned int	rs1:16;		// 31 -16 (rs1)
}f_PCCARDPRS;

//----------------------
//	PCCARDFEV	(16Ch) Cardbus: Socket/Function Force Event Register
//----------------------
typedef struct {		//-.-\\ Socket(host)
	unsigned int	STC:1;		// Bit  0 (STC) : Simulate PCCARDEVT[STC] to 1
	unsigned int	CD1:1;		// Bit  1 (CD1) : Simulate PCCARDEVT[CD1] to 1
	unsigned int	CD2:1;		// Bit  2 (CD2) : Simulate PCCARDEVT[CD2] to 1
	unsigned int	PWC:1;		// Bit  3 (PWC) : Simulate PCCARDEVT[PWC] to 1
	unsigned int	C16:1;		// Bit  4 (C16) : Simulate 16-bit Inserted
	unsigned int	C32:1;		// Bit  5 (C32) : Simulate CardBus Inserted
	unsigned int	rs0:1;		// Bit  6 (rs0)
	unsigned int	CNO:1;		// Bit  7 (CNO) : Simulate No PC Card
	unsigned int	DTL:1;		// Bit  8 (DTL) : Write PCCARDPRS[DTL]
	unsigned int	BVR:1;		// Bit  9 (BVR) : Write PCCARDPRS[BVR]
	unsigned int	C5V:1;		// Bit 10 (C5V) : Write PCCARDPRS[C5V]
	unsigned int	C3V:1;		// Bit 11 (C3V) : Write PCCARDPRS[C3V]
	unsigned int	CXV:1;		// Bit 12 (CXV) : Write PCCARDPRS[CXV]
	unsigned int	CYV:1;		// Bit 13 (CYV) : Write PCCARDPRS[CYV]
	unsigned int	CVS:1;		// Bit 14 (CVS) : CVS Test Interrogating
	unsigned int	rs1:1;		// Bit 15 (rs1)
	unsigned int	SPC:1;		// Bit 16 (SPC) : Simulate PCCARDEVT[SPC] to 1
	unsigned int	IRC:1;		// Bit 17 (IRC) : Simulate PCCARDEVT[IRC] to 1
	unsigned int	WPC:1;		// Bit 18 (WPC) : Simulate PCCARDEVT[WPC] to 1
	unsigned int	RDA:1;		// Bit 19 (RDA) : Simulate PCCARDEVT[RDA] to 1
	unsigned int	WPW:1;		// Bit 20 (WPW) : Simulate PCCARDEVT[WPW] to 1
	unsigned int	INA:1;		// Bit 21 (INA) : Simulate PCCARDEVT[INA] to 1
	unsigned int	rs2:10;		// 31- 22 (rs2)
}s_PCCARDFEV;
typedef struct {		//-.-\\ Function(agent)
	unsigned int	rs0:4;		//  3 - 0 (rs0)
	unsigned int	GWA:1;		// Bit  4 (GWA) : Force Wakeup Event
	unsigned int	rs1:10;		// 14 - 5 (rs1)
	unsigned int	INT:1;		// Bit 15 (INT) : Force Function Event Interrupt
	unsigned int	rs2:16;		// 31 -16 (rs2)
}f_PCCARDFEV;

//----------------------
//	PCCARDCON	(170h) Cardbus: Socket Control Register
//----------------------
typedef struct {		//-.-\\ Socket(host)
	unsigned int	VPP:3;		//  2 - 0 (VPP) : VPP (0=off,1=12V,2=5V,3=3.3V)
	unsigned int	rs0:1;		// Bit  3 (rs0)
	unsigned int	VCC:3;		//  6 - 4 (VCC) : VCC (0=off, 2=5V, 3=3.3V)
	unsigned int	rs1:16;		// 22 - 7 (rs1)
	unsigned int	ACT:1;		// Bit 23 (ACT) : Card Signals Active
	unsigned int	CST:8;		// 31 -24 (CST) : Card State
}s_PCCARDCON;

//----------------------
//	PCCARD16C	(174h) PCMCIA Host: Control Register
//----------------------
typedef struct {		//-.-\\ host
	unsigned int	CDT:1;		// Bit  0 (CDT) : Card Type(0=MemOnly,1=I/O&Mem)
	unsigned int	rs0:2;		//  2 - 1 (rs0)
//jjhawk 5/22 , BSP is already defined in pollio.h
//	unsigned int	BSP:1;		// Bit  3 (BSP) : Byte Swapping to PCMCIA	
	unsigned int	BS:1;		// Bit  3 (BSP) : Byte Swapping to PCMCIA	
	unsigned int	MMS:1;		// Bit  4 (MMS) : Mem Data Width(0=8bit,1=16bit)
	unsigned int	IOS:1;		// Bit  5 (IOS) : I/O Data Width(0=8bit,1=16bit)
	unsigned int	IOE:1;		// Bit  6 (IOE) : IOIS# Enable (0=use [IOS])	
	unsigned int	rs1:19;		// 25 - 7 (rs1)
	unsigned int	WPS:1;		// Bit 26 (WPS) : WP(IOIS16#) Pin Signal Status
	unsigned int	INS:1;		// Bit 27 (INS) : INPACK# Pin Signal Status
	unsigned int	RDY:1;		// Bit 28 (RDY) : READY(IREQ#) Pin Status
	unsigned int	BVD:2;		// 30 -29 (BVD) : BVD[2:1](SPKR#,STSCHG#) Status
	unsigned int	C16:1;		// Bit 31 (C16) : 16-bit PC Card Is Installed
}s_PCCARD16C;

//----------------------
//	PCCARD16M	(178h) PCMCIA Host: Memory Timing Register
//	PCCARD16I	(17Ch) PCMCIA Host: I/O Timinig Register
//----------------------

// PAST, PSST, PSL, PSHT Definition Timing Diagram
//
//		|-PAST-|---PSST----|--------PSL------|PSHT-|
//               __    __    __    __    __    __    __    __    __
//HCLK		|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__
//              _ _______________________________________________ ____
//ADDR		_|_______________________________________________|____
//		_______ ___________________________________ __________
//REG#,CE#	_______|___________________________________|__________
//              ___________________                   ________________
//OE#WE#IOxx#	                   |_________________|
//		___________________ _____________________________ ____
//DATA(MEMW)	___________________|_____________________________|____
//		_______ _________________________________________ ____
//DATA(IOW)	_______|_________________________________________|____
//		                                 _____
//DATA(READ)	---------------------------------_____----------------
//
// PAST : Address Setup Time
// PSST : Strobe Setup Time
// PSL  : Strobe Length	
// PSHT : Strobe Hold Time
typedef struct {		//-.-\\ host
	unsigned int	AST:4;		//  3 - 0 (AST) : Address Setup Time
	unsigned int	rs0:4;		//  7 - 4 (rs0)
	unsigned int	SST:4;		// 11 - 8 (SST) : Strobe Setup Time
	unsigned int	rs1:4;		// 15 -12 (rs1)
	unsigned int	SL :6;		// 21 -16 (SL ) : Strobe Length
	unsigned int	rs2:2;		// 23 -22 (rs2)
	unsigned int	SHT:4;		// 27 -24 (SHT) : Strobe Hold Time
	unsigned int	rs3:4;		// 31 -28 (rs3)
}s_PCCARD16T;


//----------------------
//	PDMACON0	(180h) PMDA CH0 Control & Status Register
//	PDMACON1	(1A0h) PMDA CH1 Control & Status Register
//----------------------
typedef struct {
	unsigned int	RE :1;		// Bit  0 (RE)  : Run Enable (PDMARUNn aliased)
	unsigned int	IRP:1;		// Bit  1 (IRP) : Route PDMA Interrupt to PCI
	unsigned int	BSE:1;		// Bit  2 (BSE) : Byte-Swapping Enable
	unsigned int	rs0:1;		// Bit  3 (rs0) : 0
	unsigned int	S  :1;		// Bit  4 (S)   : Source Selection (1=PCI)
	unsigned int	D  :1;		// Bit  5 (D)   : Destination Selection (1=PCI)
	unsigned int	SAF:1;		// Bit  6 (SAF) : Source Address Fix
	unsigned int	DAF:1;		// Bit  7 (DAF) : Destination Address Fix
	unsigned int	SCM:4;		// Bit  8       : 0 (read)
				// 11 - 9 (SCM) : Source PCI Command
	unsigned int	DCM:4;		// Bit 12       : 1 (write)
				// 15 -13 (DCM) : Destination PCI Command
	unsigned int	rs1:13;		// 28 -16 (rs1)
	unsigned int	ERR:1;		// Bit 29 (ERR) : PDMA Done with Error
	unsigned int	PRG:1;		// Bit 30 (PRG) : PDMA programmed from PCI bus
	unsigned int	BSY:1;		// Bit 31 (BS)  : PDMA busy
}s_PDMACON;

#define	AHB_SIDE	0x0	// sPDMACONn.S , sPDMACONn.D
#define	PCI_SIDE	0x1	// sPDMACONn.S , sPDMACONn.D


//----------------------
//	PDMASRC0	(184h) PDMA CH0 Source Address Register
//	PDMASRC1	(1A4h) PDMA CH1 Source Address Register
//	PDMADST0	(188h) PDMA CH0 Destination Address Register
//	PDMADST1	(1A8h) PDMA CH1 Destination Address Register
//	PDMACNT0	(18Ch) PDMA CH0 Transfer Byte Count Register
//	PDMACNT1	(1ACh) PDMA CH1 Transfer Byte Count Register
//----------------------
//typedef struct {
//	unsigned int	:32;		
//}	s_PDMA;



//----------------------------------------------------------------------------
//			Declarations for PCI SFR (SFR Structure Base-Type)

#define	sPCIHID		( *(volatile	   s_PCIHID	*) PCIHID	)
#define	sPCIHSC		( *(volatile	   s_PCIHSC	*) PCIHSC	)
#define	sPCIHCODE	( *(volatile	   s_PCIHCODE	*) PCIHCODE	)
#define	sPCIHLINE	( *(volatile	   s_PCIHLINE	*) PCIHLINE	)
#define	sPCIHBAR0	( *(volatile	   s_PCIHMBAR	*) PCIHBAR0	)
#define	sPCIHBAR1	( *(volatile	   s_PCIHMBAR	*) PCIHBAR1	)
#define	sPCIHBAR2	( *(volatile	   s_PCIHIBAR	*) PCIHBAR2	)
#define	sPCIHCISP	( *(volatile	   s_PCIHCISP	*) PCIHCISP	)
#define	sPCIHSSID	( *(volatile	   s_PCIHSSID	*) PCIHSSID	)
#define	sPCIHCAP	( *(volatile	   s_PCIHCAP	*) PCIHCAP	)
#define	sPCIHLTIT	( *(volatile	   s_PCIHLTIT	*) PCIHLTIT	)
#define	sPCIHTIMER	( *(volatile	   s_PCIHTIMER	*) PCIHTIMER	)
#define	sPCIHPMR0	( *(volatile	   s_PCIHPMR0	*) PCIHPMR0	)
#define	sPCIHPMR1	( *(volatile	   s_PCIHPMR1	*) PCIHPMR1	)

#define	sPCIHCMD	( *(volatile s_PCIHSC_D	*) PCIHSC	)
#define	sPCIHSTS	( *(volatile s_PCIHSC_D	*) PCIHSC	)
#define	sPCIHHDTYPE	( *(volatile	   s_PCIHLINE_D	*) PCIHLINE	)
#define	sPCIHBIST	( *(volatile	   s_PCIHLINE_D	*) PCIHLINE	)
#define	sPCIHPMC	( *(volatile	   s_PCIHPMR0_D	*) PCIHPMR0	)
#define	sPCIHPMCSR	( *(volatile	   s_PCIHPMR1_D	*) PCIHPMR1	)
#define	sPCIHPMBSE	( *(volatile	   s_PCIHPMR1_D	*) PCIHPMR1	)

#define	sPCICON		( *(volatile	   s_PCICON	*) PCICON	)
#define	sPCISET		( *(volatile	   s_PCISET	*) PCISET	)
#define	sPCIINTEN	( *(volatile	   s_PCIINTEN	*) PCIINTEN	)
#define	sPCIINTST	( *(volatile 		s_PCIINTST	*) PCIINTST	)
#define	sPCIINTAD	r32( PCIINTAD	)
#define	sPCIBATAPM	( *(volatile	   s_PCIBATAPM	*) PCIBATAPM	)
#define	sPCIBATAPI	( *(volatile	   s_PCIBATAPI	*) PCIBATAPI	)
#define	sPCIRCC		( *(volatile	   s_PCIRCC	*) PCIRCC	)
#define	sPCIDIAG0	( *(volatile	   s_PCIDIAG0	*) PCIDIAG0	)
#define	sPCIDIAG1	( *(volatile	   s_PCIDIAG1	*) PCIDIAG1	)
#define	sPCIBELAP	( *(volatile	   s_PCIBEL	*) PCIBELAP	)
#define	sPCIBELPA	( *(volatile	   s_PCIBEL	*) PCIBELPA	)
#define	sPCIMAIL0	r32( PCIMAIL0	)
#define	sPCIMAIL1	r32( PCIMAIL1	)
#define	sPCIMAIL2	r32( PCIMAIL2	)
#define	sPCIMAIL3	r32( PCIMAIL3	)
#define	sPCIBATPA0	( *(volatile	   s_PCIBATPA0	*) PCIBATPA0	)
#define	sPCIBAM0 	( *(volatile	   s_PCIBAM0	*) PCIBAM0	)
#define	sPCIBATPA1	( *(volatile	   s_PCIBATPA1	*) PCIBATPA1	)
#define	sPCIBAM1 	( *(volatile	   s_PCIBAM1	*) PCIBAM1	)
#define	sPCIBATPA2	( *(volatile	   s_PCIBATPA2	*) PCIBATPA2	)
#define	sPCIBAM2 	( *(volatile	   s_PCIBAM2	*) PCIBAM2	)
#define	sPCISWAP 	( *(volatile	   s_PCISWAP	*) PCISWAP	)
#define	sPCCARDEVT	( *(volatile 	   s_PCCARDEVT	*) PCCARDEVT	)
#define	sPCCARDEVM	( *(volatile	   s_PCCARDEVM	*) PCCARDEVM	)
#define	sPCCARDPRS	( *(volatile	   s_PCCARDPRS	*) PCCARDPRS	)
#define	sPCCARDFEV	( *(volatile	   s_PCCARDFEV	*) PCCARDFEV	)
#define	fPCCARDEVT	( *(volatile 	   f_PCCARDEVT	*) PCCARDEVT	)
#define	fPCCARDEVM	( *(volatile	   f_PCCARDEVM	*) PCCARDEVM	)
#define	fPCCARDPRS	( *(volatile	   f_PCCARDPRS	*) PCCARDPRS	)
#define	fPCCARDFEV	( *(volatile	   f_PCCARDFEV	*) PCCARDFEV	)
#define	sPCCARDCON	( *(volatile	   s_PCCARDCON	*) PCCARDCON	)
#define	sPCCARD16C	( *(volatile	   s_PCCARD16C	*) PCCARD16C	)
#define	sPCCARD16M	( *(volatile	   s_PCCARD16T	*) PCCARD16M	)
#define	sPCCARD16I	( *(volatile	   s_PCCARD16T	*) PCCARD16I	)

#define	sPDMACON0	( *(volatile	   s_PDMACON	*) PDMACON0	)
#define	sPDMASRC0	r32( PDMASRC0	)
#define	sPDMADST0	r32( PDMADST0	)
#define	sPDMACNT0	r32( PDMACNT0	)
#define	sPDMARUN0	r32( PDMARUN0	)
#define	sPDMACON1	( *(volatile	   s_PDMACON	*) PDMACON1	)
#define	sPDMASRC1	r32( PDMASRC1	)
#define	sPDMADST1	r32( PDMADST1	)
#define	sPDMACNT1	r32( PDMACNT1	)
#define	sPDMARUN1	r32( PDMARUN1	)
#define	sPDMACON(ch)	( *(volatile	   s_PDMACON	*) PDMACON(ch)	)
#define	sPDMASRC(ch)	r32( PDMASRC(ch) )
#define	sPDMADST(ch)	r32( PDMADST(ch) )
#define	sPDMACNT(ch)	r32( PDMACNT(ch) )
#define	sPDMARUN(ch)	r32( PDMARUN(ch) )

//	const
//	-----
// 'const' type are used for preventing following WRITE-1-TO-CLEAR BIT problem
//	CODE WITH BUG:
//		if( sPCIINTST.PRD ) {
//			...
//			sPCIINTST.PRD = 1; // --> NO! (CAUSE SIDE EFFECT)
//		}
//	compile(sPCIINTST.PRD=1;) -->
//		LDR	r1, [r0, #&sPCIINTST]	; read
//		ORR	r1, #1			; or
//		STR	r1, [r0, #&sPCIINTST]	; write back
//	This code can clear other bits by side effect, so DONOT use this code!
//
//	CODE WITHOUT BUG - 1: (recommended)
//		s_PCIINTST pending;
//		pending		= sPCIINTST;	// read
//		co_sPCIINTST	= pending;	// write back to clear each W/C
//		//rPCIINTST	= *(unsigned long *)&pending;	// same as above
//		if( pending.PRD ) {..}	// use
//	CODE WITHOUT BUG - 2:
//		rPCIINTST = (1<<..);
// Warning!!
//	DONOT USE AS FOLLOWING (only for W/C bit)
// 		rPCIINTST |= (1<<..);	--> modify it to "rPCIINTST = (1<<..);"
//	BE CAREFUL IF USE AS FOLLOWING
// 		sPCIINTST.PRD = 1;	--> W/C bits might be cleared



//----------------------------------------------------------------------------
//			Declarations for PCI SFR (SFR Structure Co-Type)

// co_ : Code Optimization   or   prefix which means 'additional'
// 	 - no volatile & const declaration
// 	 - These macros should be used in left side of equation.

#define	co_sPCIHID	( *(s_PCIHID	*) PCIHID	)
#define	co_sPCIHSC	( *(s_PCIHSC	*) PCIHSC	)
#define	co_sPCIHCODE	( *(s_PCIHCODE	*) PCIHCODE	)
#define	co_sPCIHLINE	( *(s_PCIHLINE	*) PCIHLINE	)
#define	co_sPCIHBAR0	( *(s_PCIHMBAR	*) PCIHBAR0	)
#define	co_sPCIHBAR1	( *(s_PCIHMBAR	*) PCIHBAR1	)
#define	co_sPCIHBAR2	( *(s_PCIHIBAR	*) PCIHBAR2	)
#define	co_sPCIHCISP	( *(s_PCIHCISP	*) PCIHCISP	)
#define	co_sPCIHSSID	( *(s_PCIHSSID	*) PCIHSSID	)
#define	co_sPCIHCAP	( *(s_PCIHCAP	*) PCIHCAP	)
#define	co_sPCIHLTIT	( *(s_PCIHLTIT	*) PCIHLTIT	)
#define	co_sPCIHTIMER	( *(s_PCIHTIMER	*) PCIHTIMER	)
#define	co_sPCIHPMR0	( *(s_PCIHPMR0	*) PCIHPMR0	)
#define	co_sPCIHPMR1	( *(s_PCIHPMR1	*) PCIHPMR1	)

#define	co_sPCIHCMD	( *(s_PCIHSC_D	*) PCIHSC	)
#define	co_sPCIHSTS	( *(s_PCIHSC_D	*) PCIHSC	)
#define	co_sPCIHHDTYPE	( *(s_PCIHLINE_D*) PCIHLINE	)
#define	co_sPCIHBIST	( *(s_PCIHLINE_D*) PCIHLINE	)
#define	co_sPCIHPMC	( *(s_PCIHPMR0_D*) PCIHPMR0	)
#define	co_sPCIHPMCSR	( *(s_PCIHPMR1_D*) PCIHPMR1	)
#define	co_sPCIHPMBSE	( *(s_PCIHPMR1_D*) PCIHPMR1	)

#define	co_sPCICON	( *(s_PCICON	*) PCICON	)
#define	co_sPCISET	( *(s_PCISET	*) PCISET	)
#define	co_sPCIINTEN	( *(s_PCIINTEN	*) PCIINTEN	)
#define	co_sPCIINTST	( *(s_PCIINTST	*) PCIINTST	)
#define	co_sPCIINTAD	co_r32( PCIINTAD	)
#define	co_sPCIBATAPM	( *(s_PCIBATAPM	*) PCIBATAPM	)
#define	co_sPCIBATAPI	( *(s_PCIBATAPI	*) PCIBATAPI	)
#define	co_sPCIRCC	( *(s_PCIRCC	*) PCIRCC	)
#define	co_sPCIDIAG0	( *(s_PCIDIAG0	*) PCIDIAG0	)
#define	co_sPCIDIAG1	( *(s_PCIDIAG1	*) PCIDIAG1	)
#define	co_sPCIBELAP	( *(s_PCIBEL	*) PCIBELAP	)
#define	co_sPCIBELPA	( *(s_PCIBEL	*) PCIBELPA	)
#define	co_sPCIMAIL0	co_r32( PCIMAIL0	)
#define	co_sPCIMAIL1	co_r32( PCIMAIL1	)
#define	co_sPCIMAIL2	co_r32( PCIMAIL2	)
#define	co_sPCIMAIL3	co_r32( PCIMAIL3	)
#define	co_sPCIBATPA0	( *(s_PCIBATPA0	*) PCIBATPA0	)
#define	co_sPCIBAM0 	( *(s_PCIBAM0	*) PCIBAM0	)
#define	co_sPCIBATPA1	( *(s_PCIBATPA1	*) PCIBATPA1	)
#define	co_sPCIBAM1 	( *(s_PCIBAM1	*) PCIBAM1	)
#define	co_sPCIBATPA2	( *(s_PCIBATPA2	*) PCIBATPA2	)
#define	co_sPCIBAM2 	( *(s_PCIBAM2	*) PCIBAM2	)
#define	co_sPCISWAP 	( *(s_PCISWAP	*) PCISWAP	)
#define	co_sPCCARDEVT	( *(s_PCCARDEVT	*) PCCARDEVT	)
#define	co_sPCCARDEVM	( *(s_PCCARDEVM	*) PCCARDEVM	)
#define	co_sPCCARDPRS	( *(s_PCCARDPRS	*) PCCARDPRS	)
#define	co_sPCCARDFEV	( *(s_PCCARDFEV	*) PCCARDFEV	)
#define	co_fPCCARDEVT	( *(f_PCCARDEVT	*) PCCARDEVT	)
#define	co_fPCCARDEVM	( *(f_PCCARDEVM	*) PCCARDEVM	)
#define	co_fPCCARDPRS	( *(f_PCCARDPRS	*) PCCARDPRS	)
#define	co_fPCCARDFEV	( *(f_PCCARDFEV	*) PCCARDFEV	)
#define	co_sPCCARDCON	( *(s_PCCARDCON	*) PCCARDCON	)
#define	co_sPCCARD16C	( *(s_PCCARD16C	*) PCCARD16C	)
#define	co_sPCCARD16M	( *(s_PCCARD16T	*) PCCARD16M	)
#define	co_sPCCARD16I	( *(s_PCCARD16T	*) PCCARD16I	)

#define	co_sPDMACON0	( *(s_PDMACON	*) PDMACON0	)
#define	co_sPDMASRC0	co_r32( PDMASRC0	)
#define	co_sPDMADST0	co_r32( PDMADST0	)
#define	co_sPDMACNT0	co_r32( PDMACNT0	)
#define	co_sPDMARUN0	co_r32( PDMARUN0	)
#define	co_sPDMACON1	( *(s_PDMACON	*) PDMACON1	)
#define	co_sPDMASRC1	co_r32( PDMASRC1	)
#define	co_sPDMADST1	co_r32( PDMADST1	)
#define	co_sPDMACNT1	co_r32( PDMACNT1	)
#define	co_sPDMARUN1	co_r32( PDMARUN1	)
#define	co_sPDMACON(ch)	( *(s_PDMACON	*) PDMACON(ch)	)
#define	co_sPDMASRC(ch)	co_r32( PDMASRC(ch) )
#define	co_sPDMADST(ch)	co_r32( PDMADST(ch) )
#define	co_sPDMACNT(ch)	co_r32( PDMACNT(ch) )
#define	co_sPDMARUN(ch)	co_r32( PDMARUN(ch) )

#define PCI_IAE	(0x1<<1)
#define PCI_EPCLK	(0x1<<2)
#define PCI_SPLIT	(0x1<<5)
#define PCI_BUS_RESET	(0x1<<0)
#define PCI_LOGIC_RESET		(0x1<<1)
#define PCI_CLKMSK	(0x1<<2)
#define PCI_33M	(0x1<<3)
#define PDMA_BUSY	((unsigned long)0x1<<31)
#define PDMA_PRG	(0x1<<30)
#define PDMA_RE		(0x1)

#define TRCV_FIFO (0x1<<8)
#define TXMT_FIFO (0x1<<9)
#define MRCV_FIFO (0x1<<10)
#define MXMT_FIFO (0x1<<11)


/*
 *    PCI SFR Related Macros
 */
//----- Operation mode
#define PCIMode()       (!sPCICON.CAR)
#define PCCardMode()        ( sPCICON.CAR)
#define HostMode()              ( sPCICON.HST )
#define AgentMode()             (!sPCICON.HST )
#define PCI_HostMode()      (!sPCICON.CAR &&  sPCICON.HST )
#define PCI_AgentMode()     (!sPCICON.CAR && !sPCICON.HST )
#define PCCard_HostMode()   ( sPCICON.CAR &&  sPCICON.HST )
#define PCCard_AgentMode()  ( sPCICON.CAR && !sPCICON.HST )
#define PCMCIA_HostMode()   ( sPCCARD16C.C16 )


//----- Get AHB Address for PCI Bus or CardBus Transaction

#define	ENC_ADDR(device)	(device)

//AHB_ADDR_PCI_MEM, AHB_ADDR_PCI_IO, AHB_ADDR_PCI_SPEC
//MEM_BASE_WIDTH=4, IO_BASE_WIDTH=6
#define	 AHB_ADDR_PCI_MEM(addr)	(PCIMEM_ADDR|(0x0FFFFFFF&(unsigned long)(addr)))
#define	PDMA_ADDR_PCI_MEM(addr)	(addr)
#define	 AHB_ADDR_PCI_IO(addr)	(PCIIO_ADDR |(0x03FFFFFF&(unsigned long)(addr)))
#define	PDMA_ADDR_PCI_IO(addr)	(addr)
#define	 AHB_ADDR_PCI_CFG0( device, functn, offset )		\
			(PCICFG0_ADDR |	((ENC_ADDR(device)&0x1F)<<11) |	\
			((    (functn)    &0x07)<< 8) |	\
			((    (offset)    &0xFF)    )	)
#define	PDMA_ADDR_PCI_CFG0( device, functn, offset )		\
			(	( 1 << (ENC_ADDR(device)+10)) |	\
			((    (functn)    &0x07)<< 8) |	\
			((    (offset)    &0xFF)    )	)
#define	 AHB_ADDR_PCI_CFG1( bus, device, functn, offset )	\
			(PCICFG1_ADDR |	((    (bus)       &0xFF)<<16) |	\
			((ENC_ADDR(device)&0x1F)<<11) |	\
			((    (functn)    &0x07)<< 8) |	\
			((    (offset)    &0xFF)    )	)
#define	PDMA_ADDR_PCI_CFG1( bus, device, functn, offset )	\
			( ((    (bus)       &0xFF)<<16) |	\
			((ENC_ADDR(device)&0x1F)<<11) |	\
			((    (functn)    &0x07)<< 8) |	\
			((    (offset)    &0xFF)    )	)
				
#define	 AHB_ADDR_PCI_SPEC(addr) (PCISPEC_ADDR |(0x03FFFFFF&(unsigned long)(addr))
#define	PDMA_ADDR_PCI_SPEC(addr) (addr)

typedef struct{
	void *addr;
	char *name;
	u32 ResetValue;
	u32 InitializedValue;
}s_SFR;

#define ALL_WRITE1CLR 0xffffffff
#define WRITE1CLR 0x1
#define WRITE0CLR 0x0

#define Enable_Int(n) *(volatile unsigned int*)INTMASK &= (~((unsigned long)1<<(n))) //ry++c
#define GlobalEn_Int() *(volatile unsigned int*)EXTMASK &= (~((unsigned long)1<<31)) //ry++c
#define GlobalDis_Int() *(volatile unsigned int*)EXTMASK |= ((unsigned long)1<<31) //ryc++

#define IIOPDATA1 *(volatile unsigned int*)IOPDATA1

#endif
