/* -----------------------------------------------------------------------------------------------------
File Name: Tuner.c

Description: Tuner SAT_NULL driver 

Last Modification: 

author: 
/* ---------------------------------------------------------------------------------------------------- */

#ifndef ST_OSLINUX  
	#include "string.h"
	#include "stdlib.h"
#endif

#include "globaldefs.h"
#include "FE_Sat_Tuner.h" 
#include "SAT_NULL_Tuner.h"

#if 0
TUNER_Error_t SAT_NULL_TunerInit(void *pTunerInit_v,STCHIP_Handle_t *TunerHandle)
{
	SAT_TUNER_Params_t	hTunerParams = NULL;
	STCHIP_Info_t	ChipInfo;
	STCHIP_Handle_t hTuner = NULL;
	TUNER_Error_t error = TUNER_NO_ERR; 
	SAT_TUNER_Params_t pTunerInit = (SAT_TUNER_Params_t)pTunerInit_v; /*Changed to retain common function prototype for all Tuners: Change ///AG*/

    #ifdef HOST_PC
    hTunerParams = calloc (1,sizeof(SAT_TUNER_InitParams_t));	/* Allocation of the chip structure	*/ 
    #endif
    
    #ifdef CHIP_STAPI
	hTunerParams = (SAT_TUNER_Params_t)((*TunerHandle)->pData);/*Change ///AG*/
	memcpy(&ChipInfo, *TunerHandle,sizeof(STCHIP_Info_t)); /*Copy settings already contained in hTuner to ChipInfo: Change ///AG*/
    #endif

    
    if(hTunerParams != NULL)
    {
    	hTunerParams->Model		= pTunerInit->Model;    	/* Tuner model */
    	hTunerParams->Reference = pTunerInit->Reference;	/* Reference Clock in Hz */
    	hTunerParams->IF = pTunerInit->IF; 					/* IF Hz intermediate frequency */
    	hTunerParams->IQ_Wiring = pTunerInit->IQ_Wiring;	/* hardware IQ invertion */
    	hTunerParams->BandSelect = pTunerInit->BandSelect;	/* Wide band tuner (6130 like, band selection) */ 

    	strcpy((char *)ChipInfo.Name,pTunerInit->TunerName);	/* Tuner name */
    	ChipInfo.RepeaterHost = pTunerInit->RepeaterHost;		/* Repeater host */
		ChipInfo.RepeaterFn = pTunerInit->RepeaterFn;			/* Repeater enable/disable function */
		ChipInfo.Repeater = TRUE;								/* Tuner need to enable repeater */
		ChipInfo.I2cAddr = pTunerInit->TunerI2cAddress;			/* Init tuner I2C address */
    	

    	ChipInfo.pData = hTunerParams;				/* Store tunerparams pointer into Chip structure */
 
        
		/* fill elements of external chip data structure */
		ChipInfo.NbRegs   = SAT_NULL_NBREGS;
		ChipInfo.NbFields = SAT_NULL_NBFIELDS;
		ChipInfo.ChipMode = STCHIP_MODE_NOSUBADR_RD;
		ChipInfo.WrStart  = 0;
		ChipInfo.WrSize   = 0;     
		ChipInfo.RdStart  = 0;     
		ChipInfo.RdSize   = 0;     

		#ifdef HOST_PC				/* Change ///AG*/
			hTuner = ChipOpen(&ChipInfo);
			(*TunerHandle) = hTuner;
		#endif
		
		#ifdef CHIP_STAPI
			hTuner = *TunerHandle;/*obtain hTuner : change ///AG*/
			memcpy(hTuner, &ChipInfo, sizeof(STCHIP_Info_t)); /* Copy the ChipInfo to hTuner: Change ///AG*/
		#endif	
		
		if((*TunerHandle)== NULL)
			error=TUNER_INVALID_HANDLE; 
			
	}
	else
		error=TUNER_INVALID_HANDLE;
	
	return error;
    
}

TUNER_Error_t SAT_NULL_TunerSetFrequency(STCHIP_Handle_t hTuner,U32 Frequency)
{
	hTuner=hTuner;
	return TUNER_NO_ERR;
} 

U32 SAT_NULL_TunerGetFrequency(STCHIP_Handle_t hTuner)
{
	hTuner=hTuner;
	return 0;
} 


TUNER_Error_t SAT_NULL_TunerSetBandwidth(STCHIP_Handle_t hTuner,U32 Bandwidth)
{
	hTuner=hTuner;
	Bandwidth=Bandwidth;
	return TUNER_NO_ERR;
}

U32 SAT_NULL_TunerGetBandwidth(STCHIP_Handle_t hTuner)
{
	hTuner=hTuner;
	return 0;
}

TUNER_Error_t SAT_NULL_TunerSetGain(STCHIP_Handle_t hTuner,S32 Gain)
{
	hTuner=hTuner;
	Gain=Gain;
	return TUNER_NO_ERR;
}

S32 SAT_NULL_TunerGetGain(STCHIP_Handle_t hTuner)
{
	hTuner=hTuner;
	return 0;
}

TUNER_Error_t SAT_NULL_TunerSetOutputClock(STCHIP_Handle_t hTuner, S32 Divider)
{
	hTuner=hTuner;
	Divider=Divider;
	return TUNER_NO_ERR;
}


TUNER_Error_t SAT_NULL_TunerSetAttenuator(STCHIP_Handle_t hTuner, BOOL AttenuatorOn)
{
	hTuner=hTuner;
	AttenuatorOn=AttenuatorOn;
	return TUNER_NO_ERR;	
}
BOOL SAT_NULL_TunerGetAttenuator(STCHIP_Handle_t hTuner)
{
	hTuner=hTuner;
	return FALSE;
}

BOOL SAT_NULL_TunerGetStatus(STCHIP_Handle_t hTuner)
{
	hTuner=hTuner;
	return FALSE;
}

TUNER_Error_t SAT_NULL_TunerWrite(STCHIP_Handle_t hTuner)
{
	hTuner=hTuner;
	return TUNER_NO_ERR;
}

TUNER_Error_t SAT_NULL_TunerRead(STCHIP_Handle_t hTuner)
{
	hTuner=hTuner;
	return TUNER_NO_ERR;
}

SAT_TUNER_Model_t SAT_NULL_TunerGetModel(STCHIP_Handle_t hChip)
{
	SAT_TUNER_Model_t model = SAT_TUNER_NULL;
	
	if(hChip && hChip->pData)
	{
		model = ((SAT_TUNER_Params_t) (hChip->pData))->Model;  
	}
	
	return model;	
}

void SAT_NULL_TunerSetIQ_Wiring(STCHIP_Handle_t hChip,  S8 IQ_Wiring)
{
	hChip=hChip;
	IQ_Wiring=IQ_Wiring;
}

S8 SAT_NULL_TunerGetIQ_Wiring(STCHIP_Handle_t hChip)
{
	hChip=hChip;
	return (S8)TUNER_IQ_NORMAL;
}

void SAT_NULL_TunerSetReferenceFreq(STCHIP_Handle_t hChip, U32 Reference)
{
	hChip=hChip;
}

U32 SAT_NULL_TunerGetReferenceFreq(STCHIP_Handle_t hChip)
{
	hChip=hChip;
	return 0;
}

void SAT_NULL_TunerSetIF_Freq(STCHIP_Handle_t hChip, U32 IF)
{
	hChip=hChip;
}

U32 SAT_NULL_TunerGetIF_Freq(STCHIP_Handle_t hChip)
{
	hChip=hChip;
	return 0;
}

void SAT_NULL_TunerSetBandSelect(STCHIP_Handle_t hChip, U8 BandSelect)
{
	hChip=hChip;
	BandSelect=BandSelect;
}

U8 SAT_NULL_TunerGetBandSelect(STCHIP_Handle_t hChip)
{
	hChip=hChip;
	return 0;
}
#endif
#if 0
TUNER_Error_t SAT_NULL_TunerTerm(STCHIP_Handle_t hTuner)
{
	TUNER_Error_t error = TUNER_NO_ERR;
	
	if(hTuner)
	{
		#ifndef CHIP_STAPI 
			if(hTuner->pData)	
				free(hTuner->pData);
			
			ChipClose(hTuner);
		#endif
	}
	
	return error;
}
#endif



