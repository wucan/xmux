/* -----------------------------------------------------------------------------------------------------
File Name: Tuner.c

Description: Tuner driver 

Last Modification: 07/24/2007

author: HT
/* ---------------------------------------------------------------------------------------------------- */

#ifndef ST_OSLINUX 
	#include "string.h"
	#include "stdlib.h"
#endif

#include "globaldefs.h"
#include "FE_Sat_Tuner.h"

//#include "STB6000_Tuner.h"
//#include "STB6100_Tuner.h"
#include "STV6110_Tuner.h"
//#include "STV6130_Tuner.h"
#include "SAT_NULL_Tuner.h" 

									
#if 0
TUNER_Error_t FE_Sat_TunerInit(void *pTunerInit_v,STCHIP_Handle_t *TunerHandle) /*For generic function prototype: Change ///AG */
{
	TUNER_Error_t error = TUNER_NO_ERR;
	SAT_TUNER_Params_t pTunerInit = (SAT_TUNER_Params_t)pTunerInit_v; /*Changed to retain common function prototype for all Tuners: Change ///AG*/
   
   	switch(pTunerInit->Model)
   	{
   		case SAT_TUNER_NULL:
			error=SAT_NULL_TunerInit(pTunerInit,TunerHandle);
		break;
			
		case TUNER_STB6000:   
			error=STB6000_TunerInit(pTunerInit,TunerHandle);
		break;

		case TUNER_STB6100:   
			error=STB6100_TunerInit(pTunerInit,TunerHandle);
		break;
			
		case TUNER_STV6110:
			error=STV6110_TunerInit(pTunerInit,TunerHandle);
		break;
			
		case TUNER_STV6130:
			error=STV6130_TunerInit(pTunerInit,TunerHandle);
		break;			


		default:
			error=TUNER_TYPE_ERR;
		break;
		
	}
	
	return error;
}

TUNER_Error_t FE_Sat_TunerSetFrequency(STCHIP_Handle_t hTuner,U32 Frequency)
{
	TUNER_Error_t error = TUNER_NO_ERR;
	SAT_TUNER_Params_t hTunerParams = NULL;
	
	hTunerParams =  (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams && !hTuner->Error)	
	{
		switch(hTunerParams->Model)
		{
			
			case SAT_TUNER_NULL:
				error=SAT_NULL_TunerSetFrequency(hTuner,Frequency); 	
			break;
			
			//case TUNER_STB6000:
			//	error=STB6000_TunerSetFrequency(hTuner,Frequency); 
			//break;

			//case TUNER_STB6100:
			//	error=STB6100_TunerSetFrequency(hTuner,Frequency);
			//break;
			
			case TUNER_STV6110:
				error=STV6110_TunerSetFrequency(hTuner,Frequency); 
			break;
			
			//case TUNER_STV6130:
			//	error=STV6130_TunerSetFrequency(hTuner,Frequency); 	
			//break;
			
			default:
				error=TUNER_TYPE_ERR;  
			break;
		}
	}
		
	return error;
} 
#endif
TUNER_Error_t FE_Sat_TunerSetFrequency_E(U32 Frequency,U32 TunerReference) 
{
	TUNER_Error_t error = TUNER_NO_ERR;
	STV6110_TunerSetFrequency_E(Frequency,TunerReference); 
	return error;
} 
#if 0
U32 FE_Sat_TunerGetFrequency(STCHIP_Handle_t hTuner)
{
	SAT_TUNER_Params_t hTunerParams = NULL; 
	U32 frequency=0;

	hTunerParams = (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams && !hTuner->Error)	
	{
		switch(hTunerParams->Model)
		{

			case SAT_TUNER_NULL:
				frequency=SAT_NULL_TunerGetFrequency(hTuner); 
			break;

			//case TUNER_STB6000:
			//	frequency=STB6000_TunerGetFrequency(hTuner);
			
			//break;

			//case TUNER_STB6100:
			//	frequency=STB6100_TunerGetFrequency(hTuner);
			//break;
			
			case TUNER_STV6110:
				frequency=STV6110_TunerGetFrequency(hTuner);
			break;
			
			//case TUNER_STV6130:
 		//		frequency=STV6130_TunerGetFrequency(hTuner);
 			//break;

			default:
				frequency=0;
			break;
		}
	}
	return frequency;
} 
#endif
U32 FE_Sat_TunerGetFrequency6110(U32 TunerReference)
{
	U32 frequency=0;

	frequency=STV6110_TunerGetFrequency_E(TunerReference);

	return frequency;
} 

#if 0
TUNER_Error_t FE_Sat_TunerSetBandwidth(STCHIP_Handle_t hTuner,U32 Bandwidth)
{
	TUNER_Error_t error = TUNER_NO_ERR; 
	SAT_TUNER_Params_t hTunerParams = NULL;

	
	hTunerParams = (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams && !hTuner->Error)	
	{
		switch(hTunerParams->Model)
		{
		
			case SAT_TUNER_NULL:
				error=SAT_NULL_TunerSetBandwidth(hTuner,Bandwidth);
			break;

			//case TUNER_STB6000:
			//	error=STB6000_TunerSetBandwidth(hTuner,Bandwidth);
			//break;

			//case TUNER_STB6100:
			//	error=STB6100_TunerSetBandwidth(hTuner,Bandwidth);
			//break;
			
			case TUNER_STV6110:
				error=STV6110_TunerSetBandwidth(hTuner,Bandwidth);
			break;
			
			//case TUNER_STV6130:
			//	error=STV6130_TunerSetBandwidth(hTuner,Bandwidth);
			//break;
			
			default:
				error=TUNER_TYPE_ERR;  
			break;
		}
	}
	
	return error;
}
#endif
TUNER_Error_t FE_Sat_TunerSetBandwidth_E(U32 Bandwidth)
{
	TUNER_Error_t error = TUNER_NO_ERR; 
	STV6110_TunerSetBandwidth_E(Bandwidth);
	
	return error;
}
#if 0
U32 FE_Sat_TunerGetBandwidth(STCHIP_Handle_t hTuner)
{
	SAT_TUNER_Params_t hTunerParams = NULL;
	U32 bandwidth = 0;

	hTunerParams = (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams && !hTuner->Error)	
	{
		switch(hTunerParams->Model)
		{
			
			case SAT_TUNER_NULL:
				bandwidth=	SAT_NULL_TunerGetBandwidth(hTuner);
			break;

			//case TUNER_STB6000:
			//	bandwidth=	STB6000_TunerGetBandwidth(hTuner);
			//break;

			//case TUNER_STB6100:
			//	bandwidth=	STB6100_TunerGetBandwidth(hTuner);
			//break;
			
			case TUNER_STV6110:
				bandwidth=	STV6110_TunerGetBandwidth(hTuner);
			break;
			
			//case TUNER_STV6130:
			//	bandwidth=	STV6130_TunerGetBandwidth(hTuner);
			//break;
			
			default:
			break;
		}
	}
	
	return bandwidth;
}
#endif
TUNER_Error_t FE_Sat_TunerSetGain_E(S32 Gain)
{
	TUNER_Error_t error = TUNER_NO_ERR;
	STV6110_TunerSetGain_E(Gain);
	
	return error;
}

#if 0
S32 FE_Sat_TunerGetGain(STCHIP_Handle_t hTuner) 
{
	SAT_TUNER_Params_t hTunerParams = NULL;
	S32 gain=0;
	
	hTunerParams = (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams )	
	{
		switch(hTunerParams->Model)
		{	
			case SAT_TUNER_NULL:
				gain=SAT_NULL_TunerGetGain(hTuner); 
			break;

			//case TUNER_STB6000:
			//	gain= STB6000_TunerGetGain(hTuner); 
			//break;

			//case TUNER_STB6100:
			//	gain= STB6100_TunerGetGain(hTuner); 
			//break;
			
			case TUNER_STV6110:
				gain= STV6110_TunerGetGain(hTuner); 
			break;

			//case TUNER_STV6130:
			//	gain= STV6130_TunerGetGain(hTuner);
			//break;

			default:
			break;
		}
	}
	
	return gain;
}
#endif

void FE_Sat_TunerSetOutputClock_E(S32 Divider)
{
	STV6110_TunerSetOutputClock_E(Divider);
}

#if 0
TUNER_Error_t FE_Sat_TunerSetAttenuator(STCHIP_Handle_t hTuner, BOOL AttenuatorOn)
{
	TUNER_Error_t error = TUNER_NO_ERR;
	SAT_TUNER_Params_t hTunerParams = NULL;
	
	hTunerParams = (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams && !hTuner->Error)	
	{
		switch(hTunerParams->Model)
		{	
			case SAT_TUNER_NULL:
				error=	SAT_NULL_TunerSetAttenuator(hTuner,AttenuatorOn);
			break;
			
			//case TUNER_STB6000:
			//	error=	STB6000_TunerSetAttenuator(hTuner,AttenuatorOn);
			//break;
			
			//case TUNER_STB6100:
			//	error=	STB6100_TunerSetAttenuator(hTuner,AttenuatorOn); 
			//break;

			case TUNER_STV6110:
				error=	STV6110_TunerSetAttenuator(hTuner,AttenuatorOn);
			break;
			
			//case TUNER_STV6130:
			//	error=	STV6110_TunerSetAttenuator(hTuner,AttenuatorOn);
			//break;
			
			default:
				error=TUNER_TYPE_ERR;
			break;	
		}
	}
	
	return error;

} 

BOOL FE_Sat_TunerGetAttenuator(STCHIP_Handle_t hTuner)
{
	SAT_TUNER_Params_t hTunerParams = NULL;
	BOOL attenuator=FALSE;

	hTunerParams = (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams && !hTuner->Error)	
	{
	
		switch(hTunerParams->Model)
		{
			case SAT_TUNER_NULL:
				attenuator = SAT_NULL_TunerGetAttenuator(hTuner); ;  
			break;
			
			//case TUNER_STB6000:
			//	attenuator = STB6000_TunerGetAttenuator(hTuner);
			//break;
			
			//case TUNER_STB6100:
			//	attenuator = STB6100_TunerGetAttenuator(hTuner);
			//break;
			
			case TUNER_STV6110:
				attenuator = STV6110_TunerGetAttenuator(hTuner);
			break;
			
			//case TUNER_STV6130:
			//	attenuator = STV6130_TunerGetAttenuator(hTuner);
			//break;
			
			default:
				/* nothing to do here */
			break;
		}
	}
	
	return attenuator;

	
}
BOOL FE_Sat_TunerGetStatus(STCHIP_Handle_t hTuner)
{
	SAT_TUNER_Params_t hTunerParams = NULL;
	BOOL locked = FALSE;

	hTunerParams = (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams && !hTuner->Error)	
	{
	
		switch(hTunerParams->Model)
		{
			case SAT_TUNER_NULL:
				locked = SAT_NULL_TunerGetStatus(hTuner);
			break;
			
			//case TUNER_STB6000:
			//	locked = STB6000_TunerGetStatus(hTuner);
			//break;
			
			//case TUNER_STB6100:
			//	locked = STB6100_TunerGetStatus(hTuner);
			//break;
			
			case TUNER_STV6110:
				locked = STV6110_TunerGetStatus(hTuner);
			break;
			
			//case TUNER_STV6130:
			//	locked = STV6130_TunerGetStatus(hTuner);
			//break;
			
			default:
				/* nothing to do here */
			break;
		}
	}
	
	return locked;
}

/*TunerWrite is not required here as it is internal function for every tuner all tuners///AG*/
TUNER_Error_t FE_Sat_TunerWrite(STCHIP_Handle_t hTuner)
{
	TUNER_Error_t error = TUNER_NO_ERR;
	STCHIP_Error_t chipError=CHIPERR_NO_ERROR;
	SAT_TUNER_Params_t hTunerParams = NULL;
	
	hTunerParams =  (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams)	
	{
		switch(hTunerParams->Model)
		{	
			case SAT_TUNER_NULL:
				error=SAT_NULL_TunerWrite(hTuner);
			break;
			
			//case TUNER_STB6000:
			//	error=STB6000_TunerWrite(hTuner);
			//break;

			//case TUNER_STB6100:
			//	error=STB6100_TunerWrite(hTuner); 
			//break;
			
			case TUNER_STV6110:
				error=STV6110_TunerWrite(hTuner); 
			break;
			
			//case TUNER_STV6130:
			//	error=STV6130_TunerWrite(hTuner); 
			//break;

			default:
				error=TUNER_TYPE_ERR;
			break;
		}
		
		if(error!=TUNER_TYPE_ERR)
		{
			switch (chipError)
			{
				case CHIPERR_NO_ERROR:
				break;
					
				case CHIPERR_INVALID_HANDLE:
				break;
					
				case CHIPERR_INVALID_REG_ID:
				break;
					
				case CHIPERR_INVALID_FIELD_ID:
				break;
					
				case CHIPERR_INVALID_FIELD_SIZE:
				break;
					
				case CHIPERR_I2C_NO_ACK:
				default:
				break;
					
				case CHIPERR_I2C_BURST:
				break;
			}
		}
	}
	
	return error;
}


/*TunerRead is not required here ///AG*/
TUNER_Error_t FE_Sat_TunerRead(STCHIP_Handle_t hTuner)
{
	TUNER_Error_t error = TUNER_NO_ERR;
	STCHIP_Error_t chipError=CHIPERR_NO_ERROR;
	SAT_TUNER_Params_t hTunerParams = NULL;
	
	hTunerParams =  (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams)	
	{
		switch(hTunerParams->Model)
		{	
			
			case SAT_TUNER_NULL:
				error =SAT_NULL_TunerRead(hTuner); 
			break;

			//case TUNER_STB6000:
			//	error =STB6000_TunerRead(hTuner);
			//break;

			//case TUNER_STB6100:
			//	error =STB6100_TunerRead(hTuner);
			//break;
			
			case TUNER_STV6110:
				error =STV6110_TunerRead(hTuner);
			break;
			
			//case TUNER_STV6130:
			//	error =STV6130_TunerRead(hTuner);
			//break;

			default:
				error=TUNER_TYPE_ERR;
			break;
		}
		
		if(error!=TUNER_TYPE_ERR)
		{
			switch (chipError)
			{
				case CHIPERR_NO_ERROR:
				break;
					
				case CHIPERR_INVALID_HANDLE:
				break;
					
				case CHIPERR_INVALID_REG_ID:
				break;
					
				case CHIPERR_INVALID_FIELD_ID:
				break;
					
				case CHIPERR_INVALID_FIELD_SIZE:
				break;
					
				case CHIPERR_I2C_NO_ACK:
				default:
				break;
					
				case CHIPERR_I2C_BURST:
				break;
			}
		}

	}
	
	return error;
}

SAT_TUNER_Model_t FE_Sat_TunerGetModel(STCHIP_Handle_t hChip)
{
	SAT_TUNER_Model_t model = SAT_TUNER_NULL;
	
	if(hChip && hChip->pData)
	{
		model = ((SAT_TUNER_Params_t) (hChip->pData))->Model;  
	}
	
	return model;	
}

void FE_Sat_TunerSetIQ_Wiring(STCHIP_Handle_t hChip, S8 IQ_Wiring)
{
	if(hChip && hChip->pData)
	{
		((SAT_TUNER_Params_t) (hChip->pData))->IQ_Wiring = (TUNER_IQ_t)IQ_Wiring;  
	}	
}

S8 FE_Sat_TunerGetIQ_Wiring(STCHIP_Handle_t hChip)
{
	TUNER_IQ_t wiring = TUNER_IQ_NORMAL;
	
	if(hChip && hChip->pData)
	{
		wiring = ((SAT_TUNER_Params_t) (hChip->pData))->IQ_Wiring;  
	}
	
	return (S8)wiring;
}

void FE_Sat_TunerSetReferenceFreq(STCHIP_Handle_t hChip, U32 Reference)
{
	if(hChip && hChip->pData)
	{
		((SAT_TUNER_Params_t) (hChip->pData))->Reference = Reference;  
	}
}

U32 FE_Sat_TunerGetReferenceFreq(STCHIP_Handle_t hChip)
{
	U32 reference = 0;
	
	if(hChip && hChip->pData)
	{
		reference = ((SAT_TUNER_Params_t) (hChip->pData))->Reference;  
	}
	
	return reference;
}

void FE_Sat_TunerSetIF_Freq(STCHIP_Handle_t hChip, U32 IF)
{
	if(hChip && hChip->pData)
	{
		((SAT_TUNER_Params_t) (hChip->pData))->IF = IF;  
	}
}

U32 FE_Sat_TunerGetIF_Freq(STCHIP_Handle_t hChip)
{
	U32 ifreq = 0;
	
	if(hChip && hChip->pData)
	{
		ifreq = ((SAT_TUNER_Params_t) (hChip->pData))->IF;  
	}
	
	return ifreq;
}

void FE_Sat_TunerSetBandSelect(STCHIP_Handle_t hChip, U8 BandSelect)
{
	if(hChip && hChip->pData)
	{
		((SAT_TUNER_Params_t) (hChip->pData))->BandSelect = (SAT_TUNER_WIDEBandS_t)BandSelect;  
	}
}

U8 FE_Sat_TunerGetBandSelect(STCHIP_Handle_t hChip)
{
	U8 ifreq = 0;
	
	if(hChip && hChip->pData)
	{
		ifreq = ((SAT_TUNER_Params_t) (hChip->pData))->BandSelect;  
	}
	
	return (U8)ifreq;
}
#endif
#if 0
TUNER_Error_t FE_Sat_TunerTerm(STCHIP_Handle_t hTuner)
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



