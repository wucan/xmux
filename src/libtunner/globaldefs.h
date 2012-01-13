#ifndef GLOBALDEFS
	#define GLOBALDEFS
         #define CHIP_STAPI
	//#ifdef CHIP_STAPI
	//	//#include "stcommon.h" /*changed ///AG 3*/
	//	#ifdef HOST_PC
	//		#undef HOST_PC
	//	#endif
	//#else
	//	#define HOST_PC
		
	//#endif
	
	#include "gen_macros.h"
	#include "gen_types.h"
	#include "gen_csts.h"
	#include "gen_spcar.h"
	#include "stddefs.h"
	#include "STFE_utilities.h"
	
	//#ifdef HOST_PC
	//	#include <utility.h> 
//	#else
		//#define WAIT_N_MS(X) STOS_TaskDelayUs(X *1000)/*STTUNER delay function*/
		//#define WAIT_N_MS(X) while(X){X--;}/*STTUNER delay function*/
	//#endif
#endif
	
	
