// ********************************************************
//
//  $Workfile: $ 
//
//   Author/Copyright	Gero Kuehn / GkWare e.K.
//						Humboldtstrasse 177
//						45149 Essen
//						GERMANY 
//						Tel: +49 174 520 8026
//						Email: support@gkware.com
//						Web: http://www.gkware.com
//
//						galaxis technology ag / omniscience multimedia lab gmbh				       						
//
//	$Date: 2004/04/23 15:41:15 $
//	$Revision: 1.2 $
//
// ********************************************************
//
//  This file contains the portable CI Lib CA-PMT Generator
//  
#include "ci_define.h"

//#ifdef ENABLE_CI

//#define CIPMT_DEBUG_ENABLE
//#define CIPMT_DUMP_ENABLE

#ifdef CIPMT_DEBUG_ENABLE
#define CIPMT_DEBUG(x) printf ##x
#else
#define CIPMT_DEBUG(x)
#endif

// *************************************************************
// externals
//BYTE  PMTBuf[MAX_LEN_CA_PMT];
//WORD  wPMTLen = 0;


#ifndef __SI_ENGINE_H
//BYTE *GetPMTDescriptor(BYTE *pBuf,BYTE bTag,BYTE index);
//BYTE* GetPMTESDescriptor(BYTE *pBuf,BYTE bESEntry,BYTE bTag,BYTE index);
//BOOL GetPMTESEntry(BYTE *pBuf,BYTE bESEntry,BYTE *pbStreamType,WORD *pwElementaryPID);
#endif
//typedef unsigned char BOOL;
//typedef unsigned char BYTE;
//typedef unsigned short WORD;
//typedef unsigned int DWORD;
#define PROGFIRST 0
#define PROGMORE 1
#define PROGLAST 2
#define PROGONLY 3
#define MAX_CI_SESSIONS		10
#define MAX_CASYS_IDS		40
#define CI_T_MAXMENUNAME	20
#define MAX_LEN_CA_PMT		1024
#define MAX_LEN_CA_PMT 1024

#define MAX_CI_SLOTS 2
#define MAX_CI_RESOURCES 10
#define NULL 0
BYTE PMTBuf[MAX_LEN_CA_PMT];
WORD wPMTLen=0;
typedef enum
{
  CA_PMT_LIST_MANAGEMENT_MORE=0,
  CA_PMT_LIST_MANAGEMENT_FIRST,
  CA_PMT_LIST_MANAGEMENT_LAST,
  CA_PMT_LIST_MANAGEMENT_ONLY,
  CA_PMT_LIST_MANAGEMENT_ADD,
  CA_PMT_LIST_MANAGEMENT_UPDATE
} CaPmtListManagement;
typedef struct
{
	unsigned long	UTC_Time;
	unsigned long	MJD;
}typ_CiTimeDate;


//SendCIMessage Tags
#define	CI_MSG_NONE				0
#define	CI_MSG_CI_INFO			1
#define	CI_SLOT_EMPTY				0
#define	CI_SLOT_MODULE_INSERTED		1
#define	CI_SLOT_MODULE_OK			2
//#define CI_SLOT_CA_OK				3
//#define	CI_SLOT_DBG_MSG			4
//#define CI_SLOT_UNKNOWN_STATUS	0xFF
#define	CI_MMI_MENU				2
#define	CI_MMI_LIST				3
#define	CI_MMI_TEXT				4
#define	CI_MMI_REQUEST_INPUT	5
//#define	CI_MSG_INPUT_COMPLETE	6
//#define	CI_MMI_LIST_MORE		7
//#define	CI_MMI_MENU_MORE		8
#define	CI_MMI_CLOSE_MMI_IMM	9
//#define	CI_MSG_MODUL_UPDATE		0xA
//#define	CI_MSG_CLOSE_FILTER		0xB
//#define	CI_PSI_COMPLETE			0xC



/////////////////////////////////////////////////////////////////
// TPDUs
#define CI_T_SB				0x80 // Host <-  Module
#define CI_T_RCV			0x81 // Host  -> Module
#define CI_T_CREATE			0x82 // Host  -> Module
#define CI_T_CREATE_REPLY	0x83 // Host <-  Module
#define CI_T_DELETE_TC		0x84 // Host <-> Module
#define CI_T_DELETE_REPLY	0x85 // Host <-> Module
#define CI_T_REQUEST_TC		0x86 // Host <-  Module
#define CI_T_NEW_TC			0x87 // Host  -> Module
#define CI_T_TC_ERROR		0x88 // Host  -> Module
#define CI_T_DATALAST		0xA0 // Host <-> Module
#define CI_T_DATAMORE		0xA1 // Host <-> Module
// SPDUs
#define CI_SESSION_OPEN_REQUEST		0x91
#define CI_SESSION_OPEN_RESPONSE	0x92
#define CI_SESSION_CREATE			0x93
#define CI_SESSION_CREATE_RESPONSE	0x94
#define CI_SESSION_CLOSE_REQUEST	0x95
#define CI_SESSION_CLOSE_RESPONSE	0x96
#define CI_SESSION_DATA				0x90
extern system_ids_t sys_ids[];
extern unsigned short ca_support_sid;
BOOL IsCASysIdInList(WORD wCASysId)
{
	 int i = 0;


    while ( sys_ids[0].pi_system_ids[i] )
    {
        if (  sys_ids[0].pi_system_ids[i]  == wCASysId )
            return 1;
        i++;
    }

    return 0;
}

BYTE *GetPMTDescriptor(BYTE *pBuf,BYTE bTag,BYTE index)
{
	WORD wLen = ((pBuf[1]&0x0F)<<8)|pBuf[2];
	WORD wLoopLen;
	int hitcount = 0;
	int i;

	wLen-=1; // CRC
	wLoopLen = ((pBuf[10]&0x0F)<<8)|pBuf[11];
	i=12;
	while(wLoopLen && (i<wLen))
	{
		if(pBuf[i]==bTag) {
			if(hitcount == index)
				return &pBuf[i];
			hitcount++;
		}
		wLoopLen-=pBuf[i+1]+2;
		i+=pBuf[i+1]+2;
	}
	return 0;
}
BYTE *GetPMTESDescriptor(BYTE *pSection,BYTE bESEntry,BYTE bDescriptorTag, BYTE bCount)
{
	WORD wSectionLength = ((pSection[1]&0x0F)<<8)|pSection[2];
	WORD wProgInfoDescrLength;

	int nPos = 10;
	WORD wESLoopCount = 0;
		WORD wESDescrLength;
		int DescrEnd;
		BYTE bHitCount;
	wSectionLength-=1; // CRC

	wProgInfoDescrLength = ((pSection[nPos]&0x0F)<<8) | pSection[nPos+1];		
	nPos +=2;	
	
	// skip the program descriptors
	nPos += wProgInfoDescrLength;
	// stream loop
	while(nPos < wSectionLength)
	{
		nPos +=3;
		wESDescrLength = ((pSection[nPos]&0x0F)<<8) | pSection[nPos+1];
		nPos +=2;
		
		DescrEnd = nPos + wESDescrLength;
		bHitCount=0;
		
		if(bESEntry == wESLoopCount)
		{
			while((nPos <= DescrEnd) &&(nPos < wSectionLength-4)){
				if(pSection[nPos]==bDescriptorTag){
					if((bHitCount==bCount)&&(bESEntry == wESLoopCount))
						return &pSection[nPos];
					else
						bHitCount++;
				}
				nPos+=pSection[nPos+1]+2;
				
			}
		} else
			nPos = DescrEnd;
		
		if(bESEntry == wESLoopCount)
			return NULL;
		wESLoopCount ++;
	}
	return NULL;}

BOOL GetPMTESEntry(BYTE *pSection,BYTE bESEntry,BYTE *pStreamType, WORD *pPID)
{
	WORD wSectionLength = ((pSection[1]&0x0F)<<8)|pSection[2];
	WORD wProgInfoDescrLength;

	int nPos = 10;
	WORD wESLoopCount = 0;
	WORD wESDescrLength;
	int DescrEnd;
	BYTE bHitCount;
	wSectionLength-=1; // CRC

	wProgInfoDescrLength = ((pSection[nPos]&0x0F)<<8) | pSection[nPos+1];		
	nPos +=2;	
	
	// skip the program descriptors
	nPos += wProgInfoDescrLength;
	// stream loop
	while(nPos < wSectionLength)
	{
		*pStreamType = pSection[nPos];
		*pPID = ((pSection[nPos+1]<<8)|pSection[nPos+2])&0x1FFF;
		nPos +=3;
		wESDescrLength = ((pSection[nPos]&0x0F)<<8) | pSection[nPos+1];
		nPos +=2;
		
		DescrEnd = nPos + wESDescrLength;
		bHitCount=0;
		
		if(bESEntry == wESLoopCount)
			return TRUE;
		else
			nPos = DescrEnd;
		
		if(bESEntry == wESLoopCount)
			return FALSE;
		wESLoopCount ++;
	}
	return FALSE;
}



WORD Generate_CA_PMT(BYTE *CAPMTBuf,BYTE *pBuf, unsigned int  dwLen,int Progtype)
{
	WORD wSizePtr;
	WORD wLoopSizePtr;
	WORD wTotalLen;
	WORD wLoopLen;
	BYTE bMatchedDescriptors = 0;
	
	int i;
	//	memcpy(PMTBuf,pBuf,dwLen);
	CIPMT_DEBUG(("\r\n"));
	
	memcpy(PMTBuf,pBuf,dwLen);
	wPMTLen = (WORD)dwLen;
	CAPMTBuf[0] = 0x9F;
	CAPMTBuf[1] = 0x80;
	CAPMTBuf[2] = 0x32;
	wSizePtr = 3;
	
	// 1 byte size
	wTotalLen = 4; // 5 for long size
	CAPMTBuf[3] = 0;  // size comes here
	
	// 
	// Larger ca_pmts are handled below
	// Do not enable the following two parts  (for testing only)
	//
	/*
	// 2byte size
	CAPMTBuf[3] = 0;  // size comes here
	CAPMTBuf[4] = 0;  // size comes here
	wTotalLen = 5; // 5 for long size
	*/
	/* // 3 byte size
	CAPMTBuf[3] = 0;  // size comes here
	CAPMTBuf[4] = 0;  // size comes here
	CAPMTBuf[5] = 0;  // size comes here
	wTotalLen = 6; // 5 for long long size
	*/
	switch(Progtype)
	{
		case PROGFIRST:
		CAPMTBuf[wTotalLen++] = CA_PMT_LIST_MANAGEMENT_FIRST;
		break;
		case PROGMORE:
		CAPMTBuf[wTotalLen++] = CA_PMT_LIST_MANAGEMENT_MORE;
		break;
		case PROGLAST:
		CAPMTBuf[wTotalLen++] = CA_PMT_LIST_MANAGEMENT_LAST;
		break;
		case PROGONLY:
		CAPMTBuf[wTotalLen++] = CA_PMT_LIST_MANAGEMENT_ONLY;
		break;
	}
	//CAPMTBuf[wTotalLen++] = CA_PMT_LIST_MANAGEMENT_ONLY;
	CAPMTBuf[wTotalLen++] = pBuf[3];  // program number
	CAPMTBuf[wTotalLen++] = pBuf[4];  // program number
	CAPMTBuf[wTotalLen++] = pBuf[5];  // version
	wLoopSizePtr = wTotalLen;
	CAPMTBuf[wTotalLen++] = 0;  // size comes here
	CAPMTBuf[wTotalLen++] = 0;  // size comes here
	//wTotalLen = 11;
	wLoopLen = 0;
	i=0;
	while(GetPMTDescriptor(pBuf,0x09,i))
	{
		BYTE bSize;
		BYTE *pDesc = GetPMTDescriptor(pBuf,0x09,i);
		
		if(IsCASysIdInList((pDesc[2]<<8)|pDesc[3]))
		{
			if(bMatchedDescriptors==0) {
				//CAPMTBuf[wTotalLen] = 0x01;
				CAPMTBuf[wTotalLen] = 0x01;//dlh7.7.27
				wTotalLen++;
				wLoopLen++;
			}
			
			CIPMT_DEBUG(("Descriptor %02X Len %02X (%02X%02X %02X%02X)\r\n",pDesc[0],pDesc[1],pDesc[2],pDesc[3],pDesc[4],pDesc[5]));
			bSize=pDesc[1]+2;
			memcpy(&CAPMTBuf[wTotalLen],pDesc,bSize);
			wTotalLen+=bSize;
			wLoopLen+=bSize;
			bMatchedDescriptors++;
		} else {
			CIPMT_DEBUG(("Discarding CA Descriptor due to CASysId Filtering\r\n"));
		}
		i++;
	}
	CIPMT_DEBUG(("total len %04X, LoopLen %04X\r\n",wTotalLen,wLoopLen));
	CAPMTBuf[wLoopSizePtr] = wLoopLen>>8;
	CAPMTBuf[wLoopSizePtr+1] = wLoopLen;
	wLoopLen = 0;
	// The stream loop
	i=0;
	{
		BYTE bStreamType;
		WORD wElementaryPid;
		while(GetPMTESEntry(pBuf,i,&bStreamType,&wElementaryPid))
		{
			int j = 0;
			if(bStreamType>=0x0f)
			{
				i++;
				continue;
			}
			CAPMTBuf[wTotalLen] = bStreamType;
			CAPMTBuf[wTotalLen+1] = wElementaryPid>>8;
			CAPMTBuf[wTotalLen+2] = wElementaryPid;
                        printf("streamtype:%d,pid:%d\n",bStreamType,wElementaryPid);
			wLoopSizePtr = wTotalLen+3;
			CAPMTBuf[wTotalLen+3] = 0; // looplen
			CAPMTBuf[wTotalLen+4] = 0;
			wTotalLen+=5;
			wLoopLen=0;
			bMatchedDescriptors = 0;
			while(GetPMTESDescriptor(pBuf,i,0x09,j))
			{
				BYTE bSize;
				BYTE *pDesc = GetPMTESDescriptor(pBuf,i,0x09,j);
				if(IsCASysIdInList(pDesc[2]<<8)|pDesc[3])
				{
					if(bMatchedDescriptors==0) 
					{
						CAPMTBuf[wTotalLen] = 0x01;//dlh7.7.27
						wTotalLen++;
						wLoopLen++;
					}
					
					CIPMT_DEBUG(("  Descriptor %02X Len %02X (%02X%02X %02X%02X)\r\n",pDesc[0],pDesc[1],pDesc[2],pDesc[3],pDesc[4],pDesc[5]));
					bSize=pDesc[1]+2;
					memcpy(&CAPMTBuf[wTotalLen],pDesc,bSize);
					wTotalLen+=bSize;
					wLoopLen+=bSize;
					bMatchedDescriptors++;
				} else {
					CIPMT_DEBUG(("Discarding CA Descriptor due to CASysId Filtering\r\n"));
				}
				j++;
			}	
			CAPMTBuf[wLoopSizePtr] = wLoopLen>>8;
			CAPMTBuf[wLoopSizePtr+1] = wLoopLen;
			i++;
		}
	}

	CAPMTBuf[wSizePtr] = wTotalLen-4;


	if (((wTotalLen-4) > 127) && ((wTotalLen-4) < 255))
	{
	 	for (i=wTotalLen;i>3;i--)
  			CAPMTBuf[i+1] = CAPMTBuf[i];
  		CAPMTBuf[3] = 0x81;				
 		CAPMTBuf[4] = wTotalLen-4;
	}	
	if ((wTotalLen-4) >= 255)
	{
	 	for (i=wTotalLen;i>3;i--)
  			CAPMTBuf[i+2] = CAPMTBuf[i];
  		CAPMTBuf[3] = 0x82;				
 		CAPMTBuf[4] = (wTotalLen-4)>>8;
 		CAPMTBuf[5] = (wTotalLen-4)&0xFF;
	}	
	return wTotalLen;
}


void SendTransportPacket(BYTE ConnId,BYTE *pData,DWORD dwLen)
{
	BYTE Frame[512];
	unsigned char buff[0x100] = {0};
 	uint8_t p_data1[512];
	int i;
	Frame[0]=ConnId;
	//printf("SendTransportPacket %08X\r\n",pBuf);
	Frame[1]=0;
	memcpy(Frame+2,pData,dwLen);
	dwLen=dwLen+2;
	#if 1
 	read_ci(IO_CARDB,buff); 

    	if(dwLen>128)
    	{
    		write_card_io(0,Frame, 128);
	p_data1[0]=0x01;
	p_data1[1]=0x00;
	memcpy(p_data1+2,Frame+128,dwLen-128+2);
	for(i=0;i<4;i++)
	{
       	ci_io_read_u8(0x01);	
	}
	write_card_io(0,p_data1, dwLen-128+2);
	
    }
    else
    {
       printf("_______________________________________\n");
       for(i=0;i<dwLen;i++)
       {
	 printf("0x%x,",Frame[i]);
       } 
       printf("\n_______________________________________\n");

    	if(write_card_io(0,Frame, dwLen) != dwLen)
    	{
        	printf("cannot write to CAM device\n");
    	}
     }
	
	#endif

}

void Send_T_DataLong(BYTE bConnId,BYTE *pData,DWORD dwLen)
{
	BYTE Msg[1024];	
	if(dwLen+3>=0x80)
	{
		Msg[0]=CI_T_DATALAST;
		Msg[1]=0x82;
		Msg[2]=(BYTE)(dwLen+1)>>8;
		Msg[3]=(BYTE)(dwLen+1)&0xFF;
		Msg[4]=bConnId;
		memcpy(Msg+5,pData,dwLen);
		SendTransportPacket(bConnId,Msg,dwLen+5);
	}
	else
	{
		// small 
		Msg[0]=CI_T_DATALAST;
		Msg[1]=(BYTE)(dwLen+1);
		Msg[2]=bConnId;
		if(dwLen)
			memcpy(Msg+3,pData,dwLen);
		SendTransportPacket(bConnId,Msg,3+dwLen);
	}
}
void Send_SPDU_Data(BYTE bConnID,WORD wSessionNumber,BYTE *pData,DWORD dwLen)
{
	BYTE pFrame[1024];
	//int offset;
	if(dwLen>1024)
	return;
	pFrame[0]=CI_SESSION_DATA;
	pFrame[1]=2;
	pFrame[2]=(BYTE)(wSessionNumber>>8);
	pFrame[3]=(BYTE)(wSessionNumber);
	if(dwLen)
		memcpy(pFrame+4,pData,dwLen);

	Send_T_DataLong(bConnID,pFrame,dwLen+4);
}
void CaSupport_SendPMT(BYTE *pData,DWORD dwLen)
{
	
          Send_SPDU_Data(0x01,
	ca_support_sid,pData,dwLen);		
	
}
unsigned char CA_SEND_PMT(unsigned char *pBuf, unsigned int dwLen,int type)
{
	static unsigned char  CAPMTBuf[MAX_LEN_CA_PMT];
	unsigned short  wCAPMTLen;
//	printf("CA send pmt dwlen:%d\n",dwLen);
//       printf("pBuf:0x%x,0x%x,0x%x,0x%x,0x%x,0x%x",pBuf[0],pBuf[1],pBuf[2],pBuf[3],pBuf[4],pBuf[5]);
          printf("CA send pmt dwlen:%d\n",dwLen);

        //int i;
	// If the CI lib detects a module it re-uses the buffer ptr
	if(dwLen==0)
	return FALSE;
  //   printf("test0\n");
	if(PMTBuf != pBuf) 
	{
		if(dwLen > 1024)
		return FALSE;
		memcpy(PMTBuf,pBuf,dwLen);
		wPMTLen = (WORD)dwLen;
	}
//	printf("test1\n");
	// generate the correct PMT for the modules
	// some module(s) require CA-ID based filtering
	//for(i=0; i<=g_dwNumPCMCIASlots; i++)
	{
		wCAPMTLen = Generate_CA_PMT(CAPMTBuf,pBuf,dwLen,type);
                printf("wCAPMTLen:%d\n",wCAPMTLen);
		CaSupport_SendPMT(CAPMTBuf,wCAPMTLen);
	}
}


//#endif //ENABLE_CI
