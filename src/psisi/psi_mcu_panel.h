
#ifndef _psi_mcu_panel_H_
#define _psi_mcu_panel_H_

#define defMcuBufMax 4096
#define defProgramNameLen 33 // program name limit to 32 byte
#define defProgitemMax 32    // max program number in one channel
#define defProgPiditemMax 6

#define defChannelItemMax 8

typedef struct  // output one program to mcu_panel
{
	unsigned short prognum;
	unsigned short PMT_PID_IN;
	unsigned short PMT_PID_OUT;
	unsigned short PCR_PID_IN;
	unsigned short PCR_PID_OUT;
	unsigned short PIDS[defProgPiditemMax][2]; //in and out 6*2
	unsigned char progname[2][defProgramNameLen]; // in and out 
	unsigned char csc;
}PROG_INFO_TO_MCU_T, *lpPROG_INFO_TO_MCU_T;  

typedef struct  // output one program from mcu_panel
{
	unsigned char chn;
	unsigned char prognum;
	unsigned int nstatus;
	PROG_INFO_TO_MCU_T progitem[defProgitemMax];	
}CHN_PROG_TO_MCU_T;

typedef struct  // get the programs of eight channels from mcu_panel
{
	unsigned char chn;
	unsigned short prognum;
	unsigned short PMT_PID;
	unsigned short PCR_PID;
	unsigned short PIDS[defProgPiditemMax];
}PROG_INFO_FR_MCU_T, *lpPROG_INFO_FR_MCU_T;

typedef struct  // get the programs of eight channels from mcu_panel
{
	unsigned char chncount;
	PROG_INFO_TO_MCU_T progitem[defProgitemMax];	
}CHN_PROG_FROM_MCU_T;


typedef struct 
{
	unsigned char u08fg;
	unsigned int nSvrIp;
	unsigned char strMac[6];
	unsigned int nLocIp;
	unsigned int nMask;
	unsigned int ngw;
	unsigned char csc;

}NET_ETH0_T, *lpNET_ETH0_T;

typedef struct 
{
	unsigned int nbitrate;
	unsigned char u08len;
	unsigned char csc;

}OUT_RATE_T, *lpOUT_RATE_T;

#endif


