/* -------------------------------------------------------------------------
File Name: chip.h

Description: Present a register based interface to hardware connected on an I2C bus.

Copyright (C) 1999-2001 STMicroelectronics

History:
   date: 10-October-2001
version: 1.0.0
 author: SA
comment: STAPIfied by GP

---------------------------------------------------------------------------- */

/* define to prevent recursive inclusion */
#ifndef H_CHIP
#define H_CHIP


/* includes ---------------------------------------------------------------- */

/* STAPI (ST20) requirements */
#if defined(CHIP_STAPI)
 #include "stddefs.h"    /* Standard definitions */
// #include "ioarch.h"
 //#include "ioreg.h"
#endif

/* PC (IA) requirements */
#if defined(HOST_PC)
 #include "globaldefs.h"    /* Standard definitions */
#endif


#ifdef __cplusplus
 extern "C"
 {
#endif                  /* __cplusplus */
#include "stddefs.h"

/* definitions ------------------------------------------------------------- */

/* maximum number of chips that can be opened */
#define MAXNBCHIP 10


/* enumerations------------------------------------------------------------- */


/* access modes for fields and registers */
typedef enum
{
    STCHIP_ACCESS_WR,  /* can be read and written */
    STCHIP_ACCESS_R,   /* only be read from */
    STCHIP_ACCESS_W,   /* only be written to */
    STCHIP_ACCESS_NON  /* cannot be read or written (guarded register, e.g. register skipped by ChipApplyDefaultValues() etc.) */
}
STCHIP_Access_t;


/* register field type */
typedef enum
{
    CHIP_UNSIGNED = 0,
    CHIP_SIGNED = 1
}
STCHIP_FieldType_t;


/* error codes */
typedef enum
{
    CHIPERR_NO_ERROR = 0,       /* No error encountered */
    CHIPERR_INVALID_HANDLE,     /* Using of an invalid chip handle */
    CHIPERR_INVALID_REG_ID,     /* Using of an invalid register */
    CHIPERR_INVALID_FIELD_ID,   /* Using of an Invalid field */
    CHIPERR_INVALID_FIELD_SIZE, /* Using of a field with an invalid size */
    CHIPERR_I2C_NO_ACK,         /* No acknowledge from the chip */
    CHIPERR_I2C_BURST           /* Two many registers accessed in burst mode */
}
STCHIP_Error_t;


/* how to access I2C bus */
typedef enum
{
    STCHIP_MODE_SUBADR_8,       /* <addr><reg8><data><data>        (e.g. demod chip) */
    STCHIP_MODE_SUBADR_16,      /* <addr><reg8><data><data>        (e.g. demod chip) */    
    STCHIP_MODE_NOSUBADR,       /* <addr><data>|<data><data><data> (e.g. tuner chip) */
    STCHIP_MODE_NOSUBADR_RD
}
STCHIP_Mode_t;

/* structures -------------------------------------------------------------- */


/* register information */
typedef struct
{
    U16				Addr;     /* Address */
    U8				Value;    /* Current value */
}
STCHIP_Register_t;


/* data about a specific chip */
typedef struct stchip_Info_t
{
    U8					I2cAddr;          /* Chip I2C address */
    ST_String_t			Name[30];         /* Name of the chip */
    S32					NbRegs;           /* Number of registers in the chip */
    S32					NbFields;         /* Number of fields in the chip */
    STCHIP_Register_t  *pRegMapImage;     /* Pointer to register map */
    STCHIP_Error_t      Error;            /* Error state */
    STCHIP_Mode_t       ChipMode;         /* Access bus in demod (SubAdr) or tuner (NoSubAdr) mode */  
	U8					ChipID;           /* Chip cut ID */				
#if defined(CHIP_STAPI)                   /* pass other STAPI specific parameters to this driver */
   // ST_Partition_t     *MemoryPartition;
#endif

	BOOL                Repeater;         /* Is repeater enabled or not ? */
	struct stchip_Info_t *RepeaterHost;   /* Owner of the repeater */
	STCHIP_Error_t 		(*RepeaterFn)(struct stchip_Info_t *hChip,BOOL State);    /* Pointer to repeater routine */ 
	
	/* Parameters needed for non sub address devices */
	U32					WrStart;		  /* Id of the first writable register */	
	U32					WrSize;           /* Number of writable registers */	
	U32					RdStart;		  /* Id of the first readable register */
	U32					RdSize;			  /* Number of readable registers */

	void				*pData;			  /* pointer to chip data */
}
STCHIP_Info_t;

typedef STCHIP_Info_t *STCHIP_Handle_t;  /* Handle to a chip */
typedef STCHIP_Info_t *TUNER_Handle_t;

typedef STCHIP_Error_t (*STCHIP_RepeaterFn_t)(STCHIP_Handle_t hChip,BOOL State);    /* Pointer to repeater routine */

typedef struct
{
   STCHIP_Info_t *Chip;     	/* pointer to parameters to pass to the CHIP API */
   U32            NbDefVal; 	/* number of default values (must match number of STC registers) */
} 
Demod_InitParams_t;


/* functions --------------------------------------------------------------- */

/* Creation and destruction routines */
STCHIP_Handle_t ChipOpen(STCHIP_Info_t *hChipOpenParams);

STCHIP_Error_t  ChipClose   (STCHIP_Handle_t hChip);

/* Utility routines */
STCHIP_Handle_t ChipGetFirst(void);
STCHIP_Handle_t ChipGetNext(STCHIP_Handle_t hPrevChip);
STCHIP_Handle_t ChipGetHandleFromName(ST_String_t *Name);


STCHIP_Error_t  ChipChangeAddress(STCHIP_Handle_t hChip, U8 I2cAddr);
STCHIP_Error_t  ChipSetRepeater(STCHIP_Handle_t hChip,BOOL Repeater);

STCHIP_Error_t	ChipApplyDefaultValues(STCHIP_Handle_t hChip,U16 RegAddr, U8 RegsVal);  /* reset */
STCHIP_Error_t  ChipUpdateDefaultValues(STCHIP_Handle_t hChip,STCHIP_Register_t  *pRegMap);

//S32             ChipCheckAck(STCHIP_Handle_t hChip);

STCHIP_Error_t  ChipGetError(STCHIP_Handle_t hChip);
STCHIP_Error_t  ChipResetError(STCHIP_Handle_t hChip);

/* Access routines */
STCHIP_Error_t	ChipSetOneRegister(STCHIP_Handle_t hChip,U16 RegAddr,U8 Data); 
S32				ChipGetOneRegister(STCHIP_Handle_t hChip, U16 RegAddr);

STCHIP_Error_t	ChipSetRegisters(STCHIP_Handle_t hChip, U16 FirstReg, S32 NbRegs); 
STCHIP_Error_t	ChipGetRegisters(STCHIP_Handle_t hChip, U16 FirstReg, S32 NbRegs);
void ChipSetRegisters_E(U8 hChipAddress, STCHIP_Mode_t mode,BOOL Repeater,U16 FirstReg, S32 NbRegs,U8* databuf);

STCHIP_Error_t	ChipSetField(STCHIP_Handle_t hChip,U32 FieldId,S32 Value);
STCHIP_Error_t ChipSetField_E(U8 chipaddress, STCHIP_Mode_t mode, BOOL Repeater, U32 FieldId, S32 Value);
S32				ChipGetField(STCHIP_Handle_t hChip,U32 FieldId);
STCHIP_Error_t	ChipSetFieldImage(STCHIP_Handle_t hChip, U32 FieldId, S32 Value);
S32				ChipGetFieldImage(STCHIP_Handle_t hChip, U32 FieldId);


#ifdef __cplusplus
 }
#endif                  /* __cplusplus */

#endif          /* H_CHIP */

/* End of chip.h */
