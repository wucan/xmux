#ifndef H_FE_SAT_TUNER
	#define H_FE_SAT_TUNER
	
	#include "chip.h"
	#include "STFE_utilities.h"
	

	/* structures -------------------------------------------------------------- */
	typedef enum
	{
		SAT_TUNER_NULL = 0,		/* unknown tuner */
		TUNER_STB6000,
		TUNER_STB6100,		/* RF magic STB6100 tuner */
		TUNER_STV6110,		/* 6110 tuner */
		TUNER_STV6130		/* 6130 tuner */
	}
	SAT_TUNER_Model_t;
	
	typedef enum
	{
		LBRFA_HBRFB = 0,	/*Low band on RFA, High band on RFB*/
		LBRFB_HBRFA = 1	/*Low band on RFB, High band on RFA*/
	}
	SAT_TUNER_WIDEBandS_t;		/* for wide band tuner 6130 */

    typedef struct
    {
		SAT_TUNER_Model_t Model;    	/* Tuner model */
		ST_String_t TunerName;		/* Tuner name */
		U8 TunerI2cAddress;			/* Tuner I2C address */
		U32            Reference;	/* reference frequency (Hz) */ 
		U32            IF;       	/* Intermediate frequency (KHz) */
		TUNER_IQ_t    IQ_Wiring; 	/* hardware I/Q invertion */
		SAT_TUNER_WIDEBandS_t	BandSelect;	/* for wide band tuner (6130 like)hardware band wiring */
		STCHIP_Handle_t RepeaterHost;
		STCHIP_RepeaterFn_t RepeaterFn;

		#ifdef CHIP_STAPI
			U32 TopLevelHandle; /*Used in STTUNER : Change ///AG*/
		#endif
		
		void *additionalParams;
    }
    SAT_TUNER_InitParams_t;   

    typedef SAT_TUNER_InitParams_t *SAT_TUNER_Params_t;

	/* functions --------------------------------------------------------------- */

	/* create instance of tuner register mappings */
	
	//TUNER_Error_t FE_Sat_TunerInit(void *pTunerInit,STCHIP_Handle_t *TunerHandle);
								
	TUNER_Error_t FE_Sat_TunerSetFrequency(STCHIP_Handle_t hTuner,U32 Frequency);
	U32 FE_Sat_TunerGetFrequency(STCHIP_Handle_t hTuner); 
	//TUNER_Error_t FE_Sat_TunerSetBandwidth(STCHIP_Handle_t hTuner,U32 Bandwidth);
	U32 FE_Sat_TunerGetBandwidth(STCHIP_Handle_t hTuner);
	BOOL FE_Sat_TunerGetStatus(STCHIP_Handle_t hTuner);
	TUNER_Error_t FE_Sat_TunerWrite(STCHIP_Handle_t hTuner); 
	TUNER_Error_t FE_Sat_TunerRead(STCHIP_Handle_t hTuner);
	TUNER_Error_t FE_Sat_TunerSetGain(STCHIP_Handle_t hTuner,S32 Gain);
	S32 FE_Sat_TunerGetGain(STCHIP_Handle_t hTuner);
	TUNER_Error_t FE_Sat_TunerSetOutputClock(STCHIP_Handle_t hTuner, S32 Divider);
	
	TUNER_Error_t FE_Sat_TunerSetAttenuator(STCHIP_Handle_t hTuner, BOOL AttenuatorOn); 
	BOOL FE_Sat_TunerGetAttenuator(STCHIP_Handle_t hTuner);

	SAT_TUNER_Model_t	FE_Sat_TunerGetModel(STCHIP_Handle_t hTuner);  
	void			FE_Sat_TunerSetIQ_Wiring(STCHIP_Handle_t hTuner, S8 IQ_Wiring);/*changed to retain common function prototypes ///AG*/ 
	S8		FE_Sat_TunerGetIQ_Wiring(STCHIP_Handle_t hTuner);
	void			FE_Sat_TunerSetReferenceFreq(STCHIP_Handle_t hTuner, U32 Reference);
	U32				FE_Sat_TunerGetReferenceFreq(STCHIP_Handle_t hTuner);
	void			FE_Sat_TunerSetIF_Freq(STCHIP_Handle_t hTuner, U32 IF);
	U32				FE_Sat_TunerGetIF_Freq(STCHIP_Handle_t hTuner);
	void FE_Sat_TunerSetBandSelect(STCHIP_Handle_t hChip, U8 BandSelect);
	U8 FE_Sat_TunerGetBandSelect(STCHIP_Handle_t hChip);

	TUNER_Error_t FE_Sat_TunerTerm(STCHIP_Handle_t hTuner);
#endif
