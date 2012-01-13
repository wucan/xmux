/* -----------------------------------------------------------------------------------------------------
File Name: Tuner.c

Description: STV6110 Tuner driver 

Last Modification: 

author: 
 ---------------------------------------------------------------------------------------------------- */

#ifndef ST_OSLINUX  
	#include "string.h"
	#include "stdlib.h"
#endif

#include "globaldefs.h"
#include "FE_Sat_Tuner.h"
#include "STV6110_Tuner.h"
//#include "Chip.h"

	/* STV6110 tuner definition */ 
/*CTRL1*/
#define RSTV6110_CTRL1  0x0000
#define FSTV6110_K  0x000000f8
#define FSTV6110_LPT  0x00000004
#define FSTV6110_RX  0x00000002
#define FSTV6110_SYN  0x00000001

/*CTRL2*/
#define RSTV6110_CTRL2  0x0001
#define FSTV6110_CO_DIV  0x000100c0
#define FSTV6110_REFOUTSEL  0x00010010
#define FSTV6110_BBGAIN  0x0001000f

/*TUNING0*/
#define RSTV6110_TUNING0  0x0002
#define FSTV6110_NDIV_LSB  0x000200ff

/*TUNING1*/
#define RSTV6110_TUNING1  0x0003
#define FSTV6110_RDIV  0x000300c0
#define FSTV6110_PRESC32ON  0x00030020
#define FSTV6110_DIV4SEL  0x00030010
#define FSTV6110_NDIV_MSB  0x0003000f

/*CTRL3*/
#define RSTV6110_CTRL3  0x0004
#define FSTV6110_DCLOOP_OFF  0x00040080
#define FSTV6110_RCCLKOFF  0x00040040 
#define FSTV6110_ICP  0x00040020
#define FSTV6110_CF  0x0004001f

/*STAT1*/
#define RSTV6110_STAT1  0x0005
#define FSTV6110_TEST1  0x000500f8
#define FSTV6110_CALVCOSTRT  0x00050004
#define FSTV6110_CALRCSTRT  0x00050002
#define FSTV6110_LOCKPLL  0x00050001

/*STAT2*/
#define RSTV6110_STAT2  0x0006
#define FSTV6110_TEST2  0x000600ff

/*STAT3*/
#define RSTV6110_STAT3  0x0007
#define FSTV6110_TEST3  0x000700ff

/*Nbregs and Nbfields moved to STV6110_Tuner.h: Change ///AG */
extern void WAIT_N_MS(unsigned int count); 
void STV6110_TunerWrite_E(U8 chipaddress,STCHIP_Mode_t mode,BOOL Repeater,U16 WrStart,U8 WrSize,U8* databuf );
U32	 ChipGetField_E(U8 chipaddress,STCHIP_Mode_t mode,BOOL Repeater, U32 FieldId);
#if 0									
TUNER_Error_t STV6110_TunerInit(void *pTunerInit_v,STCHIP_Handle_t *TunerHandle)
{
	SAT_TUNER_Params_t	hTunerParams = NULL;
	STCHIP_Info_t	ChipInfo;
	STCHIP_Handle_t hTuner = NULL;
	TUNER_Error_t error = TUNER_NO_ERR; 
	SAT_TUNER_Params_t pTunerInit = (SAT_TUNER_Params_t)pTunerInit_v; /*Changed to retain common function prototype for all Tuners: Change ///AG*/
 	 STCHIP_Register_t		DefSTV6110Val[STV6110_NBREGS]=
 	 {
 	 	{RSTV6110_CTRL1		,0x07	},
 	 	{RSTV6110_CTRL2		,0x11	},
 	 	{RSTV6110_TUNING0	,0xdc	},
 	 	{RSTV6110_TUNING1	,0x85	},
 	 	{RSTV6110_CTRL3		,0x17	},
 	 	{RSTV6110_STAT1		,0x01	},
 	 	{RSTV6110_STAT2		,0xe6	},
 	 	{RSTV6110_STAT3		,0x1e	}
 	 };
 	 
 	 
    /*
    **   REGISTER CONFIGURATION
    **     ----------------------
    */
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
 
    
			

	ChipInfo.NbRegs   = STV6110_NBREGS;
	ChipInfo.NbFields = STV6110_NBFIELDS;
	ChipInfo.ChipMode = STCHIP_MODE_SUBADR_8;
	ChipInfo.WrStart  = RSTV6110_CTRL1;
	ChipInfo.WrSize   = 8;
	ChipInfo.RdStart  = RSTV6110_CTRL1;
	ChipInfo.RdSize   = 8;

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
	
	if(hTuner != NULL)
	{
		/*******************************
		**   CHIP REGISTER MAP IMAGE INITIALIZATION
		**     ----------------------
		********************************/
		ChipUpdateDefaultValues(hTuner,DefSTV6110Val);
				
			/*******************************
		 **   REGISTER CONFIGURATION
		 **     ----------------------
		 ********************************/
		error=STV6110_TunerWrite(hTuner);
	}

	}
	else
		error=TUNER_INVALID_HANDLE;
	
	return error;
}
#endif
void STV6110_TunerInit_E(void)
{
	U8 chipaddress=0xC0;
	long int tmp=0;
 	U8 reg[8]={0x07,0x11,0xdc,0x85,0x17,0x01,0xe6,0x1e};
 	//U8 reg[8]={0x07,0xb3,0xe9,0xd4,0x17,0x06,0xe6,0x1e};
	//U8 reg[8]={0x07,0xb3,0x3b,0xc1,0xd4,0x06,0x00,0x00};
	//U8 	 reg[8]={0x07,0x63,0x96,0xc2,0xc7,0x07,0x00,0x00};//1324
	//U8 	 reg[8]={0x07,0x63,0x96,0xc2,0xc7,0x07,0x00,0x00};//1324

 	U8 i;
	U8 dat=0;
	U8 dat1=0xff;
	

	
	for(i=0;i<8;i++)
		{
			 ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,i,reg[i]);
		
		}

	
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CO_DIV,1);

}

TUNER_Error_t STV6110_TunerSetFrequency_E(U32 Frequency,U32 TunerReference)
{
	TUNER_Error_t error = TUNER_NO_ERR;
	SAT_TUNER_Params_t hTunerParams = NULL;
	/*Deleted unused variable 'frequency' to remove warnings: Change ///AG*/
	U32 divider;
	U32 p,
		Presc,
		rDiv,
		r;
	
	S32 rDivOpt=0,
		pCalcOpt=1000,
		pVal,
		pCalc,
		i;
		
       U8 chipaddress=0xC0;
       U8 dat0,dat1;
	TunerReference=16000000;
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_K,((TunerReference/1000000)-16));
	if (Frequency<=1023000) 
	{
		p=1;
		Presc=0;
		//display_hex(1,0); 
	}
	else if (Frequency<=1300000)
	{
		p=1;
		Presc=1;
		//display_hex(2,0); 
	}
	else if (Frequency<=2046000)
	{
		p=0;
		Presc=0;
		//display_hex(3,0); 
	}
	else 
	{
		p=0;
		Presc=1;
	}
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_DIV4SEL,p);
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_PRESC32ON,Presc);

	pVal=(int)(PowOf2(p+1)*10); 
	for(rDiv=0;rDiv<=3;rDiv++)
	{
		pCalc= (TunerReference/100000)/(PowOf2(rDiv+1));
		if((ABS((S32)(pCalc-pVal))) < (ABS((S32)(pCalcOpt-pVal))))
		rDivOpt= rDiv;
					
		pCalcOpt= (TunerReference/100000)/(PowOf2(rDivOpt+1));
             
	}
			
	
	
	r=PowOf2(rDivOpt+1);
	divider= (Frequency * r * PowOf2(p+1)*10)/(TunerReference/1000);  
	divider=(divider+5)/10;
       //showu32(rDivOpt,0);
	//showu32(divider,1);
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_RDIV,rDivOpt);
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_NDIV_MSB,MSB(divider));
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_NDIV_LSB,LSB(divider));
				
				
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CALVCOSTRT,1);		/* VCO Auto Calibration */
				
	i=0;
	//while((i<10) && (ChipGetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CALVCOSTRT)!=0))
	//while((i<10) && (ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CALVCOSTRT)!=CHIPERR_NO_ERROR))
	while(i<10)
	{
		WAIT_N_MS(1);	/* wait for VCO auto calibration */
		i++;
	}
	
	return error;
} 

U32 STV6110_TunerGetFrequency_E(U32 TunerReference)
{
	SAT_TUNER_Params_t hTunerParams = NULL; 
	U32 frequency = 0;
	/*Deleted unused variable 'stepsize' to remove warnings: Change ///AG*/
	U32 nbsteps;
	U32 divider = 0;
	/*Deleted unused variable 'swallow' to remove warnings: Change ///AG*/
	U32 psd2;
       U8 chipaddress=0xc0;



			

				divider = MAKEWORD(ChipGetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_NDIV_MSB),ChipGetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_NDIV_LSB));	/*N*/
				nbsteps = ChipGetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_RDIV);	/*Rdiv*/
				psd2 = ChipGetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_DIV4SEL);	/*p*/
				
				frequency = divider * (TunerReference/1000);
				frequency = frequency/(PowOf2(nbsteps+psd2));
				frequency /= 4;


	return frequency;
} 
TUNER_Error_t STV6110_TunerSetBandwidth_E(U32 Bandwidth)
{
	TUNER_Error_t error = TUNER_NO_ERR; 
	U8 u8;
	S32 i=0; /*Deleted unused variable 'filter' to remove warnings: Change ///AG*/
	U8 chipaddress=0xC0;

	if((Bandwidth/2) > 36000000)   /*F[4:0] BW/2 max =31+5=36 mhz for F=31*/
	u8 = 31;
	else if((Bandwidth/2) < 5000000) /* BW/2 min = 5Mhz for F=0 */
	u8 = 0;
	else							 /*if 5 < BW/2 < 36*/
	u8 = (Bandwidth/2)/1000000 - 5;

	//u8=3;//dlh do
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_RCCLKOFF,0);	 /* Activate the calibration Clock */
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CF,u8);		 /* Set the LPF value */	
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CALRCSTRT,1); /* Start LPF auto calibration*/
				
	i=0;
	while((i<10) && (ChipGetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CALRCSTRT)!=0))
	{
		WAIT_N_MS(1);	/* wait for LPF auto calibration */
		i++;
	}
	ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_RCCLKOFF,1);	 /* calibration done, desactivate the calibration Clock */  
	
	return error;
}
#if 0
U32 STV6110_TunerGetBandwidth(STCHIP_Handle_t hTuner)
{
	SAT_TUNER_Params_t hTunerParams = NULL;
	U32 bandwidth = 0;
	U8 u8 = 0;
	
	hTunerParams = (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams && !hTuner->Error)	
	{
				STV6110_TunerRead(hTuner);
				u8 = ChipGetField(hTuner,FSTV6110_CF);
				
				bandwidth = (u8+5)*2000000;	/* x2 for ZIF tuner BW/2=F+5 Mhz*/
			
	}
	
	return bandwidth;
}
#endif
TUNER_Error_t STV6110_TunerSetGain_E(S32 Gain)
{
	TUNER_Error_t error = TUNER_NO_ERR;
	U8 chipaddress=0xC0;

				ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_BBGAIN,(Gain/2));

	return error;
}
#if 0
S32 STV6110_TunerGetGain(STCHIP_Handle_t hTuner)
{
	SAT_TUNER_Params_t hTunerParams = NULL;
	S32 gain=0;
	
	hTunerParams = (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams && !hTuner->Error)	
	{
			
		gain=2*ChipGetField(hTuner,FSTV6110_BBGAIN);
	}
	
	return gain;
}
#endif
TUNER_Error_t STV6110_TunerSetOutputClock_E(S32 Divider) 
{
	/*sets the crystal oscillator divisor value, for the output clock
	Divider =:
	0 ==> Tuner output clock not used
	1 ==> divide by 1
	2 ==> divide by 2
	4 ==> divide by 4
	8 ==> divide by 8
	*/
	TUNER_Error_t error = TUNER_NO_ERR;
	#if 0
        U8 chipaddress=0xc0;
	/*Allowed values 1,2,4 and 8*/
	switch(Divider)
	{
		case 1:
		default:
			ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CO_DIV,0);
		break;
					
		case 2:
			ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CO_DIV,1);
		break;
					
		case 4:
			ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CO_DIV,2);
		break;
					
		case 8:
		case 0: /*Tuner output clock not used then divide by 8 (the 6110 can not stop completely the out clock)*/  
			ChipSetField_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CO_DIV,3);
		break;
	}
#endif
	return error;
}

TUNER_Error_t STV6110_TunerSetAttenuator(STCHIP_Handle_t hTuner, BOOL AttenuatorOn)
{
	hTuner=hTuner;
	AttenuatorOn=AttenuatorOn;
	return TUNER_NO_ERR;	
}
BOOL STV6110_TunerGetAttenuator(STCHIP_Handle_t hTuner)
{
	hTuner=hTuner;
	return FALSE;
}

#if 0
BOOL STV6110_TunerGetStatus(STCHIP_Handle_t hTuner)
{
	/*Deleted unused variable 'error' to remove warnings: Change ///AG*/
	SAT_TUNER_Params_t hTunerParams = NULL;
	BOOL locked = FALSE;
	/*Deleted unused variable 'u8' to remove warnings: Change ///AG*/
	
	hTunerParams = (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams && !hTuner->Error)	
	{
		STV6110_TunerRead(hTuner); 
		

				if(!hTuner->Error)
				{
					locked = ChipGetField(hTuner,FSTV6110_LOCKPLL);
				}

	}
	
	return locked;
}

TUNER_Error_t STV6110_TunerWrite(STCHIP_Handle_t hTuner)
{
	TUNER_Error_t error = TUNER_NO_ERR;
	STCHIP_Error_t chipError=CHIPERR_NO_ERROR;
	SAT_TUNER_Params_t hTunerParams = NULL;
	
	hTunerParams =  (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams)	
	{

				chipError=ChipSetRegisters(hTuner,hTuner->WrStart,hTuner->WrSize);
				switch (chipError)
				{
					case CHIPERR_NO_ERROR:
						error = TUNER_NO_ERR;
					break;
					
					case CHIPERR_INVALID_HANDLE:
						error = TUNER_INVALID_HANDLE; 
					break;
					
					case CHIPERR_INVALID_REG_ID:
						error = TUNER_INVALID_REG_ID;
					break;
					
					case CHIPERR_INVALID_FIELD_ID:
						error = TUNER_INVALID_FIELD_ID;
					break;
					
					case CHIPERR_INVALID_FIELD_SIZE:
						error = TUNER_INVALID_FIELD_SIZE;
					break;
					
					case CHIPERR_I2C_NO_ACK:
					default:
						error = TUNER_I2C_NO_ACK;	
					break;
					
					case CHIPERR_I2C_BURST:
						error = TUNER_I2C_BURST;
					break;
				}


	}
	
	return error;
}
#endif
void STV6110_TunerWrite_E(U8 chipaddress,STCHIP_Mode_t mode,BOOL Repeater,U16 WrStart,U8 WrSize,U8* databuf )
{
	ChipSetRegisters_E(chipaddress,mode,Repeater,WrStart,WrSize,databuf);
}
#if 0
TUNER_Error_t STV6110_TunerRead(STCHIP_Handle_t hTuner)
{
	TUNER_Error_t error = TUNER_NO_ERR;
	STCHIP_Error_t chipError=CHIPERR_NO_ERROR;
	SAT_TUNER_Params_t hTunerParams = NULL;
	
	hTunerParams =  (SAT_TUNER_Params_t) hTuner->pData;
	
	if(hTuner && hTunerParams)	
	{
			

				chipError=ChipGetRegisters(hTuner,hTuner->RdStart,hTuner->RdSize);
				switch (chipError)
				{
					case CHIPERR_NO_ERROR:
						error = TUNER_NO_ERR;
					break;
					
					case CHIPERR_INVALID_HANDLE:
						error = TUNER_INVALID_HANDLE; 
					break;
					
					case CHIPERR_INVALID_REG_ID:
						error = TUNER_INVALID_REG_ID;
					break;
					
					case CHIPERR_INVALID_FIELD_ID:
						error = TUNER_INVALID_FIELD_ID;
					break;
					
					case CHIPERR_INVALID_FIELD_SIZE:
						error = TUNER_INVALID_FIELD_SIZE;
					break;
					
					case CHIPERR_I2C_NO_ACK:
					default:
						error = TUNER_I2C_NO_ACK;	
					break;
					
					case CHIPERR_I2C_BURST:
						error = TUNER_I2C_BURST;
					break;
				}


	}
	
	return error;
}
#endif
SAT_TUNER_Model_t STV6110_TunerGetModel(STCHIP_Handle_t hChip)
{
	SAT_TUNER_Model_t model = SAT_TUNER_NULL;
	
	if(hChip && hChip->pData)
	{
		model = ((SAT_TUNER_Params_t) (hChip->pData))->Model;  
	}
	
	return model;	
}

void STV6110_TunerSetIQ_Wiring(STCHIP_Handle_t hChip, S8 IQ_Wiring) /*To make generic prototypes Change ///AG*/
{
	if(hChip && hChip->pData)
	{
		((SAT_TUNER_Params_t) (hChip->pData))->IQ_Wiring = (TUNER_IQ_t)IQ_Wiring;  
	}	
}

S8 STV6110_TunerGetIQ_Wiring(STCHIP_Handle_t hChip)  /*To make generic prototypes Change ///AG*/
{
	TUNER_IQ_t wiring = TUNER_IQ_NORMAL; /*Change ///AG*/
	
	if(hChip && hChip->pData)
	{
		wiring = ((SAT_TUNER_Params_t) (hChip->pData))->IQ_Wiring;  
	}
	
	return (S8)wiring;
}

void STV6110_TunerSetReferenceFreq(STCHIP_Handle_t hChip, U32 Reference)
{
	if(hChip && hChip->pData)
	{
		((SAT_TUNER_Params_t) (hChip->pData))->Reference = Reference;  
	}
}



void STV6110_TunerSetIF_Freq(STCHIP_Handle_t hChip, U32 IF)
{
	if(hChip && hChip->pData)
	{
		((SAT_TUNER_Params_t) (hChip->pData))->IF = IF;  
	}
}

U32 STV6110_TunerGetIF_Freq(STCHIP_Handle_t hChip)
{
	U32 ifreq = 0;
	
	if(hChip && hChip->pData)
	{
		ifreq = ((SAT_TUNER_Params_t) (hChip->pData))->IF;  
	}
	
	return ifreq;
}

void STV6110_TunerSetBandSelect(STCHIP_Handle_t hChip, U8 BandSelect)
{
	if(hChip && hChip->pData)
	{
		((SAT_TUNER_Params_t) (hChip->pData))->BandSelect = (SAT_TUNER_WIDEBandS_t)BandSelect;  
	}
}

U8 STV6110_TunerGetBandSelect(STCHIP_Handle_t hChip)
{
	U32 ifreq = 0;
	
	if(hChip && hChip->pData)
	{
		ifreq = ((SAT_TUNER_Params_t) (hChip->pData))->BandSelect;  
	}
	
	return (U8)ifreq;
}

#if 0
TUNER_Error_t STV6110_TunerTerm(STCHIP_Handle_t hTuner)
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



