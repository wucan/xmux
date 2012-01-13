/* -------------------------------------------------------------------------
File Name: STV0903_init.c

Description: STV0903 registers initialization	V3.0 "July/11/2008" 


author: BJ
---------------------------------------------------------------------------- */

#include "globaldefs.h" 
#include "STV0903_init.h"
#include "STV0903_util.h"
#include "STV0903_Drv.h"
#include "globaldefs.h"
#define STV0903_I2C_ADDRESS 0xD0
extern void WAIT_N_MS(unsigned int count);
#define FSTV6110_CO_DIV  0x000100c0

const STCHIP_Register_t STV0903DefVal[STV0903_NBREGS]=
{
    { RSTV0903_MID,    0x12 }    ,/* MID */
    { RSTV0903_DACR1,    0x00 }    ,/* DACR1 */
    { RSTV0903_DACR2,    0x00 }    ,/* DACR2 */
    { RSTV0903_OUTCFG,    0x00 }    ,/* OUTCFG */
    { RSTV0903_MODECFG,    0xff }    ,/* MODECFG */
    { RSTV0903_IRQSTATUS3,    0x00 }    ,/* IRQSTATUS3 */
    { RSTV0903_IRQSTATUS2,    0x00 }    ,/* IRQSTATUS2 */
    { RSTV0903_IRQSTATUS1,    0x00 }    ,/* IRQSTATUS1 */
    { RSTV0903_IRQSTATUS0,    0x00 }    ,/* IRQSTATUS0 */
    { RSTV0903_IRQMASK3,    0x3f }    ,/* IRQMASK3 */
    { RSTV0903_IRQMASK2,    0xff }    ,/* IRQMASK2 */
    { RSTV0903_IRQMASK1,    0xff }    ,/* IRQMASK1 */
    { RSTV0903_IRQMASK0,    0xff }    ,/* IRQMASK0 */
    { RSTV0903_I2CCFG,    0x08 }    ,/* I2CCFG */
    { RSTV0903_I2CRPT,    0x44 }    ,/* I2CRPT */
    { RSTV0903_P2_I2CRPT,    0xb2 }    ,/* P2_I2CRPT */
    { RSTV0903_CLKI2CFG,    0x82 }    ,/* CLKI2CFG */
    { RSTV0903_GPIO1CFG,    0x82 }    ,/* GPIO1CFG */
    { RSTV0903_GPIO2CFG,    0x82 }    ,/* GPIO2CFG */
    { RSTV0903_GPIO3CFG,    0x82 }    ,/* GPIO3CFG */
    { RSTV0903_GPIO4CFG,    0x82 }    ,/* GPIO4CFG */
    { RSTV0903_GPIO5CFG,    0x82 }    ,/* GPIO5CFG */
    { RSTV0903_GPIO6CFG,    0x82 }    ,/* GPIO6CFG */
    { RSTV0903_GPIO7CFG,    0x82 }    ,/* GPIO7CFG */
    { RSTV0903_GPIO8CFG,    0x82 }    ,/* GPIO8CFG */
    { RSTV0903_GPIO9CFG,    0x82 }    ,/* GPIO9CFG */
    { RSTV0903_GPIO10CFG,    0x82 }    ,/* GPIO10CFG */
    { RSTV0903_GPIO11CFG,    0x82 }    ,/* GPIO11CFG */
    { RSTV0903_GPIO12CFG,    0x82 }    ,/* GPIO12CFG */
    { RSTV0903_GPIO13CFG,    0x82 }    ,/* GPIO13CFG */
    { RSTV0903_CS0CFG,    0x82 }    ,/* CS0CFG */
    { RSTV0903_CS1CFG,    0x82 }    ,/* CS1CFG */
    { RSTV0903_STDBYCFG,    0x82 }    ,/* STDBYCFG */
    { RSTV0903_DIRCLKCFG,    0x82 }    ,/* DIRCLKCFG */
    { RSTV0903_AGCRF1CFG,    0x11 }    ,/* AGCRF1CFG */
    { RSTV0903_SDAT1CFG,    0xb0 }    ,/* SDAT1CFG */
    { RSTV0903_SCLT1CFG,    0xb2 }    ,/* SCLT1CFG */
    { RSTV0903_DISEQCO1CFG,    0x14 }    ,/* DISEQCO1CFG */
    { RSTV0903_ERRORCFG,    0xd8 }    ,/* ERRORCFG */
    { RSTV0903_DPNCFG,    0xd4 }    ,/* DPNCFG */
    { RSTV0903_STROUTCFG,    0xd6 }    ,/* STROUTCFG */
    { RSTV0903_CLKOUTCFG,    0xda }    ,/* CLKOUTCFG */
    { RSTV0903_DATA7CFG,    0xdc }    ,/* DATA7CFG */
    { RSTV0903_FSKTFC2,    0x8c }    ,/* FSKTFC2 */
    { RSTV0903_FSKTFC1,    0x75 }    ,/* FSKTFC1 */
    { RSTV0903_FSKTFC0,    0xc2 }    ,/* FSKTFC0 */
    { RSTV0903_FSKTDELTAF1,    0x02 }    ,/* FSKTDELTAF1 */
    { RSTV0903_FSKTDELTAF0,    0x0c }    ,/* FSKTDELTAF0 */
    { RSTV0903_FSKTCTRL,    0x04 }    ,/* FSKTCTRL */
    { RSTV0903_FSKRFC2,    0x10 }    ,/* FSKRFC2 */
    { RSTV0903_FSKRFC1,    0x75 }    ,/* FSKRFC1 */
    { RSTV0903_FSKRFC0,    0xc2 }    ,/* FSKRFC0 */
    { RSTV0903_FSKRK1,    0x3a }    ,/* FSKRK1 */
    { RSTV0903_FSKRK2,    0x74 }    ,/* FSKRK2 */
    { RSTV0903_FSKRAGCR,    0x28 }    ,/* FSKRAGCR */
    { RSTV0903_FSKRAGC,    0xff }    ,/* FSKRAGC */
    { RSTV0903_FSKRALPHA,    0x17 }    ,/* FSKRALPHA */
    { RSTV0903_FSKRPLTH1,    0x80 }    ,/* FSKRPLTH1 */
    { RSTV0903_FSKRPLTH0,    0x00 }    ,/* FSKRPLTH0 */
    { RSTV0903_FSKRDF1,    0x80 }    ,/* FSKRDF1 */
    { RSTV0903_FSKRDF0,    0x00 }    ,/* FSKRDF0 */
    { RSTV0903_FSKRSTEPP,    0x30 }    ,/* FSKRSTEPP */
    { RSTV0903_FSKRSTEPM,    0x70 }    ,/* FSKRSTEPM */
    { RSTV0903_FSKRDET1,    0x00 }    ,/* FSKRDET1 */
    { RSTV0903_FSKRDET0,    0x00 }    ,/* FSKRDET0 */
    { RSTV0903_FSKRDTH1,    0x11 }    ,/* FSKRDTH1 */
    { RSTV0903_FSKRDTH0,    0x89 }    ,/* FSKRDTH0 */
    { RSTV0903_FSKRLOSS,    0x06 }    ,/* FSKRLOSS */
    { RSTV0903_ACRPRESC2,    0x01 }    ,/* ACRPRESC2 */
    { RSTV0903_ACRDIV2,    0x14 }    ,/* ACRDIV2 */
    { RSTV0903_DISTXCTL,    0x22 }    ,/* DISTXCTL */
    { RSTV0903_DISRXCTL,    0x80 }    ,/* DISRXCTL */
    { RSTV0903_DISRX_ST0,    0x04 }    ,/* DISRX_ST0 */
    { RSTV0903_DISRX_ST1,    0x20 }    ,/* DISRX_ST1 */
    { RSTV0903_DISRXDATA,    0x00 }    ,/* DISRXDATA */
    { RSTV0903_DISTXDATA,    0x00 }    ,/* DISTXDATA */
    { RSTV0903_DISTXSTATUS,    0x20 }    ,/* DISTXSTATUS */
    { RSTV0903_F22TX,    0xc0 }    ,/* F22TX */
    { RSTV0903_F22RX,    0xc0 }    ,/* F22RX */
    { RSTV0903_ACRPRESC,    0x01 }    ,/* ACRPRESC */
    { RSTV0903_ACRDIV,    0x14 }    ,/* ACRDIV */
    { RSTV0903_NCOARSE,    0x13 }    ,/* NCOARSE */
    { RSTV0903_SYNTCTRL,    0x22 }    ,/* SYNTCTRL */
    { RSTV0903_FILTCTRL,    0x01 }    ,/* FILTCTRL */
    { RSTV0903_PLLSTAT,    0x00 }    ,/* PLLSTAT */
    { RSTV0903_STOPCLK1,    0x48 }    ,/* STOPCLK1 */
    { RSTV0903_STOPCLK2,    0x14 }    ,/* STOPCLK2 */
    { RSTV0903_TSTTNR0,    0x04 }    ,/* TSTTNR0 */
    { RSTV0903_TSTTNR1,    0x27 }    ,/* TSTTNR1 */
    { RSTV0903_TSTTNR2,    0x21 }    ,/* TSTTNR2 */
    { RSTV0903_IQCONST,    0x00 }    ,/* IQCONST */
    { RSTV0903_NOSCFG,    0x1c }    ,/* NOSCFG */
    { RSTV0903_ISYMB,    0x01 }    ,/* ISYMB */
    { RSTV0903_QSYMB,    0x01 }    ,/* QSYMB */
    { RSTV0903_AGC1CFG,    0x54 }    ,/* AGC1CFG */
    { RSTV0903_AGC1CN,    0xd9 }    ,/* AGC1CN */
    { RSTV0903_AGC1REF,    0x58 }    ,/* AGC1REF */
    { RSTV0903_IDCCOMP,    0x03 }    ,/* IDCCOMP */
    { RSTV0903_QDCCOMP,    0xfd }    ,/* QDCCOMP */
    { RSTV0903_POWERI,    0x0c }    ,/* POWERI */
    { RSTV0903_POWERQ,    0x0b }    ,/* POWERQ */
    { RSTV0903_AGC1AMM,    0x0b }    ,/* AGC1AMM */
    { RSTV0903_AGC1QUAD,    0x09 }    ,/* AGC1QUAD */
    { RSTV0903_AGCIQIN1,    0x00 }    ,/* AGCIQIN1 */
    { RSTV0903_AGCIQIN0,    0x00 }    ,/* AGCIQIN0 */
    { RSTV0903_DEMOD,    0x08 }    ,/* DEMOD */
    { RSTV0903_DMDMODCOD,    0x18 }    ,/* DMDMODCOD */
    { RSTV0903_DSTATUS,    0x10 }    ,/* DSTATUS */
    { RSTV0903_DSTATUS2,    0x09 }    ,/* DSTATUS2 */
    { RSTV0903_DMDCFGMD,    0xf9 }    ,/* DMDCFGMD */
    { RSTV0903_DMDCFG2,    0x3b }    ,/* DMDCFG2 */
    { RSTV0903_DMDISTATE,    0x5c }    ,/* DMDISTATE */
    { RSTV0903_DMDT0M,    0x20 }    ,/* DMDT0M */
    { RSTV0903_DMDSTATE,    0x1c }    ,/* DMDSTATE */
    { RSTV0903_DMDFLYW,    0xc0 }    ,/* DMDFLYW */
    { RSTV0903_DSTATUS3,    0x00 }    ,/* DSTATUS3 */
    { RSTV0903_DMDCFG3,    0xc4 }    ,/* DMDCFG3 */
    { RSTV0903_DMDCFG4,    0x00 }    ,/* DMDCFG4 */
    { RSTV0903_CORRELMANT,    0x70 }    ,/* CORRELMANT */
    { RSTV0903_CORRELABS,    0x88 }    ,/* CORRELABS */
    { RSTV0903_CORRELEXP,    0xaa }    ,/* CORRELEXP */
    { RSTV0903_PLHMODCOD,    0x22 }    ,/* PLHMODCOD */
    { RSTV0903_AGCK32,    0x00 }    ,/* AGCK32 */
    { RSTV0903_AGC2O,    0x5b }    ,/* AGC2O */
    { RSTV0903_AGC2REF,    0x38 }    ,/* AGC2REF */
    { RSTV0903_AGC1ADJ,    0x0e }    ,/* AGC1ADJ */
    { RSTV0903_AGC2I1,    0x00 }    ,/* AGC2I1 */
    { RSTV0903_AGC2I0,    0x60 }    ,/* AGC2I0 */
    { RSTV0903_CARCFG,    0xe4 }    ,/* CARCFG */
    { RSTV0903_ACLC,    0x1a }    ,/* ACLC */
    { RSTV0903_BCLC,    0x09 }    ,/* BCLC */
    { RSTV0903_CARFREQ,    0xed }    ,/* CARFREQ */
    { RSTV0903_CARHDR,    0x08 }    ,/* CARHDR */
    { RSTV0903_LDT,    0xd0 }    ,/* LDT */
    { RSTV0903_LDT2,    0xb0 }    ,/* LDT2 */
    { RSTV0903_CFRICFG,    0xf8 }    ,/* CFRICFG */
    { RSTV0903_CFRUP1,    0x35 }    ,/* CFRUP1 */
    { RSTV0903_CFRUP0,    0x7f }    ,/* CFRUP0 */
    { RSTV0903_CFRLOW1,    0xca }    ,/* CFRLOW1 */
    { RSTV0903_CFRLOW0,    0x81 }    ,/* CFRLOW0 */
    { RSTV0903_CFRINIT1,    0xc1 }    ,/* CFRINIT1 */
    { RSTV0903_CFRINIT0,    0x81 }    ,/* CFRINIT0 */
    { RSTV0903_CFRINC1,    0x03 }    ,/* CFRINC1 */
    { RSTV0903_CFRINC0,    0x01 }    ,/* CFRINC0 */
    { RSTV0903_CFR2,    0xc1 }    ,/* CFR2 */
    { RSTV0903_CFR1,    0x81 }    ,/* CFR1 */
    { RSTV0903_CFR0,    0x00 }    ,/* CFR0 */
    { RSTV0903_LDI,    0x00 }    ,/* LDI */
    { RSTV0903_TMGCFG,    0xd2 }    ,/* TMGCFG */
    { RSTV0903_RTC,    0x88 }    ,/* RTC */
    { RSTV0903_RTCS2,    0x66 }    ,/* RTCS2 */
    { RSTV0903_TMGTHRISE,    0x20 }    ,/* TMGTHRISE */
    { RSTV0903_TMGTHFALL,    0x00 }    ,/* TMGTHFALL */
    { RSTV0903_SFRUPRATIO,    0xf0 }    ,/* SFRUPRATIO */
    { RSTV0903_SFRLOWRATIO,    0x70 }    ,/* SFRLOWRATIO */
    { RSTV0903_KREFTMG,    0xc1 }    ,/* KREFTMG */
    { RSTV0903_SFRSTEP,    0x58 }    ,/* SFRSTEP */
    { RSTV0903_TMGCFG2,    0x01 }    ,/* TMGCFG2 */
    { RSTV0903_SFRINIT1,    0x03 }    ,/* SFRINIT1 */
    { RSTV0903_SFRINIT0,    0x01 }    ,/* SFRINIT0 */
    { RSTV0903_SFRUP1,    0xaa }    ,/* SFRUP1 */
    { RSTV0903_SFRUP0,    0x9c }    ,/* SFRUP0 */
    { RSTV0903_SFRLOW1,    0x82 }    ,/* SFRLOW1 */
    { RSTV0903_SFRLOW0,    0x41 }    ,/* SFRLOW0 */
    { RSTV0903_SFR3,    0x03 }    ,/* SFR3 */
    { RSTV0903_SFR2,    0x01 }    ,/* SFR2 */
    { RSTV0903_SFR1,    0x00 }    ,/* SFR1 */
    { RSTV0903_SFR0,    0x00 }    ,/* SFR0 */
    { RSTV0903_TMGREG2,    0x00 }    ,/* TMGREG2 */
    { RSTV0903_TMGREG1,    0x00 }    ,/* TMGREG1 */
    { RSTV0903_TMGREG0,    0x00 }    ,/* TMGREG0 */
    { RSTV0903_TMGLOCK1,    0x00 }    ,/* TMGLOCK1 */
    { RSTV0903_TMGLOCK0,    0x00 }    ,/* TMGLOCK0 */
    { RSTV0903_TMGOBS,    0x10 }    ,/* TMGOBS */
    { RSTV0903_EQUALCFG,    0x41 }    ,/* EQUALCFG */
    { RSTV0903_EQUAI1,    0x00 }    ,/* EQUAI1 */
    { RSTV0903_EQUAQ1,    0x00 }    ,/* EQUAQ1 */
    { RSTV0903_EQUAI2,    0x00 }    ,/* EQUAI2 */
    { RSTV0903_EQUAQ2,    0x00 }    ,/* EQUAQ2 */
    { RSTV0903_EQUAI3,    0x00 }    ,/* EQUAI3 */
    { RSTV0903_EQUAQ3,    0x00 }    ,/* EQUAQ3 */
    { RSTV0903_EQUAI4,    0x00 }    ,/* EQUAI4 */
    { RSTV0903_EQUAQ4,    0x00 }    ,/* EQUAQ4 */
    { RSTV0903_EQUAI5,    0x00 }    ,/* EQUAI5 */
    { RSTV0903_EQUAQ5,    0x00 }    ,/* EQUAQ5 */
    { RSTV0903_EQUAI6,    0x00 }    ,/* EQUAI6 */
    { RSTV0903_EQUAQ6,    0x00 }    ,/* EQUAQ6 */
    { RSTV0903_EQUAI7,    0x00 }    ,/* EQUAI7 */
    { RSTV0903_EQUAQ7,    0x00 }    ,/* EQUAQ7 */
    { RSTV0903_EQUAI8,    0x00 }    ,/* EQUAI8 */
    { RSTV0903_EQUAQ8,    0x00 }    ,/* EQUAQ8 */
    { RSTV0903_NNOSDATAT1,    0x5a }    ,/* NNOSDATAT1 */
    { RSTV0903_NNOSDATAT0,    0x8a }    ,/* NNOSDATAT0 */
    { RSTV0903_NNOSDATA1,    0x80 }    ,/* NNOSDATA1 */
    { RSTV0903_NNOSDATA0,    0xb0 }    ,/* NNOSDATA0 */
    { RSTV0903_NNOSPLHT1,    0x27 }    ,/* NNOSPLHT1 */
    { RSTV0903_NNOSPLHT0,    0x66 }    ,/* NNOSPLHT0 */
    { RSTV0903_NNOSPLH1,    0x00 }    ,/* NNOSPLH1 */
    { RSTV0903_NNOSPLH0,    0x00 }    ,/* NNOSPLH0 */
    { RSTV0903_NOSDATAT1,    0x4f }    ,/* NOSDATAT1 */
    { RSTV0903_NOSDATAT0,    0x39 }    ,/* NOSDATAT0 */
    { RSTV0903_NOSDATA1,    0x62 }    ,/* NOSDATA1 */
    { RSTV0903_NOSDATA0,    0x87 }    ,/* NOSDATA0 */
    { RSTV0903_NOSPLHT1,    0x22 }    ,/* NOSPLHT1 */
    { RSTV0903_NOSPLHT0,    0x7a }    ,/* NOSPLHT0 */
    { RSTV0903_NOSPLH1,    0x00 }    ,/* NOSPLH1 */
    { RSTV0903_NOSPLH0,    0x00 }    ,/* NOSPLH0 */
    { RSTV0903_CAR2CFG,    0x26 }    ,/* CAR2CFG */
    { RSTV0903_ACLC2,    0x29 }    ,/* ACLC2 */
    { RSTV0903_BCLC2,    0x2b }    ,/* BCLC2 */
    { RSTV0903_CFR22,    	0x00	}    ,/* P2_CFR22 */
    { RSTV0903_CFR21,    	0x00	}    ,/* P2_CFR21 */
    { RSTV0903_CFR20,    	0x00	}    ,/* P2_CFR20 */
    { RSTV0903_ACLC2S2Q,    0x68 }    ,/* ACLC2S2Q */
    { RSTV0903_ACLC2S28,    0x68 }    ,/* ACLC2S28 */
    { RSTV0903_BCLC2S2Q,    0x86 }    ,/* BCLC2S2Q */
    { RSTV0903_BCLC2S28,    0x86 }    ,/* BCLC2S28 */
    { RSTV0903_PLROOT2,    0x00 }    ,/* PLROOT2 */
    { RSTV0903_PLROOT1,    0x00 }    ,/* PLROOT1 */
    { RSTV0903_PLROOT0,    0x01 }    ,/* PLROOT0 */
    { RSTV0903_MODCODLST0,    0xff }    ,/* MODCODLST0 */
    { RSTV0903_MODCODLST1,    0xff }    ,/* MODCODLST1 */
    { RSTV0903_MODCODLST2,    0xff }    ,/* MODCODLST2 */
    { RSTV0903_MODCODLST3,    0xff }    ,/* MODCODLST3 */
    { RSTV0903_MODCODLST4,    0xff }    ,/* MODCODLST4 */
    { RSTV0903_MODCODLST5,    0xff }    ,/* MODCODLST5 */
    { RSTV0903_MODCODLST6,    0xff }    ,/* MODCODLST6 */
    { RSTV0903_MODCODLST7,    0xcc }    ,/* MODCODLST7 */
    { RSTV0903_MODCODLST8,    0xcc }    ,/* MODCODLST8 */
    { RSTV0903_MODCODLST9,    0xcc }    ,/* MODCODLST9 */
    { RSTV0903_MODCODLSTA,    0xcc }    ,/* MODCODLSTA */
    { RSTV0903_MODCODLSTB,    0xcc }    ,/* MODCODLSTB */
    { RSTV0903_MODCODLSTC,    0xcc }    ,/* MODCODLSTC */
    { RSTV0903_MODCODLSTD,    0xcc }    ,/* MODCODLSTD */
    { RSTV0903_MODCODLSTE,    0xcc }    ,/* MODCODLSTE */
    { RSTV0903_MODCODLSTF,    0xcf }    ,/* MODCODLSTF */
    { RSTV0903_DMDRESCFG,    0x29 }    ,/* DMDRESCFG */
    { RSTV0903_DMDRESADR,    0x3f }    ,/* DMDRESADR */
    { RSTV0903_DMDRESDATA7,    0x4e }    ,/* DMDRESDATA7 */
    { RSTV0903_DMDRESDATA6,    0x80 }    ,/* DMDRESDATA6 */
    { RSTV0903_DMDRESDATA5,    0x00 }    ,/* DMDRESDATA5 */
    { RSTV0903_DMDRESDATA4,    0x00 }    ,/* DMDRESDATA4 */
    { RSTV0903_DMDRESDATA3,    0x72 }    ,/* DMDRESDATA3 */
    { RSTV0903_DMDRESDATA2,    0x3c }    ,/* DMDRESDATA2 */
    { RSTV0903_DMDRESDATA1,    0x34 }    ,/* DMDRESDATA1 */
    { RSTV0903_DMDRESDATA0,    0x07 }    ,/* DMDRESDATA0 */
    { RSTV0903_FFEI1,    0x00 }    ,/* FFEI1 */
    { RSTV0903_FFEQ1,    0x00 }    ,/* FFEQ1 */
    { RSTV0903_FFEI2,    0x00 }    ,/* FFEI2 */
    { RSTV0903_FFEQ2,    0x00 }    ,/* FFEQ2 */
    { RSTV0903_FFEI3,    0x00 }    ,/* FFEI3 */
    { RSTV0903_FFEQ3,    0x00 }    ,/* FFEQ3 */
    { RSTV0903_FFEI4,    0x00 }    ,/* FFEI4 */
    { RSTV0903_FFEQ4,    0x00 }    ,/* FFEQ4 */
    { RSTV0903_FFECFG,    0x00 }    ,/* FFECFG */
    { RSTV0903_TNRCFG,    0x6c }    ,/* TNRCFG */
    { RSTV0903_TNRCFG2,    0x82 }    ,/* TNRCFG2 */
    { RSTV0903_TNRXTAL,    0x1b }    ,/* TNRXTAL */
    { RSTV0903_TNRSTEPS,    0x87 }    ,/* TNRSTEPS */
    { RSTV0903_TNRGAIN,    0x09 }    ,/* TNRGAIN */
    { RSTV0903_TNRRF1,    0x7a }    ,/* TNRRF1 */
    { RSTV0903_TNRRF0,    0x40 }    ,/* TNRRF0 */
    { RSTV0903_TNRBW,    0x1c }    ,/* TNRBW */
    { RSTV0903_TNRADJ,    0x1f }    ,/* TNRADJ */
    { RSTV0903_TNRCTL2,    0x00 }    ,/* TNRCTL2 */
    { RSTV0903_TNRCFG3,    0x02 }    ,/* TNRCFG3 */
    { RSTV0903_TNRLAUNCH,    0x00 }    ,/* TNRLAUNCH */
    { RSTV0903_TNRLD,    0x00 }    ,/* TNRLD */
    { RSTV0903_TNROBSL,    0x50 }    ,/* TNROBSL */
    { RSTV0903_TNRRESTE,    0xa8 }    ,/* TNRRESTE */
    { RSTV0903_SMAPCOEF7,    0x77 }    ,/* SMAPCOEF7 */
    { RSTV0903_SMAPCOEF6,    0x85 }    ,/* SMAPCOEF6 */
    { RSTV0903_SMAPCOEF5,    0x77 }    ,/* SMAPCOEF5 */
    { RSTV0903_DMDPLHSTAT,    0x00 }    ,/* DMDPLHSTAT */
    { RSTV0903_LOCKTIME3,    0x00 }    ,/* LOCKTIME3 */
    { RSTV0903_LOCKTIME2,    0x59 }    ,/* LOCKTIME2 */
    { RSTV0903_LOCKTIME1,    0xe1 }    ,/* LOCKTIME1 */
    { RSTV0903_LOCKTIME0,    0xe8 }    ,/* LOCKTIME0 */
    { RSTV0903_VITSCALE,    0x00 }    ,/* VITSCALE */
    { RSTV0903_FECM,    0x00 }    ,/* FECM */
    { RSTV0903_VTH12,    0xe8 }    ,/* VTH12 */
    { RSTV0903_VTH23,    0xa0 }    ,/* VTH23 */
    { RSTV0903_VTH34,    0x78 }    ,/* VTH34 */
    { RSTV0903_VTH56,    0x4e }    ,/* VTH56 */
    { RSTV0903_VTH67,    0x48 }    ,/* VTH67 */
    { RSTV0903_VTH78,    0x38 }    ,/* VTH78 */
    { RSTV0903_VITCURPUN,    0x0d }    ,/* VITCURPUN */
    { RSTV0903_VERROR,    0xff }    ,/* VERROR */
    { RSTV0903_PRVIT,    0x2f }    ,/* PRVIT */
    { RSTV0903_VAVSRVIT,    0x00 }    ,/* VAVSRVIT */
    { RSTV0903_VSTATUSVIT,    0xa5 }    ,/* VSTATUSVIT */
    { RSTV0903_VTHINUSE,    0x00 }    ,/* VTHINUSE */
    { RSTV0903_KDIV12,    0x25 }    ,/* KDIV12 */
    { RSTV0903_KDIV23,    0x3e }    ,/* KDIV23 */
    { RSTV0903_KDIV34,    0x55 }    ,/* KDIV34 */
    { RSTV0903_KDIV56,    0x6b }    ,/* KDIV56 */
    { RSTV0903_KDIV67,    0x72 }    ,/* KDIV67 */
    { RSTV0903_KDIV78,    0x7f }    ,/* KDIV78 */
    { RSTV0903_PDELCTRL1,    0x00 }    ,/* PDELCTRL1 */
    { RSTV0903_PDELCTRL2,    0x00 }    ,/* PDELCTRL2 */
    { RSTV0903_HYSTTHRESH,    0x41 }    ,/* HYSTTHRESH */
    { RSTV0903_ISIENTRY,    0x00 }    ,/* ISIENTRY */
    { RSTV0903_ISIBITENA,    0x00 }    ,/* ISIBITENA */
    { RSTV0903_MATSTR1,    0xf0 }    ,/* MATSTR1 */
    { RSTV0903_MATSTR0,    0x00 }    ,/* MATSTR0 */
    { RSTV0903_UPLSTR1,    0x05 }    ,/* UPLSTR1 */
    { RSTV0903_UPLSTR0,    0xe0 }    ,/* UPLSTR0 */
    { RSTV0903_DFLSTR1,    0x7d }    ,/* DFLSTR1 */
    { RSTV0903_DFLSTR0,    0x80 }    ,/* DFLSTR0 */
    { RSTV0903_SYNCSTR,    0x47 }    ,/* SYNCSTR */
    { RSTV0903_SYNCDSTR1,    0x00 }    ,/* SYNCDSTR1 */
    { RSTV0903_SYNCDSTR0,    0x00 }    ,/* SYNCDSTR0 */
    { RSTV0903_PDELSTATUS1,    0x04 }    ,/* PDELSTATUS1 */
    { RSTV0903_PDELSTATUS2,    0x00 }    ,/* PDELSTATUS2 */
    { RSTV0903_BBFCRCKO1,    0x00 }    ,/* BBFCRCKO1 */
    { RSTV0903_BBFCRCKO0,    0x00 }    ,/* BBFCRCKO0 */
    { RSTV0903_UPCRCKO1,    0x00 }    ,/* UPCRCKO1 */
    { RSTV0903_UPCRCKO0,    0x00 }    ,/* UPCRCKO0 */
    { RSTV0903_TSSTATEM,    0xb0 }    ,/* TSSTATEM */
    { RSTV0903_TSCFGH,    0x40 }    ,/* TSCFGH */
    { RSTV0903_TSCFGM,    0x00 }    ,/* TSCFGM */
    { RSTV0903_TSCFGL,    0x20 }    ,/* TSCFGL */
    { RSTV0903_TSINSDELH,    0x00 }    ,/* TSINSDELH */
    { RSTV0903_TSSPEED,    0xff }    ,/* TSSPEED */
    { RSTV0903_TSSTATUS,    0x42 }    ,/* TSSTATUS */
    { RSTV0903_TSSTATUS2,    0x66 }    ,/* TSSTATUS2 */
    { RSTV0903_TSBITRATE1,    0x00 }    ,/* TSBITRATE1 */
    { RSTV0903_TSBITRATE0,    0x00 }    ,/* TSBITRATE0 */
    { RSTV0903_ERRCTRL1,    0x35 }    ,/* ERRCTRL1 */
    { RSTV0903_ERRCNT12,    0x80 }    ,/* ERRCNT12 */
    { RSTV0903_ERRCNT11,    0x00 }    ,/* ERRCNT11 */
    { RSTV0903_ERRCNT10,    0x00 }    ,/* ERRCNT10 */
    { RSTV0903_ERRCTRL2,    0xc1 }    ,/* ERRCTRL2 */
    { RSTV0903_ERRCNT22,    0x00 }    ,/* ERRCNT22 */
    { RSTV0903_ERRCNT21,    0x00 }    ,/* ERRCNT21 */
    { RSTV0903_ERRCNT20,    0x00 }    ,/* ERRCNT20 */
    { RSTV0903_FECSPY,    0xa8 }    ,/* FECSPY */
    { RSTV0903_FSPYCFG,    0x2c }    ,/* FSPYCFG */
    { RSTV0903_FSPYDATA,    0x3a }    ,/* FSPYDATA */
    { RSTV0903_FSPYOUT,    0x07 }    ,/* FSPYOUT */
    { RSTV0903_FSTATUS,    0x00 }    ,/* FSTATUS */
    { RSTV0903_FBERCPT4,    0x00 }    ,/* FBERCPT4 */
    { RSTV0903_FBERCPT3,    0x00 }    ,/* FBERCPT3 */
    { RSTV0903_FBERCPT2,    0x00 }    ,/* FBERCPT2 */
    { RSTV0903_FBERCPT1,    0x00 }    ,/* FBERCPT1 */
    { RSTV0903_FBERCPT0,    0x00 }    ,/* FBERCPT0 */
    { RSTV0903_FBERERR2,    0x00 }    ,/* FBERERR2 */
    { RSTV0903_FBERERR1,    0x00 }    ,/* FBERERR1 */
    { RSTV0903_FBERERR0,    0x00 }    ,/* FBERERR0 */
    { RSTV0903_FSPYBER,    0x10 }    ,/* FSPYBER */
    { RSTV0903_RCCFGH,    0x20 }    ,/* RCCFGH */
    { RSTV0903_TSGENERAL,    0x00 }    ,/* TSGENERAL */
    { RSTV0903_TSGENERAL1X,    0x00 }    ,/* TSGENERAL1X */
    { RSTV0903_NBITER_NF4,    0x37 }    ,/* NBITER_NF4 */
    { RSTV0903_NBITER_NF5,    0x29 }    ,/* NBITER_NF5 */
    { RSTV0903_NBITER_NF6,    0x37 }    ,/* NBITER_NF6 */
    { RSTV0903_NBITER_NF7,    0x33 }    ,/* NBITER_NF7 */
    { RSTV0903_NBITER_NF8,    0x31 }    ,/* NBITER_NF8 */
    { RSTV0903_NBITER_NF9,    0x2f }    ,/* NBITER_NF9 */
    { RSTV0903_NBITER_NF10,    0x39 }    ,/* NBITER_NF10 */
    { RSTV0903_NBITER_NF11,    0x3a }    ,/* NBITER_NF11 */
    { RSTV0903_NBITER_NF12,    0x29 }    ,/* NBITER_NF12 */
    { RSTV0903_NBITER_NF13,    0x37 }    ,/* NBITER_NF13 */
    { RSTV0903_NBITER_NF14,    0x33 }    ,/* NBITER_NF14 */
    { RSTV0903_NBITER_NF15,    0x2f }    ,/* NBITER_NF15 */
    { RSTV0903_NBITER_NF16,    0x39 }    ,/* NBITER_NF16 */
    { RSTV0903_NBITER_NF17,    0x3a }    ,/* NBITER_NF17 */
    { RSTV0903_NBITERNOERR,    0x04 }    ,/* NBITERNOERR */
    { RSTV0903_GAINLLR_NF4,    0x0c }    ,/* GAINLLR_NF4 */
    { RSTV0903_GAINLLR_NF5,    0x0f }    ,/* GAINLLR_NF5 */
    { RSTV0903_GAINLLR_NF6,    0x11 }    ,/* GAINLLR_NF6 */
    { RSTV0903_GAINLLR_NF7,    0x14 }    ,/* GAINLLR_NF7 */
    { RSTV0903_GAINLLR_NF8,    0x17 }    ,/* GAINLLR_NF8 */
    { RSTV0903_GAINLLR_NF9,    0x19 }    ,/* GAINLLR_NF9 */
    { RSTV0903_GAINLLR_NF10,    0x20 }    ,/* GAINLLR_NF10 */
    { RSTV0903_GAINLLR_NF11,    0x21 }    ,/* GAINLLR_NF11 */
    { RSTV0903_GAINLLR_NF12,    0x0d }    ,/* GAINLLR_NF12 */
    { RSTV0903_GAINLLR_NF13,    0x0f }    ,/* GAINLLR_NF13 */
    { RSTV0903_GAINLLR_NF14,    0x13 }    ,/* GAINLLR_NF14 */
    { RSTV0903_GAINLLR_NF15,    0x1a }    ,/* GAINLLR_NF15 */
    { RSTV0903_GAINLLR_NF16,    0x1f }    ,/* GAINLLR_NF16 */
    { RSTV0903_GAINLLR_NF17,    0x21 }    ,/* GAINLLR_NF17 */
    { RSTV0903_CFGEXT,    0x01 }    ,/* CFGEXT */
    { RSTV0903_GENCFG,    0x1c }    ,/* GENCFG */
    { RSTV0903_LDPCERR1,    0x00 }    ,/* LDPCERR1 */
    { RSTV0903_LDPCERR0,    0x00 }    ,/* LDPCERR0 */
    { RSTV0903_BCHERR,    0x00 }    ,/* BCHERR */
    { RSTV0903_TSTRES0,    0x00 }    ,/* TSTRES0 */
    { RSTV0903_P2_TSTDISRX,    0x00 }    ,/* P2_TSTDISRX */
    { RSTV0903_TSTDISRX,    0x00 }    /* TSTDISRX */
};

const	unsigned int  STV0903_InitVal[102][2]=
	{
		{	RSTV0903_OUTCFG			,0x00	},
		{	RSTV0903_AGCRF1CFG		,0x11	},
		{	RSTV0903_STOPCLK1		,0x48	},
		{	RSTV0903_STOPCLK2		,0x14	},
		{	RSTV0903_TSTTNR1		,0x27	},
		{	RSTV0903_TSTTNR2		,0x21	},
		{	RSTV0903_DISTXCTL		,0x22	},
		{	RSTV0903_F22TX			,0xc0	},
		{	RSTV0903_F22RX			,0xc0	},
		{	RSTV0903_DISRXCTL		,0x00	},
		{	RSTV0903_TNRSTEPS		,0x87	},
		{	RSTV0903_TNRGAIN		,0x09	},
		{	RSTV0903_DMDCFGMD		,0xF9	},
		{	RSTV0903_DEMOD			,0x08	},
		{	RSTV0903_DMDCFG3		,0xc4	},
		{	RSTV0903_CARFREQ		,0xed	},
		{	RSTV0903_TNRCFG2		,0x82	},
		{	RSTV0903_TNRCFG3		,0x03	},
		{	RSTV0903_LDT			,0xd0	},
		{	RSTV0903_LDT2			,0xb8	},
		{	RSTV0903_TMGCFG			,0xd2	},
		{	RSTV0903_TMGTHRISE		,0x20	},
		{	RSTV0903_TMGTHFALL		,0x00	},
		{	RSTV0903_SFRUPRATIO		,0xf0	},
		{	RSTV0903_SFRLOWRATIO	,0x70	},
		{	RSTV0903_TSCFGL			,0x20	},
		{	RSTV0903_FECSPY			,0x88	},
		{	RSTV0903_FSPYDATA		,0x3a	},
		{	RSTV0903_FBERCPT4		,0x00	},
		{	RSTV0903_FSPYBER		,0x10	},
		{	RSTV0903_ERRCTRL1		,0x35	},
		{	RSTV0903_ERRCTRL2		,0xc1	},
		{	RSTV0903_CFRICFG		,0xf8	},
		{	RSTV0903_NOSCFG			,0x1c	},
		{	RSTV0903_DMDT0M			,0x20	},
		{	RSTV0903_CORRELMANT		,0x70	},
		{	RSTV0903_CORRELABS		,0x88	},
		{	RSTV0903_AGC2O			,0x5b	},
		{	RSTV0903_AGC2REF		,0x38	} ,
		{	RSTV0903_CARCFG			,0xe4	},
		{	RSTV0903_ACLC			,0x1A	},
		{	RSTV0903_BCLC			,0x09	} ,
		{	RSTV0903_CARHDR			,0x08	},
		{	RSTV0903_KREFTMG		,0xc1	},
		{	RSTV0903_SFRSTEP		,0x58	},
		{	RSTV0903_TMGCFG2		,0x01	},
		{	RSTV0903_CAR2CFG		,0x26	},
		{	RSTV0903_BCLC2S2Q		,0x86	},
		{	RSTV0903_BCLC2S28		,0x86	},
		{	RSTV0903_SMAPCOEF7		,0x77	},
		{	RSTV0903_SMAPCOEF6		,0x85	},
		{	RSTV0903_SMAPCOEF5		,0x77	},
		{	RSTV0903_DMDCFG2		,0x3b	},
		{	RSTV0903_MODCODLST0		,0xff	},
		{	RSTV0903_MODCODLST1		,0xff	},
		{	RSTV0903_MODCODLST2		,0xff	},
		{	RSTV0903_MODCODLST3		,0xff	},
		{	RSTV0903_MODCODLST4		,0xff	},
		{	RSTV0903_MODCODLST5		,0xff	},
		{	RSTV0903_MODCODLST6		,0xff	},
		{	RSTV0903_MODCODLST7		,0xcc	},
		{	RSTV0903_MODCODLST8		,0xcc	},
		{	RSTV0903_MODCODLST9		,0xcc	},
		{	RSTV0903_MODCODLSTA		,0xcc	},
		{	RSTV0903_MODCODLSTB		,0xcc	},
		{	RSTV0903_MODCODLSTC		,0xcc	},
		{	RSTV0903_MODCODLSTD		,0xcc	},
		{	RSTV0903_MODCODLSTE		,0xcc	},
		{	RSTV0903_MODCODLSTF		,0xcf	},
		{	RSTV0903_GENCFG			,0x1c	},
		{	RSTV0903_NBITER_NF4		,0x37	},
		{	RSTV0903_NBITER_NF5		,0x29	},
		{	RSTV0903_NBITER_NF6		,0x37	},
		{	RSTV0903_NBITER_NF7		,0x33	},
		{	RSTV0903_NBITER_NF8		,0x31	},
		{	RSTV0903_NBITER_NF9		,0x2f	},
		{	RSTV0903_NBITER_NF10	,0x39	},
		{	RSTV0903_NBITER_NF11	,0x3a	},
		{	RSTV0903_NBITER_NF12	,0x29	},
		{	RSTV0903_NBITER_NF13	,0x37	},
		{	RSTV0903_NBITER_NF14	,0x33	},
		{	RSTV0903_NBITER_NF15	,0x2f	},
		{	RSTV0903_NBITER_NF16	,0x39	},
		{	RSTV0903_NBITER_NF17	,0x3a	},
		{	RSTV0903_NBITERNOERR	,0x04	},
		{	RSTV0903_GAINLLR_NF4	,0x0C	},
		{	RSTV0903_GAINLLR_NF5	,0x0F	},
		{	RSTV0903_GAINLLR_NF6	,0x11	},
		{	RSTV0903_GAINLLR_NF7	,0x14	},
		{	RSTV0903_GAINLLR_NF8	,0x17	},
		{	RSTV0903_GAINLLR_NF9	,0x19	},
		{	RSTV0903_GAINLLR_NF10	,0x20	},
		{	RSTV0903_GAINLLR_NF11	,0x21	},
		{	RSTV0903_GAINLLR_NF12	,0x0D	},
		{	RSTV0903_GAINLLR_NF13	,0x0F	},
		{	RSTV0903_GAINLLR_NF14	,0x13	},
		{	RSTV0903_GAINLLR_NF15	,0x1A	},
		{	RSTV0903_GAINLLR_NF16	,0x1F	},
		{	RSTV0903_GAINLLR_NF17	,0x21	},
		{	RSTV0903_RCCFGH			,0x20	},
		{	RSTV0903_FECM			,0x01	}, /*disable the DSS mode */
		{	RSTV0903_PRVIT			,0x2f	}  /*disable puncture rate 6/7*/
			
	};


	const U16 STV0903_Cut20_AddOnVal[23][2]=
	{
		{	RSTV0903_DMDCFG3		,0xe8	},
		{	RSTV0903_DMDCFG4		,0x10	},
		{	RSTV0903_CARFREQ		,0x38	},
		{	RSTV0903_CARHDR			,0x20	},
		{	RSTV0903_KREFTMG		,0x5a	},
		{	RSTV0903_SMAPCOEF7		,0x06	},
		{	RSTV0903_SMAPCOEF6		,0x00	},
		{	RSTV0903_SMAPCOEF5		,0x04	},
		{	RSTV0903_NOSCFG			,0x0c	},
		{	RSTV0903_GAINLLR_NF4	,0x21	},
		{	RSTV0903_GAINLLR_NF5	,0x21	},
		{	RSTV0903_GAINLLR_NF6	,0x20	},
		{	RSTV0903_GAINLLR_NF7	,0x1F	},
		{	RSTV0903_GAINLLR_NF8	,0x1E	},
		{	RSTV0903_GAINLLR_NF9	,0x1E	},
		{	RSTV0903_GAINLLR_NF10	,0x1D	},
		{	RSTV0903_GAINLLR_NF11	,0x1B	},
		{	RSTV0903_GAINLLR_NF12	,0x20	},
		{	RSTV0903_GAINLLR_NF13	,0x20	},
		{	RSTV0903_GAINLLR_NF14	,0x20	},
		{	RSTV0903_GAINLLR_NF15	,0x20	},
		{	RSTV0903_GAINLLR_NF16	,0x20	},
		{	RSTV0903_GAINLLR_NF17	,0x21	}
	};	
	
void FE_STV0903_SetTunerType_E(void)
{//no  f4e0 no 0xf4f0 
	
 	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_TYPE,6);// 6 
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_MADDRESS,0);
	
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TNRLD,1);

}
void pll_test(void)
{

	//ChipSetOneRegister_E(0xd0,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SYNTCTRL,0x80);
	FE_STV0903_SetMclk_E(135000000,8000000); /*Set the Mclk value to 135MHz*/ 
	WAIT_N_MS(3); /*wait for PLL lock*/
	ChipSetOneRegister_E(0xd0,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SYNTCTRL,0x22);
}
#if 0
void STV0903Init(FE_STV0903_InternalParams_t_E* pParams)
{
	U8 i;
	U8 chipaddress=STV0903_I2C_ADDRESS; 
	#if 1
	//FE_STV0903_SetTunerType_E();
	
	//ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x5c);	/* Stop the demod  */
	WAIT_N_MS(3);										/* wait for demod stop (i.e. no more tuner I2C transactions) */	
	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TNRCFG,0x6c);		/* put the Tuner HW controller 1 to NO_TUNER mode */
	//ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_I2CRPT,0x44);		/* Configure the I2C repeater to off */
				
       // ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_NCOARSE,0x13);	/* Set the PLL divider */
	//ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_NCOARSE,0x43);	/* Set the PLL divider */

         WAIT_N_MS(3);										/* WAIT for 3 ms	*/
	
	//ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_I2CCFG,0x08);		/* Set the I2C to oversampling ratio 1/41 mode */ 
	//ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SYNTCTRL,0x22);	/* enable the PLL and end of STANDBY, clock input = XTALI  
	//															   Set this register to 0x20 for clock input = CLKI
	//															 */
	WAIT_N_MS(3);										/* WAIT for 3 ms	*/
	/*End of Startup Sequence */
#endif
		
	#if 0
	for(i=0;i<102;i++) 
			ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,STV0903_InitVal[i][0],STV0903_InitVal[i][1]);
	
	//ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,STV0903_InitVal[1][0],STV0903_InitVal[1][1]);;
		
	if(ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MID)>=0x20)
	{
		/*If cut 2.0 above apply addon init values*/
		for(i=0;i<23;i++)
		ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,STV0903_Cut20_AddOnVal[i][0],STV0903_Cut20_AddOnVal[i][1]);
	}
				
	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TSTRES0,0x80); 
	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TSTRES0,0x00);
	
	
	#endif
	STV6110_TunerInit_E();
	#if 0
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_IQSWAP,pParams->TunerGlobal_IQ_Inv);
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ROLLOFF_CONTROL,pParams->RollOff);
	FE_STV0903_SetTS_Parallel_Serial_E(FE_TS_PARALLEL_PUNCT_CLOCK);
	switch (pParams->TunerType)
	{
		case FE_SAT_SW_TUNER:
		default:
		ChipSetField_E(0xc0,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CO_DIV,1);			
		FE_Sat_TunerSetGain_E(6);

		
						
		break;
						
		case FE_SAT_AUTO_STB6000:
		case FE_SAT_AUTO_STB6100:
		case FE_SAT_AUTO_STV6110: 
		FE_STV0903_SetTunerType_E();
		ChipGetField_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_ACKFAIL);
		break;		
	}
	#endif
	//pParams->Quartz=16000000;              // 16MHZ
	pParams->Quartz=8000000;              // 16MHZ
	FE_STV0903_SetMclk_E(135000000,pParams->Quartz); /*Set the Mclk value to 135MHz*/ 
	WAIT_N_MS(300); /*wait for PLL lock*/
	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SYNTCTRL,0x22);		/* switch the 900 to the PLL*/
	//pParams->TunerType=pInit->TunerHWControlType;
	
}
#endif
#if 1
void STV0903Init(FE_STV0903_InternalParams_t_E* pParams)
{
	U8 i;
	U8 chipaddress=STV0903_I2C_ADDRESS; 
	#if 1
	FE_STV0903_SetTunerType_E();
	#endif
	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x5c);	/* Stop the demod  */
	WAIT_N_MS(3);										/* wait for demod stop (i.e. no more tuner I2C transactions) */	
	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TNRCFG,0x6c);		/* put the Tuner HW controller 1 to NO_TUNER mode */
	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_I2CRPT,0x44);		/* Configure the I2C repeater to off */
				
        ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_NCOARSE,0x13);	/* Set the PLL divider */
	//ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_NCOARSE,0x43);	/* Set the PLL divider */

         WAIT_N_MS(3);										/* WAIT for 3 ms	*/
	
	// the evil reg
	//ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_I2CCFG,0x08);		/* Set the I2C to oversampling ratio 1/41 mode */ 

	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SYNTCTRL,0x22);	/* enable the PLL and end of STANDBY, clock input = XTALI  
																   Set this register to 0x20 for clock input = CLKI
																 */
	WAIT_N_MS(3);										/* WAIT for 3 ms	*/
	/*End of Startup Sequence */

		

	for(i=0;i<102;i++) {
			ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,STV0903_InitVal[i][0],STV0903_InitVal[i][1]);
	}
	
	//ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,STV0903_InitVal[1][0],STV0903_InitVal[1][1]);;
		#if 1	
	if(ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MID)>=0x20)
	{
		/*If cut 2.0 above apply addon init values*/
		for(i=0;i<23;i++)
		ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,STV0903_Cut20_AddOnVal[i][0],STV0903_Cut20_AddOnVal[i][1]);
	}
				
	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TSTRES0,0x80); 
	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TSTRES0,0x00);
	
	
	#endif
	STV6110_TunerInit_E();
	#if 1
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_IQSWAP,pParams->TunerGlobal_IQ_Inv);
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ROLLOFF_CONTROL,pParams->RollOff);
	FE_STV0903_SetTS_Parallel_Serial_E(FE_TS_PARALLEL_PUNCT_CLOCK);
	switch (pParams->TunerType)
	{
		case FE_SAT_SW_TUNER:
		default:
		ChipSetField_E(0xc0,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CO_DIV,1);			
		FE_Sat_TunerSetGain_E(6);

		
						
		break;
						
		case FE_SAT_AUTO_STB6000:
		case FE_SAT_AUTO_STB6100:
		case FE_SAT_AUTO_STV6110: 
		FE_STV0903_SetTunerType_E();
		ChipGetField_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_ACKFAIL);
		break;		
	}
	//pParams->Quartz=16000000;              // 16MHZ
	pParams->Quartz=8000000;              // 16MHZ
	FE_STV0903_SetMclk_E(135000000,pParams->Quartz); /*Set the Mclk value to 135MHz*/ 
	WAIT_N_MS(300); /*wait for PLL lock*/
	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SYNTCTRL,0x22);		/* switch the 900 to the PLL*/
	//pParams->TunerType=pInit->TunerHWControlType;
	STV0903regInit();
	#endif
}
#endif
#if 0
STCHIP_Error_t STV0903_Init(Demod_InitParams_t *InitParams,STCHIP_Handle_t *hChipHandle)
{
    S32 i;
    STCHIP_Handle_t hChip;
    
    STCHIP_Error_t error;
    
   /* fill elements of external chip data structure */
    InitParams->Chip->NbRegs   = STV0903_NBREGS;
    InitParams->Chip->NbFields = STV0903_NBFIELDS;										  
    InitParams->Chip->ChipMode = STCHIP_MODE_SUBADR_16;
    InitParams->Chip->pData = NULL;
   
   #ifdef HOST_PC /*Added flag: Change ///AG*/
	(*hChipHandle) = ChipOpen(InitParams->Chip);
   #endif
	
	hChip=(*hChipHandle);
	if(hChip != NULL)
	{
			
		/*******************************
	    **   CHIP REGISTER MAP IMAGE INITIALIZATION
	    **     ----------------------
	    ********************************/
		ChipUpdateDefaultValues(hChip,STV0903DefVal);
		
		
		/*Read the ID*/
		hChip->ChipID=ChipGetOneRegister(hChip,RSTV0903_MID);
		
		if(hChip->Error == CHIPERR_NO_ERROR)
		{
			/*******************************
		    **   REGISTER CONFIGURATION
		    **     ----------------------
		    ********************************/
		    /*Startup sequence*/
			ChipSetOneRegister(hChip,RSTV0903_DMDISTATE,0x5c);	/* Stop the demod  */
			WAIT_N_MS(3);										/* wait for demod stop (i.e. no more tuner I2C transactions) */	
			ChipSetOneRegister(hChip,RSTV0903_TNRCFG,0x6c);		/* put the Tuner HW controller 1 to NO_TUNER mode */
			ChipSetOneRegister(hChip,RSTV0903_I2CRPT,0x44);		/* Configure the I2C repeater to off */
				
			ChipSetOneRegister(hChip,RSTV0903_NCOARSE,0x13);	/* Set the PLL divider */
			WAIT_N_MS(3);										/* WAIT for 3 ms	*/
	
			ChipSetOneRegister(hChip,RSTV0903_I2CCFG,0x08);		/* Set the I2C to oversampling ratio 1/41 mode */ 
			ChipSetOneRegister(hChip,RSTV0903_SYNTCTRL,0x22);	/* enable the PLL and end of STANDBY, clock input = XTALI  
																   Set this register to 0x20 for clock input = CLKI
																 */
			WAIT_N_MS(3);										/* WAIT for 3 ms	*/
			/*End of Startup Sequence */

		
			for(i=0;i<102;i++)
				ChipSetOneRegister(hChip,STV0903_InitVal[i][0],STV0903_InitVal[i][1]);;
			
			if(ChipGetOneRegister(hChip,RSTV0903_MID)>=0x20)
			{
				/*If cut 2.0 above apply addon init values*/
				for(i=0;i<23;i++)
					ChipSetOneRegister(hChip,STV0903_Cut20_AddOnVal[i][0],STV0903_Cut20_AddOnVal[i][1]);
			}
				
			ChipSetOneRegister(hChip,RSTV0903_TSTRES0,0x80); 
			ChipSetOneRegister(hChip,RSTV0903_TSTRES0,0x00);
		}
		
		error = hChip->Error;
	}
	else
		error = CHIPERR_INVALID_HANDLE;
	
	return error;
}
#endif			

#define W tuner_device_write_register

void tunner_0903_init_reg_table()
{
	int i;
	unsigned char v;

#if 0
	v = 0x5c;
	W(RSTV0903_DMDISTATE, &v, 1);	/* Stop the demod  */
	WAIT_N_MS(3);			/* wait for demod stop (i.e. no more tuner I2C transactions) */	
	v = 0x6c;
	W(RSTV0903_TNRCFG, &v, 1);	/* put the Tuner HW controller 1 to NO_TUNER mode */
	v = 0x44;
	W(RSTV0903_I2CRPT, &v, 1);	/* Configure the I2C repeater to off */
				
	v = 0x13;
        W(RSTV0903_NCOARSE, &v, 1);	/* Set the PLL divider */

	v = 0x43;
	W(RSTV0903_NCOARSE, &v, 1);	/* Set the PLL divider */

	WAIT_N_MS(3);			/* WAIT for 3 ms */
	
	// the evil reg
	v = 0x08;
	W(RSTV0903_I2CCFG, &v, 1);	/* Set the I2C to oversampling ratio 1/41 mode */ 

	v = 0x22;
	W(RSTV0903_SYNTCTRL, &v, 1);	/* enable the PLL and end of STANDBY, clock input = XTALI  
					Set this register to 0x20 for clock input = CLKI
					*/
	WAIT_N_MS(3);			/* WAIT for 3 ms */

	for(i = 0; i < 102; i++)
		W(STV0903_InitVal[i][0], &STV0903_InitVal[i][1], 1);
#else
	v = 0x6c;
	W(RSTV0903_TNRCFG, &v, 1);	/* put the Tuner HW controller 1 to NO_TUNER mode */
	WAIT_N_MS(3);			/* WAIT for 3 ms */
	v = 0x22;
	W(RSTV0903_SYNTCTRL, &v, 1);	/* enable the PLL and end of STANDBY, clock input = XTALI  
					Set this register to 0x20 for clock input = CLKI
					*/
	WAIT_N_MS(3);			/* WAIT for 3 ms */
#endif
}

