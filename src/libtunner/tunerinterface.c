//8k
#define     main_heard 

#include "FESAT_CommLLA_Str.h"
#include "STFE_utilities.h"
#include "STV0903_Drv.h"
typedef struct
{
	unsigned long Frequency; 
	unsigned long SymbolRate; 
	unsigned long LOFrequency;
	unsigned char Polarization;
	unsigned char LNB;
	unsigned char F22K;
}tunerinfo;

typedef struct
{
	unsigned char Lock; 
	unsigned char Standard; 
	char Level;
	unsigned int CN;
	unsigned char PunctureRate;
	unsigned int Ber;
}__attribute__ ((__packed__)) tunerstatus;
 tunerinfo dvbstunerinfo;
 tunerstatus dvbsstatus;
//#pragma interrupt_handler TIME0_INT:0x11

				   
/*const Initsdt[188]={ 
0X47,0X40,0X11,0X17,0X00,0X42,0XF0,0X2C,0X07,0XCB,0XC9,0X00,0X00,0X07,0XD0,0XFF, 
0X00,0X01,0XFC,0X00,0X1B,0X48,0X19,0X01,0X0F,0X52,0X6F,0X68,0X64,0X65,0X20,0X26, 
0X20,0X53,0X63,0X68,0X77,0X61,0X72,0X7A,0X07,0X43,0X61,0X73,0X74,0X6C,0X65,0X20, 
0XC1,0X69,0X3E,0XDF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF, 
0XFF,0XFF,0XFF,0X00
};*/
//unsigned char a[10];
static  FE_STV0903_LOOKUP_t FE_STV0903_S1_CN_LookUp =	{
	52,
	{												
	{	0,		8917	},  /*C/N=-0dB*/
	{	05,		8801	},  /*C/N=0.5dB*/		
	{	10,		8667	},  /*C/N=1.0dB*/															
	{	15,		8522	},  /*C/N=1.5dB*/															
	{	20,		8355	},  /*C/N=2.0dB*/												
	{	25,		8175	},  /*C/N=2.5dB*/															
	{	30,		7979	},  /*C/N=3.0dB*/															
	{	35,		7763	},  /*C/N=3.5dB*/															
	{	40,		7530	},  /*C/N=4.0dB*/															
	{	45,		7282	},  /*C/N=4.5dB*/														
	{	50,		7026	},  /*C/N=5.0dB*/															
	{	55,		6781	},  /*C/N=5.5dB*/															
	{	60,		6514	},  /*C/N=6.0dB*/															
	{	65,		6241	},  /*C/N=6.5dB*/														
	{	70,		5965	},  /*C/N=7.0dB*/															
	{	75,		5690	},  /*C/N=7.5dB*/															
	{	80,		5424	},  /*C/N=8.0dB*/															
	{	85,		5161	},  /*C/N=8.5dB*/															
	{	90,		4902	},  /*C/N=9.0dB*/															
	{	95,		4654	},  /*C/N=9.5dB*/															
	{	100,	4417	},  /*C/N=10.0dB*/															
	{	105,	4186	},  /*C/N=10.5dB*/															
	{	110,	3968	},  /*C/N=11.0dB*/															
	{	115,	3757	},  /*C/N=11.5dB*/															
	{	120,	3558	},  /*C/N=12.0dB*/															
	{	125,	3366	},  /*C/N=12.5dB*/															
	{	130,	3185	},  /*C/N=13.0dB*/															
	{	135,	3012	},  /*C/N=13.5dB*/															
	{	140,	2850	},  /*C/N=14.0dB*/															
	{	145,	2698	},  /*C/N=14.5dB*/															
	{	150,	2550	},  /*C/N=15.0dB*/															
	{	160,	2283	},  /*C/N=16.0dB*/															
	{	170,	2042	},  /*C/N=17.0dB*/															
	{	180,	1827	},  /*C/N=18.0dB*/															
	{	190,	1636	},  /*C/N=19.0dB*/															
	{	200,	1466	},  /*C/N=20.0dB*/														
	{	210,	1315	},  /*C/N=21.0dB*/															
	{	220,	1181	},  /*C/N=22.0dB*/															
	{	230,	1064	},  /*C/N=23.0dB*/															
	{	240,	960		},  /*C/N=24.0dB*/															
	{	250,	869		},  /*C/N=25.0dB*/															
	{	260,	792		},  /*C/N=26.0dB*/															
	{	270,	724		},  /*C/N=27.0dB*/															
	{	280,	665		},  /*C/N=28.0dB*/															
	{	290,	616		},  /*C/N=29.0dB*/															
	{	300,	573		},  /*C/N=30.0dB*/															
	{	310,	537		},  /*C/N=31.0dB*/															
	{	320,	507		},  /*C/N=32.0dB*/															
	{	330,	483		},  /*C/N=33.0dB*/															
	{	400,	398		},  /*C/N=40.0dB*/															
	{	450,	381		},  /*C/N=45.0dB*/															
	{	500,	377		}	/*C/N=50.0dB*/															
}
};
FE_STV0903_LOOKUP_t FE_STV0903_S2_CN_LookUp =	{
															

55,
															

{													
	{	-30,	13348	},	/*C/N=-3dB*/
	{	-20,	12640	},  /*C/N=-2dB*/															
	{	-10	,	11883	},  /*C/N=-1dB*/															
	{	0,		11101	},  /*C/N=-0dB*/															
	{	05,		10718	},  /*C/N=0.5dB*/															
	{	10,		10339	},  /*C/N=1.0dB*/															
	{	15,		9947	},  /*C/N=1.5dB*/															
	{	20,		9552	},  /*C/N=2.0dB*/															
	{	25,		9183	},  /*C/N=2.5dB*/															
	{	30,		8799	},  /*C/N=3.0dB*/															
	{	35,		8422	},  /*C/N=3.5dB*/															
	{	40,		8062	},  /*C/N=4.0dB*/															
	{	45,		7707	},  /*C/N=4.5dB*/															
	{	50,		7353	},  /*C/N=5.0dB*/															
	{	55,		7025	},  /*C/N=5.5dB*/															
	{	60,		6684	},  /*C/N=6.0dB*/															
	{	65,		6331	},  /*C/N=6.5dB*/															
	{	70,		6036	},  /*C/N=7.0dB*/															
	{	75,		5727	},  /*C/N=7.5dB*/															
	{	80,		5437	},  /*C/N=8.0dB*/															
	{	85,		5164	},  /*C/N=8.5dB*/															
	{	90,		4902	},  /*C/N=9.0dB*/															
	{	95,		4653	},  /*C/N=9.5dB*/															
	{	100,	4408	},  /*C/N=10.0dB*/															
	{	105,	4187	},  /*C/N=10.5dB*/															
	{	110,	3961	},  /*C/N=11.0dB*/															
	{	115,	3751	},  /*C/N=11.5dB*/															
	{	120,	3558	},  /*C/N=12.0dB*/															
	{	125,	3368	},  /*C/N=12.5dB*/															
	{	130,	3191	},  /*C/N=13.0dB*/														
	{	135,	3017	},  /*C/N=13.5dB*/															
	{	140,	2862	},  /*C/N=14.0dB*/															
	{	145,	2710	},  /*C/N=14.5dB*/															
	{	150,	2565	},  /*C/N=15.0dB*/															
	{	160,	2300	},  /*C/N=16.0dB*/															
	{	170,	2058	},  /*C/N=17.0dB*/															
	{	180,	1849	},  /*C/N=18.0dB*/															
	{	190,	1663	},  /*C/N=19.0dB*/															
	{	200,	1495	},  /*C/N=20.0dB*/															
	{	210,	1349	},  /*C/N=21.0dB*/															
	{	220,	1222	},  /*C/N=22.0dB*/															
	{	230,	1110	},  /*C/N=23.0dB*/															
	{	240,	1011	},  /*C/N=24.0dB*/															
	{	250,	925		},  /*C/N=25.0dB*/															
	{	260,	853		},  /*C/N=26.0dB*/															
	{	270,	789		},  /*C/N=27.0dB*/															
	{	280,	734		},  /*C/N=28.0dB*/															
	{	290,	690		},  /*C/N=29.0dB*/															
	{	300,	650		},  /*C/N=30.0dB*/														
       {	310,	619		},  /*C/N=31.0dB*/															
	{	320,	593		},  /*C/N=32.0dB*/															
	{	330,	571		},  /*C/N=33.0dB*/															
	{	400,	498		},  /*C/N=40.0dB*/															
	{	450,	484		},  /*C/N=45.0dB*/															
	{	500,	481		}	/*C/N=50.0dB*/
															
}
};


FE_STV0903_InternalParams_t_E  STV0903Params;
void	GetSignalInfo();
 

void WAIT_N_MS(unsigned int count)
{
	usleep(count*1000);
}
void T6110_TEST(void)
{
	U8 chipaddress=0xC0;
	long int tmp=0;
 	U8 reg[8]={0x07,0x11,0xdc,0x85,0x17,0x01,0xe6,0x1e};

 	U8 i;
	U8 dat=0;
	U8 dat1=0xff;
        #if 0
	for(i=0;i<8;i++)
	{
			 ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_8,TRUE,i,reg[i]);
	}
        #endif
        #if 1
	for(i=0;i<8;i++)
	{
		dat = ChipGetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_8, TRUE,i, 0, 1);
	      printf("6110 reg:%d,value:0x%x\n",i,dat);
	}
	#endif
}
void EEPROM_INIT(void)
{    
     unsigned char e_temp;
     unsigned char InitData[16]={
					  0x36,//同步头
					  0x60,0xe7,0xbc,0x00,//freq channel1 12380000MHZ jiangsu
					  0xe0,0x9d,0xa3,0x01, //sym channel1 27500000
					  0xa0,0x6c,0xac,0x00,//lofreq channel1 11300000
					  0x00,//pol channel1 V
					  0x00,//lnb channel1 off
					  0x01//f22k channel1 off
					   
                   };
	 
	
	 unsigned int temp;
	 memcpy(&dvbstunerinfo,InitData+1,sizeof(tunerinfo));
//	 memcpy(&dvbstunerinfo[TUNER1],InitData+sizeof(tunerinfo)+1,sizeof(tunerinfo));
}
#if 0
void EEPROM_READ(void)
{    
        unsigned int temp;
	 char tempData[EEPROMSIZE];
	  read_eeprom(0,tempData,EEPROMSIZE);
	  //hex_out(tempData,EEPROMSIZE);
	  Delay_ms(100);
	 memcpy(&dvbstunerinfo[TUNER0],tempData+1,sizeof(tunerinfo));
	 memcpy(&dvbstunerinfo[TUNER1],tempData+sizeof(tunerinfo)+1,sizeof(tunerinfo));
	 //memcpy(&bcm3033_info,tempData+2*sizeof(tunerinfo)+1,sizeof(bcm3033info));
	 memcpy((char*)&PQam,tempData+2*sizeof(tunerinfo)+1,sizeof(PROG_QAM_T));
	 Delay_ms(100);
	 hex_out(&dvbstunerinfo[TUNER0].Frequency ,4); 
	 Delay_ms(100);
	 hex_out(&dvbstunerinfo[TUNER1].Frequency ,4);  
	 change_rf_asc_to_dig(); 
	 temp=change_interlace_asc_to_dig();
	 
        memcpy(bcm3033_info.rf_freq,(unsigned char*)&cur_rf_frq,3); 
	 memcpy(bcm3033_info.interlace,(unsigned char*)&temp,3); 
	bcm3033_info.baud=(unsigned short)change_baud_asc_to_dig();
	//hex_out(&bcm3033_info.baud, 2);
	//bcm3033_info.baud=6875;
	bcm3033_info.att=(PQam.att[0]-0x30)*10+(PQam.att[1]-0x30);
	bcm3033_info.info=bcm3033_info_get();
	 //text_out(PQam.if_frq,5);
	 // text_out(PQam.interlace,6);
	//text_out(PQam.rf_frq ,6);
	 // text_out(PQam.rate,4);
}
#endif

void tuner_hw_reset()
{
    tuner_device_reset();
}


void printfvalue(unsigned char*name,unsigned int len,unsigned char reg)
{
	unsigned char dat;
	unsigned char txt3[2]={'\n'};
	dat=ChipGetOneRegister_E(0xd0,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DSTATUS,0,0);
	//text_out(name,len);
	//hex_out(&dat,1);
	//text_out(txt3, 2);
       WAIT_N_MS(100);
}

void STV0903regInit(void)
{
	U8 chipaddress=0xd0; 
	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,0xf43d,0x3a);	
	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,0xf43e,0x20);	
	ChipSetOneRegister_E(chipaddress,STCHIP_MODE_SUBADR_16,FALSE,0xf416,0x15);

}
void Set_Polarization(unsigned char mode)
{
	tuner_device_pol(mode);
}
void Set_Lnb(unsigned char mode)
{
       tuner_device_lnb(mode);
}
void Set_F22K(unsigned char mode)
{
	tuner_device_f22k(mode);
}
void tuner_init_set(void)
{
	Set_Polarization(dvbstunerinfo.Polarization);
	Set_Lnb(dvbstunerinfo.LNB);
	Set_F22K(dvbstunerinfo.F22K);
}
void GetTunerRegister_E(void)
{
	U8 databuf[10];
	U8 i;
	
	//ChipSetField_E(0xc0,STCHIP_MODE_SUBADR_8,TRUE,FSTV6110_CO_DIV,1);			
	for(i=0;i<8;i++)
	{
		//Write_Stv0903(0xd0,0xf12a,0xa0);
                ChipSetOneRegister_E(0xd0,1,FALSE,0xf12a,0xa0); 
	  	//databuf[i]=Read_6110(0xc0,i);
                databuf[i]=ChipSetOneRegister_E(0xc0,0,1,i,0,0);	
		//Write_Stv0903(0xd0,0xf12a,0x00);
                ChipSetOneRegister_E(0xd0,1,FALSE,0xf12a,0x0); 
	//	printf("0x%x,",databuf[i]);
	}
        for(i=0;i<8;i++)
        {
		printf("0x%x,",databuf[i]);
        }
        printf("\n");
}
void tuner_port_test(void)
{
U8 	ChipID;
ChipID=ChipGetOneRegister_E(0xd0,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_MID,0,0);
printf("ChipID:%x\n",ChipID);
//GetTunerRegister_E();
}


void tuner_init()
{
	FE_Sat_InitParams_t		InitParams;
	FE_STV0903_InternalParams_t_E STV0903Params0;
	//tuner_hw_reset(type);
	InitParams.DemodI2cAddr   		=	0xD0;			/* Demod I2c address */
	InitParams.DemodRefClock  		= 	8000000;// 8MHZ		/* Refrence clock for the Demodulator in Hz (27MHz,30MHz,....)/*/
	InitParams.DemodType			=	FE_SAT_SINGLE;	/* use the 903 as a dual demodulator 
															(the 903 could also be used as a single demod to enable the DVBS2 16APSK and 32APSK modes)*/
	InitParams.RollOff				=  	FE_SAT_35;			/* NYQUIST Filter value (used for DVBS1/DSS, DVBS2 is automatic)*/


	InitParams.TunerHWControlType   =  	FE_SAT_SW_TUNER;	/* Tuner1 HW controller Type */
       InitParams.PathTSClock		 	=  	FE_TS_SERIAL_PUNCT_CLOCK; /* Path1 TS settings		*/

   	  strcpy(InitParams.TunerName,"STV6110_1");  			/* Tuner 1 name */
      	 InitParams.TunerModel			=	TUNER_STV6110;	/* Tuner 1 model (SW control) */
	InitParams.Tuner_I2cAddr		=	0xC0;			/* Tuner 1 I2c address */
 	InitParams.TunerRefClock		=	16000000;		/* Tuner 1 Refrence/Input clock for the Tuner in Hz (27MHz,30MHz,16MHz....)/*/
	InitParams.TunerOutClkDivider	=	2;				/* Tuner 1 Clock Divider for 6110 1,2,4 or 8 */
	InitParams.TunerIQ_Inversion	=	FE_SAT_IQ_NORMAL;/*FE_SAT_IQ_SWAPPED;	/* Tuner 1 Global IQ setting (coonction of IQ from the tuner1 to the demod */
	InitParams.TunerBasebandGain	=	6;				/* Tuner 1 base band gain recomanded value is 6dB*/
	//ErrorSTV0903=FE_STV0903_Init(&InitParams,&HandleSTV0903);
	STV0903Params.TunerType=InitParams.TunerHWControlType;
	STV0903Params.TunerGlobal_IQ_Inv=InitParams.TunerIQ_Inversion;
	STV0903Params.RollOff=InitParams.RollOff;
	STV0903Params.Quartz = InitParams.DemodRefClock;	/*Ext clock in Hz*/
	STV0903Params.TunerReference=InitParams.TunerRefClock;
	STV0903Init(&STV0903Params);//0903初始化
}

void Set_Tuner_Dvbs(U32 	Frequency, U32 	LOFrequency,U32 SymbolRate)
{
	 FE_Sat_SearchResult_t	SearchResults;	
	 FE_Sat_SearchParams_t	SearchParams;					/* Front-end search parameters */
	 FE_LLA_Error_t			ErrorSTV0903;
	// U32 					tmpFreq;					/* Transponder frequency (in KHz)								*/
	 U32 					tmpSym;	
	 unsigned char freq[11]={'f','r','e','g',':'};
	 unsigned char txt[2]={'\n'};
	 unsigned char txt2[11]={'S','e','t','t','u','n','e','r','0','\n'};
	  printf("Freqency:%d;LOFrequency;%d;SymbolRate:%d\n",Frequency,
            LOFrequency,SymbolRate);
	 tuner_hw_reset();
		//Delay_ms(10);  
	 tuner_init();
	 usleep(10000);
	 if(LOFrequency>=Frequency)
	  SearchParams.Frequency=LOFrequency-Frequency;
	 else
	  SearchParams.Frequency=Frequency-LOFrequency;
	SearchParams.SymbolRate= SymbolRate;	//	符号率				/*	Symbol rate  (in bds)	*/
	//hex_out((char*)&Frequency,4);
	//hex_out((char*)&tmpSym,4);
	 
	SearchParams.SearchRange	= 10000000;//10 MHZ	所搜范围				/*	Range of the search (in Hz)	*/	
       //SearchParams.Frequency		=1249000;	// 1250MHZ						/*	Tuner frequency (in KHz)	*/
	//SearchParams.SymbolRate		=6670000;	//	符号率				/*	Symbol rate  (in bds)	*/
	//SearchParams.SearchRange	= 10000000;//10 MHZ	所搜范围				/*	Range of the search (in Hz)	*/	

	SearchParams.Standard		= FE_SAT_AUTO_SEARCH;//自动搜索模式			/*	STANDARD automatic,DVBS1 ,DVBS2 or DSS	*/
	SearchParams.IQ_Inversion	= FE_SAT_IQ_AUTO;					/*	I,Q Inversion search	*/
	SearchParams.SearchAlgo		= FE_SAT_COLD_START;				/*	Search mode Blind, Cold start or Warm Start */
	SearchParams.PunctureRate	= FE_SAT_PR_UNKNOWN;   

	ErrorSTV0903 = FE_STV0903_Search_E(&STV0903Params,&SearchParams,&SearchResults);		/* Launch the search algorithm for Path1	*/
}


void tunertest(void)
{
	 unsigned long Transponders,i;
	 FE_Sat_InitParams_t		InitParams;
	 InternalParamsPtr		HandleSTV0903;
	 FE_Sat_SearchParams_t	SearchParams;					/* Front-end search parameters */
	 FE_Sat_SearchResult_t	SearchResults;				/* Front-end search results */
	 //FE_Sat_SearchResult_t	*ScanResult[200];
	 FE_LLA_Error_t			ErrorSTV0903;
													/* Front-end error */
	
	FE_Sat_SignalInfo_t		pInfo;					/*Signal info (tuner freq,c/n,ber...) struct */
	FE_STV0903_InternalParams_t_E  STV0903Params;
	U8 dat=0;
	U8 cnt=0;
	S32 power;
	U32 ber;

  //      load_data();
	 
      	
        tuner_hw_reset();
	strcpy(InitParams.DemodName,"STV0903");				/* Demod name */
	InitParams.DemodI2cAddr   		=	0xD0;			/* Demod I2c address */
	InitParams.DemodRefClock  		= 	8000000;// 8MHZ		/* Refrence clock for the Demodulator in Hz (27MHz,30MHz,....)/*/
	InitParams.DemodType			=	FE_SAT_SINGLE;	/* use the 903 as a dual demodulator 
															(the 903 could also be used as a single demod to enable the DVBS2 16APSK and 32APSK modes)*/
	InitParams.RollOff				=  	FE_SAT_35;			/* NYQUIST Filter value (used for DVBS1/DSS, DVBS2 is automatic)*/


	InitParams.TunerHWControlType   =  	FE_SAT_SW_TUNER;	/* Tuner1 HW controller Type */
       InitParams.PathTSClock		 	=  	FE_TS_SERIAL_PUNCT_CLOCK; /* Path1 TS settings		*/

   	strcpy(InitParams.TunerName,"STV6110_1");  			/* Tuner 1 name */
      	InitParams.TunerModel			=	TUNER_STV6110;	/* Tuner 1 model (SW control) */
	InitParams.Tuner_I2cAddr		=	0xC0;			/* Tuner 1 I2c address */
 	InitParams.TunerRefClock		=	16000000;		/* Tuner 1 Refrence/Input clock for the Tuner in Hz (27MHz,30MHz,16MHz....)/*/
	InitParams.TunerOutClkDivider	=	2;				/* Tuner 1 Clock Divider for 6110 1,2,4 or 8 */
	InitParams.TunerIQ_Inversion	=	FE_SAT_IQ_NORMAL;/*FE_SAT_IQ_SWAPPED;	/* Tuner 1 Global IQ setting (coonction of IQ from the tuner1 to the demod */
	InitParams.TunerBasebandGain	=	6;				/* Tuner 1 base band gain recomanded value is 6dB*/
	
		

      // tuner_init();
       tuner_init_set();
	

       Set_Tuner_Dvbs(dvbstunerinfo.Frequency,
		dvbstunerinfo.LOFrequency,dvbstunerinfo.SymbolRate);



	while(1)
	{
		 
         	GetSignalInfo();
	//	GetSignalInfo(TUNER1);
	//	uartcmd_check_settiing();

	//	Set_run_led();
	}

}
void Get_CN_String(long CNVal,unsigned char Standard)
{
	unsigned char *p = &dvbsstatus.CN;
	char  str[4];
	//str=&(dvbsstatus[input].CN);
	//unsigned char txtcn[3]={'c','n','='};
	//unsigned char txt3[2]={'\n'};
	unsigned int val;
	hex_dump("CNVAL", &CNVal, 4);
	val=CNVal;
        printf("CNVal:%d\n",val);
	//hex_out(&val, 2);
	if(val==1000)
	val=500;
	str[0]=val/100;
	str[1]=(val-str[0]*100)/10;
	str[2]='.';
	str[3]=(val-str[0]*100)-str[1]*10;
	str[0]+=0x30;
	str[1]+=0x30;
	str[3]+=0x30;
	hex_dump("str", str, 4);
	p[0] = str[0];
	p[1] = str[1];
	p[2] = str[2];
	p[3] = str[3];
	hex_dump("p", p, 4);
        printf("CN:%s\n",&dvbsstatus.CN);
	//text_out(txtcn,3);
			//text_out(&dvbsstatus[input].CN,4); 
			//text_out(str,4); 
		//	text_out(txt3, 2); 
//	memcpy(&(dvbsstatus[input].CN,str,4);
}
void	GetSignalInfo()
{

                     unsigned char dat;
                     long cnval;

		//	curtuner=input;
			dat=ChipGetOneRegister_E(0xd0,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_DSTATUS,0,0);
		//	if(curtuner==TUNER0)
		//	statusstr[5]='0';
		//	else
		//	statusstr[5]='1';
			//uartcmd_check_settiing();
			//text_out(statusstr,13);
			//hex_out(&dat,1);
			//text_out(txt3, 2);
			//return;
                    //WAIT_N_MS(10);
                  //    uartcmd_check_settiing();
			
			if(dat&0x08)
			{
				dvbsstatus.Lock=1;
			}
			else
			{
				dvbsstatus.Lock=0;
			}
			
                    dvbsstatus.Level=(char)FE_STV0903_GetRFLevel_E();
			//uartcmd_check_settiing();
			//return;
			dvbsstatus.Ber=FE_STV0903_GetBer_E();
	            //  uartcmd_check_settiing();
			dvbsstatus.PunctureRate=FE_STV0903_GetPunctureRate_E();
		//	uartcmd_check_settiing();
	              switch(FE_STV0903_GetStandard_E())
	              {
	              	case FE_SAT_DVBS1_STANDARD:
				dvbsstatus.Standard=1; 
				break;
				case FE_SAT_DVBS2_STANDARD:
				dvbsstatus.Standard=2;
				break;
				case FE_SAT_DSS_STANDARD:
				dvbsstatus.Standard=0;
				break;
	              }

		       // uartcmd_check_settiing();
				#if 1
			if(dvbsstatus.Standard==FE_SAT_DVBS2_STANDARD)
			{
				dvbsstatus.CN=FE_STV0903_CarrierGetQuality_E(&FE_STV0903_S2_CN_LookUp);
				Get_CN_String(dvbsstatus.CN,FE_SAT_DVBS2_STANDARD);
				hex_dump("dvbstatus", &dvbsstatus, sizeof(tunerstatus));
			}
			else /*DVBS1/DSS*/
			{
				dvbsstatus.CN=FE_STV0903_CarrierGetQuality_E(&FE_STV0903_S1_CN_LookUp);
				Get_CN_String(dvbsstatus.CN,FE_SAT_DVBS1_STANDARD);
				hex_dump("dvbstatus", &dvbsstatus, sizeof(tunerstatus));
			}
                    #endif
			//text_out(txtber,4);
			
			//hex_out(&dvbsstatus[input].Ber,4);
			//
			//text_out(txt3, 2);
                     WAIT_N_MS(100);
			#if 0
			text_out(txtlevel,4);
			hex_out(&dvbsstatus[input].Level,1);
			text_out(txt3, 2);
                     WAIT_N_MS(100);

			text_out(txtstandard,9);
			hex_out(&dvbsstatus[input].Standard,1);
			text_out(txt3, 2);
                     WAIT_N_MS(100);

			text_out(txtber,4);
			hex_out(&dvbsstatus[input].Ber,4);
			text_out(txt3, 2);
                     WAIT_N_MS(100);


			text_out(txtpunc,5);
			hex_out(&dvbsstatus[input].PunctureRate,1);
			text_out(txt3, 2);
                     WAIT_N_MS(100);

		       text_out(txtcn,3);
			hex_out(&dvbsstatus[input].CN,4);
			text_out(txt3, 2);
                     WAIT_N_MS(100);
                    #endif
			
}
#if 0
void Parse_tuner_parameter()
{
	//unsigned char txt[10]={'t','u','n','e','r','g','e','t','\n'};
	if(uartinfo.wrflag==WRITE)
	{
	
		hex_out(uartinfo.data,sizeof(tunerinfo));	
		memcpy(&(dvbstunerinfo),uartinfo.data,sizeof(tunerinfo));
        	write_eeprom(1+type*sizeof(tunerinfo),(char*)&dvbstunerinfo,sizeof(tunerinfo));
		tuner_hw_reset();
		//Delay_ms(10);  
		tuner_init();
       	Set_Tuner_Dvbs(dvbstunerinfo.Frequency,
			dvbstunerinfo.LOFrequency,dvbstunerinfo.SymbolRate);
		//STV0903regInit();
		Set_Polarization(dvbstunerinfo.Polarization);
		//Set_Lnb(type,dvbstunerinfo[type].LNB);
		Set_F22K(dvbstunerinfo.F22K);

		
	}
	else
	{
		unsigned char buff[30];
		unsigned char len=0;
		unsigned crc=0;
		unsigned int i;
		//hex_out(txt, 10);
		//Delay_ms(10);  
		buff[0]=SERIAL_HEADER;
		buff[1]=uartinfo.reg>>8;
		buff[1]|=0x80;
		buff[2]=uartinfo.reg&0x00ff;
		buff[3]=MODE_TUNER_LEN>>8;
		buff[4]=MODE_TUNER_LEN&0x00ff;
		memcpy(buff+5,&(dvbstunerinfo),sizeof(tunerinfo));
		for(i=0;i<5+sizeof(tunerinfo);i++)
		crc+=buff[i];
		buff[5+sizeof(tunerinfo)]=crc;
		text_out(buff, 6+sizeof(tunerinfo));
	}
		
}
void Parse_tuner_status()
{
	unsigned char buff[30];
	unsigned char len=0;
	unsigned crc=0;
	unsigned int i;
	Delay_ms(10);  
	buff[0]=SERIAL_HEADER;
	buff[1]=uartinfo.reg>>8;
	buff[1]|=0x80;
	buff[2]=uartinfo.reg&0x00ff;
	buff[3]=MODE_TUNER_STATUS_LEN>>8;
	buff[4]=MODE_TUNER_STATUS_LEN&0x00ff;
	memcpy(buff+5,&(dvbsstatus),sizeof(tunerstatus));
	for(i=0;i<5+sizeof(tunerstatus);i++)
	crc+=buff[i];
	buff[5+sizeof(tunerstatus)]=crc;
	text_out(buff, 6+sizeof(tunerstatus));
}
#endif
#if 0
void func_set(void)
{
	  char txt6[11]={'R','E','C','M','D','0','0','0','0','\n'};
	  unsigned char data;
	data=(uartinfo.reg>>8)&0x00ff;
	txt6[5]=data/16;
	if(txt6[5]<10)
	txt6[5]+=0x30;
	else
	txt6[5]+=55;
	txt6[6]=data%16;
	if(txt6[6]<10)
	txt6[6]+=0x30;
	else
	txt6[6]+=55;

	data=uartinfo.reg&0x00ff;
	txt6[7]=data/16;
	if(txt6[7]<10)
	txt6[7]+=0x30;
	else
        txt6[7]+=55;
	txt6[8]=data%16;
	if(txt6[8]<10)
	txt6[8]+=0x30;
	 else
	 txt6[8]+=55;
		
    
	switch(uartinfo.reg)
	{
		case MODE_TUNER0_WR:
		Parse_tuner_parameter();	
		break;
              case MODE_TUNER0_RD:
		Parse_tuner_parameter();	
		break;
		case MODE_TUNER0_STATUS:
		Parse_tuner_status();
		break;
		default:
		break;
	}
	  //text_out(txt6, 11);	
}
#endif
#if 0

void set_att(void)
{
   unsigned char data;
 
   //read_eeprom(ATT_ADDR,&slave_data[ATT_ADDR],2);//读EEPROM首地址值
   data=change_att_asc_to_dig();
   //read_eeprom(ATT_ADDR,&slave_data[ATT_ADDR],1);//读EEPROM首地址值
   data=(0x3f-data*2)&0x3f;
   data=(data<<2);
   write_att(data);
}
 #endif

