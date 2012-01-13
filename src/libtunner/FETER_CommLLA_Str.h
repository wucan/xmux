#ifndef FETER_COMMLLA_STR_H
	#define FETER_COMMLLA_STR_H

	/*************************************************************
								INCLUDES
	*************************************************************/	
	#include "stddefs.h"    /* Standard definitions */
	
	#include "FE_Ter_Tuner.h"
 
	/****************************************************************
		Structures/typedefs common to all cable demodulators
	****************************************************************/
		#define AGC_LOCK_FLAG   1<<5
	#define SYR_LOCK_FLAG	1<<6
	#define TPS_LOCK_FLAG	1<<7
	#define CHANNEL_SIZE 8 /* for an 8MHz channel wide OFDM channel */

	/* for previous lock (during scan)*/
	#define NO_LOCK		   0
	#define LOCK_OK	   1

	/*options for current lock trial*/
	#define NINV 0
	#define INV 1

		
	#define PAL			1
	#define GLOBAL		1<<1
	
	#define NUM_CHANNELS 40 
	
	#define STEP  166
	
	/* channel offset constant */
	#define CENTER_OFFSET 1<<1
	#define RIGHT_OFFSET 1<<2
	#define CARRIER 	240	
	/* end channel offset constant */
		/* type of modulation (common) */
	typedef enum
	{
		NOAGC=0,
		AGCOK=5,
		NOTPS=6,
		TPSOK=7,
		NOSYMBOL=8,
		BAD_CPQ=9,
		PRFOUNDOK=10,
		NOPRFOUND=11,
		LOCKOK=12,
		NOLOCK=13,
		SYMBOLOK=15,
		CPAMPOK=16,
		NOCPAMP=17,
		SWNOK=18
		
	} FE_TER_SignalStatus_t;

	typedef enum
	{
		FE_NO_ERROR,
	    FE_INVALID_HANDLE,
	    FE_BAD_PARAMETER,
	    FE_MISSING_PARAMETER,
	    FE_ALREADY_INITIALIZED,
	    FE_I2C_ERROR,
	    FE_SEARCH_FAILED,
	    FE_TRACKING_FAILED,
	    FE_TERM_FAILED
	} FE_TER_Error_t;
	
	
/*To make similar structures between sttuner and LLA and removed dependency of sttuner  SS*/	
	typedef enum
	{
        FE_MOD_NONE   = 0x00,  /* Modulation unknown */
        FE_MOD_ALL    = 0x3FF, /* Logical OR of all MODs */
        FE_MOD_QPSK   = 1,
        FE_MOD_8PSK   = (1 << 1),
        FE_MOD_QAM    = (1 << 2),
        FE_MOD_4QAM   = (1 << 3),
        FE_MOD_16QAM  = (1 << 4),
        FE_MOD_32QAM  = (1 << 5),
        FE_MOD_64QAM  = (1 << 6),
        FE_MOD_128QAM = (1 << 7),
        FE_MOD_256QAM = (1 << 8),
        FE_MOD_BPSK   = (1 << 9),
        FE_MOD_16APSK,
        FE_MOD_32APSK,
        FE_MOD_8VSB  =  (1 << 10)
	} FE_TER_Modulation_t;
	

	typedef enum 
	{
		MODE_2K,
		MODE_8K,
		MODE_4K
	}
	FE_TER_Mode_t;
	
	typedef enum 
	{
		GUARD_1_32,               /* Guard interval = 1/32 */ 
		GUARD_1_16,               /* Guard interval = 1/16 */ 
		GUARD_1_8,                /* Guard interval = 1/8  */ 
		GUARD_1_4                 /* Guard interval = 1/4  */
	}
	FE_TER_Guard_t;

	typedef enum
	{
		HIER_ALPHA_NONE,              /* Regular modulation */ 
		HIER_ALPHA_1,                 /* Hierarchical modulation a = 1*/
		HIER_ALPHA_2,                 /* Hierarchical modulation a = 2*/
		HIER_ALPHA_4                  /* Hierarchical modulation a = 4*/
	}
	FE_TER_Hierarchy_ALpha_t;

typedef enum 
    {
        HIER_NONE,		/*Hierarchy None*/                   
        HIER_LOW_PRIO,  	/*Hierarchy : Low Priority*/        
        HIER_HIGH_PRIO,    	/*Hierarchy : High Priority*/     
        HIER_PRIO_ANY   	/*Hierarchy  :Any*/          
    } FE_TER_Hierarchy_t;

	typedef enum
	{
		INVERSION_NONE = 0,
		INVERSION      = 1,
		INVERSION_AUTO = 2,
		INVERSION_UNK  = 4
	}
	FE_TER_Spectrum_t;
	
	
	typedef enum 
	{
		FORCENONE  = 0,
		FORCE_M_G		= 1
	}
	FE_TER_Force_t;


	typedef enum 
	{
		CHAN_BW_6M  = 6,
		CHAN_BW_7M  = 7,
		CHAN_BW_8M  = 8
	} 
	FE_TER_ChannelBW_t;

	typedef enum 
    {
        FEC_NONE = 0x00,    /* no FEC rate specified */
        FEC_ALL = 0xFF,     /* Logical OR of all FECs */
        FEC_1_2 = 1,
        FEC_2_3 = (1 << 1),
        FEC_3_4 = (1 << 2),
        FEC_4_5 = (1 << 3),
        FEC_5_6 = (1 << 4),
        FEC_6_7 = (1 << 5),
        FEC_7_8 = (1 << 6),
        FEC_8_9 = (1 << 7)
    }
    FE_TER_FECRate_t;
		
	
	
	
	
	typedef enum
	{
		FE_TPS_1_2	=	0,
		FE_TPS_2_3	=	1,   
		FE_TPS_3_4	=	2,   
		FE_TPS_5_6	=	3,   
		FE_TPS_7_8	=	4   
	} FE_TER_Rate_TPS_t;
	
	typedef enum
	{
		FE_1_2	=	0,
		FE_2_3	=	1,   
		FE_3_4	=	2,   
		FE_5_6	=	3,   
		FE_6_7	=	4,
		FE_7_8	=	5   
	} FE_TER_Rate_t;

	

	
	
	typedef enum
	{
		FE_NO_FORCE =0,
		FE_FORCE_PR_1_2 = 1 ,
		FE_FORCE_PR_2_3 = 1<<1,
		FE_FORCE_PR_3_4 = 1<<2,
		FE_FORCE_PR_5_6 = 1<<3 ,
		FE_FORCE_PR_7_8 = 1<<4
	}FE_TER_Force_PR_t ;
	


	typedef enum
	{
		NOT_FORCED  = 0,
		WAIT_TRL	= 1,
		WAIT_AGC	= 2,
		WAIT_SYR	= 3,
		WAIT_PPM	= 4,
		WAIT_TPS	= 5,
		MONITOR_TPS	= 6,
		RESERVED	= 7
	}FE_TER_State_Machine_t ;
	
	
	typedef enum
	{
		NORMAL_IF_TUNER=0, 
		LONGPATH_IF_TUNER=1,
		IQ_TUNER=2 
		
	}FE_TER_IF_IQ_Mode;
	
	typedef enum
	{
		FE_362_PARITY_ON,
		FE_362_PARITY_OFF
	} FE_TER_DataParity_t;
	
	typedef enum
	{
        FE_TER_CLOCK_POLARITY_DEFAULT ,  /*Clock polarity default*/
		FE_TER_CLOCK_POLARITY_FALLING  ,       /*Clock polarity in rising edge*/
		FE_TER_CLOCK_POLARITY_RISING  ,     /*Clock polarity in falling edge*/ 
		FE_TER_CLOCK_NONINVERTED  ,       /*Non inverted*/
		FE_TER_CLOCK_INVERTED       /*inverted*/   		          
	} FE_TER_DataClockPolarity_t;

	typedef enum
	{								
		FE_TS_MODE_DEFAULT,    /* TS output not changeable */
        FE_TS_MODE_SERIAL,     /* TS output serial */
        FE_TS_MODE_PARALLEL,   /* TS output parallel */
        FE_TS_MODE_DVBCI       /* TS output DVB-CI */
	} FE_TER_TSOutputMode_t;
	
	typedef enum
	{
		FE_TER_PARALLEL_CLOCK,
		FE_TER_SERIAL_CLOCK
	}	
		FE_TER_Clock_t;
	/****************************************************************
						INIT STRUCTURES
	 ****************************************************************/


	typedef struct
	{
		char 					DemodName[20];	/* STB0297E name */
		U8						DemodI2cAddr;	/* STB0297E I2c address */
        TUNER_Model_t			TunerModel;		/* Tuner model */
        char 					TunerName[20];  /* Tuner name */
		U8						TunerI2cAddr;	/* Tuner I2c address */
		FE_TER_DataClockPolarity_t		ClockPolarity;    	    /* Parity bytes */
		FE_TER_TSOutputMode_t	PathTSMode;		/* TS Format */
		FE_TER_Clock_t			PathTSClock;
		S32						Crystal_Hz;		/* XTAL value */
		U32						TunerIF_kHz;	/* Tuner IF value */
	}FE_TER_InitParams_t;
	
	typedef struct
	{
		FE_TER_SignalStatus_t	SignalType;		/*	Type of founded signal	*/
		FE_TER_FECRate_t		PunctureRate;	/*	Puncture rate found	*/
		double 					Frequency;		/*	Transponder frequency (KHz)	*/
		FE_TER_Mode_t			Mode;			/*	Mode 2K or 8K	*/
		FE_TER_Guard_t			Guard;			/*	Guard interval 	*/
		FE_TER_Modulation_t 	Modulation;		/*  modulation   */
		FE_TER_Hierarchy_t		hier;
		FE_TER_Rate_TPS_t		HPRate;
		FE_TER_Rate_TPS_t		LPRate;
		FE_TER_Rate_TPS_t		pr;
		U32 SymbolRate; 						/*	Symbol rate (Bds)	*/
	} FE_TER_InternalResults_t;

	/****************************************************************
	        					SEARCH STRUCTURES
	 ****************************************************************/

	typedef struct
	{
		U32                 Frequency;
		FE_TER_IF_IQ_Mode   IF_IQ_Mode; 
		FE_TER_Mode_t       Mode;
		FE_TER_Guard_t      Guard;
		FE_TER_Force_t      Force;
		FE_TER_Hierarchy_t  Hierarchy;    
		FE_TER_Spectrum_t   Inv;
		FE_TER_ChannelBW_t  ChannelBW;
		S8                  EchoPos;
		U32 				SearchRange;	
	} FE_TER_SearchParams_t;


	typedef struct
	{
		U32 Frequency;
		U32 Agc_val;	
		FE_TER_Mode_t 			Mode;
		FE_TER_Guard_t 			Guard;
		FE_TER_Modulation_t 	Modulation;		/*modulation*/
		FE_TER_Hierarchy_t		hier;
		FE_TER_Spectrum_t		Sense;	/*0 spectrum not inverted  */
		FE_TER_Hierarchy_ALpha_t Hierarchy_Alpha; /** Hierarchy Alpha level used for reporting**/
		FE_TER_Rate_TPS_t		HPRate;
		FE_TER_Rate_TPS_t		LPRate;
		FE_TER_Rate_TPS_t		pr;
		FE_TER_FECRate_t		FECRates;
		FE_TER_State_Machine_t	State;
		S8 						Echo_pos;
		FE_TER_SignalStatus_t	SignalStatus;
		BOOL 					Locked;
		int 					ResidualOffset;
	} FE_TER_SearchResult_t;

	/************************
		INFO STRUCTURE
	************************/
	typedef struct
	{
		BOOL Locked;						/* Channel locked						*/
		U32 Frequency;						/* Channel frequency (in KHz)			*/
		U32 SymbolRate;						/* Channel symbol rate  (in Mbds)		*/
		FE_TER_Modulation_t Modulation;	/* modulation							*/
		FE_TER_Spectrum_t SpectInversion;	/* Spectrum Inversion					*/
		S32 Power_dBmx10;					/* Power of the RF signal (dBm x 10)	*/			
		U32	CN_dBx10;						/* Carrier to noise ratio (dB x 10)		*/
		U32	BER;							/* Bit error rate (x 10000)				*/
	
	} FE_TER_SignalInfo_t;

	typedef struct
	{
	U32 Frequency ;
	FE_TER_SearchResult_t Result;
	} FE_TER_Scan_Result_t;
	

	
#endif
