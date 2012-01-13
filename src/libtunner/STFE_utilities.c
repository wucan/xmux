/* -------------------------------------------------------------------------
File Name: STFE_utilities.c

Description: Integer compute utilities functions       


author: FE APPL LAB
---------------------------------------------------------------------------- */


/* includes ---------------------------------------------------------------- */
#ifndef ST_OSLINUX 
	#include "stdlib.h" 
	#include "stdio.h"
#endif
#include "globaldefs.h"

	#define MULT32X32(a,b) (((((U32)((a)>>16))*((U32)((b)>>16)))<<1) +((((U32)((a)>>16))*((U32)(b&0x0000ffff)))>>15) + ((((U32)((b)>>16))*((U32)((a)&0x0000ffff)))>>15))

/***************************************************** 
**FUNCTION  ::  PowOf4
**ACTION    ::  Compute  4^n (where n is an integer) 
**PARAMS IN ::  number -> n
**PARAMS OUT::  NONE
**RETURN    ::  4^n
*****************************************************/ 
S32 PowOf4(S32 number)
{
	S32 i;
	S32 result=1;

	for(i=0;i<number;i++)
		result*=4;

	return result;
}


/*****************************************************
**FUNCTION	::	PowOf2
**ACTION	::	Compute  2^n (where n is an integer) 
**PARAMS IN	::	number -> n
**PARAMS OUT::	NONE
**RETURN	::	2^n
*****************************************************/ 
S32 PowOf2(S32 number)
{
	S32 i;
	S32 result=1;
	
	for(i=0;i<number;i++)
		result*=2;
		
	return result;
}

/*****************************************************
**FUNCTION	::	GetPowOf2
**ACTION	::	Compute  x according to y with y=2^x 
**PARAMS IN	::	number -> y
**PARAMS OUT::	NONE
**RETURN	::	x
*****************************************************/
U32 GetPowOf2(S32 number)
{
	S32 i=0;
	
	while(PowOf2(i)<number) i++;
		
	return i;
}



/****************************************************
**FUNCTION	::	BinaryFloatDiv
**ACTION	::	
**PARAMS IN	::	n1,n2 operands for division n1/n2
**			::	n1<n2
**				
**PARAMS OUT::	result=pow(2,precision)*n1/n2;
*****************************************************/

U32 BinaryFloatDiv(U32 n1, U32 n2, S32 precision)
{
	S32 i=0;
	U32 result=0;
	
	/*	division de N1 par N2 avec N1<N2	*/
	while(i<=precision) /*	n1>0	*/
	{
		if(n1<n2)
		{
			result*=2;      
			n1*=2;
		}
		else
		{
			result=result*2+1;
			n1=(n1-n2)*2;
		}
		i++;
	}
	
	return result;
}

/*****************************************************
**FUNCTION	::	Get2Comp
**ACTION	::	Compute  the 2s complement 
**PARAMS IN	::	Number		-> a	 
			::	bits number	-> width	
**PARAMS OUT::	
**RETURN	::	
*****************************************************/
S32 Get2Comp(S32 a, S32 width)
{
	if(width == 32)
    	return a;
	else
		return (a >= (1<<(width-1))) ? (a - (1<<width)): a;
}

/*****************************************************
**FUNCTION	::	XtoPowerY
**ACTION	::	Compute  x^y (where x and y are integers) 
**PARAMS IN	::	Number -> x
**				Power -> y
**PARAMS OUT::	NONE
**RETURN	::	2^n
*****************************************************/
S32 XtoPowerY(S32 Number, U32 Power)
{
	S32 i,
		result = 1;
	
	for(i=0;i<Power;i++)
		result *= Number;
		
	return result;
}

/*****************************************************
**FUNCTION	::	RoundToNextHighestInteger
**ACTION	::	return Ceil(n.d)  
**PARAMS IN	::	Number -> integer part
**				Digits -> decimal part
**PARAMS OUT::	NONE
**RETURN	::	Ceil(n.d)
*****************************************************/
S32 RoundToNextHighestInteger(S32 Number,U32 Digits)
{
	S32 result = 0,
		highest = 0;
	
	highest = XtoPowerY(10,Digits);
	result = Number / highest;
	
	if((Number - (Number/highest)*highest) > 0)
		result += 1;
	
	return result;
}


/*****************************************************
**FUNCTION	::	Log2Int
**ACTION	::	return log2(n)  
**PARAMS IN	::	Number -> integer 
**PARAMS OUT::	None 
**RETURN	::	log2(n)
*****************************************************/
U32 Log2Int(S32 number)
{
	S32 i;

	i = 0;
	while(PowOf2(i) <= ABS(number))
		i++;

	if (number == 0)
		i= 1;
    
    return i - 1;
}

/*****************************************************
**FUNCTION	::	SqrtInt
**ACTION	::	Compute  sqrt(n) (where n is an integer) 
**PARAMS IN	::	number -> n
**PARAMS OUT::	NONE
**RETURN	::	sqrt(n)
*****************************************************/
S32 SqrtInt(S32 Sq)
{
	S32 an,
		increment,
		an1;
	
	increment=0;
	an=Sq;

	while(increment<15) 
	{
		increment=increment+1;
    		an1=(an+Sq/an)/2;
    		an=an1; 
    }
    return an;
}




static U8 l1d8[256] =	/* Lookup table to evaluate exponent*/
 {
     8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


S16 DSPnormalize(S32 *arg, S16 maxnorm)
{

	S32 input;
	S32 one, two, three, four;
	S32  retval;
	input = (*arg<0)?~(*arg):*arg;

	one = ((U8) (input >> 24));   
	two = ((U8) (input >> 16));
	three = ((U8)(input >> 8));
	four  = ((U8)(input));
     
	retval = (S16)(one ? l1d8[one]-1L  : two ? 7L+ l1d8[two] : three ? 15L + l1d8[three] : 23L + l1d8[four]);
	return ((retval>maxnorm)? maxnorm:(S16)(retval)); 


} /* end DSPnormalize*/

/*****************************************************
**FUNCTION	::	Log10Int
**ACTION	::	Compute  log(n) (where n is an integer) 
**PARAMS IN	::	number -> n
**PARAMS OUT::	NONE
**RETURN	::	log(n)
*****************************************************/
S32 Log10Int(S32 logarg)
{
 	S32 powval1,powval2,powval3,powval4,powval5,powval6,powval7,powval8;
	S32 coeff0, coeff1, coeff2;
	S32 powexp_l;
	S16 powexp;
	S32 SignFlag = 0;
	
	/*Initialize coeffs 0.31 format*/
	coeff0	= 1422945213;		/* This coeff in 0.31 format;*/
	coeff1	= 2143609158;
	coeff2	= 724179374;

        /*Normalize power measure */
	powexp = DSPnormalize(&logarg, 32);
	logarg<<=powexp;
	
	if(powexp&0x8000)
	{
		SignFlag = 1;
		powexp	= -powexp;
	}
	
	powexp = 31 - powexp;
	powexp_l = ((S32)(powexp)<<24);
	powval1 = powexp_l;
	powval4 = MULT32X32(logarg,coeff1);
	powval5 = coeff0 - powval4;
	powval2 = MULT32X32(logarg,logarg);
	powval3 = MULT32X32(powval2,coeff2);
	powval6 = powval5 + powval3;
	powval7 = powval6>>5;
	powval8 = powval1 - powval7;

	if(SignFlag ==1)
		powval8 = -powval8;
	
	return(powval8);	
	
}
