/* -------------------------------------------------------------------------
File Name: STV0903_drv.c

Description: STV0903 driver LLA	V3.0 "July/11/2008" 


author: BJ
---------------------------------------------------------------------------- */


/* includes ---------------------------------------------------------------- */
#ifndef ST_OSLINUX  
	#include "stdlib.h" 
	#include "stdio.h"
	#include "string.h"
#endif

#include "STV0903_util.h"
#include "STV0903_Drv.h"
#include "globaldefs.h"
#define TUNER0 0
#define TUNER1 1
#ifdef HOST_PC
//	#include <utility.h>  
	#ifndef NO_GUI
//		#include "STV0903_GUI.h" 
//		#include "Appl.h"
//		#include "Pnl_Report.h"
		//#include <formatio.h> 
	#endif
#endif

#define STV0903_I2C_ADDRESS 0xD0
#define STV0903_BLIND_SEARCH_AGC2_TH  700
typedef struct
{
	unsigned char Lock; 
	unsigned char Standard; 
	char Level;
	unsigned long CN;
	unsigned char PunctureRate;
	long Ber;
}tunerstatus;
extern unsigned char curtuner;
extern  tunerstatus dvbsstatus[];
/****************************************************************************************************************************
 *****************************************************************************************************************************/
	
						/****************************************************************
						 ****************************************************************
						 **************************************************************** 
						 **															   **
						 **		***********************************************		   **
						 **						Static Data					   **
				 		 **		***********************************************		   **
						 **															   **
				 		 **															   **
				 		 ****************************************************************
				 		 **************************************************************** 
				 		 ****************************************************************/
	
	
 /****************************************************************************************************************************
 *****************************************************************************************************************************/

/************************ 
Current LLA revision	
*************************/
static const char RevisionSTV0903[50]  = "STV0903-LLA_REL_3.0_JULY_11_2008";

/************************ 
DVBS1 and DSS C/N Luk-Up table	
*************************/


/************************ 
DVBS2 C/N Luk-Up table	
*************************/


											

/************************ 
RF level C/N Luk-Up table	
*************************/


typedef struct
{
	FE_STV0903_ModCod_t ModCode;
	U8 CarLoopPilotsOn_2;
	U8 CarLoopPilotsOff_2;
	U8 CarLoopPilotsOn_5;
	U8 CarLoopPilotsOff_5;
	U8 CarLoopPilotsOn_10;
	U8 CarLoopPilotsOff_10;
	U8 CarLoopPilotsOn_20;
	U8 CarLoopPilotsOff_20;
	U8 CarLoopPilotsOn_30;
	U8 CarLoopPilotsOff_30;
	
}FE_STV0903_CarLoopOPtim;

extern void WAIT_N_MS(unsigned int count);
/********************************************************************* 
Cut 1.x Tracking carrier loop carrier QPSK 1/2 to 8PSK 9/10 long Frame 	
*********************************************************************/



/********************************************************************* 
Cut 2.0 Tracking carrier loop carrier QPSK 1/2 to 8PSK 9/10 long Frame 	
*********************************************************************/


												
/****************************************************************************************************************************
 *****************************************************************************************************************************/
	
						/****************************************************************
						 ****************************************************************
						 **************************************************************** 
						 **															   **
						 **		***********************************************		   **
						 **						PRIVATE ROUTINES					   **
				 		 **		***********************************************		   **
						 **															   **
				 		 **															   **
				 		 ****************************************************************
				 		 **************************************************************** 
				 		 ****************************************************************/
	
	
 /****************************************************************************************************************************
 *****************************************************************************************************************************/

/*****************************************************
**FUNCTION	::	GetMclkFreq
**ACTION	::	Set the STV0903 master clock frequency
**PARAMS IN	::  hChip		==>	handle to the chip
**				ExtClk		==>	External clock frequency (Hz)
**PARAMS OUT::	NONE
**RETURN	::	Synthesizer frequency (Hz)
*****************************************************/
#if 0
U32 FE_STV0903_GetMclkFreq(STCHIP_Handle_t hChip, U32 ExtClk)
{
	U32 mclk = 90000000,
		div = 0,
		ad_div = 0;
		
	/*******************************************
		If the Field SELX1RATIO == 1 then
			MasterClock = (M_DIV +1)*Quartz/4
		else
			MasterClock = (M_DIV +1)*Quartz/6
	********************************************/
	
	div=ChipGetField(hChip,FSTV0903_M_DIV);
	ad_div= ((ChipGetField(hChip,FSTV0903_SELX1RATIO)==1)?4:6);
	
	mclk=(div+1)*ExtClk/ad_div;

	return mclk; 
}
#endif
U32 FE_STV0903_GetMclkFreq_E(U32 ExtClk)
{
	U32 mclk = 0,//  mclk = 8 000 000;
		div = 0,
		ad_div = 0;
	U8  dat;
		
	/*******************************************
		If the Field SELX1RATIO == 1 then
			MasterClock = (M_DIV +1)*Quartz/4
		else
			MasterClock = (M_DIV +1)*Quartz/6
	********************************************/
	
	div=ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_M_DIV);
	
	ad_div= ((ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SELX1RATIO)==1)?4:6);




// ExtClk = 16Mhz
	mclk=(div+1)*ExtClk/ad_div;

//while(1){;}
	//ExtClk 参考频率
	return mclk; 
}
/*****************************************************
**FUNCTION	::	GetErrorCount
**ACTION	::	return the number of errors from a given counter
**PARAMS IN	::  hChip		==>	handle to the chip
**			::	Counter		==>	used counter 1 or 2.
			::  
**PARAMS OUT::	NONE
**RETURN	::	Synthesizer frequency (Hz)
*****************************************************/
U32 FE_STV0903_GetErrorCount_E(FE_STV0903_ERRORCOUNTER Counter)
{
	U32 lsb,msb,hsb,errValue;
	
	
	/*Read the Error value*/
	switch(Counter)
	{
		case FE_STV0903_COUNTER1:
		default:
			hsb = ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ERR_CNT12);
			msb = ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ERR_CNT11);
			lsb = ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ERR_CNT10);
		break;
	
		case FE_STV0903_COUNTER2:
			hsb = ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ERR_CNT22);
			msb = ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ERR_CNT21);
			lsb = ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ERR_CNT20);	
		break;
	}
	
	/*Cupute the Error value 3 bytes (HSB,MSB,LSB)*/
	errValue=(hsb<<16)+(msb<<8)+(lsb);
	return (errValue);
}

/*****************************************************
**FUNCTION	::	STV0903_RepeaterFn  (First repeater )
**ACTION	::	Set the repeater On or OFF 
**PARAMS IN	::  hChip		==>	handle to the chip
			::	State		==> repeater state On/Off.
**PARAMS OUT::	NONE
**RETURN	::	Error (if any)
*****************************************************/
#if 0
STCHIP_Error_t FE_STV0903_RepeaterFn(STCHIP_Handle_t hChip,BOOL State)
{
	STCHIP_Error_t error = CHIPERR_NO_ERROR;
	
	if(hChip != NULL)
	{
		if(State == TRUE)
			ChipSetField(hChip,FSTV0903_I2CT_ON,1);  
	}
	
	return error;
}
#endif

STCHIP_Error_t FE_STV0903_RepeaterFn_E(BOOL State)
{
	STCHIP_Error_t error = CHIPERR_NO_ERROR;
	U8 chipaddress=STV0903_I2C_ADDRESS;
	if(State == TRUE)
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_I2CT_ON,1);  
		
	return error;
}
/*****************************************************
--FUNCTION	::	CarrierWidth
--ACTION	::	Compute the width of the carrier
--PARAMS IN	::	SymbolRate	->	Symbol rate of the carrier (Kbauds or Mbauds)
--				RollOff		->	Rolloff * 100
--PARAMS OUT::	NONE
--RETURN	::	Width of the carrier (KHz or MHz) 
--***************************************************/
U32 FE_STV0903_CarrierWidth(U32 SymbolRate, FE_STV0903_RollOff_t RollOff)
{
	U32 rolloff;
	
	switch(RollOff)
	{
		case FE_SAT_20:
			rolloff=20;
		break;
		
		case FE_SAT_25:
			rolloff=25;
			break;
		
		case FE_SAT_35:
		default:
			rolloff=35;
			break;
	}
	return (SymbolRate  + (SymbolRate*rolloff)/100);
}

void FE_STV0903_SetTS_Parallel_Serial_E(FE_STV0903_Clock_t PathTS)
{
	#if 1
	switch(PathTS)
	{
		case FE_TS_SERIAL_PUNCT_CLOCK:
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TSFIFO_SERIAL,0x01);	/*Serial mode*/ 
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TSFIFO_DVBCI,0);
		break;
		
		case FE_TS_SERIAL_CONT_CLOCK:
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TSFIFO_SERIAL,0x01);	/*Serial mode*/ 
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TSFIFO_DVBCI,1);
		break;
		
		case FE_TS_PARALLEL_PUNCT_CLOCK:		
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TSFIFO_SERIAL,0x00); /*Parallel mode*/
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TSFIFO_DVBCI,0);
		break;
		
		case FE_TS_DVBCI_CLOCK:
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TSFIFO_SERIAL,0x00); /*Parallel mode*/
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TSFIFO_DVBCI,0);
		break;
		default:
		break;
	}
#endif
}

/*****************************************************
--FUNCTION	::	TunerSetType
--ACTION	::	Set the STV0903 Tuner controller
--PARAMS IN	::	Chip Handel
--			::	Tuner Type (control type SW, AUTO 6000 ,AUTO 6100 or AUTO 6110
			::	
--PARAMS OUT::	NONE
--RETURN	::	 
--***************************************************/
#if 0
void FE_STV0903_SetTunerType(STCHIP_Handle_t hChip,
				  FE_STV0903_Tuner_t TunerType,
				  U8 I2cAddr,
				  U32 Reference,
				  U8 OutClkDiv
				  )
{
	
	switch(TunerType)
	{
		case FE_SAT_AUTO_STB6000:
		
			ChipSetOneRegister(hChip,RSTV0903_TNRCFG,0x1c);
			ChipSetOneRegister(hChip,RSTV0903_TNRCFG2,0x86);
			ChipSetOneRegister(hChip,RSTV0903_TNRCFG3,0x18);
			ChipSetOneRegister(hChip,RSTV0903_TNRXTAL,(Reference/1000000));
			ChipSetOneRegister(hChip,RSTV0903_TNRSTEPS,0x05);
			ChipSetOneRegister(hChip,RSTV0903_TNRGAIN,0x17);
			ChipSetOneRegister(hChip,RSTV0903_TNRADJ,0x1f);
			ChipSetOneRegister(hChip,RSTV0903_TNRCTL2,0x0);

			ChipSetField(hChip,FSTV0903_TUN_TYPE,1);
		break;
	
		case FE_SAT_AUTO_STB6100:
			ChipSetOneRegister(hChip,RSTV0903_TNRCFG,0x3c);
			ChipSetOneRegister(hChip,RSTV0903_TNRCFG2,0x86);
			ChipSetOneRegister(hChip,RSTV0903_TNRCFG3,0x18); 
			ChipSetOneRegister(hChip,RSTV0903_TNRXTAL,(Reference/1000000));
			ChipSetOneRegister(hChip,RSTV0903_TNRSTEPS,0x05);
			ChipSetOneRegister(hChip,RSTV0903_TNRGAIN,0x17);
			ChipSetOneRegister(hChip,RSTV0903_TNRADJ,0x1f);
			ChipSetOneRegister(hChip,RSTV0903_TNRCTL2,0x0);
		
			ChipSetField(hChip,FSTV0903_TUN_TYPE,3);
		break;
	
		case FE_SAT_AUTO_STV6110:
		
			ChipSetOneRegister(hChip,RSTV0903_TNRCFG,0x4c);
			ChipSetOneRegister(hChip,RSTV0903_TNRCFG2,0x06);
			ChipSetOneRegister(hChip,RSTV0903_TNRCFG3,0x18);
			ChipSetOneRegister(hChip,RSTV0903_TNRXTAL,(Reference/1000000));
			ChipSetOneRegister(hChip,RSTV0903_TNRSTEPS,0x05);
			ChipSetOneRegister(hChip,RSTV0903_TNRGAIN,0x41);
			ChipSetOneRegister(hChip,RSTV0903_TNRADJ,0);
			ChipSetOneRegister(hChip,RSTV0903_TNRCTL2,0x97);
		 
			ChipSetField(hChip,FSTV0903_TUN_TYPE,4);
			
			switch(OutClkDiv)
			{
				case 1:
				default:
					ChipSetField(hChip,FSTV0903_TUN_KDIVEN,0);
				break;
						
				case 2:
					ChipSetField(hChip,FSTV0903_TUN_KDIVEN,1);
				break;
						
				case 4:
					ChipSetField(hChip,FSTV0903_TUN_KDIVEN,2);
				break;
						
				case 8:
					ChipSetField(hChip,FSTV0903_TUN_KDIVEN,3);
				break;
			}
		break;
	
		case FE_SAT_SW_TUNER:
		default:
			ChipSetField(hChip,FSTV0903_TUN_TYPE,6);
		break;

	}
			
	switch(I2cAddr)
	{
		case 0xC0:
		default:
			ChipSetField(hChip,FSTV0903_TUN_MADDRESS,0);
		break;
						
		case 0xC2:
			ChipSetField(hChip,FSTV0903_TUN_MADDRESS,1);
		break;
						
		case 0xC4:
			ChipSetField(hChip,FSTV0903_TUN_MADDRESS,2);
		break;
				
		case 0xC6:
			ChipSetField(hChip,FSTV0903_TUN_MADDRESS,3);
		break;
	}
	ChipSetOneRegister(hChip,RSTV0903_TNRLD,1);
}
#endif
#if 0
void FE_STV0903_SetTunerType_E(
				  FE_STV0903_Tuner_t TunerType,
				  U8 I2cAddr,
				  U32 Reference,
				  U8 OutClkDiv
				  )
{
	
	switch(TunerType)
	{
		case FE_SAT_AUTO_STV6110:
		
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TNRCFG,0x4c);
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TNRCFG2,0x06);
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TNRCFG3,0x18);
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TNRXTAL,(Reference/1000000));
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TNRSTEPS,0x05);
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TNRGAIN,0x41);
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TNRADJ,0);
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TNRCTL2,0x97);
		 
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_TYPE,4);
			
			switch(OutClkDiv)
			{
				case 1:
				default:
					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_KDIVEN,0);
				break;
						
				case 2:
					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_KDIVEN,1);
				break;
						
				case 4:
					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_KDIVEN,2);
				break;
						
				case 8:
					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_KDIVEN,3);
				break;
			}
		break;
	
		case FE_SAT_SW_TUNER:
		default:
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_TYPE,6);
		break;

	}
			
	switch(I2cAddr)
	{
		case 0xC0:
		default:
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_MADDRESS,0);
		break;
						
		case 0xC2:
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_MADDRESS,1);
		break;
						
		case 0xC4:
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_MADDRESS,2);
		break;
				
		case 0xC6:
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_MADDRESS,3);
		break;
	}
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TNRLD,1);
}
#endif

/*****************************************************
--FUNCTION	::	GetTunerFrequency
--ACTION	::	Return the current tuner frequency (KHz)
--PARAMS IN	::	Chip Handel
			::	Tuner Handel
			::	Tuner Type
			::		
--PARAMS OUT::	NONE
--RETURN	::	Tuner Frequency  in KHz. 
--***************************************************/
#if 0
U32 FE_STV0903_GetTunerFrequency(STCHIP_Handle_t hChip,
					  TUNER_Handle_t hTuner,
					  FE_STV0903_Tuner_t TunerType)
{
	U32 tunerFrequency;

	S32	freq,round;
	
	switch(TunerType)
	{
		case FE_SAT_AUTO_STB6000:
		case FE_SAT_AUTO_STB6100:
		case FE_SAT_AUTO_STV6110:
			
			/*	Formulat :
				Tuner_Frequency(MHz)	= Regs /64
				Tuner_granularity(MHz)	= Regs	/2048
				
				real_Tuner_Frequency =	Tuner_Frequency(MHz) - Tuner_granularity(MHz) 
				
			*/
			freq=(ChipGetField(hChip,FSTV0903_TUN_RFFREQ2)<<10)+
			(ChipGetField(hChip,FSTV0903_TUN_RFFREQ1)<<2)+
			ChipGetField(hChip,FSTV0903_TUN_RFFREQ0);
	
			freq=(freq *1000)/64;
		
			round=(ChipGetField(hChip,FSTV0903_TUN_RFRESTE1)>>2)+
			ChipGetField(hChip,FSTV0903_TUN_RFRESTE0);
			
			round=(round *1000)/2048;	
			
			tunerFrequency=freq-round;
			
			if(ChipGetField(hChip,FSTV0903_TUN_ACKFAIL))
				hTuner->Error=CHIPERR_I2C_NO_ACK;
		break;
		
		case FE_SAT_SW_TUNER:
		default:
			/* Read the tuner frequency by SW (SW Tuner API)*/
			tunerFrequency=FE_Sat_TunerGetFrequency(hTuner);
		break;
	}
	
	return tunerFrequency;
	
}
#endif
U32 FE_STV0903_GetTunerFrequency_E(FE_STV0903_Tuner_t TunerType,U32 TunerReference)
{
	U32 tunerFrequency;

	S32	freq,round;
	
	switch(TunerType)
	{
		case FE_SAT_AUTO_STB6000:
		case FE_SAT_AUTO_STB6100:
		case FE_SAT_AUTO_STV6110:
			
			/*	Formulat :
				Tuner_Frequency(MHz)	= Regs /64
				Tuner_granularity(MHz)	= Regs	/2048
				
				real_Tuner_Frequency =	Tuner_Frequency(MHz) - Tuner_granularity(MHz) 
				
			*/
			freq=(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_RFFREQ2)<<10)+
			(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_RFFREQ1)<<2)+
			ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_RFFREQ0);
	
			freq=(freq *1000)/64;
		
			round=(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_RFRESTE1)>>2)+
			ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_RFRESTE0);
			
			round=(round *1000)/2048;	
			
			tunerFrequency=freq-round;
			
			ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TUN_ACKFAIL);

		break;
		
		case FE_SAT_SW_TUNER:
		default:
			/* Read the tuner frequency by SW (SW Tuner API)*/
			tunerFrequency=FE_Sat_TunerGetFrequency6110(TunerReference);
			break;
	}
	
	return tunerFrequency;
	
}

/*****************************************************
--FUNCTION	::	SetTuner
--ACTION	::	Set tuner frequency (KHz) and the low pass filter (Bandwidth in Hz)
--PARAMS IN	::	Chip Handel
			::	Tuner Handel
			::	Tuner Type
			::	Frequency (Khz)
			::	Bandwidth (Hz)
--PARAMS OUT::	NONE
--RETURN	::	None. 
--***************************************************/
#if 0
void FE_STV0903_SetTuner(STCHIP_Handle_t hChip,
				  TUNER_Handle_t hTuner,
				  FE_STV0903_Tuner_t TunerType,
				  U32 Frequency,
				  U32 Bandwidth
				  )
{
	U32 tunerFrequency;



	
	switch(TunerType)
	{
		case FE_SAT_AUTO_STB6000:
		case FE_SAT_AUTO_STB6100:
		case FE_SAT_AUTO_STV6110:
		
			/*	Formulat:
				Tuner_frequency_reg= Frequency(MHz)*64
			*/
			tunerFrequency=(Frequency*64)/1000;
				
			ChipSetField(hChip,FSTV0903_TUN_RFFREQ2,(tunerFrequency>>10));
			ChipSetField(hChip,FSTV0903_TUN_RFFREQ1,(tunerFrequency>>2)&0xff);
			ChipSetField(hChip,FSTV0903_TUN_RFFREQ0,(tunerFrequency & 0x03));
			/*Low Pass Filter = BW /2 (MHz)	*/ 
			ChipSetField(hChip,FSTV0903_TUN_BW,Bandwidth/2000000);
			/*Tuner Write trig	*/
			ChipSetOneRegister(hChip,RSTV0903_TNRLD,1);
			
			WAIT_N_MS(2);
			if(ChipGetField(hChip,FSTV0903_TUN_ACKFAIL))
				hTuner->Error=CHIPERR_I2C_NO_ACK;
		break;
		
		case FE_SAT_SW_TUNER:
		default:
			/*	Set tuner Frequency by SW	(SW Tuner API)*/
			FE_Sat_TunerSetFrequency(hTuner,Frequency);
			/*	Set tuner BW by SW (SW tuner API)	*/ 
			FE_Sat_TunerSetBandwidth(hTuner,Bandwidth);
		break;
	}
	
}
#endif
void FE_STV0903_SetTuner_E(FE_STV0903_Tuner_t TunerType,
				U32 TunerReference,
				  U32 Frequency,
				  U32 Bandwidth
				  )
{
	U32 tunerFrequency;
	FE_Sat_TunerSetFrequency_E(Frequency,TunerReference);
	/*	Set tuner BW by SW (SW tuner API)	*/ 
	FE_Sat_TunerSetBandwidth_E(Bandwidth);
}

/*****************************************************
--FUNCTION	::	GetTunerStatus
--ACTION	::	Return the status of the tuner Locked or not
--PARAMS IN	::	Chip Handel
			::	Tuner Handel
			::	Tuner Type
			::	
--PARAMS OUT::	NONE
--RETURN	::	Boolean (tuner Lock status). 
--***************************************************/
#if 0
BOOL FE_STV0903_GetTunerStatus(STCHIP_Handle_t hChip,
				  TUNER_Handle_t hTuner,
				  FE_STV0903_Tuner_t TunerType)
{
	
	BOOL lock;
	
	switch(TunerType)
	{
		case FE_SAT_AUTO_STB6000:
		case FE_SAT_AUTO_STB6100:
		case FE_SAT_AUTO_STV6110:
		/* case of automatic controled tuner
		   Read the tuner lock flag from the STV0903
		*/
			if(ChipGetField(hChip,FSTV0903_TUN_I2CLOCKED)==1)
				lock=TRUE;
			else
				lock=FALSE;
			
			if(ChipGetField(hChip,FSTV0903_TUN_ACKFAIL))
				hTuner->Error=CHIPERR_I2C_NO_ACK;
		break;
		
		case FE_SAT_SW_TUNER:
		default:
		/*	case of SW controled tuner
			Read the lock flag from the tuner (SW Tuner API )
		*/
			 lock=FE_Sat_TunerGetStatus(hTuner);
		break;
	}
	return lock;
}
#endif
/*****************************************************
--FUNCTION	::	GetCarrierFrequency
--ACTION	::	Return the carrier frequency offset
--PARAMS IN	::	MasterClock	->	Masterclock frequency (Hz)
			::		
--PARAMS OUT::	NONE
--RETURN	::	Frequency offset in Hz. 
--***************************************************/
#if 0
S32 FE_STV0903_GetCarrierFrequency(STCHIP_Handle_t hChip,U32 MasterClock )
{
	S32	derot,
		rem1,
		rem2,
		intval1,
		intval2,
		carrierFrequency;
	
	/*	Read the carrier frequency regs value	*/
	derot=(ChipGetField(hChip,FSTV0903_CAR_FREQ2)<<16)+
	      (ChipGetField(hChip,FSTV0903_CAR_FREQ1)<<8)+
		 (ChipGetField(hChip,FSTV0903_CAR_FREQ0));

	/*	cumpute the signed value	*/ 
	derot=Get2Comp(derot,24);
		
	/*
		Formulat:
		carrier_frequency = MasterClock * Reg / 2^24
	*/
	intval1=MasterClock/PowOf2(12);
	intval2=derot/PowOf2(12);

	rem1=MasterClock%PowOf2(12);
	rem2=derot%PowOf2(12);
	derot=(intval1*intval2)+
		  ((intval1*rem2)/PowOf2(12))+
		  ((intval2*rem1)/PowOf2(12));  	/*only for integer calculation */
	
		
	carrierFrequency=derot;
	
	return carrierFrequency;
}
#endif
S32 FE_STV0903_GetCarrierFrequency_E(U32 MasterClock )
{
	U32	derot,
		rem1,
		rem2,
		intval1,
		intval2,
		carrierFrequency;
	
	/*	Read the carrier frequency regs value	*/
	derot=(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_CAR_FREQ2)<<16)+
	      (ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_CAR_FREQ1)<<8)+
		 (ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_CAR_FREQ0));

	/*	cumpute the signed value	*/ 
	derot=Get2Comp(derot,24);
		
	/*
		Formulat:
		carrier_frequency = MasterClock * Reg / 2^24
	*/
	intval1=MasterClock/PowOf2(12);
	intval2=derot/PowOf2(12);

	rem1=MasterClock%PowOf2(12);
	rem2=derot%PowOf2(12);
	derot=(intval1*intval2)+
		  ((intval1*rem2)/PowOf2(12))+
		  ((intval2*rem1)/PowOf2(12));  	/*only for integer calculation */
	
		
	carrierFrequency=derot;
	
	return carrierFrequency;
}
#if 0
S32 FE_STV0903_TimigGetOffset(STCHIP_Handle_t hChip,U32 SymbolRate)
{
	S32 timingoffset;
	
	/*	Formulat :
		SR_Offset = TMGRREG * SR /2^29
		TMGREG is 3 bytes registers value 
		SR is the current symbol rate
	*/
	timingoffset=(ChipGetOneRegister(hChip,RSTV0903_TMGREG2)<<16)+
	    	(ChipGetOneRegister(hChip,RSTV0903_TMGREG2+1)<<8)+
				(ChipGetOneRegister(hChip,RSTV0903_TMGREG2+2));
		
		timingoffset=Get2Comp(timingoffset,24);
		
	
	if(timingoffset==0)
		timingoffset=1;	
	timingoffset=((S32)SymbolRate*10)/((S32)PowOf2(24)/timingoffset);
	timingoffset/= 320;
	
	return timingoffset;
	
	
}
#endif
S32 FE_STV0903_TimigGetOffset_E(U32 SymbolRate)
{
	S32 timingoffset;
	
	/*	Formulat :
		SR_Offset = TMGRREG * SR /2^29
		TMGREG is 3 bytes registers value 
		SR is the current symbol rate
	*/
	timingoffset=(ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGREG2,0,0)<<16)+
	    	(ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGREG2+1,0,0)<<8)+
				(ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGREG2+2,0,0));
		
		timingoffset=Get2Comp(timingoffset,24);
		
	
	if(timingoffset==0)
		timingoffset=1;	
	timingoffset=((S32)SymbolRate*10)/((S32)PowOf2(24)/timingoffset);
	timingoffset/= 320;
	
	return timingoffset;
	
	
}


/*****************************************************
--FUNCTION	::	GetSymbolRate
--ACTION	::	Get the current symbol rate
--PARAMS IN	::	hChip		->	handle to the chip
			::	MasterClock	->	Masterclock frequency (Hz)
			::	
--PARAMS OUT::	NONE
--RETURN	::	Symbol rate in Symbol/s
*****************************************************/
#if 0
U32 FE_STV0903_GetSymbolRate(STCHIP_Handle_t hChip,U32 MasterClock)
{
	S32	rem1,
		rem2,
		intval1,
		intval2,
		symbolRate;
	
	symbolRate =(ChipGetField(hChip,FSTV0903_SYMB_FREQ3)<<24)+
				(ChipGetField(hChip,FSTV0903_SYMB_FREQ2)<<16)+
				(ChipGetField(hChip,FSTV0903_SYMB_FREQ1)<<8)+
				(ChipGetField(hChip,FSTV0903_SYMB_FREQ0));		
	
	/*	Formulat :
		Found_SR = Reg * MasterClock /2^32
	*/
	
	intval1=(MasterClock)/PowOf2(16);
	intval2=(symbolRate)/PowOf2(16);

	rem1=(MasterClock)%PowOf2(16);
	rem2=(symbolRate)%PowOf2(16);

	symbolRate	=(intval1*intval2)+
		  		 ((intval1*rem2)/PowOf2(16))+
		  		 ((intval2*rem1)/PowOf2(16));  	/*only for integer calculation */

	return (symbolRate);
	
}
#endif
U32 FE_STV0903_GetSymbolRate_E(S32 MasterClock)
{
	S32	rem1,
		rem2,
		intval1,
		intval2,
		symbolRate;
	U8 	ChipID;
	ChipID=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MID,0,0);
	symbolRate =(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SYMB_FREQ3)<<24)+
				(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SYMB_FREQ2)<<16)+
				(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SYMB_FREQ1)<<8)+
				(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SYMB_FREQ0));		
	
	/*	Formulat :
		Found_SR = Reg * MasterClock /2^32
	*/
	
	intval1=(MasterClock)/PowOf2(16);
	intval2=(symbolRate)/PowOf2(16);

	rem1=(MasterClock)%PowOf2(16);
	rem2=(symbolRate)%PowOf2(16);

	symbolRate	=(intval1*intval2)+
		  		 ((intval1*rem2)/PowOf2(16))+
		  		 ((intval2*rem1)/PowOf2(16));  	/*only for integer calculation */

	return (symbolRate);
	
}

/*****************************************************
--FUNCTION	::	SetSymbolRate
--ACTION	::	Get the Symbol rate
--PARAMS IN	::	hChip		->	handle to the chip
			::	MasterClock	->	Masterclock frequency (Hz)
			::	SymbolRate	->	Symbol Rate (Symbol/s)
			::	
--PARAMS OUT::	NONE
--RETURN	::	None
*****************************************************/
#if 0
void FE_STV0903_SetSymbolRate(STCHIP_Handle_t hChip,U32 MasterClock,U32 SymbolRate)
{
	U32	symb;
	
	if(SymbolRate>6000000)
	{
		symb=(SymbolRate/1000)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
		symb/=(MasterClock/1000);
	}
	else
	{
		symb=(SymbolRate/100)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
		symb/=(MasterClock/100);
	}
	
	
	ChipSetOneRegister(hChip,RSTV0903_SFRINIT1,(symb>>8) & 0xFF);	/* Write the MSB */
	ChipSetOneRegister(hChip,RSTV0903_SFRINIT1+1,(symb & 0xFF));	/* Write the LSB */
	
}
#endif
void FE_STV0903_SetSymbolRate_E(U8 chipaddress,STCHIP_Mode_t mode,BOOL Repeater,U32 MasterClock,U32 SymbolRate)
{
	U32	symb;
	U8 dat=0;
	//MasterClock=MasterClock*2;
	
	if(SymbolRate>6000000)
	{
		symb=(SymbolRate/1000)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
		symb/=(MasterClock/1000);
	}
	else
	{
		symb=(SymbolRate/100)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
		symb/=(MasterClock/100);
	}
	
	
	ChipSetOneRegister_E(chipaddress,mode,Repeater,RSTV0903_SFRINIT1,(symb>>8) & 0xFF);	/* Write the MSB */
	ChipSetOneRegister_E(chipaddress,mode,Repeater,RSTV0903_SFRINIT1+1,(symb & 0xFF));	/* Write the LSB */

    
	
	
}

/*****************************************************
--FUNCTION	::	SetMaxSymbolRate
--ACTION	::	Get the Symbol rate
--PARAMS IN	::	hChip		->	handle to the chip
			::	MasterClock	->	Masterclock frequency (Hz)
			::	SymbolRate	->	Symbol Rate Max to SR + 5%
			::	
--PARAMS OUT::	NONE
--RETURN	::	None
*****************************************************/
#if 0
void FE_STV0903_SetMaxSymbolRate(STCHIP_Handle_t hChip,U32 MasterClock,U32 SymbolRate)
{
	U32	symb;
	
	SymbolRate = 105*(SymbolRate /100);
	if(SymbolRate>6000000)
	{
		symb=(SymbolRate/1000)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
		symb/=(MasterClock/1000);
	}
	else
	{
		symb=(SymbolRate/100)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
		symb/=(MasterClock/100);
	}
	
	
	ChipSetOneRegister(hChip,RSTV0903_SFRUP1,(symb>>8) & 0x7F);	/* Write the MSB */
	ChipSetOneRegister(hChip,RSTV0903_SFRUP0,(symb & 0xFF));	/* Write the LSB */
	
}
#endif
void FE_STV0903_SetMaxSymbolRate_E(U8 chipaddress,STCHIP_Mode_t mode,BOOL Repeater,U32 MasterClock,U32 SymbolRate)
{
	U32	symb;
	
	SymbolRate = 105*(SymbolRate /100);
	if(SymbolRate>6000000)
	{
		symb=(SymbolRate/1000)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
		symb/=(MasterClock/1000);
	}
	else
	{
		symb=(SymbolRate/100)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
		symb/=(MasterClock/100);
	}
	
	
	ChipSetOneRegister_E(chipaddress,mode,Repeater,RSTV0903_SFRUP1,(symb>>8) & 0x7F);	/* Write the MSB */
	ChipSetOneRegister_E(chipaddress,mode,Repeater,RSTV0903_SFRUP0,(symb & 0xFF));	/* Write the LSB */
	
}

/*****************************************************
--FUNCTION	::	SetMinSymbolRate
--ACTION	::	Get the Symbol rate
--PARAMS IN	::	hChip		->	handle to the chip
			::	MasterClock	->	Masterclock frequency (Hz)
			::	SymbolRate	->	Symbol Rate Min to SR - 5% (Symbol/s)
			::	
--PARAMS OUT::	NONE
--RETURN	::	None
***********************************z******************/
#if 0
void FE_STV0903_SetMinSymbolRate(STCHIP_Handle_t hChip,U32 MasterClock,U32 SymbolRate)
{
	U32	symb;
	
	SymbolRate = 95*(SymbolRate /100);
	if(SymbolRate>6000000)
	{
		symb=(SymbolRate/1000)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
		symb/=(MasterClock/1000);
	}
	else
	{
		symb=(SymbolRate/100)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
		symb/=(MasterClock/100);
	}
	
	
	ChipSetOneRegister(hChip,RSTV0903_SFRLOW1,(symb>>8) & 0xFF);	/* Write the MSB */
	ChipSetOneRegister(hChip,RSTV0903_SFRLOW0,(symb & 0xFF));	/* Write the LSB */
	
}
#endif
void FE_STV0903_SetMinSymbolRate_E(U8 chipaddress,STCHIP_Mode_t mode,BOOL Repeater,U32 MasterClock,U32 SymbolRate)
{
	U32	symb;
	
	SymbolRate = 95*(SymbolRate /100);
	if(SymbolRate>6000000)
	{
		symb=(SymbolRate/1000)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
		symb/=(MasterClock/1000);
	}
	else
	{
		symb=(SymbolRate/100)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
		symb/=(MasterClock/100);
	}
	
	
	ChipSetOneRegister_E(chipaddress,mode,Repeater,RSTV0903_SFRLOW1,(symb>>8) & 0xFF);	/* Write the MSB */
	ChipSetOneRegister_E(chipaddress,mode,Repeater,RSTV0903_SFRLOW0,(symb & 0xFF));	/* Write the LSB */
	
}

/*****************************************************
--FUNCTION	::	GetStandard
--ACTION	::	Return the current standrad (DVBS1,DSS or DVBS2
--PARAMS IN	::	hChip		->	handle to the chip
			::	
--PARAMS OUT::	NONE
--RETURN	::	standard (DVBS1, DVBS2 or DSS
*****************************************************/
#if 0
FE_STV0903_TrackingStandard_t	FE_STV0903_GetStandard(STCHIP_Handle_t hChip)
{
	FE_STV0903_TrackingStandard_t foundStandard;
	
	
	if(ChipGetField(hChip,FSTV0903_HEADER_MODE)==2)
			foundStandard=FE_SAT_DVBS2_STANDARD;			/*	The demod Find DVBS2*/
			
	else if(ChipGetField(hChip,FSTV0903_HEADER_MODE)==3)			/*	The demod Find DVBS1/DSS*/			
	{
		if(ChipGetField(hChip,FSTV0903_DSS_DVB)==1)
			foundStandard=FE_SAT_DSS_STANDARD;				/*	Viterbi Find DSS*/
		else
			foundStandard=FE_SAT_DVBS1_STANDARD;			/*	Viterbi Find DVBS1*/ 
	}
	else
		foundStandard=FE_SAT_UNKNOWN_STANDARD;				/*	The demod find nothing, unknown standard	*/
	return foundStandard; 
}
#endif
FE_STV0903_TrackingStandard_t	FE_STV0903_GetStandard_E(void)
{
	FE_STV0903_TrackingStandard_t foundStandard;
	U8 dat=0;
	
	if(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_HEADER_MODE)==2)
			foundStandard=FE_SAT_DVBS2_STANDARD;			/*	The demod Find DVBS2*/
			
	else if(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_HEADER_MODE)==3)			/*	The demod Find DVBS1/DSS*/			
	{
		if(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DSS_DVB)==1)
			foundStandard=FE_SAT_DSS_STANDARD;				/*	Viterbi Find DSS*/
		else
			foundStandard=FE_SAT_DVBS1_STANDARD;			/*	Viterbi Find DVBS1*/ 
	}
	else
		foundStandard=FE_SAT_UNKNOWN_STANDARD;				/*	The demod find nothing, unknown standard	*/
        dat=foundStandard;
	//display_hex(dat,6);
	return foundStandard; 
}

/*****************************************************
--FUNCTION	::	GetRFLevel
--ACTION	::	Return power of the signal
--PARAMS IN	::	hChip	->	handle to the chip
			::	lookup	->	LUT for RF level estimation.
			::		
--PARAMS OUT::	NONE
--RETURN	::	Power of the signal (dBm), -100 if no signal 
--***************************************************/
S32 FE_STV0903_GetRFLevel_E(void)
{
	FE_STV0903_LOOKUP_t	lookup = 
	{
		14,
		{
			{-5, 0xCAA1},   /*-5dBm*/
			{-10,0xC229},   /*-10dBm*/
			{-15,0xBB08},   /*-15dBm*/
			{-20,0xB4BC},   /*-20dBm*/
			{-25,0xAD5A},   /*-25dBm*/
			{-30,0xA298},   /*-30dBm*/
			{-35,0x98A8},   /*-35dBm*/
			{-40,0x8389},   /*-40dBm*/
			{-45,0x59BE},   /*-45dBm*/
			{-50,0x3A14},   /*-50dBm*/
			{-55,0x2D11},   /*-55dBm*/
			{-60,0x210D},   /*-60dBm*/
			{-65,0xA14F},   /*-65dBm*/
			{-70,0x7AA}		/*-70dBm*/
		}
	};
	unsigned short reg[14]={0xCAA1,0xC229,0xBB08,0xB4BC,0xAD5A,0xA298,0x98A8,0x8389,0x59BE,0x3A14,0x2D11,0x210D,0xA14F,0x7AA};
	char lvl[14]={-5,-10,-15,-20,-25,-30,-35,-40,-45,-50,-55,-60,-65,-70};
	unsigned short agcGain = 0,
		Imin,
		Imax;
	U8 dat=0;
	char rfLevel = 0;
       unsigned char i;
	  unsigned char offset;
		/* Read the AGC integrator registers (2 Bytes)	*/
		agcGain=MAKEWORD(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_AGCIQ_VALUE1),ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_AGCIQ_VALUE0));
             // dat=agcGain>>8;
	       //hex_out(&dat,1);
		//dat=agcGain;
	      // hex_out(&dat,1);
		//display_hex(dat, 2);
		
		Imin = 0;
		Imax = lookup.size-1;
		for(i=0;i<14;i++)
		{
			if(agcGain>=reg[i])
			{break;}
		}
		//dat=i;
	       //hex_out(&dat,1);

		 offset=(reg[i-1]-agcGain)*5/(reg[i-1]-reg[i]);
		// hex_out(&offset,1);
		 rfLevel=lvl[i-1]-offset;
		// hex_out(&rfLevel,1);		
	
	return rfLevel;
}

/*****************************************************
--FUNCTION	::	CarrierGetQuality
--ACTION	::	Return the carrier to noise of the current carrier
--PARAMS IN	::	hChip	->	handle to the chip
			::	lookup	->	LUT for CNR level estimation.
--PARAMS OUT::	NONE
--RETURN	::	C/N of the carrier, 0 if no carrier 
--***************************************************/
S32 FE_STV0903_CarrierGetQuality_E(FE_STV0903_LOOKUP_t *lookup)   
{
	S32 c_n = -100,
	regval,
	Imin,
	Imax,
	i,
	lockFlagField,
	noiseField1,
	noiseField0;
	
	lockFlagField =	FSTV0903_LOCK_DEFINITIF;
	if(FE_STV0903_GetStandard_E()==FE_SAT_DVBS2_STANDARD)
	{
		/*If DVBS2 use PLH normilized noise indicators*/
		noiseField1 =FSTV0903_NOSPLHT_NORMED1;
		noiseField0 =FSTV0903_NOSPLHT_NORMED0;
	}
	else
	{
		/*if not DVBS2 use symbol normalized noise indicators*/ 
		noiseField1 =FSTV0903_NOSDATAT_NORMED1;
		noiseField0 =FSTV0903_NOSDATAT_NORMED0;
	}
		
	if(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,lockFlagField))
	{
		if((lookup != NULL) && lookup->size)
		{
			regval=0;
			//WAIT_N_MS(1);
			//uartcmd_check_settiing();
			//WAIT_N_MS(1);
			//uartcmd_check_settiing();
			//WAIT_N_MS(1);
			//uartcmd_check_settiing();
			//WAIT_N_MS(1);
			//uartcmd_check_settiing();
			//WAIT_N_MS(1);
			//uartcmd_check_settiing();
			for(i=0;i<16;i++)
			{
				regval += MAKEWORD(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,noiseField1),ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,noiseField0));
				//WAIT_N_MS(1);
				//uartcmd_check_settiing();
			}
			
			regval /=16;

			Imin = 0;
			Imax = lookup->size-1;
	
			if(INRANGE(lookup->table[Imin].regval,regval,lookup->table[Imax].regval))
			{
				while((Imax-Imin)>1)
				{
					i=(Imax+Imin)/2; 
					if(INRANGE(lookup->table[Imin].regval,regval,lookup->table[i].regval))
						Imax = i;
					else
						Imin = i;
				}
		
				c_n =	((regval - lookup->table[Imin].regval)
						* (lookup->table[Imax].realval - lookup->table[Imin].realval)
						/ (lookup->table[Imax].regval - lookup->table[Imin].regval))
						+ lookup->table[Imin].realval;
			}
			else if(regval<lookup->table[Imin].regval)
				c_n = 1000;
		}
	}
	
     //printf("c_n:%d\n",c_n);
       if(c_n==-100)
        c_n=0;
	return c_n; 
}


/*****************************************************
--FUNCTION	::	GetBer
--ACTION	::	Return the Viterbi BER if DVBS1/DSS or the PER if DVBS2
--PARAMS IN	::	hChip	->	handle to the chip
--PARAMS OUT::	NONE
--RETURN	::	ber/per scalled to 1e7 
--***************************************************/
U32 FE_STV0903_GetBer_E(void)
{
	U32 ber = XtoPowerY(10,7),i;
	S32 demodState;
	//U8 dat=0;
	
	
	demodState=ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_HEADER_MODE);
	//WAIT_N_MS(500);  
	//dat=demodState;
	//display_hex(dat,8);
	switch(demodState)
	{
		case FE_SAT_SEARCH:
		case FE_SAT_PLH_DETECTED:
		default:
			ber=XtoPowerY(10,7);		/*demod Not locked ber = 1*/
		break;
		
		case FE_SAT_DVBS_FOUND:

			ber=0;
			/* Average 5 ber values */ 
			for(i=0;i<5;i++)
			{
				//WAIT_N_MS(50);
				ber += FE_STV0903_GetErrorCount_E(FE_STV0903_COUNTER1);
			}
	
			ber/=5;
	
			if(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_PRFVIT))	/*	Check for carrier	*/
			{
				/*	Error Rate	*/
				ber *= 9766; 		/*ber = ber * 10^7/2^10 */
				ber /= PowOf2(10);	/*  theses two lines => ber = ber * 10^7/2^20	*/
				ber/=8;
			
			}
		break;
		
		case FE_SAT_DVBS2_FOUND:
		
			ber=0;
			/* Average 5 ber values */ 
			for(i=0;i<5;i++)
			{
				//WAIT_N_MS(100);
				ber += FE_STV0903_GetErrorCount_E(FE_STV0903_COUNTER1);
			}
			ber/=5;
			
			
			if(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_PKTDELIN_LOCK))	/*	Check for S2 FEC Lock	*/
			{
				ber *= 9766; 					/*	ber = ber * 10^7/2^10 */
				ber/=PowOf2(13);				/*  theses two lines => PER = ber * 10^7/2^23	*/
			}
			
		break;
	}
	
	return ber;
}
#if 0
FE_STV0903_Rate_t FE_STV0903_GetPunctureRate(STCHIP_Handle_t hChip)
{
	S32 rateField;
	FE_STV0903_Rate_t punctureRate;	
	
	rateField=ChipGetField(hChip,FSTV0903_VIT_CURPUN);
	switch(rateField)
	{
		case 13:
			punctureRate=FE_SAT_PR_1_2;
		break;
		
		case 18:
			punctureRate=FE_SAT_PR_2_3;
		break;
		
		case 21:
			punctureRate=FE_SAT_PR_3_4;
		break;
		
		case 24:
			punctureRate=FE_SAT_PR_5_6;
		break;
		
		case 25:
			punctureRate=FE_SAT_PR_6_7;
		break;
		
		case 26:
			punctureRate=FE_SAT_PR_7_8;
		break;
		
		default:
			punctureRate=FE_SAT_PR_UNKNOWN;
		break;
	}
	
	return	punctureRate; 
}
#endif
FE_STV0903_Rate_t FE_STV0903_GetPunctureRate_E(void)
{
	S32 rateField;
	FE_STV0903_Rate_t punctureRate;	
	
	rateField=ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_VIT_CURPUN);
	switch(rateField)
	{
		case 13:
			punctureRate=FE_SAT_PR_1_2;
		break;
		
		case 18:
			punctureRate=FE_SAT_PR_2_3;
		break;
		
		case 21:
			punctureRate=FE_SAT_PR_3_4;
		break;
		
		case 24:
			punctureRate=FE_SAT_PR_5_6;
		break;
		
		case 25:
			punctureRate=FE_SAT_PR_6_7;
		break;
		
		case 26:
			punctureRate=FE_SAT_PR_7_8;
		break;
		
		default:
			punctureRate=FE_SAT_PR_UNKNOWN;
		break;
	}
	
	return	punctureRate; 
}
void FE_STV0903_GetLockTimeout(S32 *DemodTimeout,S32 *FecTimeout, S32 SymbolRate, FE_STV0903_SearchAlgo_t Algo)
{
	switch (Algo)
	{
		case FE_SAT_BLIND_SEARCH:

			if(SymbolRate<=1500000)  /*10Msps< SR <=15Msps*/
			{
				(*DemodTimeout)=1500;
				(*FecTimeout)=400;
			}
			else if(SymbolRate<=5000000)  /*10Msps< SR <=15Msps*/
			{
				(*DemodTimeout)=1000;
				(*FecTimeout)=300;
			}
			else   /*SR >20Msps*/
			{
				(*DemodTimeout)=700;
				(*FecTimeout)=100;
			}
		break;
		
		case FE_SAT_COLD_START:
		case FE_SAT_WARM_START:
		default:
			if(SymbolRate <= 1000000)  /*SR <=1Msps*/
			{
				(*DemodTimeout)=4500;
				(*FecTimeout)=1700;
			}
			else if(SymbolRate<=2000000)  /*1Msps < SR <=2Msps*/
			{
				(*DemodTimeout)=2500;
				(*FecTimeout)=1100;
			}
			else if(SymbolRate<=5000000)  /*2Msps< SR <=5Msps*/
			{
				(*DemodTimeout)=1000;
				(*FecTimeout)=550;
			}
			else if(SymbolRate<=10000000)  /*5Msps< SR <=10Msps*/
			{
				(*DemodTimeout)=700;
				(*FecTimeout)=250;
			}
			else if(SymbolRate<=20000000)  /*10Msps< SR <=20Msps*/
			{
				(*DemodTimeout)=400;
				(*FecTimeout)=130;
			}
			else   /*SR >20Msps*/
			{
				(*DemodTimeout)=300;
				(*FecTimeout)=100;
			}
		break;

	}
	
	if(Algo==FE_SAT_WARM_START)
	{
		(*DemodTimeout)/=2;
	}

}
#if 0
BOOL FE_STV0903_CheckTimingLock(STCHIP_Handle_t hChip)
{
	BOOL timingLock=FALSE;
	S32 i,
		timingcpt=0;
	U8 carFreq,
		tmgTHhigh,
		tmgTHLow;
		
	carFreq=ChipGetOneRegister(hChip,RSTV0903_CARFREQ);		
	tmgTHhigh= ChipGetOneRegister(hChip,RSTV0903_TMGTHRISE);	
	tmgTHLow= ChipGetOneRegister(hChip,RSTV0903_TMGTHFALL);  
		
	ChipSetOneRegister(hChip,RSTV0903_TMGTHRISE,0x20 );	/*use higher timing lock TH */
	ChipSetOneRegister(hChip,RSTV0903_TMGTHFALL,0);		/*use higher timing lock TH */

	ChipSetField(hChip,FSTV0903_CFR_AUTOSCAN,0);		/* stop the carrier offset search */
	ChipSetOneRegister(hChip,RSTV0903_RTC,0x80);		/*set the DVBS1 timing loop beta value to 0 (stop timing loop) */ 	 
	ChipSetOneRegister(hChip,RSTV0903_RTCS2,0x40);	/*set the DVBS2 timing loop beta value to 0 (stop timing loop) */ 
	ChipSetOneRegister(hChip,RSTV0903_CARFREQ,0x0);	/*Stop the carrier loop */
		
	ChipSetOneRegister(hChip,RSTV0903_CFRINIT1,0x0);	/*set the carrier offset to 0*/ 
	ChipSetOneRegister(hChip,RSTV0903_CFRINIT0,0x0); /*set the carrier offset to 0*/
	ChipSetOneRegister(hChip,RSTV0903_AGC2REF,0x65);
	/*Trig an acquisition (start the search) only for timing check no need to white for lock*/
	ChipSetOneRegister(hChip,RSTV0903_DMDISTATE,0x18);		 
	WAIT_N_MS(5);
	for(i=0;i<10;i++)
	{
		if(ChipGetField(hChip,FSTV0903_TMGLOCK_QUALITY)>=2)	/*read the timing lock quality*/
			timingcpt++;
		WAIT_N_MS(1);
	}
	if(timingcpt>=3)	/*timing locked when timing quality is higer than 2 for 3 times */
		timingLock =TRUE;
		
	ChipSetOneRegister(hChip,RSTV0903_AGC2REF,0x38);
	ChipSetOneRegister(hChip,RSTV0903_RTC,0x88);		/*enable the DVBS1 timing loop*/ 
	ChipSetOneRegister(hChip,RSTV0903_RTCS2,0x68);	/*enable the DVBS2 timing loop*/
		
	ChipSetOneRegister(hChip,RSTV0903_CARFREQ,carFreq);	/*enable the carrier loop */
		
	ChipSetOneRegister(hChip,RSTV0903_TMGTHRISE,tmgTHhigh );	/*set back the previous tming TH */
	ChipSetOneRegister(hChip,RSTV0903_TMGTHFALL,tmgTHLow);   /*set back the previous tming TH */
		
	return timingLock; 

}
#endif
BOOL FE_STV0903_CheckTimingLock_E(void)
{
	BOOL timingLock=FALSE;
	S32 i,
		timingcpt=0;
	U8 carFreq,
		tmgTHhigh,
		tmgTHLow;
		
	carFreq=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0,0);		
	tmgTHhigh= ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGTHRISE,0,0);	
	tmgTHLow= ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGTHFALL,0,0);  
	//
		
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGTHRISE,0x20 );	/*use higher timing lock TH */
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGTHFALL,0);		/*use higher timing lock TH */

	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_CFR_AUTOSCAN,0);		/* stop the carrier offset search */
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_RTC,0x80);		/*set the DVBS1 timing loop beta value to 0 (stop timing loop) */ 	 
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_RTCS2,0x40);	/*set the DVBS2 timing loop beta value to 0 (stop timing loop) */ 
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0x0);	/*Stop the carrier loop */
		
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT1,0x0);	/*set the carrier offset to 0*/ 
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT0,0x0); /*set the carrier offset to 0*/
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2REF,0x65);
	/*Trig an acquisition (start the search) only for timing check no need to white for lock*/
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x18);		 
	WAIT_N_MS(5);
	for(i=0;i<10;i++)
	{																//锁的状态
		if(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TMGLOCK_QUALITY)>=2)	/*read the timing lock quality*/
					timingcpt++;
		WAIT_N_MS(1);
	}
	
	if(timingcpt>=3)	/*timing locked when timing quality is higer than 2 for 3 times */
		timingLock =TRUE;
		
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2REF,0x38);
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_RTC,0x88);		/*enable the DVBS1 timing loop*/ 
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_RTCS2,0x68);	/*enable the DVBS2 timing loop*/
		
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,carFreq);	/*enable the carrier loop */
		
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGTHRISE,tmgTHhigh );	/*set back the previous tming TH */
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGTHFALL,tmgTHLow);   /*set back the previous tming TH */
		
	return timingLock; 

}

#if 0
BOOL FE_STV0903_GetDemodLock(STCHIP_Handle_t hChip,S32 TimeOut)
{
	S32 timer=0,
		lock=0;

	FE_STV0903_SEARCHSTATE_t	demodState;
#if 0
#ifdef HOST_PC 
	#ifndef NO_GUI
		double lockTime;
		char message[100];
		lockTime = Timer();
	
	#endif
#endif
#endif
	while((timer<TimeOut)&&(lock==0))
	{

		demodState=ChipGetField(hChip,FSTV0903_HEADER_MODE);
		switch(demodState)
		{
			case FE_SAT_SEARCH:
			case FE_SAT_PLH_DETECTED :	/* no signal*/ 
			default :
				lock = 0;			
			break;
			
			case FE_SAT_DVBS2_FOUND:	/*found a DVBS2 signal */ 
			case FE_SAT_DVBS_FOUND:
				lock = ChipGetField(hChip,FSTV0903_LOCK_DEFINITIF);		
			break;
		}
		
		if(lock==0)
			WAIT_N_MS(10);
		timer+=10;
	}
#if 0
#ifdef HOST_PC 
	#ifndef NO_GUI
		lockTime=Timer()-lockTime;
		Fmt(message,"Demod Lock Time = %f ms",lockTime*1000.0);
		if(lock)
			ReportInsertMessage("DEMOD LOCK OK");
		else
			ReportInsertMessage("DEMOD LOCK FAIL");

		ReportInsertMessage(message);
	#endif
#endif
	#endif	
	return lock;
}
#endif
BOOL FE_STV0903_GetDemodLock_E(S32 TimeOut)
{
	S32 timer=0,
		lock=0;

	FE_STV0903_SEARCHSTATE_t	demodState;
	U8 dat=0;
	while((timer<TimeOut)&&(lock==0))
	{
		demodState=ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_HEADER_MODE);
		dat=demodState;
//		display_hex(dat,4);
	//	while(1) {;}
		switch(demodState)
		{
			case FE_SAT_SEARCH://搜索状态  0
			case FE_SAT_PLH_DETECTED :	/* no signal*/ //01
			default :
				lock = 0;			
			break;
			
			case FE_SAT_DVBS2_FOUND:	/*found a DVBS2 signal *///10  搜索到的。。。
			case FE_SAT_DVBS_FOUND:// 11 
				lock = ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_LOCK_DEFINITIF);		
			break;
		}
		
		if(lock==0)
			WAIT_N_MS(10);
		timer+=10;
	}
	return lock;
}

BOOL GetFECLock_E(S32 TimeOut)
{
	S32 timer=0,
	lock=0;

	FE_STV0903_SEARCHSTATE_t	demodState;

		
	demodState=ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_HEADER_MODE);
	while((timer<TimeOut)&&(lock==0))
	{

		switch(demodState)
		{
			case FE_SAT_SEARCH:
			case FE_SAT_PLH_DETECTED :	/* no signal*/ 
			default :
				lock = 0;			
			break;
			
			case FE_SAT_DVBS2_FOUND:	/*found a DVBS2 signal */ 
				lock = ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_PKTDELIN_LOCK);
			break;
			
			case FE_SAT_DVBS_FOUND:
				lock = ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_LOCKEDVIT);		
			break;
		}
		
		if(lock==0)
		{
			
			WAIT_N_MS(10);
			timer+=10;
		}
	}

	return lock;
}
#if 0
BOOL GetFECLock(S32 TimeOut)
{
	S32 timer=0,
	lock=0;

	FE_STV0903_SEARCHSTATE_t	demodState;
#if 0
#ifdef HOST_PC 
	#ifndef NO_GUI
		double lockTime;
		char message[100];
		lockTime = Timer();
	
	#endif
#endif
#endif
		
	demodState=ChipGetField(hChip,FSTV0903_HEADER_MODE);
	while((timer<TimeOut)&&(lock==0))
	{

		switch(demodState)
		{
			case FE_SAT_SEARCH:
			case FE_SAT_PLH_DETECTED :	/* no signal*/ 
			default :
				lock = 0;			
			break;
			
			case FE_SAT_DVBS2_FOUND:	/*found a DVBS2 signal */ 
				lock = ChipGetField(hChip,FSTV0903_PKTDELIN_LOCK);
			break;
			
			case FE_SAT_DVBS_FOUND:
				lock = ChipGetField(hChip,FSTV0903_LOCKEDVIT);		
			break;
		}
		
		if(lock==0)
		{
			
			WAIT_N_MS(10);
			timer+=10;
		}
	}
#if 0	
#ifdef HOST_PC 
	#ifndef NO_GUI
		lockTime=Timer()-lockTime;
		Fmt(message,"FEC Lock Time = %f ms",lockTime*1000.0);
	
		if(lock)
			ReportInsertMessage("FEC LOCK OK");
		else
			ReportInsertMessage("FEC LOCK FAIL");

		ReportInsertMessage(message);
	#endif
#endif
#endif		
	return lock;
}
#endif
/*****************************************************
--FUNCTION	::	WaitForLock
--ACTION	::	Wait until Demod+ FEC locked or timout
--PARAMS IN	::	hChip	->	handle to the chip
			::	TimeOut	->	Time out in ms
--PARAMS OUT::	NONE
--RETURN	::	Lock status true or false
--***************************************************/
#if 0
BOOL FE_STV0903_WaitForLock(STCHIP_Handle_t hChip,S32 DemodTimeOut,S32 FecTimeOut)
{

	S32 timer=0,
		lock=0;
	

#if 0
#ifdef HOST_PC 
	#ifndef NO_GUI
		double lockTime;
		char message[100];
		lockTime = Timer();
	
	#endif
#endif
#endif
	
	
 
	lock=FE_STV0903_GetDemodLock(hChip,DemodTimeOut);
	if(lock)
		lock=lock && GetFECLock(hChip,FecTimeOut);
	
 	if(lock)
	{
		lock=0;
		while((timer<FecTimeOut) && (lock==0))
		{
			lock = ChipGetField(hChip,FSTV0903_TSFIFO_LINEOK); /*Check the stream merger Lock (good packet at the output)*/
			WAIT_N_MS(1);
			timer++;
		}
	}
#if 0
#ifdef HOST_PC 
	#ifndef NO_GUI
		lockTime=Timer()-lockTime;
		Fmt(message,"Total Lock Time = %f ms",lockTime*1000.0);
		if(lock)
			ReportInsertMessage("LOCK OK");
		else
			ReportInsertMessage("LOCK FAIL");
			
		ReportInsertMessage(message);
	
	#endif
#endif
#endif
	if(lock)
		return TRUE;
	else
		return FALSE;
}
#endif
BOOL FE_STV0903_WaitForLock_E(S32 DemodTimeOut,S32 FecTimeOut)
{

	S32 timer=0,
		lock=0;
		
	lock=FE_STV0903_GetDemodLock_E(DemodTimeOut);
	if(lock)
		lock=lock && GetFECLock_E(FecTimeOut);
	
 	if(lock)
	{
		lock=0;
		while((timer<FecTimeOut) && (lock==0))
		{
			lock = ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TSFIFO_LINEOK); /*Check the stream merger Lock (good packet at the output)*/
			WAIT_N_MS(1);
			timer++;
		}
	}

	if(lock)
		return TRUE;
	else
		return FALSE;
}
#if 0
void FE_STV0903_StopALL_S2_Modcod(STCHIP_Handle_t hChip)
{
	
	S32 i;

		for(i=0;i<16;i++)
			ChipSetOneRegister(hChip,RSTV0903_MODCODLST0+i,0xFF);
}
#endif
#if 0
void FE_STV0903_ActivateS2ModCode(STCHIP_Handle_t hChip)
{
	 S32 RegIndex;

	
		ChipSetOneRegister(hChip,RSTV0903_MODCODLST0,0xFF);
		ChipSetOneRegister(hChip,RSTV0903_MODCODLST1,0xFC);
		ChipSetOneRegister(hChip,RSTV0903_MODCODLSTF,0xCF); 
		for(RegIndex=0;RegIndex<13;RegIndex++)
			ChipSetOneRegister(hChip,RSTV0903_MODCODLST2+RegIndex,0xcc);
}
#endif
void FE_STV0903_ActivateS2ModCode_E(void)
{
	 S32 RegIndex;

	
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MODCODLST0,0xFF);
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MODCODLST1,0xFC);
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MODCODLSTF,0xCF); 
		for(RegIndex=0;RegIndex<13;RegIndex++)
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MODCODLST2+RegIndex,0xcc);
}

#if 0
void FE_STV0903_SetViterbiThAcq(STCHIP_Handle_t hChip)
{
	ChipSetOneRegister(hChip,RSTV0903_VTH12,0x96);	
	ChipSetOneRegister(hChip,RSTV0903_VTH23,0x64);
	ChipSetOneRegister(hChip,RSTV0903_VTH34,0x36);
	ChipSetOneRegister(hChip,RSTV0903_VTH56,0x23);
	ChipSetOneRegister(hChip,RSTV0903_VTH67,0x1E);
	ChipSetOneRegister(hChip,RSTV0903_VTH78,0x19);
}
#endif
void FE_STV0903_SetViterbiThAcq_E(void)
{
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VTH12,0x96);	
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VTH23,0x64);
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VTH34,0x36);
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VTH56,0x23);
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VTH67,0x1E);
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VTH78,0x19);
}

#if 0
void FE_STV0903_SetViterbiThTracq(STCHIP_Handle_t hChip)
{
	ChipSetOneRegister(hChip,RSTV0903_VTH12,0xd0);	
	ChipSetOneRegister(hChip,RSTV0903_VTH23,0x7D);
	ChipSetOneRegister(hChip,RSTV0903_VTH34,0x53);
	ChipSetOneRegister(hChip,RSTV0903_VTH56,0x2F);
	ChipSetOneRegister(hChip,RSTV0903_VTH67,0x24);
	ChipSetOneRegister(hChip,RSTV0903_VTH78,0x1F);
}
#endif
void FE_STV0903_SetViterbiThTracq_E(void)
{
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VTH12,0xd0);	
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VTH23,0x7D);
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VTH34,0x53);
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VTH56,0x2F);
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VTH67,0x24);
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VTH78,0x1F);
}
#if 0 
void FE_STV0903_SetViterbiStandard(STCHIP_Handle_t hChip,FE_STV0903_SearchStandard_t Standard,FE_STV0903_Rate_t PunctureRate)
{
	switch(Standard)
	{
		case FE_SAT_AUTO_SEARCH:
			ChipSetOneRegister(hChip,RSTV0903_FECM,0x00);	/* Enable only DVBS1 ,DSS seach mast be made on demande by giving Std DSS*/
			ChipSetOneRegister(hChip,RSTV0903_PRVIT,0x2F);  /* Enable all PR exept 6/7 */
		break;
		
		case FE_SAT_SEARCH_DVBS1:
			ChipSetOneRegister(hChip,RSTV0903_FECM,0x00);	/* Disable DSS */
			switch(PunctureRate)
			{
				case FE_SAT_PR_UNKNOWN:
				default:
					ChipSetOneRegister(hChip,RSTV0903_PRVIT,0x2F);   /* Enable All PR */
				break;
				
				case FE_SAT_PR_1_2:
					ChipSetOneRegister(hChip,RSTV0903_PRVIT,0x01);   /* Enable 1/2 PR only */
				break;
				
				case FE_SAT_PR_2_3:
					ChipSetOneRegister(hChip,RSTV0903_PRVIT,0x02);   /* Enable 2/3 PR only */
				break;
				
				case FE_SAT_PR_3_4:
					ChipSetOneRegister(hChip,RSTV0903_PRVIT,0x04);   /* Enable 3/4 PR only */
				break;
				
				case FE_SAT_PR_5_6:
					ChipSetOneRegister(hChip,RSTV0903_PRVIT,0x08);   /* Enable 5/6 PR only */
				break;
				
				case FE_SAT_PR_7_8:
					ChipSetOneRegister(hChip,RSTV0903_PRVIT,0x20);   /* Enable 7/8 PR only */
				break;
			}

		break;
		
		case FE_SAT_SEARCH_DSS:
			ChipSetOneRegister(hChip,RSTV0903_FECM,0x80);	/* Disable DVBS1 */
			switch(PunctureRate)
			{
				case FE_SAT_PR_UNKNOWN:
				default:
					ChipSetOneRegister(hChip,RSTV0903_PRVIT,0x13);   /* Enable 1/2, 2/3 and 6/7 PR */
				break;
				
				case FE_SAT_PR_1_2:
					ChipSetOneRegister(hChip,RSTV0903_PRVIT,0x01);   /* Enable 1/2 PR only */
				break;
				
				case FE_SAT_PR_2_3:
					ChipSetOneRegister(hChip,RSTV0903_PRVIT,0x02);   /* Enable 2/3 PR only */
				break;
				
				case FE_SAT_PR_6_7:
					ChipSetOneRegister(hChip,RSTV0903_PRVIT,0x10);   /* Enable 7/8 PR only */
				break;
			}

		break;
		
		default:
		break;
	}
}
#endif

void FE_STV0903_SetViterbiStandard_E(FE_STV0903_SearchStandard_t Standard,FE_STV0903_Rate_t PunctureRate)
{
	switch(Standard)
	{
		case FE_SAT_AUTO_SEARCH:
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_FECM,0x00);	/* Enable only DVBS1 ,DSS seach mast be made on demande by giving Std DSS*/
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PRVIT,0x2F);  /* Enable all PR exept 6/7 */
		break;
		
		case FE_SAT_SEARCH_DVBS1:
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_FECM,0x00);	/* Disable DSS */
			switch(PunctureRate)
			{
				case FE_SAT_PR_UNKNOWN:
				default:
					ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PRVIT,0x2F);   /* Enable All PR */
				break;
				
				case FE_SAT_PR_1_2:
					ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PRVIT,0x01);   /* Enable 1/2 PR only */
				break;
				
				case FE_SAT_PR_2_3:
					ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PRVIT,0x02);   /* Enable 2/3 PR only */
				break;
				
				case FE_SAT_PR_3_4:
					ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PRVIT,0x04);   /* Enable 3/4 PR only */
				break;
				
				case FE_SAT_PR_5_6:
					ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PRVIT,0x08);   /* Enable 5/6 PR only */
				break;
				
				case FE_SAT_PR_7_8:
					ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PRVIT,0x20);   /* Enable 7/8 PR only */
				break;
			}

		break;
		
		case FE_SAT_SEARCH_DSS:
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_FECM,0x80);	/* Disable DVBS1 */
			switch(PunctureRate)
			{
				case FE_SAT_PR_UNKNOWN:
				default:
					ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PRVIT,0x13);   /* Enable 1/2, 2/3 and 6/7 PR */
				break;
				
				case FE_SAT_PR_1_2:
					ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PRVIT,0x01);   /* Enable 1/2 PR only */
				break;
				
				case FE_SAT_PR_2_3:
					ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PRVIT,0x02);   /* Enable 2/3 PR only */
				break;
				
				case FE_SAT_PR_6_7:
					ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PRVIT,0x10);   /* Enable 7/8 PR only */
				break;
			}

		break;
		
		default:
		break;
	}
}
/*****************************************************
--FUNCTION	::	SetSearchStandard
--ACTION	::	Set the Search standard (Auto, DVBS2 only or DVBS1/DSS only)
--PARAMS IN	::	hChip	->	handle to the chip
			::	Standard	->	Search standard
--PARAMS OUT::	NONE
--RETURN	::	none
--***************************************************/
#if 1
void FE_STV0903_SetSearchStandard_E(FE_STV0903_SearchStandard_t DemodSearchStandard)
{
	switch(DemodSearchStandard)
	{
		case FE_SAT_SEARCH_DVBS1:
		case FE_SAT_SEARCH_DSS: 
			/*If DVBS1/DSS only disable DVBS2 search*/
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,1);
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,0);
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_FRESVIT1,0); /*Activate Viterbi decoder */ 
			
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ACLC,0x1a);	/*enable DVBS1/DSS carrier loop  */
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_BCLC,0x09);  /*enable DVBS1/DSS carrier loop  */
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CAR2CFG,0x22);	/* disable the DVBS2 carrier loop*/
			
			FE_STV0903_SetViterbiThAcq_E();
			FE_STV0903_SetViterbiStandard_E(FE_SAT_AUTO_SEARCH,FE_SAT_PR_UNKNOWN); 
		break;
				
		case FE_SAT_SEARCH_DVBS2:
			/*If DVBS2 only activate the DVBS2 and DVBS1 search and stop the VITERBI*/
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,0);
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,0);

			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,1);
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,1);
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_FRESVIT1,1); /*Stop the Viterbi decoder */ 
					
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ACLC,0);	/*stop DVBS1/DSS carrier loop  */
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_BCLC,0);  /*stop DVBS1/DSS carrier loop  */
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CAR2CFG,0x26);	/* enable the DVBS2 carrier loop*/
			/*Activate all S2 modcode for search*/
			FE_STV0903_ActivateS2ModCode_E();
					
		break;

		case FE_SAT_AUTO_SEARCH:
		default:
			/*If automatic first disable both std then enable both DVBS1/DSS and DVBS2 search*/ 
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,0);
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,0);

			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,1);
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,1);
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_FRESVIT1,0); /*Activate the Viterbi decoder */ 

				
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ACLC,0x1a);	/*enable DVBS1/DSS carrier loop  */
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_BCLC,0x09);  /*enable DVBS1/DSS carrier loop  */
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CAR2CFG,0x26);	/* enable the DVBS2 carrier loop*/
			/*Activate all S2 modcode for search*/
			FE_STV0903_ActivateS2ModCode_E();
			
			FE_STV0903_SetViterbiThAcq_E();
			FE_STV0903_SetViterbiStandard_E(FE_SAT_AUTO_SEARCH,FE_SAT_PR_UNKNOWN); 
					
		break;
	}
}
#endif
#if 0
void FE_STV0903_SetSearchStandard(FE_STV0903_InternalParams_t *pParams)
	switch(pParams->DemodSearchStandard)
	{
		case FE_SAT_SEARCH_DVBS1:
		case FE_SAT_SEARCH_DSS: 
			/*If DVBS1/DSS only disable DVBS2 search*/
			ChipSetField(pParams->hDemod,FSTV0903_DVBS1_ENABLE,1);
			ChipSetField(pParams->hDemod,FSTV0903_DVBS2_ENABLE,0);
			ChipSetField(pParams->hDemod,FSTV0903_FRESVIT1,0); /*Activate Viterbi decoder */ 
			
			ChipSetOneRegister(pParams->hDemod,RSTV0903_ACLC,0x1a);	/*enable DVBS1/DSS carrier loop  */
			ChipSetOneRegister(pParams->hDemod,RSTV0903_BCLC,0x09);  /*enable DVBS1/DSS carrier loop  */
			ChipSetOneRegister(pParams->hDemod,RSTV0903_CAR2CFG,0x22);	/* disable the DVBS2 carrier loop*/
			
			FE_STV0903_SetViterbiThAcq(pParams->hDemod);
			FE_STV0903_SetViterbiStandard(pParams->hDemod,pParams->DemodSearchStandard,pParams->DemodPunctureRate); 
		break;
				
		case FE_SAT_SEARCH_DVBS2:
			/*If DVBS2 only activate the DVBS2 and DVBS1 search and stop the VITERBI*/
			ChipSetField(pParams->hDemod,FSTV0903_DVBS1_ENABLE,0);
			ChipSetField(pParams->hDemod,FSTV0903_DVBS2_ENABLE,0);

			ChipSetField(pParams->hDemod,FSTV0903_DVBS1_ENABLE,1);
			ChipSetField(pParams->hDemod,FSTV0903_DVBS2_ENABLE,1);
			ChipSetField(pParams->hDemod,FSTV0903_FRESVIT1,1); /*Stop the Viterbi decoder */ 
					
			ChipSetOneRegister(pParams->hDemod,RSTV0903_ACLC,0);	/*stop DVBS1/DSS carrier loop  */
			ChipSetOneRegister(pParams->hDemod,RSTV0903_BCLC,0);  /*stop DVBS1/DSS carrier loop  */
			ChipSetOneRegister(pParams->hDemod,RSTV0903_CAR2CFG,0x26);	/* enable the DVBS2 carrier loop*/
			/*Activate all S2 modcode for search*/
			FE_STV0903_ActivateS2ModCode(pParams->hDemod);
					
		break;

		case FE_SAT_AUTO_SEARCH:
		default:
			/*If automatic first disable both std then enable both DVBS1/DSS and DVBS2 search*/ 
			ChipSetField(pParams->hDemod,FSTV0903_DVBS1_ENABLE,0);
			ChipSetField(pParams->hDemod,FSTV0903_DVBS2_ENABLE,0);

			ChipSetField(pParams->hDemod,FSTV0903_DVBS1_ENABLE,1);
			ChipSetField(pParams->hDemod,FSTV0903_DVBS2_ENABLE,1);
			ChipSetField(pParams->hDemod,FSTV0903_FRESVIT1,0); /*Activate the Viterbi decoder */ 

				
			ChipSetOneRegister(pParams->hDemod,RSTV0903_ACLC,0x1a);	/*enable DVBS1/DSS carrier loop  */
			ChipSetOneRegister(pParams->hDemod,RSTV0903_BCLC,0x09);  /*enable DVBS1/DSS carrier loop  */
			ChipSetOneRegister(pParams->hDemod,RSTV0903_CAR2CFG,0x26);	/* enable the DVBS2 carrier loop*/
			/*Activate all S2 modcode for search*/
			FE_STV0903_ActivateS2ModCode(pParams->hDemod);
			
			FE_STV0903_SetViterbiThAcq(pParams->hDemod);
			FE_STV0903_SetViterbiStandard(pParams->hDemod,pParams->DemodSearchStandard,pParams->DemodPunctureRate); 
					
		break;
	}
}
#endif
/*****************************************************
--FUNCTION	::	StartSearch
--ACTION	::	Trig the Demod to start a new search
--PARAMS IN	::	pParams	->	Pointer FE_STV0903_InternalParams_t structer
--PARAMS OUT::	NONE
--RETURN	::	none
--***************************************************/
#if 0
void FE_STV0903_StartSearch(FE_STV0903_InternalParams_t *pParams)
{
			
	
	ChipSetField(pParams->hDemod,FSTV0903_I2C_DEMOD_MODE,0x1F);	/*Reset the Demod*/
			
	if(pParams->hDemod->ChipID==0x10) 
	ChipSetOneRegister(pParams->hDemod,RSTV0903_CORRELEXP,0xAA);	/*Set Correlation frames reg for acquisition for cut 1.0 only*/ 
			
	if(pParams->hDemod->ChipID<0x20)
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CARHDR,0x55); /*Not applicable for cut 2.0*/
	if(pParams->DemodSymbolRate <=5000000)
	{																														
		
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CARCFG,0x44);
		/* If the symbol rate is <= 5Msps*/

		/*Set The carrier search up and low to manual mode [-8Mhz,+8Mhz] */ 
		/*CFR UP Setting*/
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRUP1,0x0F);
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRUP0,0xFF);
				
		/*CFR Low Setting*/
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRLOW1,0xF0);
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRLOW0,0x00);
		
		/*enlarge the timing bandwith for Low SR*/
		ChipSetOneRegister(pParams->hDemod,RSTV0903_RTCS2,0x68);
	}
	else
	{
		/* If the symbol rate is >5 Msps
		Set The carrier search up and low to auto mode */
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CARCFG,0xC4);
		/*reduce the timing bandwith for high SR*/
		ChipSetOneRegister(pParams->hDemod,RSTV0903_RTCS2,0x44);
	}
			  
	ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRINIT1,0);		/*init the demod frequency offset to 0 */
	ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRINIT0,0);
			
	if(pParams->hDemod->ChipID>=0x20) /*enable the equalizer for search */
	{
		ChipSetOneRegister(pParams->hDemod,RSTV0903_EQUALCFG,0x41);	
		ChipSetOneRegister(pParams->hDemod,RSTV0903_FFECFG,0x41);
		
		if( (pParams->DemodSearchStandard == FE_SAT_SEARCH_DVBS1) || (pParams->DemodSearchStandard == FE_SAT_SEARCH_DSS) || (pParams->DemodSearchStandard ==FE_SAT_AUTO_SEARCH)) 
		{
			ChipSetOneRegister(pParams->hDemod,RSTV0903_VITSCALE,0x82);	/* open the ReedSolomon to viterbi feed back until demod lock*/
			ChipSetOneRegister(pParams->hDemod,RSTV0903_VAVSRVIT,0x0);	/* set Viterbi hysteresis for search*/
		}
	}
	
	ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRSTEP,0x00) ;		/* Slow down the timing loop*/
	ChipSetOneRegister(pParams->hDemod,RSTV0903_TMGTHRISE,0xE0) ;	/* timing stting for warm/cold start*/
	ChipSetOneRegister(pParams->hDemod,RSTV0903_TMGTHFALL,0xC0) ;	/* timing stting for warm/cold start*/
	ChipSetField(pParams->hDemod,FSTV0903_CFR_AUTOSCAN,0);			/* stop the carrier frequency search loop*/
	ChipSetField(pParams->hDemod,FSTV0903_SCAN_ENABLE,0);			/* stop the carrier frequency search loop*/
	ChipSetField(pParams->hDemod,FSTV0903_S1S2_SEQUENTIAL,0);		/* S1 and  S2 search in sequential if both enabled*/
			
	if(pParams->hDemod->ChipID>=0x20)	/*Frequency offset detector setting*/
	{
		if(pParams->DemodSymbolRate <10000000) 
			ChipSetOneRegister(pParams->hDemod,RSTV0903_CARFREQ,0x4C);
		else
			ChipSetOneRegister(pParams->hDemod,RSTV0903_CARFREQ,0x4B);
	}
	else
	{
		if(pParams->DemodSymbolRate <10000000) 
			ChipSetOneRegister(pParams->hDemod,RSTV0903_CARFREQ,0xef);
		else
			ChipSetOneRegister(pParams->hDemod,RSTV0903_CARFREQ,0xed);
	}
	switch(pParams->DemodSearchAlgo)
	{
		case FE_SAT_WARM_START:/*The symbol rate and the exact carrier Frequency are known */
			
			ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDISTATE,0x1F);		/*Reset the Demod*/
			ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDISTATE,0x18);		/*Trig an acquisition (start the search)*/
		break;
				
		case FE_SAT_COLD_START:/*The symbol rate is known*/

			ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDISTATE,0x1F);		/*Reset the Demod*/
			ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDISTATE,0x15);		/*Trig an acquisition (start the search)*/
		break;
		
		default:
			/*Nothing to do in case of blind search, blind  is handled by "FE_STV0903_BlindSearchAlgo" function */
		break;
	}
}
#endif
void FE_STV0903_StartSearch_E(S32 DemodSymbolRate,FE_STV0903_SearchAlgo_t DemodSearchAlgo,FE_STV0903_SearchStandard_t	DemodSearchStandard)
{
	U8 	ChipID;
	ChipID=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MID,0,0);
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_I2C_DEMOD_MODE,0x1F);	/*Reset the Demod*/
			
	if(ChipID==0x10) 
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELEXP,0xAA);	/*Set Correlation frames reg for acquisition for cut 1.0 only*/ 
			
	if(ChipID<0x20)
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARHDR,0x55); /*Not applicable for cut 2.0*/
	if(DemodSymbolRate <=5000000)
	{																														
		
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARCFG,0x44);
		/* If the symbol rate is <= 5Msps*/

		/*Set The carrier search up and low to manual mode [-8Mhz,+8Mhz] */ 
		/*CFR UP Setting*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRUP1,0x0F);
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRUP0,0xFF);
				
		/*CFR Low Setting*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRLOW1,0xF0);
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRLOW0,0x00);
		
		/*enlarge the timing bandwith for Low SR*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_RTCS2,0x68);
	}
	else
	{
		/* If the symbol rate is >5 Msps
		Set The carrier search up and low to auto mode */
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARCFG,0xC4);
		/*reduce the timing bandwith for high SR*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_RTCS2,0x44);
	}
			  
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT1,0);		/*init the demod frequency offset to 0 */
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT0,0);
			
	if(ChipID>=0x20) /*enable the equalizer for search */
	{
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_EQUALCFG,0x41);	
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_FFECFG,0x41);
		
		if( (DemodSearchStandard == FE_SAT_SEARCH_DVBS1) || (DemodSearchStandard == FE_SAT_SEARCH_DSS) || (DemodSearchStandard ==FE_SAT_AUTO_SEARCH)) 
		{
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VITSCALE,0x82);	/* open the ReedSolomon to viterbi feed back until demod lock*/
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VAVSRVIT,0x0);	/* set Viterbi hysteresis for search*/
		}
	}
	
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRSTEP,0x00) ;		/* Slow down the timing loop*/
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGTHRISE,0xE0) ;	/* timing stting for warm/cold start*/
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGTHFALL,0xC0) ;	/* timing stting for warm/cold start*/
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_CFR_AUTOSCAN,0);			/* stop the carrier frequency search loop*/
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SCAN_ENABLE,0);			/* stop the carrier frequency search loop*/
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_S1S2_SEQUENTIAL,0);		/* S1 and  S2 search in sequential if both enabled*/
			
	if(ChipID>=0x20)	/*Frequency offset detector setting*/
	{
		if(DemodSymbolRate <10000000) 
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0x4C);
		else
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0x4B);
	}
	else
	{
		if(DemodSymbolRate <10000000) 
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0xef);
		else
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0xed);
	}
	switch(DemodSearchAlgo)
	{
		case FE_SAT_WARM_START:/*The symbol rate and the exact carrier Frequency are known */
			
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1F);		/*Reset the Demod*/
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x18);		/*Trig an acquisition (start the search)*/
		break;
				
		case FE_SAT_COLD_START:/*The symbol rate is known*/

			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1F);		/*Reset the Demod*/
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x15);		/*Trig an acquisition (start the search)*/
		break;
		
		default:
			/*Nothing to do in case of blind search, blind  is handled by "FE_STV0903_BlindSearchAlgo" function */
		break;
	}
}
U8 FE_STV0903_GetOptimCarrierLoop(S32 SymbolRate,FE_STV0903_ModCod_t ModCode,S32 Pilots,U8 ChipID)
{
	U8 aclcValue=0x29;
	S32	i;
	FE_STV0903_CarLoopOPtim *carLoopS2;
	FE_STV0903_CarLoopOPtim	FE_STV0903_S2CarLoop[14]	={  /*Modcod		2MPon 		2MPoff		5MPon 		5MPoff		10MPon 		10MPoff		20MPon 		20MPoff		30MPon 		30MPoff	*/
													{FE_SAT_QPSK_12,	0x1C,		0x0D,		0x1B,		0x2C,		0x3A,		0x1C,		0x2A,		0x3B,		0x2A,		0x1B},
													{FE_SAT_QPSK_35,	0x2C,		0x0D,		0x2B,		0x2C,		0x3A,		0x0C,		0x3A,		0x2B,		0x2A,		0x0B},
													{FE_SAT_QPSK_23,	0x2C,		0x0D,		0x2B,		0x2C,		0x0B,		0x0C,		0x3A,		0x1B,		0x2A,		0x3A},
													{FE_SAT_QPSK_34,	0x3C,		0x0D,		0x3B,		0x1C,		0x0B,		0x3B,		0x3A,		0x0B,		0x2A,		0x3A},
													{FE_SAT_QPSK_45,	0x3C,		0x0D,		0x3B,		0x1C,		0x0B,		0x3B,		0x3A,		0x0B,		0x2A,		0x3A},
													{FE_SAT_QPSK_56,	0x0D,		0x0D,		0x3B,		0x1C,		0x0B,		0x3B,		0x3A,		0x0B,		0x2A,		0x3A},
													{FE_SAT_QPSK_89,	0x0D,		0x0D,		0x3B,		0x1C,		0x1B,		0x3B,		0x3A,		0x0B,		0x2A,		0x3A},
													{FE_SAT_QPSK_910,	0x1D,		0x0D,		0x3B,		0x1C,		0x1B,		0x3B,		0x3A,		0x0B,		0x2A,		0x3A},
													{FE_SAT_8PSK_35,	0x29,		0x3B,		0x09,		0x2B,		0x38,		0x0B,		0x18,		0x1A,		0x08,		0x0A},
													{FE_SAT_8PSK_23,	0x0A,		0x3B,		0x29,		0x2B,		0x19,		0x0B,		0x38,		0x1A,		0x18,		0x0A},
													{FE_SAT_8PSK_34,	0x3A,		0x3B,		0x2A,		0x2B,		0x39,		0x0B,		0x19,		0x1A,		0x38,		0x0A},
													{FE_SAT_8PSK_56,	0x1B,		0x3B,		0x0B,		0x2B,		0x1A,		0x0B,		0x39,		0x1A,		0x19,		0x0A},
													{FE_SAT_8PSK_89,	0x3B,		0x3B,		0x0B,		0x2B,		0x2A,		0x0B,		0x39,		0x1A,		0x29,		0x39},
													{FE_SAT_8PSK_910,	0x3B,		0x3B, 		0x0B,		0x2B, 		0x2A,		0x0B,		0x39,		0x1A,		0x29,		0x39},
												};
       FE_STV0903_CarLoopOPtim	FE_STV0903_S2CarLoopCut20[14]	=
												{  /*Modcod		2MPon 		2MPoff		5MPon 		5MPoff		10MPon 		10MPoff		20MPon 		20MPoff		30MPon 		30MPoff	*/
													{FE_SAT_QPSK_12,	0x1F,		0x3F,		0x1E,		0x3F,		0x3D,		0x1F,		0x3D,		0x3E,		0x3D,		0x1E},
													{FE_SAT_QPSK_35,	0x2F,		0x3F,		0x2E,		0x2F,		0x3D,		0x0F,		0x0E,		0x2E,		0x3D,		0x0E},
													{FE_SAT_QPSK_23,	0x2F,		0x3F,		0x2E,		0x2F,		0x0E,		0x0F,		0x0E,		0x1E,		0x3D,		0x3D},
													{FE_SAT_QPSK_34,	0x3F,		0x3F,		0x3E,		0x1F,		0x0E,		0x3E,		0x0E,		0x1E,		0x3D,		0x3D},
													{FE_SAT_QPSK_45,	0x3F,		0x3F,		0x3E,		0x1F,		0x0E,		0x3E,		0x0E,		0x1E,		0x3D,		0x3D},
													{FE_SAT_QPSK_56,	0x3F,		0x3F,		0x3E,		0x1F,		0x0E,		0x3E,		0x0E,		0x1E,		0x3D,		0x3D},
													{FE_SAT_QPSK_89,	0x3F,		0x3F,		0x3E,		0x1F,		0x1E,		0x3E,		0x0E,		0x1E,		0x3D,		0x3D},
													{FE_SAT_QPSK_910,	0x3F,		0x3F,		0x3E,		0x1F,		0x1E,		0x3E,		0x0E,		0x1E,		0x3D,		0x3D},
													{FE_SAT_8PSK_35,	0x3c,		0x3e,		0x1c,		0x2e,		0x0c,		0x1e,		0x2b,		0x2d,		0x1b,		0x1d},
													{FE_SAT_8PSK_23,	0x1d,		0x3e,		0x3c,		0x2e,		0x2c,		0x1e,		0x0c,		0x2d,		0x2b,		0x1d},
													{FE_SAT_8PSK_34,	0x0e,		0x3e,		0x3d,		0x2e,		0x0d,		0x1e,		0x2c,		0x2d,		0x0c,		0x1d},
													{FE_SAT_8PSK_56,	0x2e,		0x3e,		0x1e,		0x2e,		0x2d,		0x1e,		0x3c,		0x2d,		0x2c,		0x1d},
													{FE_SAT_8PSK_89,	0x3e,		0x3e,		0x1e,		0x2e,		0x3d,		0x1e,		0x0d,		0x2d,		0x3c,		0x1d},
													{FE_SAT_8PSK_910,	0x3e,		0x3e, 		0x1e,		0x2e, 		0x3d,		0x1e,		0x1d,		0x2d,		0x0d,		0x1d},
												};
	if(ChipID <= 0x12)
		carLoopS2=FE_STV0903_S2CarLoop;
	else if(ChipID==0x20)
		carLoopS2=FE_STV0903_S2CarLoopCut20;
	else
		carLoopS2=FE_STV0903_S2CarLoop;

	if(ModCode<FE_SAT_QPSK_12)
		i=0;
	else
	{
		i=0;
		while((i<14)&& (ModCode!=carLoopS2[i].ModCode))i++;
	
		if(i>=14)
			i=13;
	}

	if(Pilots)
	{
		if(SymbolRate<=3000000)
			aclcValue=carLoopS2[i].CarLoopPilotsOn_2;
		else if(SymbolRate<=7000000)
			aclcValue=carLoopS2[i].CarLoopPilotsOn_5;
		else if(SymbolRate<=15000000)
			aclcValue=carLoopS2[i].CarLoopPilotsOn_10;
		else if(SymbolRate<=25000000)
			aclcValue=carLoopS2[i].CarLoopPilotsOn_20; 
		else
			aclcValue=carLoopS2[i].CarLoopPilotsOn_30; 
	}
	else
	{
		if(SymbolRate<=3000000)
			aclcValue=carLoopS2[i].CarLoopPilotsOff_2;
		else if(SymbolRate<=7000000)
			aclcValue=carLoopS2[i].CarLoopPilotsOff_5;
		else if(SymbolRate<=15000000)
			aclcValue=carLoopS2[i].CarLoopPilotsOff_10;
		else if(SymbolRate<=25000000)
			aclcValue=carLoopS2[i].CarLoopPilotsOff_20; 
		else
			aclcValue=carLoopS2[i].CarLoopPilotsOff_30; 
	}

	return aclcValue;
}

/*****************************************************
--FUNCTION	::	TrackingOptimization
--ACTION	::	Set Optimized parameters for tracking
--PARAMS IN	::	pParams	->	Pointer FE_STV0903_InternalParams_t structer
--PARAMS OUT::	NONE
--RETURN	::	none
--***************************************************/
#if 0
void FE_STV0903_TrackingOptimization(FE_STV0903_InternalParams_t *pParams)
{
	S32 symbolRate,
		pilots,
		aclc,
		freq1,
		freq0,
		i=0,
		timed,timef,blindTunSw=0;
	
	FE_STV0903_RollOff_t rolloff;
	FE_STV0903_ModCod_t foundModcod;
	
	
	symbolRate=FE_STV0903_GetSymbolRate(pParams->hDemod,pParams->MasterClock);		/*	Read the Symbol rate	*/          
	symbolRate += FE_STV0903_TimigGetOffset(pParams->hDemod,symbolRate);
	switch(pParams->DemodResults.Standard)
	{
		case FE_SAT_DVBS1_STANDARD:
			if( pParams->DemodSearchStandard == FE_SAT_AUTO_SEARCH)
			{
				ChipSetField(pParams->hDemod,FSTV0903_DVBS1_ENABLE,1);
				ChipSetField(pParams->hDemod,FSTV0903_DVBS2_ENABLE,0);
			}
			/*Set the rolloff to the manual value (given at the initialization)*/
			ChipSetField(pParams->hDemod,FSTV0903_ROLLOFF_CONTROL,pParams->RollOff);
			ChipSetField(pParams->hDemod,FSTV0903_MANUAL_ROLLOFF,1);
			
			ChipSetOneRegister(pParams->hDemod,RSTV0903_ERRCTRL1,0x75);	/* force to viterbi bit error */ 
		break;
		
		case FE_SAT_DSS_STANDARD:
			if( pParams->DemodSearchStandard == FE_SAT_AUTO_SEARCH) 
			{
				ChipSetField(pParams->hDemod,FSTV0903_DVBS1_ENABLE,1);
				ChipSetField(pParams->hDemod,FSTV0903_DVBS2_ENABLE,0);
			}
			/*Set the rolloff to the manual value (given at the initialization)*/
			ChipSetField(pParams->hDemod,FSTV0903_ROLLOFF_CONTROL,pParams->RollOff); 
			ChipSetField(pParams->hDemod,FSTV0903_MANUAL_ROLLOFF,1);
			
			ChipSetOneRegister(pParams->hDemod,RSTV0903_ERRCTRL1,0x75);	/* force to viterbi bit error */ 
		break;
		
		case FE_SAT_DVBS2_STANDARD:
			ChipSetOneRegister(pParams->hDemod,RSTV0903_ACLC,0);	/*stop DVBS1/DSS carrier loop  */
			ChipSetOneRegister(pParams->hDemod,RSTV0903_BCLC,0);  /*stop DVBS1/DSS carrier loop  */

			ChipSetField(pParams->hDemod,FSTV0903_DVBS1_ENABLE,0);
			ChipSetField(pParams->hDemod,FSTV0903_DVBS2_ENABLE,1);

			/*Carrier loop setting*/
			foundModcod=ChipGetField(pParams->hDemod,FSTV0903_PLH_MODCOD);
			pilots=ChipGetField(pParams->hDemod,FSTV0903_PLH_TYPE) & 0x01;
			
			aclc= FE_STV0903_GetOptimCarrierLoop(symbolRate,foundModcod,pilots,pParams->hDemod->ChipID);
			if(foundModcod<=FE_SAT_QPSK_910)
				ChipSetOneRegister(pParams->hDemod,RSTV0903_ACLC2S2Q,aclc);
			else if (foundModcod<=FE_SAT_8PSK_910)
			{
				ChipSetOneRegister(pParams->hDemod,RSTV0903_ACLC2S2Q,0x2a);/*stop the carrier update for QPSK*/
				ChipSetOneRegister(pParams->hDemod,RSTV0903_ACLC2S28,aclc);
			}
				
			if(pParams->hDemod->ChipID<=0x11)
			{
				ChipSetField(pParams->hDemod,FSTV0903_FRESVIT1,1); /*Stop Viterbi decoder if DVBS2 found*/
			}
			ChipSetOneRegister(pParams->hDemod,RSTV0903_ERRCTRL1,0x67);	/* error counter to  DVBS2 packet error rate */ 
		break;

		case FE_SAT_UNKNOWN_STANDARD:
		default:
			ChipSetField(pParams->hDemod,FSTV0903_DVBS1_ENABLE,1);
			ChipSetField(pParams->hDemod,FSTV0903_DVBS2_ENABLE,1);
		break;
	}
	
	freq1=ChipGetOneRegister(pParams->hDemod,RSTV0903_CFR2);				/*read the carrier frequency*/
	freq0=ChipGetOneRegister(pParams->hDemod,RSTV0903_CFR1);
	rolloff=ChipGetField(pParams->hDemod,FSTV0903_ROLLOFF_STATUS);			/*	Read the rolloff		*/

	if(pParams->DemodSearchAlgo==FE_SAT_BLIND_SEARCH) 
	{
		ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRSTEP,0x00);
		ChipSetField(pParams->hDemod,FSTV0903_SCAN_ENABLE,0);	/* stop the SR search*/
		ChipSetField(pParams->hDemod,FSTV0903_CFR_AUTOSCAN,0);	/*stop the carrier frequency search loop*/   
		ChipSetOneRegister(pParams->hDemod,RSTV0903_TMGCFG2,0x01);
		FE_STV0903_SetSymbolRate(pParams->hDemod,pParams->MasterClock,symbolRate);	/*Set the init symbol rate to the found value */
		FE_STV0903_SetMaxSymbolRate(pParams->hDemod,pParams->MasterClock,symbolRate);
		FE_STV0903_SetMinSymbolRate(pParams->hDemod,pParams->MasterClock,symbolRate);
		blindTunSw=1;
	}
			
	
	if(pParams->hDemod->ChipID>=0x20)
	{
		if( (pParams->DemodSearchStandard == FE_SAT_SEARCH_DVBS1) || (pParams->DemodSearchStandard == FE_SAT_SEARCH_DSS) || (pParams->DemodSearchStandard ==FE_SAT_AUTO_SEARCH)) 
		{
			ChipSetOneRegister(pParams->hDemod,RSTV0903_VAVSRVIT,0x0a);	/*set Viterbi hysteresis for tracking*/
			ChipSetOneRegister(pParams->hDemod,RSTV0903_VITSCALE,0x0);	/* close the ReedSolomon to viterbi feed back until demod lock*/
		}
	}
	
	if(pParams->hDemod->ChipID<0x20)
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CARHDR,0x08); /*Not applicable for cut 2.0 */
	
	if(pParams->hDemod->ChipID==0x10) 
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CORRELEXP,0x0A);	/*Set Correlation frames reg for Tracking for cut 1.0 only*/

	ChipSetOneRegister(pParams->hDemod,RSTV0903_AGC2REF,0x38); /*Set the AGC2 ref to the tracking value*/

	if((pParams->hDemod->ChipID>=0x20)||(blindTunSw==1)||(pParams->DemodSymbolRate<10000000)) 
	{
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRINIT1,freq1);				/*update the init carrier freq with the found freq offset*/
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRINIT0,freq0);
		pParams->TunerBW=FE_STV0903_CarrierWidth(symbolRate,pParams->RollOff) + 10000000;
		
		if((pParams->hDemod->ChipID>=0x20) || (blindTunSw==1))
		{   
			if(pParams->DemodSearchAlgo!=FE_SAT_WARM_START)
			{
				if(pParams->TunerType==FE_SAT_SW_TUNER)
					FE_Sat_TunerSetBandwidth(pParams->hTuner,pParams->TunerBW); 
				else
					FE_STV0903_SetTuner(pParams->hDemod,pParams->hTuner,pParams->TunerType,pParams->TunerFrequency,pParams->TunerBW);  /*Update the Tuner BW*/
			} 
		}
		if((pParams->DemodSearchAlgo==FE_SAT_BLIND_SEARCH) || (pParams->DemodResults.SymbolRate < 10000000))
			WAIT_N_MS(50);   /*if blind search wait 50ms for SR stabilization*/ 
		else
		   WAIT_N_MS(5);
		   
		FE_STV0903_GetLockTimeout(&timed,&timef,symbolRate,FE_SAT_WARM_START);
		
		if(FE_STV0903_GetDemodLock(pParams->hDemod,timed/2)==FALSE)
		{
			ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDISTATE,0x1F);		/*stop the demod*/
			ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRINIT1,freq1);		/*init the frequency offset*/
			ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRINIT0,freq0);
			ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDISTATE,0x18);		/*stop the demod*/
			i=0;
			while((FE_STV0903_GetDemodLock(pParams->hDemod,timed/2)==FALSE) && (i <=2) )  /*if the demod lose lock because of tuner BW ref change*/
			{
				ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDISTATE,0x1F);		/*stop the demod*/
				ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRINIT1,freq1);		/*init the frequency offset*/
				ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRINIT0,freq0);
				
				ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDISTATE,0x18);		/*stop the demod*/
				i++;
			}
		}
	}
	
	if(pParams->hDemod->ChipID>=0x20)
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CARFREQ,0x49);

	if((pParams->DemodResults.Standard==FE_SAT_DVBS1_STANDARD) || (pParams->DemodResults.Standard==FE_SAT_DSS_STANDARD)) 
		FE_STV0903_SetViterbiThTracq(pParams->hDemod);

}
#endif
void FE_STV0903_TrackingOptimization_E(FE_STV0903_InternalParams_t_E*pParams)
{
	S32 symbolRate,
		pilots,
		aclc,
		freq1,
		freq0,
		i=0,
		timed,timef,blindTunSw=0;
	
	FE_STV0903_RollOff_t rolloff;
	FE_STV0903_ModCod_t foundModcod;
	U8 	ChipID;
	ChipID=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MID,0,0);
	
	symbolRate=FE_STV0903_GetSymbolRate_E(pParams->MasterClock);		/*	Read the Symbol rate	*/          
	symbolRate += FE_STV0903_TimigGetOffset_E(symbolRate);
	switch(pParams->DemodResults.Standard)
	{
		case FE_SAT_DVBS1_STANDARD:
			if( pParams->DemodSearchStandard == FE_SAT_AUTO_SEARCH)
			{
				ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,1);
				ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,0);
			}
			/*Set the rolloff to the manual value (given at the initialization)*/
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ROLLOFF_CONTROL,pParams->RollOff);
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_MANUAL_ROLLOFF,1);
			
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ERRCTRL1,0x75);	/* force to viterbi bit error */ 
		break;
		
		case FE_SAT_DSS_STANDARD:
			if( pParams->DemodSearchStandard == FE_SAT_AUTO_SEARCH) 
			{
				ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,1);
				ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,0);
			}
			/*Set the rolloff to the manual value (given at the initialization)*/
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ROLLOFF_CONTROL,pParams->RollOff); 
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_MANUAL_ROLLOFF,1);
			
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ERRCTRL1,0x75);	/* force to viterbi bit error */ 
		break;
		
		case FE_SAT_DVBS2_STANDARD:
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ACLC,0);	/*stop DVBS1/DSS carrier loop  */
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_BCLC,0);  /*stop DVBS1/DSS carrier loop  */

			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,0);
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,1);

			/*Carrier loop setting*/
			foundModcod=ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_PLH_MODCOD);
			pilots=ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_PLH_TYPE) & 0x01;
			
			aclc= FE_STV0903_GetOptimCarrierLoop(symbolRate,foundModcod,pilots,ChipID);
			if(foundModcod<=FE_SAT_QPSK_910)
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ACLC2S2Q,aclc);
			else if (foundModcod<=FE_SAT_8PSK_910)
			{
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ACLC2S2Q,0x2a);/*stop the carrier update for QPSK*/
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ACLC2S28,aclc);
			}
				
			if(ChipID<=0x11)
			{
				ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_FRESVIT1,1); /*Stop Viterbi decoder if DVBS2 found*/
			}
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ERRCTRL1,0x67);	/* error counter to  DVBS2 packet error rate */ 
		break;

		case FE_SAT_UNKNOWN_STANDARD:
		default:
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,1);
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,1);
		break;
	}
	
	freq1=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFR2,0,0);				/*read the carrier frequency*/
	freq0=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFR1,0,0);
	rolloff=ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ROLLOFF_STATUS);			/*	Read the rolloff		*/

	if(pParams->DemodSearchAlgo==FE_SAT_BLIND_SEARCH) 
	{
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRSTEP,0x00);
		ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SCAN_ENABLE,0);	/* stop the SR search*/
		ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_CFR_AUTOSCAN,0);	/*stop the carrier frequency search loop*/   
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGCFG2,0x01);
		FE_STV0903_SetSymbolRate_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,pParams->MasterClock,symbolRate);	/*Set the init symbol rate to the found value */
		FE_STV0903_SetMaxSymbolRate_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,pParams->MasterClock,symbolRate);
		FE_STV0903_SetMinSymbolRate_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,pParams->MasterClock,symbolRate);
		blindTunSw=1;
	}
			
	
	if(ChipID>=0x20)
	{
		if( (pParams->DemodSearchStandard == FE_SAT_SEARCH_DVBS1) || (pParams->DemodSearchStandard == FE_SAT_SEARCH_DSS) || (pParams->DemodSearchStandard ==FE_SAT_AUTO_SEARCH)) 
		{
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VAVSRVIT,0x0a);	/*set Viterbi hysteresis for tracking*/
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VITSCALE,0x0);	/* close the ReedSolomon to viterbi feed back until demod lock*/
		}
	}
	
	if(ChipID<0x20)
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARHDR,0x08); /*Not applicable for cut 2.0 */
	
	if(ChipID==0x10) 
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELEXP,0x0A);	/*Set Correlation frames reg for Tracking for cut 1.0 only*/

	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2REF,0x38); /*Set the AGC2 ref to the tracking value*/

	if((ChipID>=0x20)||(blindTunSw==1)||(pParams->DemodSymbolRate<10000000)) 
	{
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT1,freq1);				/*update the init carrier freq with the found freq offset*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT0,freq0);
		pParams->TunerBW=FE_STV0903_CarrierWidth(symbolRate,pParams->RollOff) + 10000000;
		
		if((ChipID>=0x20) || (blindTunSw==1))
		{   
			if(pParams->DemodSearchAlgo!=FE_SAT_WARM_START)
			{
				if(pParams->TunerType==FE_SAT_SW_TUNER)
					FE_Sat_TunerSetBandwidth_E(pParams->TunerBW); 
				else
				FE_STV0903_SetTuner_E(FE_SAT_SW_TUNER,pParams->TunerReference,pParams->TunerFrequency,pParams->TunerBW); 
			} 
		}
		if((pParams->DemodSearchAlgo==FE_SAT_BLIND_SEARCH) || (pParams->DemodResults.SymbolRate < 10000000))
			WAIT_N_MS(10);   /*if blind search wait 50ms for SR stabilization*/ 
		else
		   WAIT_N_MS(2);
		   
		FE_STV0903_GetLockTimeout(&timed,&timef,symbolRate,FE_SAT_WARM_START);
		
		if(FE_STV0903_GetDemodLock_E(timed/2)==FALSE)
		{
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1F);		/*stop the demod*/
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT1,freq1);		/*init the frequency offset*/
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT0,freq0);
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x18);		/*stop the demod*/
			i=0;
			while((FE_STV0903_GetDemodLock_E(timed/2)==FALSE) && (i <=2) )  /*if the demod lose lock because of tuner BW ref change*/
			{
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1F);		/*stop the demod*/
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT1,freq1);		/*init the frequency offset*/
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT0,freq0);
				
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x18);		/*stop the demod*/
				i++;
			}
		}
	}
	
	if(ChipID>=0x20)
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0x49);

	if((pParams->DemodResults.Standard==FE_SAT_DVBS1_STANDARD) || (pParams->DemodResults.Standard==FE_SAT_DSS_STANDARD)) 
		FE_STV0903_SetViterbiThTracq_E();

}

/*****************************************************
--FUNCTION	::	GetSignalParams
--ACTION	::	Read signal caracteristics
--PARAMS IN	::	pParams	->	Pointer FE_STV0903_InternalParams_t structer
--PARAMS OUT::	NONE
--RETURN	::	RANGE Ok or not 
--***************************************************/
#if 0
FE_STV0903_SIGNALTYPE_t FE_STV0903_GetSignalParams(FE_STV0903_InternalParams_t *pParams) 
{
	FE_STV0903_SIGNALTYPE_t	range=FE_SAT_OUTOFRANGE;
	S32 offsetFreq,
		symbolRateOffset,
		i=0;
	
	U8 timing;
	
	WAIT_N_MS(5);
	if(pParams->DemodSearchAlgo==FE_SAT_BLIND_SEARCH)
	{
		/*	if Blind search wait for symbol rate offset information transfert from the timing loop to the
			demodulator symbol rate
		*/
		timing=ChipGetOneRegister(pParams->hDemod,RSTV0903_TMGREG2);
		i=0;
		ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRSTEP,0x5c);
		while((i<=50) && (timing!=0) && (timing!=0xFF))
		{
			timing=ChipGetOneRegister(pParams->hDemod,RSTV0903_TMGREG2);
			WAIT_N_MS(5);
			i+=5;
		}
	}
	
	pParams->DemodResults.Standard		=	FE_STV0903_GetStandard(pParams->hDemod);
	
	pParams->DemodResults.Frequency	=	FE_STV0903_GetTunerFrequency(pParams->hDemod,pParams->hTuner,pParams->TunerType);			
	offsetFreq=	FE_STV0903_GetCarrierFrequency(pParams->hDemod,pParams->MasterClock)/1000; 
	pParams->DemodResults.Frequency	+= offsetFreq;
	
	pParams->DemodResults.SymbolRate	=	FE_STV0903_GetSymbolRate(pParams->hDemod,pParams->MasterClock);
	symbolRateOffset=FE_STV0903_TimigGetOffset(pParams->hDemod,pParams->DemodResults.SymbolRate);
	pParams->DemodResults.SymbolRate	+=	symbolRateOffset;
	pParams->DemodResults.PunctureRate	=	FE_STV0903_GetPunctureRate(pParams->hDemod); 
	pParams->DemodResults.ModCode		=	ChipGetField(pParams->hDemod,FSTV0903_PLH_MODCOD);
	pParams->DemodResults.Pilots		=	ChipGetField(pParams->hDemod,FSTV0903_PLH_TYPE) & 0x01;
	pParams->DemodResults.FrameLength	=	ChipGetField(pParams->hDemod,FSTV0903_PLH_TYPE)>>1;
	
	pParams->DemodResults.RollOff		=	ChipGetField(pParams->hDemod,FSTV0903_ROLLOFF_STATUS);
	
	switch(pParams->DemodResults.Standard)
	{
		case FE_SAT_DVBS2_STANDARD:
			pParams->DemodResults.Spectrum		=ChipGetField(pParams->hDemod,FSTV0903_SPECINV_DEMOD);
			
			if(pParams->DemodResults.ModCode <= FE_SAT_QPSK_910)
				pParams->DemodResults.Modulation=FE_SAT_MOD_QPSK;
			else if(pParams->DemodResults.ModCode <= FE_SAT_8PSK_910)
				pParams->DemodResults.Modulation=FE_SAT_MOD_8PSK;
			else if(pParams->DemodResults.ModCode <= FE_SAT_16APSK_910)
				pParams->DemodResults.Modulation=FE_SAT_MOD_16APSK;
			else if(pParams->DemodResults.ModCode <= FE_SAT_32APSK_910)
				pParams->DemodResults.Modulation=FE_SAT_MOD_32APSK;
			else
				pParams->DemodResults.Modulation=FE_SAT_MOD_UNKNOWN;

		break;
				
		case FE_SAT_DVBS1_STANDARD:
		case FE_SAT_DSS_STANDARD:
		default:
			pParams->DemodResults.Modulation=FE_SAT_MOD_QPSK;
			pParams->DemodResults.Spectrum=ChipGetField(pParams->hDemod,FSTV0903_IQINV);  	

		break;
	}
			
	if((pParams->DemodSearchAlgo==FE_SAT_BLIND_SEARCH) || (pParams->DemodResults.SymbolRate < 10000000)) 
	{
		/*in case of blind search the tuner freq may has been moven, read the new tuner freq and cumpute the freq offset from the original value*/
		offsetFreq=	pParams->DemodResults.Frequency-pParams->TunerFrequency;
		pParams->TunerFrequency=FE_STV0903_GetTunerFrequency(pParams->hDemod,pParams->hTuner,pParams->TunerType);
		
		if (ABS(offsetFreq)<= ((pParams->DemodSearchRange/2000)+500))
			range=FE_SAT_RANGEOK;
		else
			if (ABS(offsetFreq)<=  (   (1000+FE_STV0903_CarrierWidth(pParams->DemodResults.SymbolRate,pParams->DemodResults.RollOff))/2000) )
			range=FE_SAT_RANGEOK;
		else
			range=FE_SAT_OUTOFRANGE;
	}
	else
	{
		if (ABS(offsetFreq)<= ((pParams->DemodSearchRange/2000)+500))
			range=FE_SAT_RANGEOK;
		else 
			range=FE_SAT_OUTOFRANGE;
	}
	
	return range;
}
#endif
FE_STV0903_SIGNALTYPE_t FE_STV0903_GetSignalParams_E(FE_STV0903_InternalParams_t_E*pParams) 
{
	FE_STV0903_SIGNALTYPE_t	range=FE_SAT_OUTOFRANGE;
	S32 offsetFreq,
		symbolRateOffset,
		i=0;
	
	U8 timing;
	
	WAIT_N_MS(5);
	if(pParams->DemodSearchAlgo==FE_SAT_BLIND_SEARCH)
	{
		/*	if Blind search wait for symbol rate offset information transfert from the timing loop to the
			demodulator symbol rate
		*/
		timing=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGREG2,0,0);
		i=0;
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRSTEP,0x5c);
		while((i<=50) && (timing!=0) && (timing!=0xFF))
		{
			timing=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGREG2,0,0);
			WAIT_N_MS(5);
			i+=5;
		}
	}
	
	pParams->DemodResults.Standard		=	FE_STV0903_GetStandard_E();
	
	pParams->DemodResults.Frequency	=	FE_STV0903_GetTunerFrequency_E(pParams->TunerType,pParams->TunerReference);			
	offsetFreq=	FE_STV0903_GetCarrierFrequency_E(pParams->MasterClock)/1000; 
	pParams->DemodResults.Frequency	+= offsetFreq;
	
	pParams->DemodResults.SymbolRate	=	FE_STV0903_GetSymbolRate_E(pParams->MasterClock);
	symbolRateOffset=FE_STV0903_TimigGetOffset_E(pParams->DemodResults.SymbolRate);
	pParams->DemodResults.SymbolRate	+=	symbolRateOffset;
	pParams->DemodResults.PunctureRate	=	FE_STV0903_GetPunctureRate_E(); 
	pParams->DemodResults.ModCode		=	ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_PLH_MODCOD);
	pParams->DemodResults.Pilots		=	ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_PLH_TYPE) & 0x01;
	pParams->DemodResults.FrameLength	=	ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_PLH_TYPE)>>1;
	
	pParams->DemodResults.RollOff		=	ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ROLLOFF_STATUS);
	
	switch(pParams->DemodResults.Standard)
	{
		case FE_SAT_DVBS2_STANDARD:
			pParams->DemodResults.Spectrum		=ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SPECINV_DEMOD);
			
			if(pParams->DemodResults.ModCode <= FE_SAT_QPSK_910)
				pParams->DemodResults.Modulation=FE_SAT_MOD_QPSK;
			else if(pParams->DemodResults.ModCode <= FE_SAT_8PSK_910)
				pParams->DemodResults.Modulation=FE_SAT_MOD_8PSK;
			else if(pParams->DemodResults.ModCode <= FE_SAT_16APSK_910)
				pParams->DemodResults.Modulation=FE_SAT_MOD_16APSK;
			else if(pParams->DemodResults.ModCode <= FE_SAT_32APSK_910)
				pParams->DemodResults.Modulation=FE_SAT_MOD_32APSK;
			else
				pParams->DemodResults.Modulation=FE_SAT_MOD_UNKNOWN;

		break;
				
		case FE_SAT_DVBS1_STANDARD:
		case FE_SAT_DSS_STANDARD:
		default:
			pParams->DemodResults.Modulation=FE_SAT_MOD_QPSK;
			pParams->DemodResults.Spectrum=ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_IQINV);  	

		break;
	}
			
	if((pParams->DemodSearchAlgo==FE_SAT_BLIND_SEARCH) || (pParams->DemodResults.SymbolRate < 10000000)) 
	{
		/*in case of blind search the tuner freq may has been moven, read the new tuner freq and cumpute the freq offset from the original value*/
		offsetFreq=	pParams->DemodResults.Frequency-pParams->TunerFrequency;
		pParams->TunerFrequency=FE_STV0903_GetTunerFrequency_E(pParams->TunerType,pParams->TunerReference);
		
		if (ABS(offsetFreq)<= ((pParams->DemodSearchRange/2000)+500))
			range=FE_SAT_RANGEOK;
		else
			if (ABS(offsetFreq)<=  (   (1000+FE_STV0903_CarrierWidth(pParams->DemodResults.SymbolRate,pParams->DemodResults.RollOff))/2000) )
			range=FE_SAT_RANGEOK;
		else
			range=FE_SAT_OUTOFRANGE;
	}
	else
	{
		if (ABS(offsetFreq)<= ((pParams->DemodSearchRange/2000)+500))
			range=FE_SAT_RANGEOK;
		else 
			range=FE_SAT_OUTOFRANGE;
	}
	
	return range;
}
FE_STV0903_SIGNALTYPE_t FE_STV0903_DVBS1AcqWorkAround_E(FE_STV0903_InternalParams_t_E*pParams)
{
			
	S32 symbolRate,
		demodTimeout,
		fecTimeout,
		freq1,
		freq0;
	FE_STV0903_SIGNALTYPE_t signalType=FE_SAT_NODATA;;
	
	
	pParams->DemodResults.Locked=FALSE;
	if(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_HEADER_MODE)==FE_SAT_DVBS_FOUND)
	{
		/*workaround for DVBS1 false lock*/
		symbolRate=FE_STV0903_GetSymbolRate_E(pParams->MasterClock);		/*	Read the Symbol rate	*/
		symbolRate+=FE_STV0903_TimigGetOffset_E(symbolRate);				/*	Read the Symbol rate offset	*/ 

		if(pParams->DemodSearchAlgo==FE_SAT_BLIND_SEARCH)
		{
			FE_STV0903_SetSymbolRate_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,pParams->MasterClock,symbolRate); 		/*	Init the symbol rate	*/
		}

		FE_STV0903_GetLockTimeout(&demodTimeout,&fecTimeout,symbolRate, FE_SAT_WARM_START);  

		freq1=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFR2,0,0);
		freq0=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFR1,0,0);
		ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_CFR_AUTOSCAN,0);	/*stop the carrier frequency search loop*/

		ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SPECINV_CONTROL,FE_SAT_IQ_FORCE_SWAPPED);
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1C);		/*stop the demod*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT1,freq1);		/*init the frequency offset*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT0,freq0);
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x18);		/*trig a warm start*/
		
		if(FE_STV0903_WaitForLock_E(demodTimeout,fecTimeout)==TRUE) 				/*wait until Lock or timeout*/
		{
			pParams->DemodResults.Locked=TRUE;
			signalType=FE_STV0903_GetSignalParams_E(pParams);		/* Read signal caracteristics*/
			FE_STV0903_TrackingOptimization_E(pParams);			/* Optimization setting for tracking */
		}
		else
		{
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SPECINV_CONTROL,FE_SAT_IQ_FORCE_NORMAL);
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1C);		/*stop the demod*/
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT1,freq1);		/*init the frequency offset*/
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT0,freq0);
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x18);		/*trig a warm start*/
	
			if(FE_STV0903_WaitForLock_E(demodTimeout,fecTimeout)==TRUE) 				/*wait until Lock or timeout*/
			{
				pParams->DemodResults.Locked=TRUE;
				signalType=FE_STV0903_GetSignalParams_E(pParams);		/* Read signal caracteristics*/
				FE_STV0903_TrackingOptimization_E(pParams);			/* Optimization setting for tracking */
			} 
		}
	}
	else
		pParams->DemodResults.Locked=FALSE;
	
	return signalType; 
}


void FE_STV0903_Setdvbs2RollOff_E(void)
{
	S32 rolloff;
       U8 	ChipID;
	ChipID=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MID,0,0);
	if(ChipID==0x10) 
	{
		/*For cut 1.1 and above the setting of the rolloff in DVBS2 is automatic in both path 1 and 2*/
		/* for cut 1.0 the setting of the rolloff should be manual from the MATSTR field of the BB Header*/
		/* the setting of the rolloff in DVBS2 , when manual, should be after FEC lock to be sure that */
		/* at least one BB frame was decoded */
		/*Set the rolloff manually from the BB header value */
		ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_MANUAL_ROLLOFF,1);
		rolloff=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MATSTR1,0,0)&0x03;	/*Read the MATSTR reg*/	
		ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ROLLOFF_CONTROL,rolloff);
	}
	else /*For cut 1.1 above set the rolloff to auto mode if DVBS2 found*/
		ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_MANUAL_ROLLOFF,0);
}

#if 0
BOOL FE_STV0903_CheckSignalPresence(FE_STV0903_InternalParams_t *pParams)
{
	S32	carrierOffset,
		agc2Integrateur,
		maxCarrier;
	
	BOOL noSignal;
	
	carrierOffset = (ChipGetOneRegister(pParams->hDemod,RSTV0903_CFR2) << 8) | ChipGetOneRegister(pParams->hDemod,RSTV0903_CFR1);
	carrierOffset=Get2Comp(carrierOffset,16) ;
	agc2Integrateur =(ChipGetOneRegister(pParams->hDemod,RSTV0903_AGC2I1) << 8) | ChipGetOneRegister(pParams->hDemod,RSTV0903_AGC2I0);
	maxCarrier = pParams->DemodSearchRange/1000; /*convert to KHz*/
 
 	maxCarrier += (maxCarrier/10);				  /* add 10% of margin*/
	maxCarrier = 65536 * (maxCarrier/2) ;/*Convert to reg value*/
	maxCarrier /= pParams->MasterClock / 1000 ;
	/*Securite anti-bug:*/
	if (maxCarrier > 0x4000)
		maxCarrier = 0x4000 ; /*maxcarrier should be<= +-1/4 Mclk */

	/*if the AGC2 integrateur > 0x2000: No signal at this position
	  Else, if the AGC2 is <=0x2000 and the |carrier offset| is >=maxcarrier then 
	  the demodulator is checking an adjacent channel*/
	if (  (agc2Integrateur > 0x2000)
		|| (carrierOffset > +2*maxCarrier)
		|| (carrierOffset < -2*maxCarrier)
	   )
		noSignal = TRUE ;
	else	
		noSignal = FALSE ;

	return noSignal;
}
#endif
BOOL FE_STV0903_CheckSignalPresence_E(S32 DemodSearchRange,
	 						S32 MasterClock)
{
	S32	agc2Integrateur,
		maxCarrier;
	
	BOOL noSignal;
	U8 carrierOffset;
	carrierOffset = (ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFR2,0,0) << 8) | ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFR1,0,0);
	carrierOffset=Get2Comp(carrierOffset,16) ;
	agc2Integrateur =(ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2I1,0,0) << 8) | ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2I0,0,0);
	maxCarrier =DemodSearchRange/1000; /*convert to KHz*/
 
 	maxCarrier += (maxCarrier/10);				  /* add 10% of margin*/
	maxCarrier = 65536 * (maxCarrier/2) ;/*Convert to reg value*/
	maxCarrier /= MasterClock / 1000 ;
	/*Securite anti-bug:*/
	if (maxCarrier > 0x4000)
		maxCarrier = 0x4000 ; /*maxcarrier should be<= +-1/4 Mclk */

	/*if the AGC2 integrateur > 0x2000: No signal at this position
	  Else, if the AGC2 is <=0x2000 and the |carrier offset| is >=maxcarrier then 
	  the demodulator is checking an adjacent channel*/
	if (  (agc2Integrateur > 0x2000)
		|| (carrierOffset > +2*maxCarrier)
		|| (carrierOffset < -2*maxCarrier)
	   )
		noSignal = TRUE ;
	else	
		noSignal = FALSE ;

	return noSignal;
}


BOOL FE_STV0903_SearchCarrierSwLoop_E(
							S32 DemodSearchRange,
	 						S32 MasterClock,
							S32 FreqIncr, 
							S32 Timeout,
							BOOL Zigzag,
							S32 MaxStep)
{
	BOOL	noSignal,
			lock=FALSE;
	
	S32	stepCpt,
		freqOffset,
		maxCarrier;
		 
	
	maxCarrier = DemodSearchRange/1000;
	maxCarrier += (maxCarrier/10);
	
	maxCarrier = 65536 * (maxCarrier/2) ;/*Convert to reg value*/
	maxCarrier /= MasterClock / 1000 ;

	if (maxCarrier > 0x4000) 
		maxCarrier = 0x4000 ; /*maxcarrier should be<= +-1/4 Mclk */ 

	/*Initialization*/
	if (Zigzag == TRUE)
		freqOffset = 0 ;
	else	
		freqOffset = -maxCarrier + FreqIncr ;
	
	stepCpt = 0 ;

	do
	{
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1C) ;	/*Stop Demod*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT1,(freqOffset >> 8) & 0xFF) ; /*init carrier freq*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT0,freqOffset & 0xFF) ;
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x18) ;	/*Trig an acquisition (start the search)*/
		/*DVBS2 only: stop the Packet Delineator:*/
		ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ALGOSWRST,1); /*stop the DVBS2 packet delin*/

		/*Next Step*/
		if (Zigzag == TRUE)
		{
			if (freqOffset >= 0)
				freqOffset = -freqOffset - 2*FreqIncr ;
			else	
				freqOffset = -freqOffset ;
		}
		else
			freqOffset += +2*FreqIncr ;
		stepCpt ++ ;
					
		lock = FE_STV0903_GetDemodLock_E(Timeout) ;
		noSignal= FE_STV0903_CheckSignalPresence_E(DemodSearchRange,MasterClock);
		/*End of the loop if:
		-lock,
		-carrier offset > maxoffset,
		-step > MaxStep
		*/
	}
	while (	  (lock == FALSE)
		   && (noSignal == FALSE)
		   && ((freqOffset-FreqIncr) <  maxCarrier)
		   && ((freqOffset+FreqIncr) > -maxCarrier)
		   && (stepCpt < MaxStep)
						  ) ;
	/*DVBS2 only: release the Packet Delineator:*/
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_ALGOSWRST,0); /*release reset DVBS2 packet delin*/
	
	return lock;
}
#if 0
void FE_STV0903_GetSwLoopParams(	FE_STV0903_InternalParams_t *pParams,
						S32 *FreqIncrement,
						S32 *SwTimeout,
						S32 *MaxSteps)
{
	S32 timeOut,
		freqInrement,
		maxSteps,
		maxCarrier;
		
	FE_STV0903_SearchStandard_t	Standard;

	maxCarrier = pParams->DemodSearchRange/1000;
	maxCarrier += maxCarrier/10;
	Standard=pParams->DemodSearchStandard;
	
	maxCarrier = 65536 * (maxCarrier/2) ;/*Convert to reg value*/
	maxCarrier /= pParams->MasterClock / 1000 ;
	
	if (maxCarrier > 0x4000) 
		maxCarrier = 0x4000 ; /*maxcarrier should be<= +-1/4 Mclk */

	freqInrement = pParams->DemodSymbolRate ;
	freqInrement /= pParams->MasterClock / 1000 ;
	freqInrement *= 256 ;
	freqInrement *= 256 ;
	freqInrement /= 1000 ;

	switch(Standard)
	{
		case FE_SAT_SEARCH_DVBS1:
		case FE_SAT_SEARCH_DSS:
			freqInrement *= 3 ; /*frequ step = 3% of symbol rate*/ 	
			timeOut=20;
		break;
		
		case FE_SAT_SEARCH_DVBS2:
			freqInrement *= 4; /*frequ step = 4% of symbol rate*/	
			timeOut=25;
		break;
		
		case FE_SAT_AUTO_SEARCH:
		default:
			freqInrement *= 3 ; /*frequ step = 3% of symbol rate*/
			timeOut=25;	
		break;
	}
	freqInrement /= 100;	/*Increment = n% of SR, n =3 for DVBS1 or auto, n= 4 for DVBS2*/

	if ((freqInrement > maxCarrier) || (freqInrement < 0)) 
		freqInrement = maxCarrier/2 ; /*Freq Increment should be <= 1/8 Mclk */
				
	/* cumpute the timout according to the SR with 27.5Msps as reference*/
	timeOut *= 27500 ;
	if (pParams->DemodSymbolRate > 0)
		timeOut /= pParams->DemodSymbolRate / 1000 ;
				
	if ((timeOut > 100) || (timeOut < 0))
		timeOut = 100 ; /*timeout<= 100ms*/
				
	maxSteps = (maxCarrier/freqInrement)+1; /*minimum step = 3*/
	if ((maxSteps > 100) || (maxSteps < 0))
	{
		maxSteps =  100; /*maxstep <= 100*/
		freqInrement= maxCarrier / maxSteps; /*if max step > 100 then max step = 100 and the increment is radjasted to cover the total range*/
	}
		
	*FreqIncrement =freqInrement;
	*SwTimeout=timeOut;
	*MaxSteps=maxSteps;

}
#endif
void FE_STV0903_GetSwLoopParams_E(
	                                   S32 DemodSearchRange,
	                                   FE_STV0903_SearchStandard_t	DemodSearchStandard,
	                                   S32 MasterClock,
	                                   S32 DemodSymbolRate,
						S32 *FreqIncrement,
						S32 *SwTimeout,
						S32 *MaxSteps)
{
	S32 timeOut,
		freqInrement,
		maxSteps,
		maxCarrier;
		
	FE_STV0903_SearchStandard_t	Standard;

	maxCarrier = DemodSearchRange/1000;
	maxCarrier += maxCarrier/10;
	Standard=DemodSearchStandard;
	
	maxCarrier = 65536 * (maxCarrier/2) ;/*Convert to reg value*/
	maxCarrier /= MasterClock / 1000 ;
	
	if (maxCarrier > 0x4000) 
		maxCarrier = 0x4000 ; /*maxcarrier should be<= +-1/4 Mclk */

	freqInrement = DemodSymbolRate ;
	freqInrement /= MasterClock / 1000 ;
	freqInrement *= 256 ;
	freqInrement *= 256 ;
	freqInrement /= 1000 ;

	switch(Standard)
	{
		case FE_SAT_SEARCH_DVBS1:
		case FE_SAT_SEARCH_DSS:
			freqInrement *= 3 ; /*frequ step = 3% of symbol rate*/ 	
			timeOut=20;
		break;
		
		case FE_SAT_SEARCH_DVBS2:
			freqInrement *= 4; /*frequ step = 4% of symbol rate*/	
			timeOut=25;
		break;
		
		case FE_SAT_AUTO_SEARCH:
		default:
			freqInrement *= 3 ; /*frequ step = 3% of symbol rate*/
			timeOut=25;	
		break;
	}
	freqInrement /= 100;	/*Increment = n% of SR, n =3 for DVBS1 or auto, n= 4 for DVBS2*/

	if ((freqInrement > maxCarrier) || (freqInrement < 0)) 
		freqInrement = maxCarrier/2 ; /*Freq Increment should be <= 1/8 Mclk */
				
	/* cumpute the timout according to the SR with 27.5Msps as reference*/
	timeOut *= 27500 ;
	if (DemodSymbolRate > 0)
		timeOut /= DemodSymbolRate / 1000 ;
				
	if ((timeOut > 100) || (timeOut < 0))
		timeOut = 100 ; /*timeout<= 100ms*/
				
	maxSteps = (maxCarrier/freqInrement)+1; /*minimum step = 3*/
	if ((maxSteps > 100) || (maxSteps < 0))
	{
		maxSteps =  100; /*maxstep <= 100*/
		freqInrement= maxCarrier / maxSteps; /*if max step > 100 then max step = 100 and the increment is radjasted to cover the total range*/
	}
		
	*FreqIncrement =freqInrement;
	*SwTimeout=timeOut;
	*MaxSteps=maxSteps;

}

BOOL FE_STV0903_SWAlgo_E(
	FE_STV0903_SearchStandard_t	DemodSearchStandard,
	S32 DemodSearchRange,
	 S32 MasterClock,
	  S32 DemodSymbolRate)
{
	BOOL lock =FALSE;
	
	BOOL	noSignal,
			zigzag;
	S32 dvbs2_fly_wheel;

	S32 freqIncrement ,
		softStepTimeout,
		trialCounter ,
		maxSteps ;

	U8 	ChipID;
	ChipID=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MID,0,0);
	/*Get the SW algo params, Increment, timeout and max steps*/
	FE_STV0903_GetSwLoopParams_E(DemodSearchRange,DemodSearchStandard,MasterClock,DemodSymbolRate,
	&freqIncrement,&softStepTimeout,&maxSteps); 
	switch(DemodSearchStandard)
	{
		case FE_SAT_SEARCH_DVBS1:
		case FE_SAT_SEARCH_DSS:
			/*accelerate the frequency detector*/
			if(ChipID>=0x20)
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0x3B) ;	/* value for cut 2.0 above */
			else
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0xEF) ;	/* value for cut 1.2 below */
			
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDCFGMD,0x49) ;	/*stop DVBS2 search, stop freq auto scan, stop SR auto scan*/
			zigzag=FALSE;
		break;
	
		case FE_SAT_SEARCH_DVBS2:
			if(ChipID>=0x20)
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELABS,0x79) ; /*value for the SW search for cut 2.0*/ 
			else
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELABS,0x68) ; /*value for the SW search for cut 1.x*/
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDCFGMD,0x89) ;  /*stop DVBS1 search, stop freq auto scan, stop SR auto scan*/
			zigzag=TRUE;
		break;

		case FE_SAT_AUTO_SEARCH:
		default:
			if(ChipID>=0x20)
			{
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0x3B) ;	/* value for cut 2.0 above */
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELABS,0x79) ; /*value for the SW search for cut 2.0*/   
			}
			else
			{
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0xEF) ;	/* value for cut 1.2 below */
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELABS,0x68) ; /*value for the SW search for cut 1.x*/   
			}

			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDCFGMD,0xc9) ;	/*enable DVBS2 and DVBS1 search, stop freq auto scan, stop SR auto scan*/
			zigzag=FALSE;
		break;
	}
	
	trialCounter = 0 ;
	do
	{
		lock= FE_STV0903_SearchCarrierSwLoop_E(DemodSearchRange,MasterClock,freqIncrement,softStepTimeout,zigzag,maxSteps);
		noSignal=FE_STV0903_CheckSignalPresence_E(DemodSearchRange,MasterClock);
		trialCounter ++ ;
		/*run the SW search 2 times maximum*/
		if (   (lock == TRUE)
			|| (noSignal == TRUE)
			|| (trialCounter == 2)
		   )
		{
			/*remove the SW algo values */
			/*Check if the demod is not losing lock in DVBS2*/
			if(ChipID>=0x20)
			{
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0x49) ;	/* value for cut 2.0 above */
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELABS,0x9e) ;	/* value for cut 2.0 below */
			}
			else
			{
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0xed) ;	/* value for cut 1.2 below */
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELABS,0x88) ;	/* value for cut 1.2 below */
			}
			if ((lock == TRUE) && (ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_HEADER_MODE)==FE_SAT_DVBS2_FOUND))
			{/*Check if the demod is not losing lock in DVBS2*/ 
				WAIT_N_MS(softStepTimeout);
				dvbs2_fly_wheel = ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_FLYWHEEL_CPT) ; /*read the number of correct frames */
				if (dvbs2_fly_wheel < 0xd)	 /*if correct frames is decrementing */
				{
					WAIT_N_MS(softStepTimeout);	/*wait*/
					dvbs2_fly_wheel = ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_FLYWHEEL_CPT) ;	/*read the number of correct frames */
				}
				if (dvbs2_fly_wheel < 0xd)
				{	
					/*FALSE lock, The demod is lossing lock */
					lock = FALSE ;
					if (trialCounter < 2) 
					{
						if(ChipID>=0x20)
							ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELABS,0x79) ; /*value for the SW search for cut 2.0*/   
						else
							ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELABS,0x68) ; /*value for the SW search for cut 1.x*/
						ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDCFGMD,0x89) ;
					}
				}
			}
		}
	
	} /*end of trial if lock or noSignal or more than 2 trial*/
	while (	  (lock == FALSE)
		   && (trialCounter < 2)
		   && (noSignal == FALSE)
		  ) ;
	return lock;
}


BOOL FE_STV0903_GetDemodLockCold_E(FE_STV0903_InternalParams_t_E*pParams,S32 DemodTimeout)
{

	BOOL lock =FALSE;
	unsigned char  tmp;
	S32 
		nbSteps,
		currentStep,
		direction,
		tunerFreq;
	
	U32 carrierStep=1200;
	U8 dat=0;
	unsigned char txt0[10]={'s','e','a','r','c','h','=','0','\n'};
	unsigned char txt1[12]={'s','e','a','r','c','h','l','o','c','k','\n'};
	unsigned char txt2[14]={'s','e','a','r','c','h','U','N','l','o','c','k','\n'};

	
	if(pParams->DemodSymbolRate >=10000000 )	/*if Symbol rate >= 10Msps check for timing lock */
	{

		lock = FE_STV0903_GetDemodLock_E((DemodTimeout/3)) ;	/* case cold start wait for demod lock */
		if(lock==FALSE)
		{
			if(FE_STV0903_CheckTimingLock_E()==TRUE)
			{
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1F);		/*Reset the Demod*/
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x15);		/*Trig an acquisition (start the search)*/
				
				lock=FE_STV0903_GetDemodLock_E(DemodTimeout) ;//靠DemodLock 靠
			}
			else
				lock = FALSE;
		}
	}
	else	/*Symbol Rate <10Msps do not check timing lock*/
	{//  < 6.67MHZ

	//   DemodTimeout靠靠縧ock靠
		lock = FE_STV0903_GetDemodLock_E(DemodTimeout/2) ;	/* case cold start wait for demod lock */   

					
		if(lock==FALSE)
		{	//靠靠
			//display_hex(3,4); 
			//#if 0
			if(pParams->DemodSymbolRate <=4000000)// (0,4M)
				carrierStep=1000; 	/*SR<=4Msps ==> step = 1MHz*/
			else if(pParams->DemodSymbolRate <=7000000)//(4MHZ,7MHZ)
				carrierStep=2000;   /*SR<=7Msps ==> step = 2MHz*/
			else if(pParams->DemodSymbolRate <=10000000)//(7MHZ,10MHZ)
				carrierStep=3000;   /*SR<=10Msps ==> step = 3MHz*/
			else						//(10MHZ,~)
				carrierStep=5000;   /*SR>10Msps ==> step = 5MHz*/

			nbSteps=((pParams->DemodSearchRange/1000)/carrierStep);// 6670/2000 = 3
			nbSteps/=2; //nbsteps = 1;
			nbSteps=(2*(nbSteps+1));//nbsteps   2*2 = 4
			
			if(nbSteps<0)	
				nbSteps=2;
			else if(nbSteps>12)
			{
				nbSteps =12;
			}//(2,12)

			currentStep=1;
			direction=1;//nbsteps = 4 
			tunerFreq=pParams->TunerFrequency;
	              pParams->TunerBW=FE_STV0903_CarrierWidth(pParams->DemodSymbolRate,pParams->RollOff) + pParams->DemodSymbolRate; 
		while((currentStep <= nbSteps) && (lock==FALSE))
			{
				if(direction>0)//arrier = 2000   adc
					tunerFreq +=(currentStep*carrierStep); // 1 * 2000   +=2khz 
				else
					tunerFreq -= (currentStep*carrierStep);//  - =2khz 

				
                            	FE_STV0903_SetTuner_E(FE_SAT_SW_TUNER,pParams->TunerReference,tunerFreq,pParams->TunerBW); 
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1C);
				if((pParams->DemodSearchStandard ==FE_SAT_SEARCH_DVBS2) || (pParams->DemodSearchStandard ==FE_SAT_AUTO_SEARCH))
				{//靠
					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,0);
					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,0);

					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,1);
					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,1);
				}
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT1,0);
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT0,0);
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1F);		
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x15);		
				lock=FE_STV0903_GetDemodLock_E(DemodTimeout/3) ;
			
				direction*=-1;
				currentStep++;
			//	display_hex(currentStep,0); 
			//	txt0[7]=currentStep+0x30;
				//text_out(txt0, 10);
			}
		//#endif
		}
	}
	return	lock; 
}
#if 0
BOOL FE_STV0903_GetDemodLockCold_E(FE_STV0903_InternalParams_t_E*pParams,S32 DemodTimeout)
{

	BOOL lock =FALSE;
	unsigned char  tmp;
	S32 
		nbSteps,
		currentStep,
		direction,
		tunerFreq;
	
	U32 carrierStep=1200;
	U8 dat=0;
	unsigned char txt0[10]={'s','e','a','r','c','h','=','0','\n'};
	unsigned char txt1[12]={'s','e','a','r','c','h','l','o','c','k','\n'};
	unsigned char txt2[14]={'s','e','a','r','c','h','U','N','l','o','c','k','\n'};

	
	if(pParams->DemodSymbolRate >=10000000 )	/*if Symbol rate >= 10Msps check for timing lock */
	{

		lock = FE_STV0903_GetDemodLock_E((DemodTimeout/3)) ;	/* case cold start wait for demod lock */
		if(lock==FALSE)
		{
			if(FE_STV0903_CheckTimingLock_E()==TRUE)
			{
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1F);		/*Reset the Demod*/
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x15);		/*Trig an acquisition (start the search)*/
				
				lock=FE_STV0903_GetDemodLock_E(DemodTimeout) ;//获得DemodLock 状态
			}
			else
				lock = FALSE;
		}
	}
	else	/*Symbol Rate <10Msps do not check timing lock*/
	{//  < 6.67MHZ

	//   DemodTimeout次数的去读lock状态
		lock = FE_STV0903_GetDemodLock_E(DemodTimeout/2) ;	/* case cold start wait for demod lock */   

					
		if(lock==FALSE)
		{	//没有锁住
			//display_hex(3,4); 
			#if 0
			if(pParams->DemodSymbolRate <=4000000)// (0,4M)
				carrierStep=1000; 	/*SR<=4Msps ==> step = 1MHz*/
			else if(pParams->DemodSymbolRate <=7000000)//(4MHZ,7MHZ)
				carrierStep=2000;   /*SR<=7Msps ==> step = 2MHz*/
			else if(pParams->DemodSymbolRate <=10000000)//(7MHZ,10MHZ)
				carrierStep=3000;   /*SR<=10Msps ==> step = 3MHz*/
			else						//(10MHZ,~)
				carrierStep=5000;   /*SR>10Msps ==> step = 5MHz*/

			nbSteps=((pParams->DemodSearchRange/1000)/carrierStep);// 6670/2000 = 3
			nbSteps/=2; //nbsteps = 1;
			nbSteps=(2*(nbSteps+1));//nbsteps   2*2 = 4
			
			if(nbSteps<0)	
				nbSteps=2;
			else if(nbSteps>12)
			{
				nbSteps =12;
			}//(2,12)

			currentStep=1;
			direction=1;//nbsteps = 4 
			tunerFreq=pParams->TunerFrequency;
	              pParams->TunerBW=FE_STV0903_CarrierWidth(pParams->DemodSymbolRate,pParams->RollOff) + pParams->DemodSymbolRate; 
		while((currentStep <= nbSteps) && (lock==FALSE))
			{
				if(direction>0)//arrier = 2000   adc
					tunerFreq +=(currentStep*carrierStep); // 1 * 2000   +=2khz 
				else
					tunerFreq -= (currentStep*carrierStep);//  - =2khz 

				
                            	FE_STV0903_SetTuner_E(FE_SAT_SW_TUNER,pParams->TunerReference,tunerFreq,pParams->TunerBW); 
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1C);
				if((pParams->DemodSearchStandard ==FE_SAT_SEARCH_DVBS2) || (pParams->DemodSearchStandard ==FE_SAT_AUTO_SEARCH))
				{//自动
					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,0);
					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,0);

					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,1);
					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,1);
				}
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT1,0);
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT0,0);
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1F);		
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x15);		
				lock=FE_STV0903_GetDemodLock_E(DemodTimeout/3) ;
			
				direction*=-1;
				currentStep++;
			//	display_hex(currentStep,0); 
			//	txt0[7]=currentStep+0x30;
				//text_out(txt0, 10);
			}
		#endif
		}
	}
	return	lock; 
}
#endif
BOOL FE_STV0903_CHECK_E(FE_STV0903_InternalParams_t_E*pParams,S32 DemodTimeout)
{

	BOOL lock =FALSE;
	unsigned char  tmp;
	S32 
		nbSteps,
		currentStep,
		direction,
		tunerFreq;
	
	U32 carrierStep=1200;
	U8 dat=0;
	unsigned char txt0[10]={'s','e','a','r','c','h','=','0','\n'};
	unsigned char txt1[12]={'s','e','a','r','c','h','l','o','c','k','\n'};
	unsigned char txt2[14]={'s','e','a','r','c','h','U','N','l','o','c','k','\n'};

	
	if(pParams->DemodSymbolRate >=10000000 )	/*if Symbol rate >= 10Msps check for timing lock */
	{

		lock = FE_STV0903_GetDemodLock_E((DemodTimeout/3)) ;	/* case cold start wait for demod lock */
		if(lock==FALSE)
		{
			if(FE_STV0903_CheckTimingLock_E()==TRUE)
			{
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1F);		/*Reset the Demod*/
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x15);		/*Trig an acquisition (start the search)*/
				
				lock=FE_STV0903_GetDemodLock_E(DemodTimeout) ;//获得DemodLock 状态
			}
			else
				lock = FALSE;
		}
	}
	else	/*Symbol Rate <10Msps do not check timing lock*/
	{//  < 6.67MHZ

	//   DemodTimeout次数的去读lock状态
		lock = FE_STV0903_GetDemodLock_E(DemodTimeout/2) ;	/* case cold start wait for demod lock */   

					
		if(lock==FALSE)
		{	//没有锁住
			//display_hex(3,4); 
			#if 1
			if(pParams->DemodSymbolRate <=4000000)// (0,4M)
				carrierStep=1000; 	/*SR<=4Msps ==> step = 1MHz*/
			else if(pParams->DemodSymbolRate <=7000000)//(4MHZ,7MHZ)
				carrierStep=2000;   /*SR<=7Msps ==> step = 2MHz*/
			else if(pParams->DemodSymbolRate <=10000000)//(7MHZ,10MHZ)
				carrierStep=3000;   /*SR<=10Msps ==> step = 3MHz*/
			else						//(10MHZ,~)
				carrierStep=5000;   /*SR>10Msps ==> step = 5MHz*/

			nbSteps=((pParams->DemodSearchRange/1000)/carrierStep);// 6670/2000 = 3
			nbSteps/=2; //nbsteps = 1;
			nbSteps=(2*(nbSteps+1));//nbsteps   2*2 = 4
			
			if(nbSteps<0)	
				nbSteps=2;
			else if(nbSteps>12)
			{
				nbSteps =12;
			}//(2,12)

			currentStep=1;
			direction=1;//nbsteps = 4 
			tunerFreq=pParams->TunerFrequency;
	              pParams->TunerBW=FE_STV0903_CarrierWidth(pParams->DemodSymbolRate,pParams->RollOff) + pParams->DemodSymbolRate; 
		while((currentStep <= nbSteps) && (lock==FALSE))
			{
				if(direction>0)//arrier = 2000   adc
					tunerFreq +=(currentStep*carrierStep); // 1 * 2000   +=2khz 
				else
					tunerFreq -= (currentStep*carrierStep);//  - =2khz 

				
                            	FE_STV0903_SetTuner_E(FE_SAT_SW_TUNER,pParams->TunerReference,tunerFreq,pParams->TunerBW); 
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1C);
				if((pParams->DemodSearchStandard ==FE_SAT_SEARCH_DVBS2) || (pParams->DemodSearchStandard ==FE_SAT_AUTO_SEARCH))
				{//自动
					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,0);
					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,0);

					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS1_ENABLE,1);
					ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_DVBS2_ENABLE,1);
				}
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT1,0);
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT0,0);
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1F);		
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x15);		
				lock=FE_STV0903_GetDemodLock_E(DemodTimeout/3) ;
			
				direction*=-1;
				currentStep++;
			//	display_hex(currentStep,0); 
			//	txt0[7]=currentStep+0x30;
				//text_out(txt0, 10);
			}
		#endif
		}
	}
	return	lock; 
}
#if 0
U32 FE_STV0903_SearchSRCoarse(FE_STV0903_InternalParams_t *pParams)
{
	BOOL timingLock =FALSE;
	S32 i,
		timingcpt=0,
		direction=1,
		nbSteps,
		currentStep=0,
		tunerFreq;
	
	U32 coarseSymbolRate=0,
		agc2Integrateur=0,
		carrierStep=1200;
	

#ifdef HOST_PC 	
	#ifndef NO_GUI
		char message[100];
	#endif
#endif
	
	
	ChipSetField(pParams->hDemod,FSTV0903_I2C_DEMOD_MODE,0x1F);	/*Reset the Demod*/
	
	ChipSetOneRegister(pParams->hDemod,RSTV0903_TMGCFG,0x12);

	ChipSetOneRegister(pParams->hDemod,RSTV0903_TMGTHRISE,0xf0) ;	/*timing lock TH high setting for blind search*/
	ChipSetOneRegister(pParams->hDemod,RSTV0903_TMGTHFALL,0xe0) ;	/*timing lock TH low setting for blind search*/
	ChipSetField(pParams->hDemod,FSTV0903_SCAN_ENABLE,1);  			/*Enable the SR SCAN*/
	ChipSetField(pParams->hDemod,FSTV0903_CFR_AUTOSCAN,1);			/*activate the carrier frequency search loop*/
	
	ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRUP1,0x83);	/* set the SR max to 65Msps Write the MSB */
	ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRUP0,0xc0);	/* set the SR max to 65Msps Write the LSB */

	ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRLOW1,0x82);	/* set the SR min to 400 Ksps Write the MSB */
	ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRLOW0,0xa0);	/* set the SR min to 400 Ksps Write the LSB */
	
	ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDT0M,0x0); 			/*force the acquisition to stop at coarse carrier state*/
	ChipSetOneRegister(pParams->hDemod,RSTV0903_AGC2REF,0x60); 
	if(pParams->hDemod->ChipID>=0x20)	
	{
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CARFREQ,0x6a);		/*Frequency offset detector setting*/
		ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRSTEP,0x95) ;		/*set the SR scan step*/
	}
	else
	{
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CARFREQ,0xed);		/*Frequency offset detector setting*/
		ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRSTEP,0x73) ;		/*set the SR scan step*/
	}
	
	ChipSetField(pParams->hDemod,FSTV0903_S1S2_SEQUENTIAL,0);		/* S1 and  S2 search in // if both enabled*/


	if(pParams->DemodSymbolRate <=2000000)
		carrierStep=1000;	/*SR<=2Msps ==> step = 1MHz*/ 	
	else if(pParams->DemodSymbolRate <=5000000)
		carrierStep=2000;   /*SR<=5Msps ==> step = 2MHz*/
	else if(pParams->DemodSymbolRate <=12000000)
		carrierStep=3000;   /*SR<=12Msps ==> step = 3MHz*/
	else
		carrierStep=5000;   /*SR>12Msps ==> step = 5MHz*/
	

	nbSteps=-1+((pParams->DemodSearchRange/1000)/carrierStep);
	nbSteps/=2;
	nbSteps=(2*nbSteps)+1;
	if(nbSteps<0)	
		nbSteps=1;
	
	else if(nbSteps>10)
	{
		nbSteps =11;
		carrierStep= (pParams->DemodSearchRange/1000)/10;
	}
		
	currentStep = 0;
	direction=1;
	tunerFreq=pParams->TunerFrequency;
	while((timingLock == FALSE) && (currentStep<nbSteps)) 
	{
		ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDISTATE,0x5F);	/*Reset the Demod*/
		ChipSetField(pParams->hDemod,FSTV0903_I2C_DEMOD_MODE,0x0);	/*Trig an acquisition (start the search)*/
	
		WAIT_N_MS(50); /*wait for coarse carrier lock*/
	
		timingcpt=0;
		for(i=0;i<10;i++)
		{
			if(ChipGetField(pParams->hDemod,FSTV0903_TMGLOCK_QUALITY)>=2)	/*read the timing lock quality*/
				timingcpt++;
			/*read the AGC2 integrator value */
			agc2Integrateur +=(ChipGetOneRegister(pParams->hDemod,RSTV0903_AGC2I1) << 8) | ChipGetOneRegister(pParams->hDemod,RSTV0903_AGC2I0);
			

		}
		agc2Integrateur/=10;
		coarseSymbolRate=FE_STV0903_GetSymbolRate(pParams->hDemod,pParams->MasterClock);
		
		currentStep++;
		direction*=-1;

		if((timingcpt>=5) && (agc2Integrateur< 0x1F00) && (coarseSymbolRate <55000000) && (coarseSymbolRate >850000)  )	
		{
			timingLock =TRUE;
		}
		
		else if(currentStep<nbSteps)
		{
			/*Set the tuner frequency and bandwidth*/
			/*if timing not locked and currentstep < maxstep move the tuner freq*/
			if(direction>0)
				tunerFreq +=(currentStep*carrierStep);
			else
				tunerFreq -= (currentStep*carrierStep);
			FE_STV0903_SetTuner(pParams->hDemod,pParams->hTuner,pParams->TunerType,tunerFreq,pParams->TunerBW);
		}
	}
	
	
	if(timingLock==FALSE)
		coarseSymbolRate =0;
	else
		coarseSymbolRate=FE_STV0903_GetSymbolRate(pParams->hDemod,pParams->MasterClock);
#if 0			
#ifdef HOST_PC 
	#ifndef NO_GUI
		if(timingLock==FALSE)
			ReportInsertMessage("Coarse carrier Not Locked");
		else
		{
			ReportInsertMessage("Coarse carrier Locked");
			Fmt(message,"coarse SR = %d",coarseSymbolRate);
			ReportInsertMessage(message);
		}
	
	#endif
#endif
#endif	
return coarseSymbolRate; 
}
#endif

U32 FE_STV0903_SearchSRCoarse_E(FE_STV0903_InternalParams_t_E*pParams)
{
	BOOL timingLock =FALSE;
	S32 i,
		timingcpt=0,
		direction=1,
		nbSteps,
		currentStep=0,
		tunerFreq;
	
	U32 coarseSymbolRate=0,
		agc2Integrateur=0,
		carrierStep=1200;
	U8 	ChipID;
	ChipID=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MID,0,0);

#ifdef HOST_PC 	
	#ifndef NO_GUI
		char message[100];
	#endif
#endif
	
	
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_I2C_DEMOD_MODE,0x1F);	/*Reset the Demod*/
	
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGCFG,0x12);

	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGTHRISE,0xf0) ;	/*timing lock TH high setting for blind search*/
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGTHFALL,0xe0) ;	/*timing lock TH low setting for blind search*/
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SCAN_ENABLE,1);  			/*Enable the SR SCAN*/
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_CFR_AUTOSCAN,1);			/*activate the carrier frequency search loop*/
	
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRUP1,0x83);	/* set the SR max to 65Msps Write the MSB */
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRUP0,0xc0);	/* set the SR max to 65Msps Write the LSB */

	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRLOW1,0x82);	/* set the SR min to 400 Ksps Write the MSB */
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRLOW0,0xa0);	/* set the SR min to 400 Ksps Write the LSB */
	
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDT0M,0x0); 			/*force the acquisition to stop at coarse carrier state*/
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2REF,0x60); 
	if(ChipID>=0x20)	
	{
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0x6a);		/*Frequency offset detector setting*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRSTEP,0x95) ;		/*set the SR scan step*/
	}
	else
	{
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0xed);		/*Frequency offset detector setting*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRSTEP,0x73) ;		/*set the SR scan step*/
	}
	
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_S1S2_SEQUENTIAL,0);		/* S1 and  S2 search in // if both enabled*/


	if(pParams->DemodSymbolRate <=2000000)
		carrierStep=1000;	/*SR<=2Msps ==> step = 1MHz*/ 	
	else if(pParams->DemodSymbolRate <=5000000)
		carrierStep=2000;   /*SR<=5Msps ==> step = 2MHz*/
	else if(pParams->DemodSymbolRate <=12000000)
		carrierStep=3000;   /*SR<=12Msps ==> step = 3MHz*/
	else
		carrierStep=5000;   /*SR>12Msps ==> step = 5MHz*/
	

	nbSteps=-1+((pParams->DemodSearchRange/1000)/carrierStep);
	nbSteps/=2;
	nbSteps=(2*nbSteps)+1;
	if(nbSteps<0)	
		nbSteps=1;
	
	else if(nbSteps>10)
	{
		nbSteps =11;
		carrierStep= (pParams->DemodSearchRange/1000)/10;
	}
		
	currentStep = 0;
	direction=1;
	tunerFreq=pParams->TunerFrequency;
	while((timingLock == FALSE) && (currentStep<nbSteps)) 
	{
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x5F);	/*Reset the Demod*/
		ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_I2C_DEMOD_MODE,0x0);	/*Trig an acquisition (start the search)*/
	
		WAIT_N_MS(50); /*wait for coarse carrier lock*/
	
		timingcpt=0;
		for(i=0;i<10;i++)
		{
			if(ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_TMGLOCK_QUALITY)>=2)	/*read the timing lock quality*/
				timingcpt++;
			/*read the AGC2 integrator value */
			agc2Integrateur +=(ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2I1,0,0) << 8) | ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2I0,0,0);
			

		}
		agc2Integrateur/=10;
		coarseSymbolRate=FE_STV0903_GetSymbolRate_E(pParams->MasterClock);
		
		currentStep++;
		direction*=-1;

		if((timingcpt>=5) && (agc2Integrateur< 0x1F00) && (coarseSymbolRate <55000000) && (coarseSymbolRate >850000)  )	
		{
			timingLock =TRUE;
		}
		
		else if(currentStep<nbSteps)
		{
			/*Set the tuner frequency and bandwidth*/
			/*if timing not locked and currentstep < maxstep move the tuner freq*/
			if(direction>0)
				tunerFreq +=(currentStep*carrierStep);
			else
				tunerFreq -= (currentStep*carrierStep);
			
			FE_STV0903_SetTuner_E(FE_SAT_SW_TUNER,pParams->TunerReference,pParams->TunerFrequency,pParams->TunerBW); 
		}
	}
	
	
	if(timingLock==FALSE)
		coarseSymbolRate =0;
	else
		coarseSymbolRate=FE_STV0903_GetSymbolRate_E(pParams->MasterClock);
#if 0			
#ifdef HOST_PC 
	#ifndef NO_GUI
		if(timingLock==FALSE)
			ReportInsertMessage("Coarse carrier Not Locked");
		else
		{
			ReportInsertMessage("Coarse carrier Locked");
			Fmt(message,"coarse SR = %d",coarseSymbolRate);
			ReportInsertMessage(message);
		}
	
	#endif
#endif
#endif	
return coarseSymbolRate; 
}

#if 0
U32 FE_STV0903_SearchSRFine(FE_STV0903_InternalParams_t *pParams)
{
	U32 coarseSymbolRate,
		coarseFreq,
		symb;
	
	coarseSymbolRate=FE_STV0903_GetSymbolRate(pParams->hDemod,pParams->MasterClock); /*read the coarse SR*/
	coarseFreq=(ChipGetOneRegister(pParams->hDemod,RSTV0903_CFR2) << 8) | ChipGetOneRegister(pParams->hDemod,RSTV0903_CFR1); /* read the coarse freq offset*/

	symb= 13*(coarseSymbolRate /10); /*SFRUP = SFR + 30%*/
	if(symb < pParams->DemodSymbolRate) /* if coarsSR + 30% < symbol do not continue*/
	{
		coarseSymbolRate=0;		/*if coarse SR +30% < user given SR ==> end the SR given by the user in blind search is the minimum expected SR  */
	
	}
	else
	{
		/* In case of Path 1*/
	
		ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDISTATE,0x1F);		/*Reset the Demod*/

		ChipSetOneRegister(pParams->hDemod,RSTV0903_TMGCFG2,0x01) ;		/*Slow down the timing loop*/
		ChipSetOneRegister(pParams->hDemod,RSTV0903_TMGTHRISE,0x20) ;	/*timing lock TH high setting for fine state*/ 
		ChipSetOneRegister(pParams->hDemod,RSTV0903_TMGTHFALL,0x00) ;	/*timing lock TH low setting for fine state*/
		ChipSetOneRegister(pParams->hDemod,RSTV0903_TMGCFG,0xd2);
		ChipSetField(pParams->hDemod,FSTV0903_CFR_AUTOSCAN,0);			
		
		if(pParams->hDemod->ChipID>=0x20)	/*Frequency offset detector setting*/
		{
			ChipSetOneRegister(pParams->hDemod,RSTV0903_CARFREQ,0x49);
		}
		else
		{
			ChipSetOneRegister(pParams->hDemod,RSTV0903_CARFREQ,0xed);
		}

	
		if(coarseSymbolRate>3000000)
		{
			symb= 13*(coarseSymbolRate /10); /*SFRUP = SFR + 30%*/         
			symb=(symb/1000)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
			symb/=(pParams->MasterClock/1000);
			ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRUP1,(symb>>8) & 0x7F);	/* Write the MSB */
			ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRUP0,(symb & 0xFF));	/* Write the LSB */
	
			symb= 10*(coarseSymbolRate /13); /*SFRLOW = SFR - 30%*/
			symb=(symb/1000)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
			symb/=(pParams->MasterClock/1000);
	
			ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRLOW1,(symb>>8) & 0x7F);	/* Write the MSB */
			ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRLOW0,(symb & 0xFF));	/* Write the LSB */

			symb=(coarseSymbolRate/1000)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
			symb/=(pParams->MasterClock/1000);
			ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRINIT1,(symb>>8) & 0xFF);	/* Write the MSB */
			ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRINIT0,(symb & 0xFF));	/* Write the LSB */


		}
		else
		{
			symb= 13*(coarseSymbolRate /10); /*SFRUP = SFR + 30%*/         
			symb=(symb/100)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
			symb/=(pParams->MasterClock/100);
			ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRUP1,(symb>>8) & 0x7F);	/* Write the MSB */
			ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRUP0,(symb & 0xFF));	/* Write the LSB */
	
			symb= 10*(coarseSymbolRate /14); /*SFRLOW = SFR - 30%*/         
			symb=(symb/100)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
			symb/=(pParams->MasterClock/100);
			ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRLOW1,(symb>>8) & 0x7F);	/* Write the MSB */
			ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRLOW0,(symb & 0xFF));	/* Write the LSB */
	
			symb=(coarseSymbolRate/100)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
			symb/=(pParams->MasterClock/100);
			ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRINIT1,(symb>>8) & 0xFF);	/* Write the MSB */
			ChipSetOneRegister(pParams->hDemod,RSTV0903_SFRINIT0,(symb & 0xFF));	/* Write the LSB */
		}

	
		ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDT0M,0x20); 	/*continue the fine state after coarse state convergence*/
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRINIT1,(coarseFreq >>8) & 0xff); /* set the init freq offset to the coarse freq offsset*/
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CFRINIT0,coarseFreq  & 0xff);	   /* set the init freq offset to the coarse freq offsset*/
		ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDISTATE,0x15);		/*Trig an acquisition (start the search) with cold start*/
	}
	return coarseSymbolRate;
}
#endif
U32 FE_STV0903_SearchSRFine_E(S32 MasterClock,S32 DemodSymbolRate)
{
	U32 coarseSymbolRate,
		coarseFreq,
		symb;
	U8 	ChipID;
	ChipID=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MID,0,0);

	
	coarseSymbolRate=FE_STV0903_GetSymbolRate_E(MasterClock); /*read the coarse SR*/
	coarseFreq=(ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFR2,0,0) << 8) | ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFR1,0,0); /* read the coarse freq offset*/

	symb= 13*(coarseSymbolRate /10); /*SFRUP = SFR + 30%*/
	if(symb < DemodSymbolRate) /* if coarsSR + 30% < symbol do not continue*/
	{
		coarseSymbolRate=0;		/*if coarse SR +30% < user given SR ==> end the SR given by the user in blind search is the minimum expected SR  */
	
	}
	else
	{
		/* In case of Path 1*/
	
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x1F,0,0);		/*Reset the Demod*/

		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGCFG2,0x01) ;		/*Slow down the timing loop*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGTHRISE,0x20) ;	/*timing lock TH high setting for fine state*/ 
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGTHFALL,0x00) ;	/*timing lock TH low setting for fine state*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGCFG,0xd2);
		ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_CFR_AUTOSCAN,0);			
		
		if(ChipID>=0x20)	/*Frequency offset detector setting*/
		{
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0x49);
		}
		else
		{
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0xed);
		}

	
		if(coarseSymbolRate>3000000)
		{
			symb= 13*(coarseSymbolRate /10); /*SFRUP = SFR + 30%*/         
			symb=(symb/1000)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
			symb/=MasterClock/1000;
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRUP1,(symb>>8) & 0x7F);	/* Write the MSB */
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRUP0,(symb & 0xFF));	/* Write the LSB */
	
			symb= 10*(coarseSymbolRate /13); /*SFRLOW = SFR - 30%*/
			symb=(symb/1000)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
			symb/=(MasterClock/1000);
	
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRLOW1,(symb>>8) & 0x7F);	/* Write the MSB */
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRLOW0,(symb & 0xFF));	/* Write the LSB */

			symb=(coarseSymbolRate/1000)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
			symb/=(MasterClock/1000);
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRINIT1,(symb>>8) & 0xFF);	/* Write the MSB */
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRINIT0,(symb & 0xFF));	/* Write the LSB */


		}
		else
		{
			symb= 13*(coarseSymbolRate /10); /*SFRUP = SFR + 30%*/         
			symb=(symb/100)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
			symb/=(MasterClock/100);
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRUP1,(symb>>8) & 0x7F);	/* Write the MSB */
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRUP0,(symb & 0xFF));	/* Write the LSB */
	
			symb= 10*(coarseSymbolRate /14); /*SFRLOW = SFR - 30%*/         
			symb=(symb/100)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
			symb/=(MasterClock/100);
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRLOW1,(symb>>8) & 0x7F);	/* Write the MSB */
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRLOW0,(symb & 0xFF));	/* Write the LSB */
	
			symb=(coarseSymbolRate/100)*65536;			/* Formulat :init_symbol_rate_regs = SR * 2^16/MasterClock */
			symb/=(MasterClock/100);
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRINIT1,(symb>>8) & 0xFF);	/* Write the MSB */
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRINIT0,(symb & 0xFF));	/* Write the LSB */
		}

	
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDT0M,0x20); 	/*continue the fine state after coarse state convergence*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT1,(coarseFreq >>8) & 0xff); /* set the init freq offset to the coarse freq offsset*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT0,coarseFreq  & 0xff);	   /* set the init freq offset to the coarse freq offsset*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x15);		/*Trig an acquisition (start the search) with cold start*/
	}
	return coarseSymbolRate;
}


U16 FE_STV0903_BlindCheckAGC2MinLevel_E(S32 MasterClock,S32 DemodSearchRange)
{
	U32 minagc2level=0xffff,
		agc2level,
		initFreq,freqStep;
		
	S32 i,j,nbSteps,direction;
	
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2REF,0x38);
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SCAN_ENABLE,1);  			/*Enable the SR SCAN*/
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_CFR_AUTOSCAN,1);			/*activate the carrier frequency search loop*/
	
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRUP1,0x83);	/* set the SR max to 65Msps Write the MSB */
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRUP0,0xc0);	/* set the SR max to 65Msps Write the LSB */

	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRLOW1,0x82);	/* set the SR min to 400 Ksps Write the MSB */
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_SFRLOW0,0xa0);	/* set the SR min to 400 Ksps Write the LSB */

	
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDT0M,0x0); 	/*force the acquisition to stop at coarse carrier state*/
	
	FE_STV0903_SetSymbolRate_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,MasterClock,1000000);
	nbSteps=-1+(DemodSearchRange/1000000);
	nbSteps/=2;
	nbSteps=(2*nbSteps)+1;
	if(nbSteps<0)	
		nbSteps=1;
	
		
	direction=1;
	freqStep=(1000000*256)/(MasterClock/256);
	initFreq=0;
	for(i=0;i<nbSteps;i++)
	{
		if(direction>0)
			initFreq = initFreq + (freqStep * i);
		else
			initFreq = initFreq - (freqStep * i);
		direction *= -1;
		
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x5C);	/*Reset the Demod*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT1,(initFreq >>8) & 0xff); /* set the init freq offset */
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CFRINIT0,initFreq  & 0xff);	   	/* set the init freq offset to the coarse freq offsset*/
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x58);	/*Reset the Demod*/
		WAIT_N_MS(10);
		
		agc2level=0;
		for(j=0;j<10;j++)
			agc2level +=(ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2I1,0,0) << 8) | ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2I0,0,0);
		
		agc2level/=10;
		
		if(agc2level<minagc2level)
			minagc2level=agc2level;
	}
	return (U16)minagc2level;
}

BOOL FE_STV0903_BlindSearchAlgo_E(FE_STV0903_InternalParams_t_E*pParams)
{
		
	S32 kRefTmg,
	   kRefTmgMax,
	   kRefTmgMin;
	
	U32 coarseSymbolRate;
	BOOL lock = FALSE, coarseFail = FALSE;
	S32 demodTimeout=500,failCpt,i,agc2OverFlow,fecTimeout=50;
	U16 agc2Integrateur;
	
	U8 dstatus2; 
	U8 	ChipID;
	ChipID=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MID,0,0);

	
	if(ChipID==0x10) 
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELEXP,0xAA);	/*Set Correlation frames reg for acquisition for cut 1.0 only*/ 
	
	if(ChipID<0x20)
	{
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARHDR,0x55); /*Not applicable for cut 2.0*/
		kRefTmgMax = 233;  /*KT max*/
		kRefTmgMin = 143;  /*KT min*/
	}
	else
	{
		kRefTmgMax = 120;  /*KT max*/
		kRefTmgMin = 30;   /*KT min*/
	
	}

	/*Set The carrier search up and low to auto mode */
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARCFG,0xC4); /*applicable for cut 1.x and 2.0 */
	/*reduce the timing bandwith for high SR*/
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_RTCS2,0x44);
	
	agc2Integrateur= FE_STV0903_BlindCheckAGC2MinLevel_E(pParams->MasterClock,pParams->DemodSearchRange);
#if 0
#ifdef HOST_PC 
	#ifndef NO_GUI
		Fmt(message,"AGC2 MIN  = %d",agc2Integrateur);
		ReportInsertMessage(message);
	
	#endif
#endif
#endif
	if(agc2Integrateur > STV0903_BLIND_SEARCH_AGC2_TH)
	{
		lock = FALSE; /*if the AGC2 > 700 then no signal around the current frequency */
		#if 0
		#ifdef HOST_PC 
			#ifndef NO_GUI
				ReportInsertMessage("NO AGC2 : NO Signal");
			#endif
		#endif
            #endif
	}
	else
	{
	
		if(ChipID>=0x20)
		{
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_EQUALCFG,0x41);	
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_FFECFG,0x41);
		
			if( (pParams->DemodSearchStandard == FE_SAT_SEARCH_DVBS1) || (pParams->DemodSearchStandard == FE_SAT_SEARCH_DSS) || (pParams->DemodSearchStandard ==FE_SAT_AUTO_SEARCH)) 
			{
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VITSCALE,0x82);	/* open the ReedSolomon to viterbi feed back until demod lock*/
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_VAVSRVIT,0x0);	/*set Viterbi hysteresis for search*/
			}
		}

		kRefTmg=kRefTmgMax;

		do
		{
		
			/*while no full lock and KT > KT min run coarse and fine search*/
			#if 0
			#ifdef HOST_PC 
				#ifndef NO_GUI
					Fmt(message,"KREFTMG = %x",kRefTmg);
					ReportInsertMessage(message);
	
				#endif
			#endif
                       #endif
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_KREFTMG,kRefTmg);
		
			if(FE_STV0903_SearchSRCoarse_E(pParams)!=0) 
			{
				/*if coarse SR != 0 then the coarse state is locked continue with the fine*/
				coarseSymbolRate=FE_STV0903_SearchSRFine_E(pParams->MasterClock,pParams->DemodSymbolRate);
				if(coarseSymbolRate!=0)
				{
					/*if if coarse SR != 0then the SR found by the coarse is >= to the minimum expected SR (given by the user)
					in this case wait until full lock or timout  */ 
					FE_STV0903_GetLockTimeout(&demodTimeout,&fecTimeout,coarseSymbolRate, FE_SAT_BLIND_SEARCH);
					lock=FE_STV0903_GetDemodLock_E(demodTimeout);
				}
				else
				{
					lock = FALSE;
				}
			
			}
			else
			{
				failCpt=0;
				agc2OverFlow=0;
				for(i=0;i<10;i++)
				{
					agc2Integrateur =(ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2I1) << 8) | ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2I0,0,0); 
					if(agc2Integrateur >= 0xff00)
						agc2OverFlow++;
					
					dstatus2= ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DSTATUS2); 
					if(((dstatus2 & 0x1) == 0x1) && ((dstatus2 >>7)==1))  /*if CFR over flow and timing overflow nothing here */
						failCpt++;
				}
			        #if 0
				#ifdef HOST_PC 
					#ifndef NO_GUI
						Fmt(message,"failCpt = %d",failCpt);
						ReportInsertMessage(message);
	
					#endif
				#endif
                             #endif
				if((failCpt > 7) || (agc2OverFlow>7))
					coarseFail=TRUE;

				lock = FALSE;
			}
			
			kRefTmg =kRefTmg-30;
		
		}while( (kRefTmg >= kRefTmgMin) && (lock == FALSE) && (coarseFail==FALSE));
	}

	return lock; 

}
#if 0
BOOL FE_STV0903_BlindSearchAlgo(FE_STV0903_InternalParams_t *pParams)
{
		
	S32 kRefTmg,
	   kRefTmgMax,
	   kRefTmgMin;
	
	U32 coarseSymbolRate;
	BOOL lock = FALSE, coarseFail = FALSE;
	S32 demodTimeout=500,failCpt,i,agc2OverFlow,fecTimeout=50;
	U16 agc2Integrateur;
	
	U8 dstatus2; 
	
#ifdef HOST_PC 
	#ifndef NO_GUI
		char message[100];
	#endif
#endif
	
	if(pParams->hDemod->ChipID==0x10) 
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CORRELEXP,0xAA);	/*Set Correlation frames reg for acquisition for cut 1.0 only*/ 
	
	if(pParams->hDemod->ChipID<0x20)
	{
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CARHDR,0x55); /*Not applicable for cut 2.0*/
		kRefTmgMax = 233;  /*KT max*/
		kRefTmgMin = 143;  /*KT min*/
	}
	else
	{
		kRefTmgMax = 120;  /*KT max*/
		kRefTmgMin = 30;   /*KT min*/
	
	}

	/*Set The carrier search up and low to auto mode */
	ChipSetOneRegister(pParams->hDemod,RSTV0903_CARCFG,0xC4); /*applicable for cut 1.x and 2.0 */
	/*reduce the timing bandwith for high SR*/
	ChipSetOneRegister(pParams->hDemod,RSTV0903_RTCS2,0x44);
	
	agc2Integrateur= FE_STV0903_BlindCheckAGC2MinLevel(pParams);
#if 0
#ifdef HOST_PC 
	#ifndef NO_GUI
		Fmt(message,"AGC2 MIN  = %d",agc2Integrateur);
		ReportInsertMessage(message);
	
	#endif
#endif
#endif
	if(agc2Integrateur > STV0903_BLIND_SEARCH_AGC2_TH)
	{
		lock = FALSE; /*if the AGC2 > 700 then no signal around the current frequency */
		#if 0
		#ifdef HOST_PC 
			#ifndef NO_GUI
				ReportInsertMessage("NO AGC2 : NO Signal");
			#endif
		#endif
            #endif
	}
	else
	{
	
		if(pParams->hDemod->ChipID>=0x20)
		{
			ChipSetOneRegister(pParams->hDemod,RSTV0903_EQUALCFG,0x41);	
			ChipSetOneRegister(pParams->hDemod,RSTV0903_FFECFG,0x41);
		
			if( (pParams->DemodSearchStandard == FE_SAT_SEARCH_DVBS1) || (pParams->DemodSearchStandard == FE_SAT_SEARCH_DSS) || (pParams->DemodSearchStandard ==FE_SAT_AUTO_SEARCH)) 
			{
				ChipSetOneRegister(pParams->hDemod,RSTV0903_VITSCALE,0x82);	/* open the ReedSolomon to viterbi feed back until demod lock*/
				ChipSetOneRegister(pParams->hDemod,RSTV0903_VAVSRVIT,0x0);	/*set Viterbi hysteresis for search*/
			}
		}

		kRefTmg=kRefTmgMax;

		do
		{
		
			/*while no full lock and KT > KT min run coarse and fine search*/
			#if 0
			#ifdef HOST_PC 
				#ifndef NO_GUI
					Fmt(message,"KREFTMG = %x",kRefTmg);
					ReportInsertMessage(message);
	
				#endif
			#endif
                       #endif
			ChipSetOneRegister(pParams->hDemod,RSTV0903_KREFTMG,kRefTmg);
		
			if(FE_STV0903_SearchSRCoarse(pParams)!=0) 
			{
				/*if coarse SR != 0 then the coarse state is locked continue with the fine*/
				coarseSymbolRate=FE_STV0903_SearchSRFine(pParams);
				if(coarseSymbolRate!=0)
				{
					/*if if coarse SR != 0then the SR found by the coarse is >= to the minimum expected SR (given by the user)
					in this case wait until full lock or timout  */ 
					FE_STV0903_GetLockTimeout(&demodTimeout,&fecTimeout,coarseSymbolRate, FE_SAT_BLIND_SEARCH);
					lock=FE_STV0903_GetDemodLock(pParams->hDemod,demodTimeout);
				}
				else
				{
					lock = FALSE;
				}
			
			}
			else
			{
				failCpt=0;
				agc2OverFlow=0;
				for(i=0;i<10;i++)
				{
					agc2Integrateur =(ChipGetOneRegister(pParams->hDemod,RSTV0903_AGC2I1) << 8) | ChipGetOneRegister(pParams->hDemod,RSTV0903_AGC2I0); 
					if(agc2Integrateur >= 0xff00)
						agc2OverFlow++;
					
					dstatus2= ChipGetOneRegister(pParams->hDemod,RSTV0903_DSTATUS2); 
					if(((dstatus2 & 0x1) == 0x1) && ((dstatus2 >>7)==1))  /*if CFR over flow and timing overflow nothing here */
						failCpt++;
				}
			        #if 0
				#ifdef HOST_PC 
					#ifndef NO_GUI
						Fmt(message,"failCpt = %d",failCpt);
						ReportInsertMessage(message);
	
					#endif
				#endif
                             #endif
				if((failCpt > 7) || (agc2OverFlow>7))
					coarseFail=TRUE;

				lock = FALSE;
			}
			
			kRefTmg =kRefTmg-30;
		
		}while( (kRefTmg >= kRefTmgMin) && (lock == FALSE) && (coarseFail==FALSE));
	}

	return lock; 

}
#endif

/*****************************************************
--FUNCTION	::	FE_STV0903_Algo
--ACTION	::	Start a search for a valid DVBS1/DVBS2 or DSS transponder
--PARAMS IN	::	pParams	->	Pointer FE_STV0903_InternalParams_t structer
--PARAMS OUT::	NONE
--RETURN	::	SYGNAL TYPE 
--***************************************************/
#if 0
FE_STV0903_SIGNALTYPE_t	FE_STV0903_Algo(FE_STV0903_InternalParams_t *pParams)
{
	S32 demodTimeout=500,
		fecTimeout=50;
	BOOL lock = FALSE;
	BOOL	noSignal=FALSE;

	FE_STV0903_SIGNALTYPE_t signalType=FE_SAT_NOCARRIER;
	
	ChipSetField(pParams->hDemod,FSTV0903_RST_HWARE,1); /*Stop the Path 1 Stream Merger during acquisition*/
	ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDISTATE,0x5C); /* Demod Stop*/
	
	if(pParams->hDemod->ChipID>=0x20)
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CORRELABS,0x9e) ; /*nominal value for cut 2.0*/   
	else
		ChipSetOneRegister(pParams->hDemod,RSTV0903_CORRELABS,0x88) ; /*nominal value for cut 1.x*/
			
	/*Get the demod and FEC timeout recomended value depending of the symbol rate and the search algo*/
	FE_STV0903_GetLockTimeout(&demodTimeout,&fecTimeout, pParams->DemodSymbolRate, pParams->DemodSearchAlgo);

	if(pParams->DemodSearchAlgo==FE_SAT_BLIND_SEARCH)
	{	/* If the Symbolrate is unknown  set the BW to the maximum */
		pParams->TunerBW=2*36000000;
		/* if Blind search (Symbolrate is unknown) use a wider symbol rate scan range*/ 
		ChipSetOneRegister(pParams->hDemod,RSTV0903_TMGCFG2,0x00);
		/* If Blind search set the init symbol rate to 1Msps*/
		FE_STV0903_SetSymbolRate(pParams->hDemod,pParams->MasterClock,1000000);
	}
	else
	{	/* If Symbolrate is known  set the BW to appropriate value */
		ChipSetOneRegister(pParams->hDemod,RSTV0903_DMDT0M,0x20); 			
		ChipSetOneRegister(pParams->hDemod,RSTV0903_TMGCFG,0xd2);
		if(pParams->DemodSymbolRate>=10000000)
			ChipSetOneRegister(pParams->hDemod,RSTV0903_AGC2REF,0x38); /*high SR, AGC2REF acquistion = AGC2REF tracking = 0x38*/
		else
			ChipSetOneRegister(pParams->hDemod,RSTV0903_AGC2REF,0x60); /*Low SR, AGC2REF acquistion =0x60 and  AGC2REF tracking = 0x38*/
				
		if(pParams->hDemod->ChipID>=0x20) 
		{
			ChipSetOneRegister(pParams->hDemod,RSTV0903_KREFTMG,0x5a);
			if(pParams->DemodSearchAlgo==FE_SAT_COLD_START) 
				pParams->TunerBW=(15*(FE_STV0903_CarrierWidth(pParams->DemodSymbolRate,pParams->RollOff) + 10000000))/10;
			else if(pParams->DemodSearchAlgo==FE_SAT_WARM_START)	/*WARM START*/
				pParams->TunerBW=FE_STV0903_CarrierWidth(pParams->DemodSymbolRate,pParams->RollOff) + 10000000;
		}
		else
		{
			ChipSetOneRegister(pParams->hDemod,RSTV0903_KREFTMG,0xc1);
			pParams->TunerBW=(15*(FE_STV0903_CarrierWidth(pParams->DemodSymbolRate,pParams->RollOff) + 10000000))/10;
		}
		/* if cold start or warm  (Symbolrate is known) use a Narrow  symbol rate scan range*/ 
		ChipSetOneRegister(pParams->hDemod,RSTV0903_TMGCFG2,0x01);
		/* Set the Init Symbol rate*/ 
		FE_STV0903_SetSymbolRate(pParams->hDemod,pParams->MasterClock,pParams->DemodSymbolRate);
		FE_STV0903_SetMaxSymbolRate(pParams->hDemod,pParams->MasterClock,pParams->DemodSymbolRate);
		FE_STV0903_SetMinSymbolRate(pParams->hDemod,pParams->MasterClock,pParams->DemodSymbolRate);
			
	}
	
	/*Set the tuner frequency and bandwidth*/
	FE_STV0903_SetTuner(pParams->hDemod,pParams->hTuner,pParams->TunerType,pParams->TunerFrequency,pParams->TunerBW); 
	/*Set the IQ inversion search mode*/
	ChipSetField(pParams->hDemod,FSTV0903_SPECINV_CONTROL,pParams->DemodSearch_IQ_Inv);
	/*Set the rolloff to manual search mode*/ 
	ChipSetField(pParams->hDemod,FSTV0903_MANUAL_ROLLOFF,1);
	
	FE_STV0903_SetSearchStandard(pParams);
	if(pParams->DemodSearchAlgo!=FE_SAT_BLIND_SEARCH)
		FE_STV0903_StartSearch(pParams);

	ChipSetField(pParams->hDemod,FSTV0903_RST_HWARE,0);	/*Release stream merger reset */
	WAIT_N_MS(3);
	ChipSetField(pParams->hDemod,FSTV0903_RST_HWARE,1); /*Stop the Path 1 Stream Merger during acquisition*/
	ChipSetField(pParams->hDemod,FSTV0903_RST_HWARE,0);	/*Release stream merger reset */

	if(pParams->DemodSearchAlgo == FE_SAT_BLIND_SEARCH)
	{
		lock=FE_STV0903_BlindSearchAlgo(pParams);  /*special algo for blind search only*/
	}
	else if(pParams->DemodSearchAlgo==FE_SAT_COLD_START)
	{
		lock=FE_STV0903_GetDemodLockCold(pParams,demodTimeout);			/*wait for demod lock specific for blind search */ 
	}
	else if(pParams->DemodSearchAlgo==FE_SAT_WARM_START)
		lock = FE_STV0903_GetDemodLock(pParams->hDemod,demodTimeout) ;	/* case warm or cold start wait for demod lock */

	if((lock==FALSE) && (pParams->DemodSearchAlgo==FE_SAT_COLD_START))
	{	
		if(pParams->DemodSymbolRate >= 10000000) /*If Timing OK and SR >= 10Msps run the SW Algo*/
		{
			if(FE_STV0903_CheckTimingLock(pParams->hDemod)==TRUE) 
				lock=FE_STV0903_SWAlgo(pParams);		/* if demod lock fail and cold start (zapping mode ) run the SW acquisition algo */
		}
	}
	

	if(lock==TRUE)
	{
		signalType=FE_STV0903_GetSignalParams(pParams);	/* Read signal caracteristics*/  
		FE_STV0903_TrackingOptimization(pParams);			/* Optimization setting for tracking */
		

		if(FE_STV0903_WaitForLock(pParams->hDemod,fecTimeout,fecTimeout)==TRUE)
		{
			lock=TRUE;
			pParams->DemodResults.Locked=TRUE;
			
			if(pParams->DemodResults.Standard==FE_SAT_DVBS2_STANDARD)
			{
				FE_STV0903_Setdvbs2RollOff(pParams->hDemod);		/*case DVBS2 found set the rolloff, the rolloff setting for DVBS2 should be done after full lock (demod+FEC)*/ 
			
				ChipSetOneRegister(pParams->hDemod,RSTV0903_PDELCTRL2,0x40);/*reset DVBS2 packet delineator  error counter */
				ChipSetOneRegister(pParams->hDemod,RSTV0903_PDELCTRL2,0);	/*reset DVBS2 packet delineator error counter */
				
				ChipSetOneRegister(pParams->hDemod,RSTV0903_ERRCTRL1,0x67);	/* reset the error counter to  DVBS2 packet error rate */
			}
			else
			{
				ChipSetOneRegister(pParams->hDemod,RSTV0903_ERRCTRL1,0x75);	/* reset the viterbi bit error rate */ 
			}

		}
		else
		{
			lock=FALSE;
			signalType=FE_SAT_NODATA;	/*if the demod is locked and not the FEC signal type is no DATA*/
			noSignal=FE_STV0903_CheckSignalPresence(pParams); 
			pParams->DemodResults.Locked=FALSE;
		}

	}
	
	if((signalType==FE_SAT_NODATA) && (noSignal == FALSE))
	{
		if(pParams->hDemod->ChipID<=0x11) /*workaround for cut 1.1 and 1.0 only*/ 
		{
			if((ChipGetField(pParams->hDemod,FSTV0903_HEADER_MODE)==FE_SAT_DVBS_FOUND) &&
				(pParams->DemodSearch_IQ_Inv<=FE_SAT_IQ_AUTO_NORMAL_FIRST))	/*case of false lock DVBS1 in IQ auto mode*/
				signalType=FE_STV0903_DVBS1AcqWorkAround(pParams);
		}
		else
			pParams->DemodResults.Locked=FALSE; 	
			
		/* */	
	}
	
	return signalType;
}
#endif

void showu32(U32 dat,U8 pos)
{
	U8 temp=0;
	if(pos==0)
	{
		temp=(U8)(dat>>24);
//		display_hex(temp,0); 
		temp=(U8)(dat>>16);
//		display_hex(temp,2);
		temp=(U8)(dat>>8);
//		display_hex(temp,4); 
//		temp=(U8)dat;
//		display_hex(temp,6); 
	}
	else
	{
		temp=(U8)(dat>>24);
//		display_hex(temp,8); 
		temp=(U8)(dat>>16);
//		display_hex(temp,10);
		temp=(U8)(dat>>8);
//		display_hex(temp,12); 
		temp=(U8)dat;
//		display_hex(temp,14); 
	}
}
void FE_TUNER_SET(void)
{
	U32 TunerBW;
	U32 Frequency=1808000;
	U32 TunerReference=16000000;
	U32 DemodSymbolRate=6000000;
	TunerBW=(15*(FE_STV0903_CarrierWidth(DemodSymbolRate,FE_SAT_35) + 10000000))/10;

	FE_Sat_TunerSetFrequency_E(Frequency,TunerReference);
	FE_Sat_TunerSetBandwidth_E(TunerBW);
}
FE_STV0903_SIGNALTYPE_t	FE_STV0903_Algo_E(FE_STV0903_InternalParams_t_E* pParams)
{
	S32 demodTimeout=500,
			fecTimeout=50;
	BOOL 	lock = FALSE;
	BOOL	noSignal=FALSE;
       	U8 		ChipID;

	FE_STV0903_SIGNALTYPE_t 	signalType=FE_SAT_NOCARRIER;
	//pParams->Quartz=16000000;
pParams->Quartz=8000000;
	
	pParams->MasterClock = FE_STV0903_GetMclkFreq_E(pParams->Quartz);	
	
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_RST_HWARE,1); /*Stop the Path 1 Stream Merger during acquisition*/
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x5C); /* Demod Stop*/

	//ChipID=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MID,0,0);
	//if(ChipID>=0x20)
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELABS,0x9e) ; /*nominal value for cut 2.0*/   
	//else
	//	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELABS,0x88) ; /*nominal value for cut 1.x*/
			
	/*Get the demod and FEC timeout recomended value depending of the symbol rate and the search algo*/
	FE_STV0903_GetLockTimeout(&demodTimeout,&fecTimeout, pParams->DemodSymbolRate, pParams->DemodSearchAlgo);
	{	/* If Symbolrate is known  set the BW to appropriate value */
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDT0M,0x20); 			
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGCFG,0xd2);
		if(pParams->DemodSymbolRate>=10000000)
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2REF,0x38); /*high SR, AGC2REF acquistion = AGC2REF tracking = 0x38*/
		else
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2REF,0x60); /*Low SR, AGC2REF acquistion =0x60 and  AGC2REF tracking = 0x38*/
				
		if(ChipID >= 0x20) 
		{
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_KREFTMG,0x5a);
			if(pParams->DemodSearchAlgo==FE_SAT_COLD_START) 
				pParams->TunerBW=(15*(FE_STV0903_CarrierWidth(pParams->DemodSymbolRate,pParams->RollOff) + 10000000))/10;
			else if(pParams->DemodSearchAlgo==FE_SAT_WARM_START)	/*WARM START*/
				pParams->TunerBW=FE_STV0903_CarrierWidth(pParams->DemodSymbolRate,pParams->RollOff) + 10000000;
		}
		else
		{
            
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_KREFTMG,0xc1);
			//pParams->TunerBW=FE_STV0903_CarrierWidth(pParams->DemodSymbolRate,pParams->RollOff) + 3000000;
		        pParams->TunerBW=(15*(FE_STV0903_CarrierWidth(pParams->DemodSymbolRate,pParams->RollOff) + 10000000))/10;
		}
		
		/* if cold start or warm  (Symbolrate is known) use a Narrow  symbol rate scan range*/ 
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGCFG2,0x01);
		/* Set the Init Symbol rate*/ 
		FE_STV0903_SetSymbolRate_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,pParams->MasterClock,pParams->DemodSymbolRate);
		FE_STV0903_SetMaxSymbolRate_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,pParams->MasterClock,pParams->DemodSymbolRate);
		FE_STV0903_SetMinSymbolRate_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,pParams->MasterClock,pParams->DemodSymbolRate);
			
	}
	
	/*Set the tuner frequency and bandwidth*/
	//FE_TUNER_SET();
	
	FE_STV0903_SetTuner_E(FE_SAT_SW_TUNER,pParams->TunerReference,pParams->TunerFrequency,pParams->TunerBW); 
	/*Set the IQ inversion search mode*/
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SPECINV_CONTROL,FE_SAT_IQ_AUTO);
	/*Set the rolloff to manual search mode*/ 
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_MANUAL_ROLLOFF,1);
	#if 1
	FE_STV0903_SetSearchStandard_E(pParams->DemodSearchStandard);
	//if(pParams->DemodSearchAlgo!=FE_SAT_BLIND_SEARCH)
//		FE_STV0903_StartSearch_E(pParams->DemodSymbolRate,pParams->DemodSearchStandard,pParams->DemodSearchAlgo);

	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_RST_HWARE,0);	/*Release stream merger reset */
	WAIT_N_MS(3);//3//
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_RST_HWARE,1); /*Stop the Path 1 Stream Merger during acquisition*/
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_RST_HWARE,0);	/*Release stream merger reset */
      // STV0903regInit();
	//STV0903regInit();
	//lock=FE_STV0903_GetDemodLockCold_E(pParams,demodTimeout);			/*wait for demod lock specific for blind search */ 
       //lock=FE_STV0903_GetDemodLock_E(demodTimeout);
       if(pParams->DemodSearchAlgo == FE_SAT_BLIND_SEARCH)
	{
		lock=FE_STV0903_BlindSearchAlgo_E(pParams);  /*special algo for blind search only*/
	}
	else if(pParams->DemodSearchAlgo==FE_SAT_COLD_START)
	{
		lock=FE_STV0903_GetDemodLockCold_E(pParams,demodTimeout);			/*wait for demod lock specific for blind search */ 
	}
	else if(pParams->DemodSearchAlgo==FE_SAT_WARM_START)
		lock = FE_STV0903_GetDemodLock_E(demodTimeout) ;	/* case warm or cold start wait for demod lock */


	//   display_hex(l,6); 
    //lock=FALSE;
   // #if 0
	if((lock==FALSE) && (pParams->DemodSearchAlgo==FE_SAT_COLD_START))
	{		//	display_hex(16,8);

		if(pParams->DemodSymbolRate >= 10000000) /*If Timing OK and SR >= 10Msps run the SW Algo*/
		{
			if(FE_STV0903_CheckTimingLock_E()==TRUE) 
				lock=FE_STV0903_SWAlgo_E(FE_SAT_AUTO_SEARCH,pParams->DemodSearchRange,pParams->MasterClock,
	            pParams->DemodSymbolRate);		/* if demod lock fail and cold start (zapping mode ) run the SW acquisition algo */
		}
	}
	//#endif 
	
    //isplay_hex(0,2);
	  //#if 0
	  //lock==TRUE;
	if(lock==TRUE)
	{
		//display_hex(17,8);

		signalType=FE_STV0903_GetSignalParams_E(pParams);	/* Read signal caracteristics*/  
		FE_STV0903_TrackingOptimization_E(pParams);			/* Optimization setting for tracking */
		

		if(FE_STV0903_WaitForLock_E(fecTimeout,fecTimeout)==TRUE)
		{
			lock=TRUE;
			pParams->DemodResults.Locked=TRUE;
			
			if(pParams->DemodResults.Standard==FE_SAT_DVBS2_STANDARD)
			{
				FE_STV0903_Setdvbs2RollOff_E();		/*case DVBS2 found set the rolloff, the rolloff setting for DVBS2 should be done after full lock (demod+FEC)*/ 
			
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PDELCTRL2,0x40);/*reset DVBS2 packet delineator  error counter */
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PDELCTRL2,0);	/*reset DVBS2 packet delineator error counter */
				
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ERRCTRL1,0x67);	/* reset the error counter to  DVBS2 packet error rate */
			}
			else
			{
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ERRCTRL1,0x75);	/* reset the viterbi bit error rate */ 
			}

		}
		else
		{
			lock=FALSE;
			signalType=FE_SAT_NODATA;	/*if the demod is locked and not the FEC signal type is no DATA*/
			noSignal=FE_STV0903_CheckSignalPresence_E(pParams->DemodSearchRange,pParams->MasterClock); 
			pParams->DemodResults.Locked=FALSE;
		}

	}
	
	if((signalType==FE_SAT_NODATA) && (noSignal == FALSE))
	{
		if(ChipID<=0x11) /*workaround for cut 1.1 and 1.0 only*/ 
		{
			if((ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_HEADER_MODE)==FE_SAT_DVBS_FOUND) &&
				(pParams->DemodSearch_IQ_Inv<=FE_SAT_IQ_AUTO_NORMAL_FIRST))	/*case of false lock DVBS1 in IQ auto mode*/
				signalType=FE_STV0903_DVBS1AcqWorkAround_E(pParams);
		}
		else
			pParams->DemodResults.Locked=FALSE; 	
			
		/* */	
	}
	//#endif
	#endif
	//display_hex(signalType,0);
	return signalType;
}
#if 0//                                            tuner 设置
FE_STV0903_SIGNALTYPE_t	FE_STV0903_Algo_E(FE_STV0903_InternalParams_t_E* pParams)
{
	S32 demodTimeout=500,
			fecTimeout=50;
	BOOL 	lock = FALSE;
	BOOL	noSignal=FALSE;
       	U8 		ChipID;

	FE_STV0903_SIGNALTYPE_t 	signalType=FE_SAT_NOCARRIER;
	//pParams->Quartz=16000000;
pParams->Quartz=8000000;
	
	pParams->MasterClock = FE_STV0903_GetMclkFreq_E(pParams->Quartz);	
	
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_RST_HWARE,1); /*Stop the Path 1 Stream Merger during acquisition*/
	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDISTATE,0x5C); /* Demod Stop*/

	//ChipID=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MID,0,0);
	//if(ChipID>=0x20)
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELABS,0x9e) ; /*nominal value for cut 2.0*/   
	//else
	//	ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CORRELABS,0x88) ; /*nominal value for cut 1.x*/
			
	/*Get the demod and FEC timeout recomended value depending of the symbol rate and the search algo*/
	FE_STV0903_GetLockTimeout(&demodTimeout,&fecTimeout, pParams->DemodSymbolRate, pParams->DemodSearchAlgo);
	{	/* If Symbolrate is known  set the BW to appropriate value */
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DMDT0M,0x20); 			
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGCFG,0xd2);
		if(pParams->DemodSymbolRate>=10000000)
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2REF,0x38); /*high SR, AGC2REF acquistion = AGC2REF tracking = 0x38*/
		else
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_AGC2REF,0x60); /*Low SR, AGC2REF acquistion =0x60 and  AGC2REF tracking = 0x38*/
				
		if(ChipID >= 0x20) 
		{
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_KREFTMG,0x5a);
			if(pParams->DemodSearchAlgo==FE_SAT_COLD_START) 
				pParams->TunerBW=(15*(FE_STV0903_CarrierWidth(pParams->DemodSymbolRate,pParams->RollOff) + 10000000))/10;
			else if(pParams->DemodSearchAlgo==FE_SAT_WARM_START)	/*WARM START*/
				pParams->TunerBW=FE_STV0903_CarrierWidth(pParams->DemodSymbolRate,pParams->RollOff) + 10000000;
		}
		else
		{
			ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_KREFTMG,0xc1);
		        pParams->TunerBW=(15*(FE_STV0903_CarrierWidth(pParams->DemodSymbolRate,pParams->RollOff) + 10000000))/10;
		}
		/* if cold start or warm  (Symbolrate is known) use a Narrow  symbol rate scan range*/ 
		ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_TMGCFG2,0x01);
		/* Set the Init Symbol rate*/ 
		FE_STV0903_SetSymbolRate_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,pParams->MasterClock,pParams->DemodSymbolRate);
		FE_STV0903_SetMaxSymbolRate_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,pParams->MasterClock,pParams->DemodSymbolRate);
		FE_STV0903_SetMinSymbolRate_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,pParams->MasterClock,pParams->DemodSymbolRate);
			
	}
	
	/*Set the tuner frequency and bandwidth*/
	//FE_TUNER_SET();
	
	FE_STV0903_SetTuner_E(FE_SAT_SW_TUNER,pParams->TunerReference,pParams->TunerFrequency,pParams->TunerBW); 
	/*Set the IQ inversion search mode*/
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SPECINV_CONTROL,FE_SAT_IQ_AUTO);
	/*Set the rolloff to manual search mode*/ 
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_MANUAL_ROLLOFF,1);
	#if 1
	FE_STV0903_SetSearchStandard_E(pParams->DemodSearchStandard);
	//if(pParams->DemodSearchAlgo!=FE_SAT_BLIND_SEARCH)
//		FE_STV0903_StartSearch_E(pParams->DemodSymbolRate,pParams->DemodSearchStandard,pParams->DemodSearchAlgo);

	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_RST_HWARE,0);	/*Release stream merger reset */
	WAIT_N_MS(3);
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_RST_HWARE,1); /*Stop the Path 1 Stream Merger during acquisition*/
	ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_RST_HWARE,0);	/*Release stream merger reset */
       STV0903regInit();
	//STV0903regInit();
	lock=FE_STV0903_GetDemodLockCold_E(pParams,demodTimeout);			/*wait for demod lock specific for blind search */ 
    //   display_hex(l,6); 
    //lock=FALSE;
    #if 1
	if((lock==FALSE) && (pParams->DemodSearchAlgo==FE_SAT_COLD_START))
	{		//	display_hex(16,8);

		if(pParams->DemodSymbolRate >= 10000000) /*If Timing OK and SR >= 10Msps run the SW Algo*/
		{
			if(FE_STV0903_CheckTimingLock_E()==TRUE) 
				lock=FE_STV0903_SWAlgo_E(FE_SAT_AUTO_SEARCH,pParams->DemodSearchRange,pParams->MasterClock,
	            pParams->DemodSymbolRate);		/* if demod lock fail and cold start (zapping mode ) run the SW acquisition algo */
		}
	}
	#endif
	
    //isplay_hex(0,2);
	  #if 0
	  //lock==TRUE;
	if(lock==TRUE)
	{
		//display_hex(17,8);

		signalType=FE_STV0903_GetSignalParams_E(pParams);	/* Read signal caracteristics*/  
		FE_STV0903_TrackingOptimization_E(pParams);			/* Optimization setting for tracking */
		

		if(FE_STV0903_WaitForLock_E(fecTimeout,fecTimeout)==TRUE)
		{
			lock=TRUE;
			pParams->DemodResults.Locked=TRUE;
			
			if(pParams->DemodResults.Standard==FE_SAT_DVBS2_STANDARD)
			{
				FE_STV0903_Setdvbs2RollOff_E();		/*case DVBS2 found set the rolloff, the rolloff setting for DVBS2 should be done after full lock (demod+FEC)*/ 
			
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PDELCTRL2,0x40);/*reset DVBS2 packet delineator  error counter */
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_PDELCTRL2,0);	/*reset DVBS2 packet delineator error counter */
				
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ERRCTRL1,0x67);	/* reset the error counter to  DVBS2 packet error rate */
			}
			else
			{
				ChipSetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_ERRCTRL1,0x75);	/* reset the viterbi bit error rate */ 
			}

		}
		else
		{
			lock=FALSE;
			signalType=FE_SAT_NODATA;	/*if the demod is locked and not the FEC signal type is no DATA*/
			noSignal=FE_STV0903_CheckSignalPresence_E(pParams->DemodSearchRange,pParams->MasterClock); 
			pParams->DemodResults.Locked=FALSE;
		}

	}
	
	if((signalType==FE_SAT_NODATA) && (noSignal == FALSE))
	{
		if(ChipID<=0x11) /*workaround for cut 1.1 and 1.0 only*/ 
		{
			if((ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_HEADER_MODE)==FE_SAT_DVBS_FOUND) &&
				(pParams->DemodSearch_IQ_Inv<=FE_SAT_IQ_AUTO_NORMAL_FIRST))	/*case of false lock DVBS1 in IQ auto mode*/
				signalType=FE_STV0903_DVBS1AcqWorkAround_E(pParams);
		}
		else
			pParams->DemodResults.Locked=FALSE; 	
			
		/* */	
	}
	#endif
	#endif
	//display_hex(signalType,0);
	return signalType;
}
#endif
#if 0
BOOL	FE_STV0903_LDPCPowerMonitoring(STCHIP_Handle_t hChip)
{
	S32	packetErrorCount,
		frameErrorCount;
	
	static BOOL ldpcStopped = FALSE;

	/*read the error counter*/
	packetErrorCount=MAKEWORD(ChipGetOneRegister(hChip,RSTV0903_UPCRCKO1),ChipGetOneRegister(hChip,RSTV0903_UPCRCKO0));
	frameErrorCount = MAKEWORD(ChipGetOneRegister(hChip,RSTV0903_BBFCRCKO1),ChipGetOneRegister(hChip,RSTV0903_BBFCRCKO0));     	
			
	ChipSetOneRegister(hChip,RSTV0903_PDELCTRL2,0x40);	/*reset packet error counter */
	ChipSetOneRegister(hChip,RSTV0903_PDELCTRL2,0);	/*reset packet error counter */
	if((packetErrorCount >=200)||(frameErrorCount>=5))
	{ 
		FE_STV0903_StopALL_S2_Modcod(hChip);   /*disable all frame type (LDPC stoped)*/
		ldpcStopped = TRUE;
	}
	else
	{
		if(ldpcStopped  == TRUE)
			FE_STV0903_ActivateS2ModCode(hChip);	/* activate the LDPC */   
		ldpcStopped = FALSE;
	}

	return ldpcStopped; 
}
#endif
	
 /****************************************************************************************************************************
 *****************************************************************************************************************************/
	
						/****************************************************************
						 ****************************************************************
						 **************************************************************** 
						 **															   **
						 **		***********************************************		   **
						 **						PUBLIC ROUTINES						   **
				 		 **		***********************************************		   **
				 		 **															   **
				 		 ****************************************************************
				 		 **************************************************************** 
				 		 ****************************************************************/
	
	
 /****************************************************************************************************************************
 *****************************************************************************************************************************/

/*****************************************************
--FUNCTION	::	FE_STV0903_GetRevision
--ACTION	::	Return current LLA version
--PARAMS IN	::	NONE
--PARAMS OUT::	NONE
--RETURN	::	Revision ==> Text string containing LLA version
--***************************************************/
ST_Revision_t FE_STV0903_GetRevision(void)
{
	return (RevisionSTV0903);
}

 /*****************************************************
--FUNCTION	::	FE_STV0903_Init
--ACTION	::	Initialisation of the STV0903 chip
--PARAMS IN	::	Handle	==>	Front End Handle
--PARAMS OUT::	NONE
--RETURN	::	Handle to STV0903
--***************************************************/
#if 0
FE_STV0903_Error_t	FE_STV0903_Init(FE_STV0903_InitParams_t		*pInit,FE_STV0903_Handle_t *Handle)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR; 
	FE_STV0903_InternalParams_t		*pParams = NULL;
	
	TUNER_Error_t	tunerError = TUNER_NO_ERR;

	STCHIP_Error_t	demodError;
	Demod_InitParams_t		DemodInitParams;	/* Demodulator chip initialisation parameters */
	STCHIP_Info_t			DemodChip;
	SAT_TUNER_InitParams_t		TunerInitParams; 
	/* Internal params structure allocation */
	Handle=Handle;
	#ifdef HOST_PC
		
		
		
		pParams = calloc(1,sizeof(FE_STV0903_InternalParams_t));
		(*Handle) = (FE_STV0903_Handle_t) pParams;
	#endif
	
	
	if(pParams != NULL)
	{
		/* Chip initialisation */
		#if defined(HOST_PC) && !defined(NO_GUI) 
			
				//pParams->hDemod  = DEMOD;
				//pParams->hTuner =  TUNER_A;
				//->Quartz  = EXTCLK;
				
				FE_STV0903_SetTunerType(pParams->hDemod,pInit->TunerHWControlType,pInit->Tuner_I2cAddr,pInit->TunerRefClock,pInit->TunerOutClkDivider);

		#else
				
				#ifdef CHIP_STAPI
					DemodInitParams.Chip = (pParams->hDemod); /*Change ///AG*/
				#else
					DemodInitParams.Chip = &DemodChip;
				#endif

				/* Demodulator (STV0903) */
				DemodInitParams.Chip = &DemodChip;
				DemodInitParams.NbDefVal = STV0903_NBREGS;
				DemodInitParams.Chip->RepeaterHost = NULL;
				DemodInitParams.Chip->RepeaterFn = NULL;
				DemodInitParams.Chip->Repeater = FALSE;
				DemodInitParams.Chip->I2cAddr = pInit->DemodI2cAddr;
				strcpy((char *)DemodInitParams.Chip->Name,pInit->DemodName);

				demodError=STV0903_Init(&DemodInitParams,&pParams->hDemod);
				
				TunerInitParams.Model=pInit->TunerModel;
				TunerInitParams.TunerName=pInit->TunerName;
				TunerInitParams.TunerI2cAddress=pInit->Tuner_I2cAddr;
				TunerInitParams.Reference=pInit->TunerRefClock;
				TunerInitParams.IF=0;
				if(pInit->TunerIQ_Inversion==FE_SAT_IQ_NORMAL)
					TunerInitParams.IQ_Wiring= TUNER_IQ_NORMAL; 
				else
					TunerInitParams.IQ_Wiring= TUNER_IQ_INVERT;
						
				/*TunerInit.BandSelect= */
				TunerInitParams.RepeaterHost=pParams->hDemod;
				TunerInitParams.RepeaterFn=FE_STV0903_RepeaterFn;
					
				tunerError=FE_Sat_TunerInit( (&TunerInitParams),&pParams->hTuner);
				
				if(demodError == CHIPERR_NO_ERROR) /*Check the demod error first*/
				{
					/*If no Error on the demod the check the Tuners*/
					if(tunerError == TUNER_NO_ERR) 
						error = FE_LLA_NO_ERROR;  /*if no error on the tuner*/
					else if(tunerError == TUNER_INVALID_HANDLE)
						error=FE_LLA_INVALID_HANDLE;   
					else if(tunerError == TUNER_TYPE_ERR)
						error=FE_LLA_BAD_PARAMETER;  /*if tuner type error*/
					else 
						error=FE_LLA_I2C_ERROR;
				}else
				{
					if(demodError == CHIPERR_INVALID_HANDLE)
						error=FE_LLA_INVALID_HANDLE;
					else
						error=FE_LLA_I2C_ERROR; 	
				}


		#endif
		
		if(pParams->hDemod != NULL) 
		{
			if(error==FE_LLA_NO_ERROR)
			{
				/*Read IC cut ID*/
				pParams->hDemod->ChipID=ChipGetOneRegister(pParams->hDemod,RSTV0903_MID);

				/*Tuner parameters*/
				pParams->TunerType=pInit->TunerHWControlType;
				pParams->TunerGlobal_IQ_Inv=pInit->TunerIQ_Inversion;
			
				pParams->RollOff=pInit->RollOff;
				#if defined(CHIP_STAPI) || defined(NO_GUI)
				pParams->Quartz = pInit->DemodRefClock;	/*Ext clock in Hz*/
			
				ChipSetField(pParams->hDemod,FSTV0903_TUN_IQSWAP,pInit->TunerIQ_Inversion);
				ChipSetField(pParams->hDemod,FSTV0903_ROLLOFF_CONTROL,pInit->RollOff);
			
				FE_STV0903_SetTS_Parallel_Serial(pParams->hDemod,pInit->PathTSClock);  /*Set TS1 and TS2 to serial or parallel mode */ 

				switch (pInit->TunerHWControlType)
				{
					case FE_SAT_SW_TUNER:
					default:
						FE_Sat_TunerSetReferenceFreq(pParams->hTuner, pInit->TunerRefClock); 
						FE_Sat_TunerSetOutputClock(pParams->hTuner,pInit->TunerOutClkDivider);
					
						FE_Sat_TunerSetGain(pParams->hTuner,pInit->TunerBasebandGain);

					break;
						
					case FE_SAT_AUTO_STB6000:
					case FE_SAT_AUTO_STB6100:
					case FE_SAT_AUTO_STV6110: 
						FE_STV0903_SetTunerType(pParams->hDemod,pInit->TunerHWControlType,pInit->Tuner_I2cAddr,pInit->TunerRefClock,pInit->TunerOutClkDivider);
						if(ChipGetField(pParams->hDemod,FSTV0903_TUN_ACKFAIL))
						{
							pParams->hTuner->Error=CHIPERR_I2C_NO_ACK;
							error=FE_LLA_I2C_ERROR; 
						}
					break;
			
				}
			
				FE_STV0903_SetMclk((FE_STV0903_Handle_t)pParams,135000000,pParams->Quartz); /*Set the Mclk value to 135MHz*/ 
				WAIT_N_MS(3); /*wait for PLL lock*/
				ChipSetOneRegister(pParams->hDemod,RSTV0903_SYNTCTRL,0x22);		/* switch the 900 to the PLL*/
 			
	 			#endif
			
				pParams->MasterClock = FE_STV0903_GetMclkFreq(pParams->hDemod,pParams->Quartz);	/*Read the cuurent Mclk*/
				
				if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) /*Check the error at the end of the Init*/
					error=FE_LLA_I2C_ERROR;
			}
		}  
		else
		{
			error=FE_LLA_INVALID_HANDLE;
		}
	}
	
	return error;
}
#endif
/*****************************************************
--FUNCTION	::	FE_STV0903_SetTSConfig
--ACTION	::	TS configuration 
--PARAMS IN	::	Handle		==>	Front End Handle
			::	pTSConfig	==> TS config parameters 	
			::	 
--PARAMS OUT::	TSSpeed_Hz	==> Current TS speed in Hz.
--RETURN	::	Error (if any)
--***************************************************/
#if 0
FE_STV0903_Error_t FE_STV0903_SetTSConfig(	FE_STV0903_Handle_t Handle ,FE_STV0903_TSConfig_t *pTSConfig,U32 *TSSpeed_Hz)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;      
	FE_STV0903_InternalParams_t	*pParams = NULL;
	
	U32 tsspeed;

	pParams = (FE_STV0903_InternalParams_t	*) Handle;
	if(pParams != NULL)
	{
		if ((pParams->hDemod->Error) || (pParams->hTuner->Error) )
			error=FE_LLA_I2C_ERROR; 
		else
		{
			if(pTSConfig->TSSyncByteEnable == FE_TS_SYNCBYTE_OFF)	  /* enable/Disable SYNC byte */
				ChipSetField(pParams->hDemod,FSTV0903_TSDEL_SYNCBYTE,1);
			else
				ChipSetField(pParams->hDemod,FSTV0903_TSDEL_SYNCBYTE,0);
			
			if(pTSConfig->TSParityBytes== FE_TS_PARITYBYTES_ON)	/*DVBS1 Data parity bytes enabling/disabling*/
				ChipSetField(pParams->hDemod,FSTV0903_TSINSDEL_RSPARITY,1);
			else
				ChipSetField(pParams->hDemod,FSTV0903_TSINSDEL_RSPARITY,0);
			
			if(pTSConfig->TSClockPolarity == FE_TS_RISINGEDGE_CLOCK)	/*TS clock Polarity setting : rising edge/falling edge*/
				ChipSetField(pParams->hDemod,FSTV0903_CLKOUT_XOR,0);
			else
				ChipSetField(pParams->hDemod,FSTV0903_CLKOUT_XOR,1);
		
		
			FE_STV0903_SetTS_Parallel_Serial(pParams->hDemod,pTSConfig->TSMode);  /*Set the TS to serial parallel or DVBCI mode */

			if(pTSConfig->TSSpeedControl== FE_TS_MANUAL_SPEED)
			{
				/*TS speed setting*/
				ChipSetField(pParams->hDemod,FSTV0903_TSFIFO_MANSPEED,3);
				switch(pTSConfig->TSMode)
				{
					case FE_TS_PARALLEL_PUNCT_CLOCK:
					case FE_TS_DVBCI_CLOCK:
						/*Formulat :
							TS_Speed_Hz = 4 * Msterclock_Hz / TSSPEED_REG
						*/
						tsspeed= (4 * pParams->MasterClock)/ pTSConfig->TSClockRate;
						if(tsspeed<=16)
							tsspeed=16; /*in parallel clock the TS speed is limited < MasterClock/4*/ 
					
						ChipSetOneRegister(pParams->hDemod,RSTV0903_TSSPEED,tsspeed); 
	
					break;
				
					case FE_TS_SERIAL_PUNCT_CLOCK:
					case FE_TS_SERIAL_CONT_CLOCK:
						/*Formulat :
							TS_Speed_Hz = 32 * Msterclock_Hz / TSSPEED_REG
						*/
						tsspeed= (16 * pParams->MasterClock)/ (pTSConfig->TSClockRate/2);
						if(tsspeed<=32)
							tsspeed=32; /*in serial clock the TS speed is limited <= MasterClock*/
					
						ChipSetOneRegister(pParams->hDemod,RSTV0903_TSSPEED,tsspeed); 
					break;
				}
			
			}
			else
			{
				ChipSetField(pParams->hDemod,FSTV0903_TSFIFO_MANSPEED,0);   	
			}
	
			switch(pTSConfig->TSMode)	/*D7/D0 permute if serial*/
			{
				case FE_TS_SERIAL_PUNCT_CLOCK:
				case FE_TS_SERIAL_CONT_CLOCK:
					if(pTSConfig->TSSwap==FE_TS_SWAP_ON)
						ChipSetField(pParams->hDemod,FSTV0903_TSFIFO_PERMDATA,1);
					else
						ChipSetField(pParams->hDemod,FSTV0903_TSFIFO_PERMDATA,0);  
				break;
			
				case FE_TS_PARALLEL_PUNCT_CLOCK:
				case FE_TS_DVBCI_CLOCK:
					ChipSetField(pParams->hDemod,FSTV0903_TSFIFO_PERMDATA,0); 
				default:
				break;
			}

			if(ChipGetField(pParams->hDemod,FSTV0903_TSFIFO_SERIAL)==0) /*Parallel mode*/
				*TSSpeed_Hz= (4 * pParams->MasterClock)/ChipGetOneRegister(pParams->hDemod,RSTV0903_TSSPEED);
			else	/*serial mode*/ 
				*TSSpeed_Hz= (16 * pParams->MasterClock)/(ChipGetOneRegister(pParams->hDemod,RSTV0903_TSSPEED)/2);
		
			if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) /*Check the error at the end of the function*/
				error=FE_LLA_I2C_ERROR; 
		}
	}
	else
		error=FE_LLA_INVALID_HANDLE; 
		
	
	return error;
}
#endif
/*****************************************************
--FUNCTION	::	FE_STV0903_GetSignalInfo
--ACTION	::	Return informations on the locked transponder
--PARAMS IN	::	Handle	==>	Front End Handle
--PARAMS OUT::	pInfo	==> Informations (BER,C/N,power ...)
--RETURN	::	Error (if any)
--***************************************************/


/*****************************************************
--FUNCTION	::	FE_STV0903_Status
--ACTION	::	Return locked status
--PARAMS IN	::	Handle	==>	Front End Handle
--PARAMS OUT::	NONE
--RETURN	::	Bool (locked or not)
--***************************************************/
#if 0
BOOL	FE_STV0903_Status(	FE_STV0903_Handle_t	Handle)
{

	FE_STV0903_InternalParams_t	*pParams = NULL;
	BOOL Locked=FALSE;
	FE_STV0903_SEARCHSTATE_t	demodState;
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR; 
	
	pParams = (FE_STV0903_InternalParams_t	*) Handle;
	
	if(pParams != NULL)
	{
		if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) 
		{
			error=FE_LLA_I2C_ERROR;
			Locked=FALSE;
		}
		else
		{
			demodState=ChipGetField(pParams->hDemod,FSTV0903_HEADER_MODE);
			switch(demodState)
			{
				case FE_SAT_SEARCH:
				case FE_SAT_PLH_DETECTED :
				default :
					Locked = FALSE;
				break;
		
				case FE_SAT_DVBS2_FOUND:
					Locked = ChipGetField(pParams->hDemod,FSTV0903_LOCK_DEFINITIF) &&
							 ChipGetField(pParams->hDemod,FSTV0903_PKTDELIN_LOCK) &&
							 ChipGetField(pParams->hDemod,FSTV0903_TSFIFO_LINEOK); 
				break;
					
				case FE_SAT_DVBS_FOUND:
					Locked = ChipGetField(pParams->hDemod,FSTV0903_LOCK_DEFINITIF) &&
						ChipGetField(pParams->hDemod,FSTV0903_LOCKEDVIT) &&
						ChipGetField(pParams->hDemod,FSTV0903_TSFIFO_LINEOK); 
				break;
			}
			
			if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) 
				error=FE_LLA_I2C_ERROR; 
		}
	}
	else
		Locked=FALSE;
		
	return Locked;
	
}
#endif
/*****************************************************
--FUNCTION	::	FE_STV0903_LDPCPowerTracking
--ACTION	::	track the LDPC activity and stop it when bad CNR < QEF 
				for the given modcode.
				In order to reduce the LDPC power consumption it  
				is recommended to call this function every 500ms from the upper layer.
--PARAMS IN	::	Handle	==>	Front End Handle
--PARAMS OUT::	NONE.
--RETURN	::	S32 0: LDPC stopped 
					1: LDPC running.
--***************************************************/
#if 0
S32 	FE_STV0903_LDPCPowerTracking(FE_STV0903_Handle_t Handle)
{

	FE_STV0903_InternalParams_t	*pParams = (FE_STV0903_InternalParams_t	*)Handle;
	S32 ldpcState = 1;
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR; 


	
	if(pParams != NULL)
	{
		if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) 
				error=FE_LLA_I2C_ERROR; 
		else
		{
			if(ChipGetField(pParams->hDemod,FSTV0903_HEADER_MODE) == FE_SAT_DVBS2_FOUND)
				if(FE_STV0903_LDPCPowerMonitoring(pParams->hDemod))
					ldpcState = 0;
				else
					ldpcState = 1;
			else
				ldpcState = 0;
			
			if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) 
				error=FE_LLA_I2C_ERROR; 

		}
	}
	return ldpcState; 
}

#endif
/*****************************************************
--FUNCTION	::	FE_STV0903_SetMclk
--ACTION	::	Set demod Master Clock  
--PARAMS IN	::	Handle	==>	Front End Handle
			::	Mclk 	==> demod master clock
			::	ExtClk 	==>	external Quartz
--PARAMS OUT::	NONE.
--RETURN	::	Error (if any)
--***************************************************/
FE_STV0903_Error_t FE_STV0903_SetMclk_E(U32 Mclk,U32 ExtClk)
{                                                                  			 // 135MHZ          8MHZ
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;
	U32 mDiv,clkSel;
	#if 1	
			clkSel= ((ChipGetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_SELX1RATIO)==1)?4:6);
			mDiv = ((clkSel*Mclk)/ExtClk)-1;// 135*6/16 = 
			ChipSetField_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,FSTV0903_M_DIV,mDiv);
			//pParams->MasterClock=FE_STV0903_GetMclkFreq_E(16000000);			
	#endif	
	return(error);
}

/*****************************************************
--FUNCTION	::	FE_STV0903_SetStandby
--ACTION	::	Set demod STANDBAY mode On/Off  
--PARAMS IN	::	Handle	==>	Front End Handle

-PARAMS OUT::	NONE.
--RETURN	::	Error (if any)
--***************************************************/
#if 0
FE_STV0903_Error_t FE_STV0903_SetStandby(FE_STV0903_Handle_t Handle, U8 StandbyOn)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;
	FE_STV0903_InternalParams_t	*pParams = (FE_STV0903_InternalParams_t	*)Handle;

	if(pParams != NULL)
	{
		if ((pParams->hDemod->Error) || (pParams->hTuner->Error) )
			error=FE_LLA_I2C_ERROR; 
		else
		{
			if(StandbyOn)
				ChipSetField(pParams->hDemod,FSTV0903_STANDBY,1);
			else
				ChipSetField(pParams->hDemod,FSTV0903_STANDBY,0);
		
			if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) /*Check the error at the end of the function*/
				error=FE_LLA_I2C_ERROR; 
		}
	}
	else
		error=FE_LLA_INVALID_HANDLE;

	return(error);
}
#endif

/*****************************************************
--FUNCTION	::	FE_STV0903_Search
--ACTION	::	Search for a valid transponder
--PARAMS IN	::	Handle	==>	Front End Handle
				pSearch ==> Search parameters
				pResult ==> Result of the search
--PARAMS OUT::	NONE
--RETURN	::	Error (if any)
--***************************************************/
#if 0
FE_STV0903_Error_t	FE_STV0903_Search(FE_STV0903_Handle_t	Handle,FE_STV0903_SearchParams_t	*pSearch,FE_STV0903_SearchResult_t	*pResult)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;
	FE_STV0903_InternalParams_t *pParams;
	
	if(Handle != NULL)
	{
		pParams = (FE_STV0903_InternalParams_t *) Handle; 

		if ((pParams->hDemod->Error) || (pParams->hTuner->Error) )
			error=FE_LLA_I2C_ERROR; 
		else
		{
			if(	(INRANGE(100000,pSearch->SymbolRate,70000000)) &&
				(INRANGE(1000000,pSearch->SearchRange,50000000))
				)
			{
				pParams->DemodSearchStandard= pSearch->Standard; 
				pParams->DemodSymbolRate= pSearch->SymbolRate;
				pParams->DemodSearchRange = pSearch->SearchRange;
				pParams->TunerFrequency=pSearch->Frequency; 
				pParams->DemodSearchAlgo=pSearch->SearchAlgo;
				pParams->DemodSearch_IQ_Inv=pSearch->IQ_Inversion;
				pParams->DemodPunctureRate=pSearch->PunctureRate;
				pParams->DemodModulation=pSearch->Modulation;
				pParams->DemodModcode=pSearch->Modcode; 

				if((FE_STV0903_Algo(pParams) == FE_SAT_RANGEOK) && (pParams->hDemod->Error == CHIPERR_NO_ERROR))
				{
					pResult->Locked = pParams->DemodResults.Locked;
					
					/* update results */
					pResult->Standard=pParams->DemodResults.Standard;
					pResult->Frequency = pParams->DemodResults.Frequency;			
					pResult->SymbolRate = pParams->DemodResults.SymbolRate;										
					pResult->PunctureRate = pParams->DemodResults.PunctureRate;
					pResult->ModCode=pParams->DemodResults.ModCode;
					pResult->Pilots=pParams->DemodResults.Pilots;
					pResult->FrameLength=pParams->DemodResults.FrameLength;
					pResult->Spectrum=pParams->DemodResults.Spectrum;
					pResult->RollOff= pParams->DemodResults.RollOff;
					pResult->Modulation=pParams->DemodResults.Modulation;

				}
				else
				{
					pResult->Locked = FALSE;
					
					switch(pParams->DemodError)
					{
						case FE_LLA_I2C_ERROR:					/*I2C error*/	
							error = FE_LLA_I2C_ERROR;
						break;
				
						case FE_LLA_NO_ERROR:
						default:
							error = FE_LLA_SEARCH_FAILED;
						break;
					}
				}
				
				if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) /*Check the error at the end of the function*/
					error=FE_LLA_I2C_ERROR; 
			}
			else
				error = FE_LLA_BAD_PARAMETER;
		}
	}
	else
		error=FE_LLA_INVALID_HANDLE;
		

	return error;

	
}
#endif
/*搜索算法
*/
// 07  58  e2 74 49 01 00 00  1250 000 
FE_STV0903_Error_t	FE_STV0903_Search_E(FE_STV0903_InternalParams_t_E* pParams,FE_STV0903_SearchParams_t	*pSearch,FE_STV0903_SearchResult_t	*pResult)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;
	FE_Sat_SIGNALTYPE_t  TMP;
	unsigned char aa =17;
	//FE_STV0903_InternalParams_t *pParams;
	//FE_STV0903_InternalParams_t_E pParams;
	//    0.1MHZ---70MHZ  搜索范围 1MHZ---50MHZ
	//INRANGE  SYMBOLRATE  (-  (0.1MHZ,70MHZ) , sybolrate (- (1MHZ,50MHZ)
	if(
		(INRANGE(100000,pSearch->SymbolRate,70000000)) &&
		(INRANGE(1000000,pSearch->SearchRange,50000000))
				)
			{
				pParams->DemodSearchStandard= pSearch->Standard; 
				pParams->DemodSymbolRate= pSearch->SymbolRate;
				pParams->DemodSearchRange = pSearch->SearchRange;
				pParams->TunerFrequency=pSearch->Frequency; 
				pParams->DemodSearchAlgo=pSearch->SearchAlgo;
				pParams->DemodSearch_IQ_Inv=pSearch->IQ_Inversion;
				pParams->DemodPunctureRate=pSearch->PunctureRate;
				pParams->DemodModulation=pSearch->Modulation;
				pParams->DemodModcode=pSearch->Modcode; 
				pParams->TunerType=FE_SAT_SW_TUNER;
				//			获得主频
	                     	//pParams->MasterClock = FE_STV0903_GetMclkFreq_E(pParams->Quartz);	
				// 			tuner 设置算法	
				TMP =FE_STV0903_Algo_E(pParams);//    获取锁定状态
				
				

						
				if(TMP == FE_SAT_RANGEOK)// && (pParams->hDemod->Error == CHIPERR_NO_ERROR))
				{
					pResult->Locked = pParams->DemodResults.Locked;
					/* update results */
					pResult->Standard=pParams->DemodResults.Standard;
					pResult->Frequency = pParams->DemodResults.Frequency;			
					pResult->SymbolRate = pParams->DemodResults.SymbolRate;										
					pResult->PunctureRate = pParams->DemodResults.PunctureRate;
					pResult->ModCode=pParams->DemodResults.ModCode;
					pResult->Pilots=pParams->DemodResults.Pilots;
					pResult->FrameLength=pParams->DemodResults.FrameLength;
					pResult->Spectrum=pParams->DemodResults.Spectrum;
					pResult->RollOff= pParams->DemodResults.RollOff;
					pResult->Modulation=pParams->DemodResults.Modulation;

				}
				else
				{
					pResult->Locked = FALSE;
					
					switch(pParams->DemodError)
					{
						case FE_LLA_I2C_ERROR:					/*I2C error*/	
							error = FE_LLA_I2C_ERROR;
						break;
				
						case FE_LLA_NO_ERROR:
						default:
							error = FE_LLA_SEARCH_FAILED;
						break;
					}
				}
			}
			else
				error = FE_LLA_BAD_PARAMETER;	
	return error;

	
}
/*****************************************************
--FUNCTION	::	FE_STV0903_DVBS2_SetGoldCode
--ACTION	::	Set the DVBS2 Gold Code
--PARAMS IN	::	Handle	==>	Front End Handle 
				U32		==>	cold code value (18bits)
--PARAMS OUT::	NONE
--RETURN	::	Error (if any )
--***************************************************/
#if 0
FE_STV0903_Error_t FE_STV0903_DVBS2_SetGoldCodeX(FE_STV0903_Handle_t	Handle,U32 GoldCode)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;
	FE_STV0903_InternalParams_t *pParams;
	

	if((void *)Handle != NULL)
	{
		pParams = (FE_STV0903_InternalParams_t *) Handle; 
		
		if(pParams != NULL)
		{
			if ((pParams->hDemod->Error) || (pParams->hTuner->Error) )
				error=FE_LLA_I2C_ERROR; 
			else
			{
				/*Gold code X setting*/
				ChipSetOneRegister(pParams->hDemod,RSTV0903_PLROOT2,(GoldCode >>16) & 0x3 ); 
				ChipSetOneRegister(pParams->hDemod,RSTV0903_PLROOT1,(GoldCode >>8) & 0xff ); 
				ChipSetOneRegister(pParams->hDemod,RSTV0903_PLROOT0,(GoldCode) & 0xff );
				
				if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) /*Check the error at the end of the function*/
					error=FE_LLA_I2C_ERROR; 
			}
		}
		else
			error = FE_LLA_INVALID_HANDLE;
	}
	else
		error = FE_LLA_INVALID_HANDLE; 
	
return error;
}
#endif

/*****************************************************
--FUNCTION	::	FE_STV0903_DiseqcTxInit
--ACTION	::	Set the diseqC Tx mode 
--PARAMS IN	::	Handle	==>	Front End Handle
			::	DiseqCMode	==> diseqc Mode : continues tone, 2/3 PWM, 3/3 PWM ,2/3 envelop or 3/3 envelop.
--PARAMS OUT::None
--RETURN	::	Error (if any)
--***************************************************/
#if 0
FE_STV0903_Error_t FE_STV0903_DiseqcInit(	FE_STV0903_Handle_t Handle ,FE_STV0903_DiseqC_TxMode DiseqCMode)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;      
	FE_STV0903_InternalParams_t	*pParams = NULL;

	pParams = (FE_STV0903_InternalParams_t	*) Handle;
	if(pParams != NULL)
	{
		if ((pParams->hDemod->Error) || (pParams->hTuner->Error) )
			error=FE_LLA_I2C_ERROR; 
		else
		{
			ChipSetField(pParams->hDemod,FSTV0903_DISTX_MODE,DiseqCMode);
		
			ChipSetField(pParams->hDemod,FSTV0903_DISEQC_RESET,1);
			ChipSetField(pParams->hDemod,FSTV0903_DISEQC_RESET,0);
		
			if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) /*Check the error at the end of the function*/
				error=FE_LLA_I2C_ERROR; 
		}
	}
	else
		error=FE_LLA_INVALID_HANDLE; 
		
	
	return error;
}
#endif

/*****************************************************
--FUNCTION	::	FE_STV0903_DiseqcSend
--ACTION	::	Read receved bytes from DiseqC FIFO 
--PARAMS IN	::	Handle	==>	Front End Handle
			::	Data	==> Table of bytes to send.
			::	NbData	==> Number of bytes to send.
--PARAMS OUT::None
--RETURN	::	Error (if any)
--***************************************************/
#if 0
FE_STV0903_Error_t FE_STV0903_DiseqcSend(	FE_STV0903_Handle_t Handle ,U8 *Data ,U32 NbData)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;      
	FE_STV0903_InternalParams_t	*pParams = NULL;
	S32 i=0; 
	
	pParams = (FE_STV0903_InternalParams_t	*) Handle;
	if(pParams != NULL)
	{
	
		if ((pParams->hDemod->Error) || (pParams->hTuner->Error) )
			error=FE_LLA_I2C_ERROR; 
		else
		{
			ChipSetField(pParams->hDemod,FSTV0903_DIS_PRECHARGE,1);
			while(i<NbData)
			{ 
				while(ChipGetField(pParams->hDemod,FSTV0903_FIFO_FULL));		/* wait for FIFO empty	*/
		
				ChipSetOneRegister(pParams->hDemod,RSTV0903_DISTXDATA,Data[i]);	/* send byte to FIFO :: WARNING don't use set field	!! */
				i++;
			}
			ChipSetField(pParams->hDemod,FSTV0903_DIS_PRECHARGE,0);
			i=0; 
			while((ChipGetField( pParams->hDemod,FSTV0903_TX_IDLE)!=1) && (i<10))
			{
				/*wait until the end of diseqc send operation */
				WAIT_N_MS(10);
				i++;     
			}
			if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) /*Check the error at the end of the function*/
				error=FE_LLA_I2C_ERROR; 
		}
	}
	else
		error=FE_LLA_INVALID_HANDLE; 
		
	
	return error;
}

#endif
/*****************************************************
--FUNCTION	::	FE_STV0903_DiseqcReceive
--ACTION	::	Read receved bytes from DiseqC FIFO 
--PARAMS IN	::	Handle	==>	Front End Handle
--PARAMS OUT::	Data	==> Table of received bytes.
			::	NbData	==> Number of received bytes.
--RETURN	::	Error (if any)
--***************************************************/
#if 0
FE_STV0903_Error_t FE_STV0903_DiseqcReceive(	FE_STV0903_Handle_t Handle ,U8 *Data ,U32 *NbData)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;      
	FE_STV0903_InternalParams_t	*pParams = NULL;
	S32 i=0;
	
	pParams = (FE_STV0903_InternalParams_t	*) Handle;
	if(pParams != NULL)
	{
		if ((pParams->hDemod->Error) || (pParams->hTuner->Error) )
			error=FE_LLA_I2C_ERROR; 
		else
		{
			*NbData=0;
			while((ChipGetField(pParams->hDemod,FSTV0903_RX_END)!=1)&&(i<10))
			{
				WAIT_N_MS(10);
				i++;
			}
			if(ChipGetField(pParams->hDemod,FSTV0903_RX_END))
			{
				*NbData=ChipGetField(pParams->hDemod,FSTV0903_FIFO_BYTENBR);
					for(i=0;i<(*NbData);i++)
						Data[i]=ChipGetOneRegister(pParams->hDemod,RSTV0903_DISRXDATA);
			}
			if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) /*Check the error at the end of the function*/
				error=FE_LLA_I2C_ERROR; 
		}
	}
	else
		error=FE_LLA_INVALID_HANDLE; 
		
	return error;
}
#endif
/*****************************************************
--FUNCTION	::	FE_STV0903_Set22KHzContinues
--ACTION	::	Set the diseqC Tx mode 
--PARAMS IN	::	Handle	==>	Front End Handle
--PARAMS OUT::None
--RETURN	::	Error (if any)
--***************************************************/
#if 0
FE_STV0903_Error_t FE_STV0903_Set22KHzContinues(	FE_STV0903_Handle_t Handle ,BOOL Enable)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;      
	FE_STV0903_InternalParams_t	*pParams = NULL;

	pParams = (FE_STV0903_InternalParams_t	*) Handle;
	if(pParams != NULL)
	{
		if ((pParams->hDemod->Error) || (pParams->hTuner->Error) )
			error=FE_LLA_I2C_ERROR; 
		else
		{
			if(Enable==TRUE)
			{
				ChipSetField(pParams->hDemod,FSTV0903_DISTX_MODE,0);/*Set the DiseqC mode to 22Khz continues tone*/
		
				ChipSetField(pParams->hDemod,FSTV0903_DISEQC_RESET,1); 
				ChipSetField(pParams->hDemod,FSTV0903_DISEQC_RESET,0);/*release DiseqC reset to enable the 22KHz tone*/
			}
			else
			{
				ChipSetField(pParams->hDemod,FSTV0903_DISTX_MODE,0);
		
				ChipSetField(pParams->hDemod,FSTV0903_DISEQC_RESET,1);/*maintain the DiseqC reset to disable the 22KHz tone*/
			}
			if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) /*Check the error at the end of the function*/
				error=FE_LLA_I2C_ERROR; 
		}
	}
	else
		error=FE_LLA_INVALID_HANDLE; 
		
	
	return error;
}
#endif


/***********************************************************************
**FUNCTION	::	FE_STV0903_SetupFSK
**ACTION	::	Setup FSK
**PARAMS IN	::	hChip		      -> handle to the chip
**				fskt_carrier	  -> FSK modulator carrier frequency  (Hz)
**				fskt_deltaf       -> FSK frequency deviation		  (Hz)
**				fskr_carrier	  -> FSK demodulator carrier frequency	(Hz)
**PARAMS OUT::	NONE
**RETURN	::	Symbol frequency
***********************************************************************/
#if 0
FE_STV0903_Error_t FE_STV0903_SetupFSK(FE_STV0903_Handle_t Handle,U32 TransmitCarrier,U32 ReceiveCarrier,U32 Deltaf)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;
	FE_STV0903_InternalParams_t *pParams;
	U32 fskt_carrier,fskt_deltaf,fskr_carrier;
	
	if((void *)Handle != NULL)
	{
		pParams = (FE_STV0903_InternalParams_t *) Handle; 
		
		if(pParams != NULL)
		{
			if ((pParams->hDemod->Error) || (pParams->hTuner->Error) )
				error=FE_LLA_I2C_ERROR; 
			else
			{
				ChipSetField(pParams->hDemod,FSTV0903_FSKT_KMOD,0x23);
			
				/*
					Formulat:
						FSKT_CAR =  2^20*transmit_carrier/MasterClock;
				
				*/
				fskt_carrier= ( 256 * TransmitCarrier ) / (pParams->MasterClock/4096);

				ChipSetField(pParams->hDemod,FSTV0903_FSKT_CAR2,((fskt_carrier >> 16) & 0x03));
				ChipSetField(pParams->hDemod,FSTV0903_FSKT_CAR1,((fskt_carrier >> 8) & 0xFF));
				ChipSetField(pParams->hDemod,FSTV0903_FSKT_CAR0,(fskt_carrier & 0xFF));


								/*
				Formulat:
						FSKT_DELTAF =  2^20*fskt_deltaf/MasterClock;
				
				*/

				fskt_deltaf=(1048576*Deltaf)/pParams->MasterClock;		 /* 2^20=1048576 */
			
				ChipSetField(pParams->hDemod,FSTV0903_FSKT_DELTAF1,((fskt_deltaf >> 8) & 0x0F));
				ChipSetField(pParams->hDemod,FSTV0903_FSKT_DELTAF0,(fskt_deltaf & 0xFF));				

				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKTCTRL,0x04);

				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRFC2,0x10);
				/*
					Formulat:
						FSKR_CAR =  2^20*receive_carrier/MasterClock;
				
				*/

				fskr_carrier=(256*ReceiveCarrier)/(pParams->MasterClock/4096);		 /* 2^20=1048576 */
			
				ChipSetField(pParams->hDemod,FSTV0903_FSKR_CAR2,((fskr_carrier >> 16) & 0x03));
				ChipSetField(pParams->hDemod,FSTV0903_FSKR_CAR1,((fskr_carrier >> 8) & 0xFF));
				ChipSetField(pParams->hDemod,FSTV0903_FSKR_CAR0,(fskr_carrier & 0xFF));

				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRK1,0x53);
				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRK2,0x94);
				
				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRAGCR,0x28); 
				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRAGC,0x5F);
				
				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRALPHA,0x13);
				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRPLTH1,0x90);
				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRPLTH0,0x45);
				

				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRDF1,0x9f);
				/*
					Formulat:
						FSKR_DELTAF =  2^20*fskt_deltaf/MasterClock;
				
				*/

				fskt_deltaf=(1048576*Deltaf)/pParams->MasterClock;		 /* 2^20=1048576 */
			
				ChipSetField(pParams->hDemod,FSTV0903_FSKR_DELTAF1,((fskt_deltaf >> 8) & 0x0F));
				ChipSetField(pParams->hDemod,FSTV0903_FSKR_DELTAF0,(fskt_deltaf & 0xFF));				


				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRSTEPP,0x02);
				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRSTEPM,0x4A);
				
				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRDET1,0);
				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRDET0,0x2F);
				
				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRDTH1,0);
				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRDTH0,0x55);
				
				ChipSetOneRegister(pParams->hDemod,RSTV0903_FSKRLOSS,0x1F);
			
				if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) /*Check the error at the end of the function*/
					error=FE_LLA_I2C_ERROR; 
			}
		}
		else
			error = FE_LLA_INVALID_HANDLE;
	}
	else
		error = FE_LLA_INVALID_HANDLE; 
	
return error;
}
#endif
/***********************************************************
**FUNCTION	::	FE_STV0903_EnableFSK
**ACTION	::	Enable - Disable FSK modulator /Demodulator
**PARAMS IN	::	hChip	 -> handle to the chip
**				mod_en	 -> FSK modulator on/off
**				demod_en -> FSK demodulator on/off	
**PARAMS OUT::	NONE
**RETURN	::	Symbol frequency
***********************************************************/
#if 0
FE_STV0903_Error_t FE_STV0903_EnableFSK(FE_STV0903_Handle_t Handle,BOOL EnableModulation,BOOL EnableDemodulation)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;
	FE_STV0903_InternalParams_t *pParams;
	EnableModulation=EnableModulation;
	if((void *)Handle != NULL)
	{
		pParams = (FE_STV0903_InternalParams_t *) Handle; 
		
		if(pParams != NULL)
		{
			if ((pParams->hDemod->Error) || (pParams->hTuner->Error) )
				error=FE_LLA_I2C_ERROR; 
			else
			{
				if(EnableDemodulation==TRUE) 
					ChipSetField(pParams->hDemod,FSTV0903_FSKT_MOD_EN,1);
				else
					ChipSetField(pParams->hDemod,FSTV0903_FSKT_MOD_EN,0);
	
				if (EnableDemodulation==FALSE)
				{
					ChipSetField(pParams->hDemod,FSTV0903_FSK_PON,0x01);
					ChipSetField(pParams->hDemod,FSTV0903_SEL_FSK,0x00);
				}
				else
				{
					ChipSetField(pParams->hDemod,FSTV0903_FSK_PON,0x00);
					ChipSetField(pParams->hDemod,FSTV0903_SEL_FSK,0x01);
				}
			
				if ((pParams->hDemod->Error) || (pParams->hTuner->Error) ) /*Check the error at the end of the function*/
					error=FE_LLA_I2C_ERROR; 

			}
		}
		else
			error = FE_LLA_INVALID_HANDLE;
	}
	else
		error = FE_LLA_INVALID_HANDLE; 
	
return error;
}
#endif
/*****************************************************
--FUNCTION	::	FE_STV0903_Term
--ACTION	::	Terminate STV0903 chip connection
--PARAMS IN	::	Handle	==>	Front End Handle
--PARAMS OUT::	NONE
--RETURN	::	Error (if any)
--***************************************************/
#if 0
FE_STV0903_Error_t	FE_STV0903_Term(FE_STV0903_Handle_t	Handle)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;
	FE_STV0903_InternalParams_t	*pParams = NULL;  
	
	pParams = (FE_STV0903_InternalParams_t	*) Handle;
	
	if(pParams != NULL)
	{
		#ifdef HOST_PC
			#ifdef NO_GUI
				ChipClose(pParams->hDemod);
				FE_Sat_TunerTerm(pParams->hTuner);
			#endif
		
			if(Handle)
				free(pParams);
		#endif
		
	}
	else
		error = FE_LLA_INVALID_HANDLE; 	
	
	return error;
}
#endif
/*****************************************************
--FUNCTION	::	FE_STV0903_SetReg
--ACTION	::	write one register  
--PARAMS IN	::	Handle	==>	Front End Handle
			::	Reg		==> register Index in the register Map
			::	Val 	==> Val to be writen
--PARAMS OUT::	NONE.
--RETURN	::	Error (if any)
--***************************************************/
#if 0
FE_STV0903_Error_t FE_STV0903_SetReg(FE_STV0903_Handle_t Handle, U16 Reg, U32 Val)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;
	FE_STV0903_InternalParams_t	*pParams = (FE_STV0903_InternalParams_t	*)Handle;

	if(pParams == NULL)
		error=FE_LLA_INVALID_HANDLE;
	else
	{
		ChipSetOneRegister(pParams->hDemod,Reg,Val);
		if (pParams->hDemod->Error)
			error=FE_LLA_I2C_ERROR; 
	}
	return(error);
}
#endif
/*****************************************************
--FUNCTION	::	FE_STV0903_GetReg
--ACTION	::	write one register  
--PARAMS IN	::	Handle	==>	Front End Handle
			::	Reg		==> register Index in the register Map
--PARAMS OUT::	Val		==> Read value.
--RETURN	::	Error (if any)
--***************************************************/
#if 0
FE_STV0903_Error_t FE_STV0903_GetReg(FE_STV0903_Handle_t Handle, U16 Reg, U32 *Val)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;
	FE_STV0903_InternalParams_t	*pParams = (FE_STV0903_InternalParams_t	*)Handle;

	if((pParams == NULL)||(Val==NULL))
		error=FE_LLA_INVALID_HANDLE;
	else
	{
		*Val=ChipGetOneRegister(pParams->hDemod,Reg);
		if (pParams->hDemod->Error)
			error=FE_LLA_I2C_ERROR; 
	}
	return(error);
}
#endif
/*****************************************************
--FUNCTION	::	FE_STV0903_SetField
--ACTION	::	write a value to a Field  
--PARAMS IN	::	Handle	==>	Front End Handle
			::	Reg		==> Field Index in the register Map
			::	Val 	==> Val to be writen
--PARAMS OUT::	NONE.
--RETURN	::	Error (if any)
--***************************************************/
#if 0
FE_STV0903_Error_t FE_STV0903_SetField(FE_STV0903_Handle_t Handle, U32 Field, S32 Val)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;
	FE_STV0903_InternalParams_t	*pParams = (FE_STV0903_InternalParams_t	*)Handle;

	if(pParams == NULL)
		error=FE_LLA_INVALID_HANDLE;
	else
	{
		ChipSetField(pParams->hDemod,Field,Val);
		if (pParams->hDemod->Error)
			error=FE_LLA_I2C_ERROR; 
	}
	return(error);
}
#endif

/*****************************************************
--FUNCTION	::	FE_STV0903_GetField
--ACTION	::	Read A Field  
--PARAMS IN	::	Handle	==>	Front End Handle
			::	Field	==> Field Index in the register Map
--PARAMS OUT::	Val		==> Read value.
--RETURN	::	Error (if any)
--***************************************************/
#if 0
FE_STV0903_Error_t FE_STV0903_GetField(FE_STV0903_Handle_t Handle, U32 Field, S32 *Val)
{
	FE_STV0903_Error_t error = FE_LLA_NO_ERROR;
	FE_STV0903_InternalParams_t	*pParams = (FE_STV0903_InternalParams_t	*)Handle;

	if((pParams == NULL)||(Val==NULL))
		error=FE_LLA_INVALID_HANDLE;
	else
	{
		*Val=ChipGetField(pParams->hDemod,Field);
		if (pParams->hDemod->Error)
			error=FE_LLA_I2C_ERROR; 
	}
	return(error);
}
#endif

