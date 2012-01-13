 #ifndef H_STV0903UTIL
	#define H_STV0903UTIL
	
	

	#include "STV0903_Drv.h"
	#include "STV0903_init.h"
	#include "FE_Sat_Tuner.h" 
	/* Counter enum */
	typedef enum
	{
		FE_STV0903_COUNTER1 = 0,
		FE_STV0903_COUNTER2 = 1
	} FE_STV0903_ERRORCOUNTER;

	/*PER/BER enum*/
	typedef enum
	{
		FE_STV0903_BER,
		FE_STV0903_PER
	
	}FE_STV0903_BerPer;
	
	/*CNR estimation enum*/
	typedef enum
	{
		FE_STV0903_UNNORMDATAM,
		FE_STV0903_UNNORMDATAMSQ,
		FE_STV0903_NORMDATAM,
		FE_STV0903_NORMDATAMSQ,
	
		FE_STV0903_UNNORMPLHM,
		FE_STV0903_UNNORMPLHMSQ,
		FE_STV0903_NORMPLHM,
		FE_STV0903_NORMPLHMSQ
	
	}FE_STV0903_CnrRegUsed;
	
	/****************************************************************
						Util FUNCTIONS
	****************************************************************/

	
	STCHIP_Error_t FE_STV0903_RepeaterFn(STCHIP_Handle_t hChip,BOOL State);
	U32 FE_STV0903_GetMclkFreq(STCHIP_Handle_t hChip, U32 ExtClk);
	
	void FE_STV0903_SetTuner(STCHIP_Handle_t hChip,
				  TUNER_Handle_t hTuner,
				  FE_STV0903_Tuner_t TunerType,
				  U32 Frequency,
				  U32 Bandwidth
				  );


	void FE_STV0903_SetSymbolRate(STCHIP_Handle_t hChip,U32 MasterClock,U32 SymbolRate);
	BOOL FE_STV0903_WaitForLock(STCHIP_Handle_t hChip,S32 DemodTimeOut,S32 FecTimeOut) ;
	void FE_STV0903_SetTunerType(STCHIP_Handle_t hChip,
				  FE_STV0903_Tuner_t TunerType,
				  U8 I2cAddr,
				  U32 Reference,
				  U8 OutClkDiv
				  );
	
	U32 FE_STV0903_CarrierWidth(U32 SymbolRate, FE_STV0903_RollOff_t RollOff);
	
	S32 FE_STV0903_GetCarrierFrequency(STCHIP_Handle_t hChip,U32 MasterClock );
	U32 FE_STV0903_GetSymbolRate(STCHIP_Handle_t hChip,U32 MasterClock);
	S32 FE_STV0903_CarrierGetQuality(STCHIP_Handle_t hChip,FE_STV0903_LOOKUP_t *lookup);
	U32 FE_STV0903_GetErrorCount(STCHIP_Handle_t hChip,FE_STV0903_ERRORCOUNTER Counter);


	BOOL	FE_STV0903_LDPCPowerMonitoring(STCHIP_Handle_t hChip);
	void FE_STV0903_ActivateS2ModCode(STCHIP_Handle_t hChip);
	void FE_STV0903_StopALL_S2_Modcod(STCHIP_Handle_t hChip);

#endif
