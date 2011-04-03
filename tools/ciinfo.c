#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>

#include "kernel_type.h"
#include "pci.h"


void hex_dump(const char *name, unsigned char *buf, int size);

#define ASIC_SWICH_SET_ADDR 0x00000220
#define TRUE 1
#define FALSE 0
#define MAX_PCMCIA_CONFIGS 10
#define MAX_PCMCIA_STRLEN 20
#define MAX_CIS_SIZE 0x150
#define trace_new		printf 
#define PCMCIA_DEBUG	printf

typedef int bool;
typedef unsigned char BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef enum {
	PCMCIA_FUNC_CUSTOM = 0,
	PCMCIA_FUNC_MEMORY = 1,
	PCMCIA_FUNC_SERIAL = 2,
	PCMCIA_FUNC_PARALLEL = 3,
	PCMCIA_FUNC_DRIVE = 4,
	PCMCIA_FUNC_VIDEO = 5,
	PCMCIA_FUNC_NETWORK = 6,
	PCMCIA_FUNC_AIMS = 7,
	PCMCIA_FUNC_SCSI = 8
} PCMCIA_FUNCTYPE;

typedef struct {
	BYTE  bConfigIndex;
	DWORD dwEAAddr;
	DWORD dwEALen;
	BYTE  bIRQDesc1;
	WORD  wIRQData;
	BYTE  fCITagsPresent;
} PCMCIA_CONFIG;

typedef struct 
{
	DWORD ConfigOffset;
#define PCMCIAINFO_MANID_VALID 0x00000001
	WORD wManufacturerId;
	WORD wCardID;
	DWORD dwValidFlags;
#define PCMCIAINFO_VERS1_VALID 0x00000002
	WORD wPCMCIAStdRev;
	char pszManufacturerName[MAX_PCMCIA_STRLEN];
	char pszProductName[MAX_PCMCIA_STRLEN];
	char pszProductInfo1[MAX_PCMCIA_STRLEN];
	char pszProductInfo2[MAX_PCMCIA_STRLEN];
#define PCMCIAINFO_FUNCID_VALID 0x0000004
	PCMCIA_FUNCTYPE FuncType;
	BYTE bFuncIDSysInfo;

	BYTE bNumConfigs;
	PCMCIA_CONFIG Config[MAX_PCMCIA_CONFIGS];
} PCMCIA_INFO;

typedef struct 
{
	BOOL bSlotInUse;
	BYTE bTransportConnID;
	WORD wSessionID;
//	struct RESOURCE_INFO *pResource;
} CI_SESSION;

#define MAX_CI_SESSIONS		10
#define MAX_CASYS_IDS		40
#define CI_T_MAXMENUNAME	20
#define CI_T_SENDQSIZE	10
#define CI_T_SENDQBUFSIZE 0x100
#define MAX_CI_TPDU 4096

typedef struct {
	WORD wBufferSize;
	// TX
	BYTE CI_T_SENDQ[CI_T_SENDQSIZE][CI_T_SENDQBUFSIZE];
	WORD CI_T_SENDQ_Size[CI_T_SENDQSIZE];

} CI_RXTXTPDUBUFFER,*CI_TPDUBUFHANDLE;

static char dvbciManufacturer[20];
static char dvbciProduct[20];
static bool dvbcicard_status=FALSE;


static void show_ci_mem_info(PCMCIA_INFO* pInfo)
{

    fflush(stdout);
    printf("[ManufacturerName :]%s\n", pInfo->pszManufacturerName);
    printf("[ProductName      :]%s\n", pInfo->pszProductName);
    printf("[ManufacturerId   :]%u\n", pInfo->wManufacturerId);
    printf("[CardID           :]%u\n", pInfo->wCardID);
    printf("[ProductInfo1     :]%s\n", pInfo->pszProductInfo1);
    printf("[ProductInfo2     :]%s\n", pInfo->pszProductInfo2);
    fflush(stdout);
}

static void Parse_PCMCIA_AttribMem(unsigned char* pAttribMem, PCMCIA_INFO *pInfo)
{
	BYTE bTag=pAttribMem[0];
	BYTE bLen=pAttribMem[1];
	int i,len;
	BYTE FieldSize;
	BYTE LastIndex;
	BYTE MaskByte;
	BYTE SubTagByte;
	BYTE SubTagLen;
	WORD STCI_IFN;
	char CiDetStr[20];
	BOOL fDefault;
	BOOL fInterface;
	BYTE bFeature;
//	WORD SpecVer;
//	WORD ManuCode;
//	WORD ProductCode;

	len=0;
	memset(pInfo,0x00,sizeof(PCMCIA_INFO));
	do
	{
		bTag=pAttribMem[0];
		bLen=pAttribMem[1];
		len=len+bLen;
		PCMCIA_DEBUG(("\n\rParse_PCMCIA_AttribMem: bTag=0x%.2x %d\n",bTag,len));
		switch(bTag)
		{
		case 0x00:
			;//_asm{ int 3	};
			
		case 0x01:
			//PCMCIA_DEBUG(("CISTPL_DEVICE\r\n"));
			// Memory type
			// Access speed
			// Memory Size
			break;
		case 0x13:
			//PCMCIA_DEBUG(("CISTPL_LINKTARGET\r\n"));
			// "CIS"
			break;
		case 0x15:
			pInfo->wPCMCIAStdRev = pAttribMem[2]<<8|pAttribMem[3];
			pInfo->dwValidFlags |= PCMCIAINFO_VERS1_VALID;
			i=4;
			strncpy(pInfo->pszManufacturerName,(char*)pAttribMem+i,MAX_PCMCIA_STRLEN);
			strncpy(dvbciManufacturer,(char*)pAttribMem+i,MAX_PCMCIA_STRLEN);
			i+=strlen((char*)pAttribMem+i)+1;
			if(i<bLen)
			{
			strncpy(pInfo->pszProductName,(char*)pAttribMem+i,MAX_PCMCIA_STRLEN);
			strncpy(dvbciProduct,(char*)pAttribMem+i,MAX_PCMCIA_STRLEN);
			}
			if(i<bLen)
			i+=strlen((char*)pAttribMem+i)+1;
			if(i<bLen)
			strncpy(pInfo->pszProductInfo1,(char*)pAttribMem+i,MAX_PCMCIA_STRLEN);
			if(i<bLen)
			i+=strlen((char*)pAttribMem+i)+1;
			if(i<bLen)
			strncpy(pInfo->pszProductInfo2,(char*)pAttribMem+i,MAX_PCMCIA_STRLEN);
			trace_new("dvbcidemo card Manufacturer:%s !\n",pInfo->pszManufacturerName);
			trace_new("dvbcidemo card Product:%s !\n",pInfo->pszProductName);
			dvbcicard_status=TRUE;
			PCMCIA_DEBUG(("CISTPL_VERS1, SpecVer %04X\r\nManufacturer:%s\r\nProduct: %s,%s %s\r\n",pInfo->wPCMCIAStdRev,pInfo->pszManufacturerName,pInfo->pszProductName,pInfo->pszProductInfo1,pInfo->pszProductInfo2 ));
			break;
		case 0x17:
			//PCMCIA_DEBUG(("CISTPL_DEVICE_A\r\n"));
			// Like 0x01... for device(s) in attribute memory
			// Memory type
			// Access speed
			// Memory Size
			break;
		case 0x1a:
			PCMCIA_DEBUG(("CISTPL_CONFIG\r\n"));
			/*{
				BYTE bNumAddrBytes;
				BYTE bNumConfigRegs;
				BYTE bLastIndex;

				bNumAddrBytes = (pAttribMem[2]&0x03)+1;
				bNumConfigRegs = ((pAttribMem[2]>>2)&0x0F)+1;
				bLastIndex = pAttribMem[3]&0x3F;
				for(i=0;i<bNumAddrBytes;i++)
				{
					pInfo->ConfigOffset = pInfo->ConfigOffset<<8;
					pInfo->ConfigOffset |= pAttribMem[4+i];
					PCMCIA_DEBUG(("ConfigOffset %04X\r\n",pInfo->ConfigOffset));
				}
				i++;
			}*/
			FieldSize = pAttribMem[2];
			LastIndex = pAttribMem[3];
			if(FieldSize==0)
				pInfo->ConfigOffset=pAttribMem[4];
			else if(FieldSize==1)
				pInfo->ConfigOffset=pAttribMem[5]<<8|pAttribMem[4];
			MaskByte = pAttribMem[5+FieldSize];
			SubTagByte = pAttribMem[6+FieldSize];
			SubTagLen = pAttribMem[7+FieldSize];
			STCI_IFN = pAttribMem[9+FieldSize]<<8|pAttribMem[8+FieldSize];
			strncpy(CiDetStr,(char*)pAttribMem+10+FieldSize,SubTagLen-2);
			CiDetStr[SubTagLen-2]=0;
			PCMCIA_DEBUG(("FieldSize %d, LastIndex %d MaskByte %02X SubTag %02X ID %02X %s\r\n",FieldSize,LastIndex,MaskByte,SubTagByte,STCI_IFN,CiDetStr));
			break;
		case 0x1b:
			PCMCIA_DEBUG(("CISTPL_CFTABLE_ENTRY\r\n"));
			{
				//BYTE bIndex;
				BYTE bIF;
				BYTE bFlags;
				int j;
				pInfo->Config[pInfo->bNumConfigs].bConfigIndex = pAttribMem[2]&0x3F;
				if(pAttribMem[2]&0x40)
					; // Default
				if(pAttribMem[2]&0x80) {
					bIF = pAttribMem[3];
					i=4;
				} else {
					i=3;
				}
				bFlags = pAttribMem[i];
				i++;
				if(bFlags & 0x03)
				{
					BYTE bPowerBits=pAttribMem[i];
					PCMCIA_DEBUG(("PowerDesc %02X\r\n",bPowerBits));
					i++;
					for(j=0;j<7;j++)
					{
						if((bPowerBits>>j)&0x01)
							i++;
						while(pAttribMem[i]&0x80)
							i++; // extension byte
					}
				}
				if(bFlags &0x04) {
					PCMCIA_DEBUG(("TimingDesc %02X\r\n",pAttribMem[i]));
					i++;
				}
				if(bFlags &0x08) {
					if(pAttribMem[i]&0x80)
					{
						BYTE bAddrBytes;
						BYTE bLenBytes;
						BYTE bSize[4]={0,1,2,4};
						BYTE bNumDesc;
						DWORD dwEALen = 0;
						DWORD dwEAAddr = 0;
						bNumDesc = pAttribMem[i+1]&0x0F;
						bAddrBytes = bSize[(pAttribMem[i+1]>>4)&0x03];
						bLenBytes = bSize[(pAttribMem[i+1]>>6)&0x03];
						PCMCIA_DEBUG(("EADesc %02X %d %d %d\r\n",pAttribMem[i+1],bNumDesc,bAddrBytes,bLenBytes));
						i+=2;
						switch(bAddrBytes)
						{
						case 1:
							dwEAAddr = pAttribMem[i];
							break;
						case 2:
							dwEAAddr = pAttribMem[i]|pAttribMem[i+1]<<8;
							break;
						case 4:
							dwEAAddr = pAttribMem[i]|pAttribMem[i+1]<<8|pAttribMem[i+2]<<16|pAttribMem[i+3]<<24;
							break;
						}
						pInfo->Config[pInfo->bNumConfigs].dwEAAddr = dwEAAddr;
						i+=bLenBytes;
						switch(bLenBytes)
						{
						case 1:
							dwEALen = pAttribMem[i];
							break;
						case 2:
							dwEALen = pAttribMem[i]|pAttribMem[i+1]<<8;
							break;
						case 4:
							dwEALen = pAttribMem[i]|pAttribMem[i+1]<<8|pAttribMem[i+2]<<16|pAttribMem[i+3]<<24;
							break;
						}
						pInfo->Config[pInfo->bNumConfigs].dwEALen = dwEALen;
						i+=bAddrBytes;
						PCMCIA_DEBUG(("Addr %04X Len %04X",dwEAAddr,dwEALen));
					} else
						i++;
				}
				if(bFlags &0x10) {
					PCMCIA_DEBUG(("IrqDesc\r\n"));
					pInfo->Config[pInfo->bNumConfigs].bIRQDesc1 = pAttribMem[i];
					if(pAttribMem[i]&0x10) {
						pInfo->Config[pInfo->bNumConfigs].wIRQData=pAttribMem[i]<<8|pAttribMem[i+1];
						i+=2;
					}
					i++;
				}
				if(bFlags &0x60) {
					PCMCIA_DEBUG(("MemoryDesc\r\n"));
					i++;
				}
				if(bFlags &0x80) {
					PCMCIA_DEBUG(("MixedDesc\r\n"));
					i++;
				}


				while(i<(bLen+2))
				{
				PCMCIA_DEBUG(("SubTag 0x%02X %d %d\n",pAttribMem[i],i,bLen));
					if(pAttribMem[i]==0xc0) {
						if(strcmp((char*)pAttribMem+i+2,"DVB_HOST")==0)
							pInfo->Config[pInfo->bNumConfigs].fCITagsPresent |= 0x01;
						PCMCIA_DEBUG(("%s\r\n",pAttribMem+i+2));
					}
					if(pAttribMem[i]==0xc1) {
						if(strcmp((char*)pAttribMem+i+2,"DVB_CI_MODULE")==0)
							pInfo->Config[pInfo->bNumConfigs].fCITagsPresent |= 0x02;
						PCMCIA_DEBUG(("%s\r\n",pAttribMem+i+2));
					}
				i+=pAttribMem[i+1]+2;
				}

				pInfo->bNumConfigs++;			
			}
			fDefault = pAttribMem[2]>>7;
			fInterface = (pAttribMem[2]>>6)&0x01;
			PCMCIA_DEBUG(("(default) "));
			if(fInterface)
				PCMCIA_DEBUG(("IF 0x%02X ",pAttribMem[3]));
			bFeature = pAttribMem[3+fInterface];
			
			PCMCIA_DEBUG(("\r\n"));
			PCMCIA_DEBUG(("%d\n",bLen));
			
			for(i=0;i<bLen;i++)
				PCMCIA_DEBUG(("0x%02X ",pAttribMem[2+i]));
			PCMCIA_DEBUG(("\r\n"));
			for(i=0;i<bLen;i++)
				PCMCIA_DEBUG(("%c  ",pAttribMem[2+i]));
			PCMCIA_DEBUG(("\r\n"));
		
			break;
		case 0x1c:
			PCMCIA_DEBUG(("CISTPL_DEVICE_OC\r\n"));
			break;
		case 0x1D:
			PCMCIA_DEBUG(("CISTPL_DEVICE_OA\r\n"));
			break;
		case 0x20:
			//PCMCIA_DEBUG(("CISTPL_MANFID\r\n"));
			pInfo->dwValidFlags |= PCMCIAINFO_MANID_VALID;
			pInfo->wManufacturerId = pAttribMem[2]<<8|pAttribMem[3];
			pInfo->wCardID = pAttribMem[4]<<8|pAttribMem[5];
			PCMCIA_DEBUG(("Manufacturer code %04X Product Code %04X\r\n",pInfo->wManufacturerId,pInfo->wCardID));
			break;
		case 0x21:
			//PCMCIA_DEBUG(("CISTPL_FUNCID\r\n"));
			pInfo->dwValidFlags |= PCMCIAINFO_FUNCID_VALID;
			pInfo->FuncType = (PCMCIA_FUNCTYPE)pAttribMem[2];
			pInfo->bFuncIDSysInfo = pAttribMem[3];
			break;
		case 0x40: // CISTPL_VERS2
			break;
		default:
			PCMCIA_DEBUG(("Tag %02X, Len %d\r\n",bTag,bLen));
			for(i=0;i<bLen;i++)
				PCMCIA_DEBUG(("%02X ",pAttribMem[2+i]));
			PCMCIA_DEBUG(("\r\n"));
			for(i=0;i<bLen;i++)
				PCMCIA_DEBUG(("%c",pAttribMem[2+i]));
			PCMCIA_DEBUG(("\r\n"));
			break;
		}
		pAttribMem+=2+bLen;
	} while(bTag!=0x14&&bTag!=0xFF&&len<MAX_CIS_SIZE);
	return;
}

uint8_t ci_r16(int off)
{
	uint16_t v = *(volatile uint16_t *)(0xC0000000 + off);
	return (uint8_t)v;
}
int main(int argc, char **argv)
{
	PCMCIA_INFO Info;
	uint8_t attr_mem[0x150];
	int i = 0, j = 0;

	if (!sPCCARDCON.ACT) {
		printf("Card is't active! do nothing!\n");
		exit(1);
	}
	while (1) {
		attr_mem[j] = ci_r16(i);
		i += 2;
		j++;
		if (j >= 0x150) {
			hex_dump("attr mem", attr_mem, 0x150);
			Parse_PCMCIA_AttribMem(attr_mem, &Info);
			show_ci_mem_info(&Info);
            		j = 0;
			i = 0;
            		break;
		}
	}

	printf("[Info.ConfigOffset is :] 0x%X\n", Info.ConfigOffset);
	printf("[Info.Config[i].bConfigIndex is :] 0x%X\n", Info.Config[1].bConfigIndex);
}

void hex_dump(const char *name, unsigned char *buf, int size)
{
	int i;

	if (size <= 0)
		return;

	printf("%s(buf %p, size %d): 0x\n", name, buf, size);
	for (i = 0; i < size; i++) {
		printf("%02x ", *buf);
		if ((i + 1) % 16 == 0)
			putchar('\n');
		buf++;
	}
	printf("\n");
}

