#ifndef H_STFE_UTILITIES
	#define H_STFE_UTILITIES

	
	#include "stddefs.h"

	typedef void *InternalParamsPtr;		/*Handle to the FE */ 
		
	#define FE_LLA_MAXLOOKUPSIZE 500
	/* One point of the lookup table */
	typedef struct
	{
		unsigned int realval;	/*	real value */
		unsigned int  regval;		/*	binary value */
	} FE_LLA_LOOKPOINT_t;

	/*	Lookup table definition	*/
	typedef struct
	{
		unsigned char size;		/*	Size of the lookup table	*/
		FE_LLA_LOOKPOINT_t table[55]; 	/*	Lookup table	*/
	}FE_LLA_LOOKUP_t;

	typedef enum
	{
		FE_LLA_NO_ERROR = ST_NO_ERROR,
		FE_LLA_INVALID_HANDLE =ST_ERROR_INVALID_HANDLE ,
		FE_LLA_ALLOCATION =ST_ERROR_NO_MEMORY ,
		FE_LLA_BAD_PARAMETER = ST_ERROR_BAD_PARAMETER ,
		FE_LLA_ALREADY_INITIALIZED = ST_ERROR_BAD_PARAMETER,
		FE_LLA_I2C_ERROR = ST_ERROR_DEVICE_BUSY,
		FE_LLA_SEARCH_FAILED ,
		FE_LLA_TRACKING_FAILED,
		FE_LLA_TERM_FAILED

	} FE_LLA_Error_t;					/*Error Type*/

	typedef enum
	{
		TUNER_IQ_NORMAL = 1,
		TUNER_IQ_INVERT = -1
	}
	TUNER_IQ_t;

	typedef enum
	{
		TUNER_LOCKED,
		TUNER_NOTLOCKED
		 
	}
	TUNER_Lock_t;

	
	typedef enum
	{
		TUNER_NO_ERR = 0,
		TUNER_TYPE_ERR,
		TUNER_INVALID_HANDLE,
		TUNER_INVALID_REG_ID,     /* Using of an invalid register */
    	TUNER_INVALID_FIELD_ID,   /* Using of an Invalid field */
    	TUNER_INVALID_FIELD_SIZE, /* Using of a field with an invalid size */
    	TUNER_I2C_NO_ACK,         /* No acknowledge from the chip */
    	TUNER_I2C_BURST           /* Two many registers accessed in burst mode */
	} TUNER_Error_t;
	
	
	/****************************************************************
						TS CONFIG STRUCTURE
	 ****************************************************************/
	/*TS configuration enum*/
	
	typedef enum
	{								
		FE_TS_SERIAL_PUNCT_CLOCK,		/*Serial punctured clock : serial STBackend		*/
		FE_TS_SERIAL_CONT_CLOCK,		/*Serial continues clock 						*/
		FE_TS_PARALLEL_PUNCT_CLOCK,		/*Parallel punctured clock : Parallel STBackend	*/
		FE_TS_DVBCI_CLOCK				/*Parallel continues clock : DVBCI				*/
		
	} FE_TS_OutputMode_t;

	typedef enum
	{								
		FE_TS_MANUAL_SPEED,				/* TS Speed manual */		
		FE_TS_AUTO_SPEED				/* TS Speed automatic */
	} FE_TS_DataRateControl_t;

	typedef enum
	{								
		FE_TS_RISINGEDGE_CLOCK,			/* TS clock rising edge  */
		FE_TS_FALLINGEDGE_CLOCK			/* TS clock falling edge */
	} FE_TS_ClockPolarity_t;
	
	typedef enum
	{								
		FE_TS_SYNCBYTE_ON,				/* TS synchro byte  ON 	  */
		FE_TS_SYNCBYTE_OFF				/* delete TS synchro byte */
	} FE_TS_SyncByteEnable_t;
	

	typedef enum
	{
		FE_TS_PARITYBYTES_ON,			/* insert TS parity bytes */
		FE_TS_PARITYBYTES_OFF			/* delete TS parity bytes */
	} FE_TS_ParityBytes_t;
	
	typedef enum
	{
		FE_TS_SWAP_ON,					/* swap TS bits LSB <-> MSB */ 
		FE_TS_SWAP_OFF					/* don't swap TS bits LSB <-> MSB */ 
	} FE_TS_Swap_t;
	
	typedef enum
	{
		FE_TS_SMOOTHER_ON,				/* enable TS smoother or fifo */
		FE_TS_SMOOTHER_OFF				/* disable TS smoother or fifo */
	} FE_TS_Smoother_t;
 
	 typedef struct
	{
		FE_TS_OutputMode_t			TSMode;				/* TS Serial pucntured, serial continues, parallel punctured or DVBSI			 		*/
		FE_TS_DataRateControl_t		TSSpeedControl;		/* TS speed control : manual or automatic 		*/ 
		FE_TS_ClockPolarity_t		TSClockPolarity;	/* TS clock polarity : rising edge/falling edge	*/
		U32 						TSClockRate;		/* TS clock rate in Hz if manual mode			*/
		FE_TS_SyncByteEnable_t		TSSyncByteEnable;	/* TS sync byte : enable/disabe					*/	
		FE_TS_ParityBytes_t			TSParityBytes;		/* TS parity bytes Enable/Disable				*/
		FE_TS_Swap_t				TSSwap;				/* TS bits swap/ unswap							*/
		FE_TS_Smoother_t			TSSmoother;			/* TS smoother enable/disable					*/

	} FE_TS_Config_t;


	S32 PowOf4(S32 number);
	S32 PowOf2(S32 number);
	U32 GetPowOf2(S32 number);
	U32 BinaryFloatDiv(U32 n1, U32 n2, S32 precision);
	S32 Get2Comp(S32 a, S32 width);
	S32 XtoPowerY(S32 Number, U32 Power);
	S32 RoundToNextHighestInteger(S32 Number,U32 Digits);
	U32 Log2Int(S32 number);
	S32 SqrtInt(S32 Sq);
	S32 Log10Int(S32 logarg);
#endif
