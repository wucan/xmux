#ifndef FECAB_COMMLLA_STR_H
	#define FECAB_COMMLLA_STR_H

	/*************************************************************
								INCLUDES
	*************************************************************/	
	#include "stddefs.h"    /* Standard definitions */
	#include "FE_Cab_Tuner.h"
 
	/****************************************************************
		Structures/typedefs common to all cable demodulators
	****************************************************************/
	typedef enum
	{
	  	FE_CAB_MOD_QAM4,
	  	FE_CAB_MOD_QAM16,
	  	FE_CAB_MOD_QAM32,
	  	FE_CAB_MOD_QAM64,
	  	FE_CAB_MOD_QAM128,
	  	FE_CAB_MOD_QAM256,
	  	FE_CAB_MOD_QAM512,
	  	FE_CAB_MOD_QAM1024
	} FE_CAB_Modulation_t;

	typedef enum
	{
		FE_CAB_SPECT_NORMAL,
		FE_CAB_SPECT_INVERTED
	}FE_CAB_SpectInv_t;
	
	typedef struct
	{
		U32 Frequency_kHz;					/* channel frequency (in kHz)		*/
		U32 SymbolRate_Bds;					/* channel symbol rate  (in bds)	*/
		U32 SearchRange_Hz;					/* range of the search (in Hz) 		*/	
		U32 SweepRate_Hz;					/* Sweep Rate (in Hz)		 		*/	
		FE_CAB_Modulation_t Modulation;		/* modulation						*/
		U8 FECType;							/* FEC when a multi FEC is used		*/
	} FE_CAB_SearchParams_t;


	typedef struct
	{
		BOOL Locked;						/* channel found	 					*/
		U32	Frequency_kHz;					/* found frequency (in kHz)				*/
		U32 SymbolRate_Bds;					/* found symbol rate (in Bds)			*/
		FE_CAB_Modulation_t Modulation;		/* modulation							*/
		FE_CAB_SpectInv_t SpectInv;			/* Spectrum Inversion					*/
		U8 Interleaver;						/* Interleaver in FEC B mode			*/
	} FE_CAB_SearchResult_t;

	/************************
		INFO STRUCTURE
	************************/
	typedef struct
	{
		BOOL Locked;						/* Channel locked						*/
		U32 Frequency;						/* Channel frequency (in KHz)			*/
		U32 SymbolRate;						/* Channel symbol rate  (in Mbds)		*/
		FE_CAB_Modulation_t Modulation;		/* modulation							*/
		FE_CAB_SpectInv_t SpectInversion;	/* Spectrum Inversion					*/
		S32 Power_dBmx10;					/* Power of the RF signal (dBm x 10)	*/			
		U32	CN_dBx10;						/* Carrier to noise ratio (dB x 10)		*/
		U32	BER;							/* Bit error rate (x 10000000)			*/
		U8 Interleaver;						/* Interleaver in FEC B mode			*/
		#ifdef HOST_PC
			double BER_dbl;					/* BER in double for GUI and DLL		*/
			double CN_dB_dbl;				/* MER in dB in double for GUI and DLL	*/
		#endif
	} FE_CAB_SignalInfo_t;

#endif
	

