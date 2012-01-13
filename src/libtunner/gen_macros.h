#ifndef H_GEN_MACROS
	#define H_GEN_MACROS
	
	/* MACRO definitions */
	#define ABS(X) ((X)<0 ? (-X) : (X))
	#define MAX(X,Y) ((X)>=(Y) ? (X) : (Y))
	#define MIN(X,Y) ((X)<=(Y) ? (X) : (Y)) 
	#define INRANGE(X,Y,Z)  (((X<=Y) && (Y<=Z))||((Z<=Y) && (Y<=X))  ?  1 : 0)  
	
	#ifndef MAKEWORD 
		#define MAKEWORD(X,Y) ((X<<8)+(Y))
	#endif
	
	#define LSB(X) ((X & 0xFF))
	#define MSB(Y) ((Y>>8)& 0xFF)
	#define MMSB(Y) ((Y>>16)& 0xFF)
	
#endif
