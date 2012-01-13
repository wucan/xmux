#ifndef H_SAT_NULL_TUNER
	#define H_SAT_NULL_TUNER
	
	#include "chip.h"
	#include "FE_Sat_Tuner.h"
	
		
	#if defined(CHIP_STAPI) /*For STAPI use only*/
		//#include "dbtypes.h"	/*for Install function*/
		//#include "sttuner.h"    /*for Install Function*/
	#endif

	#define SAT_NULL_NBREGS	1
	#define SAT_NULL_NBFIELDS	1	
	
	/* functions --------------------------------------------------------------- */

	/* create instance of tuner register mappings */
	
	TUNER_Error_t SAT_NULL_TunerInit(void *pTunerInit,STCHIP_Handle_t *TunerHandle);
								
	TUNER_Error_t SAT_NULL_TunerSetFrequency(STCHIP_Handle_t hTuner,U32 Frequency);
	U32 SAT_NULL_TunerGetFrequency(STCHIP_Handle_t hTuner); 
	TUNER_Error_t SAT_NULL_TunerSetBandwidth(STCHIP_Handle_t hTuner,U32 Bandwidth);
	U32 SAT_NULL_TunerGetBandwidth(STCHIP_Handle_t hTuner);
	BOOL SAT_NULL_TunerGetStatus(STCHIP_Handle_t hTuner);
	TUNER_Error_t SAT_NULL_TunerWrite(STCHIP_Handle_t hTuner); 
	TUNER_Error_t SAT_NULL_TunerRead(STCHIP_Handle_t hTuner);
	TUNER_Error_t SAT_NULL_TunerSetGain(STCHIP_Handle_t hTuner,S32 Gain);
	S32           SAT_NULL_TunerGetGain(STCHIP_Handle_t hTuner);
	TUNER_Error_t SAT_NULL_TunerSetOutputClock(STCHIP_Handle_t hTuner, S32 Divider);
	
	TUNER_Error_t SAT_NULL_TunerSetAttenuator(STCHIP_Handle_t hTuner, BOOL AttenuatorOn);
	BOOL SAT_NULL_TunerGetAttenuator(STCHIP_Handle_t hTuner);

	
	SAT_TUNER_Model_t	SAT_NULL_TunerGetModel(STCHIP_Handle_t hTuner);  
	void			SAT_NULL_TunerSetIQ_Wiring(STCHIP_Handle_t hTuner, S8 IQ_Wiring);
	S8		SAT_NULL_TunerGetIQ_Wiring(STCHIP_Handle_t hTuner);
	void			SAT_NULL_TunerSetReferenceFreq(STCHIP_Handle_t hTuner, U32 Reference);
	U32				SAT_NULL_TunerGetReferenceFreq(STCHIP_Handle_t hTuner);
	void			SAT_NULL_TunerSetIF_Freq(STCHIP_Handle_t hTuner, U32 IF);
	U32				SAT_NULL_TunerGetIF_Freq(STCHIP_Handle_t hTuner);
	void SAT_NULL_TunerSetBandSelect(STCHIP_Handle_t hChip, U8 BandSelect);
	U8 SAT_NULL_TunerGetBandSelect(STCHIP_Handle_t hChip);

	TUNER_Error_t SAT_NULL_TunerTerm(STCHIP_Handle_t hTuner);
	
	
	#if defined(CHIP_STAPI) /*For STAPI use only*/
		//ST_ErrorCode_t STTUNER_TUNER_SAT_NULL_Install(STTUNER_tunerDbase_t *Tuner, STTUNER_TunerType_t TunerType);
		//ST_ErrorCode_t STTUNER_TUNER_SAT_NULL_Uninstall(STTUNER_tunerDbase_t *Tuner);
	#endif

#endif
