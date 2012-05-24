/*
 * create by why @ 2009 06 10 10:56
 * ci ctrl card
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <semaphore.h>
#include "ci_define.h"
#define ASIC_SWICH_SET_ADDR 0x00000220
#define MAX_PCMCIA_CONFIGS 10
#define MAX_PCMCIA_STRLEN 20
#define MAX_CIS_SIZE 0x150
#define trace_new(...) 
#define PCMCIA_DEBUG(...)

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

typedef struct 
{
	////////////////////////////////
	// PCMCIA related
	void* hSlotHandle;	// handle to the PCMCIA Driver
	WORD wSlotNum;				// internal (0-based) slot number
	BOOL fHighPrio;				// hint to the PCMCIA Driver (for P Modules)

	////////////////////////////////
	// Buffer related
	CI_RXTXTPDUBUFFER IOBuffer_Host2CI;
	BYTE CI_T_RX[MAX_CI_TPDU];
	BYTE TPDU[MAX_CI_TPDU];
	DWORD TPDULen;

	
	////////////////////////////////
	// Misc
#ifdef NOPSOS
	short PollCount; // Polling for non-multitasking OS
#endif
	BYTE ModulReset; // internal flag... causes a call to PCMCIA Reset later

	////////////////////////////////
	// Session related
	CI_SESSION CiSessions[MAX_CI_SESSIONS];

	WORD wAppInfoSession;
	char menu_title[CI_T_MAXMENUNAME];
	BYTE title_length;

	BOOL fIsPremiereModule;
	BOOL fModuleRequiresCAIDFiltering;

	DWORD dwCASupportSession;
	WORD ca_system_ids[MAX_CASYS_IDS];
	WORD wNumCASystems;

	DWORD dwDateTimeSession;
	BYTE 	datetime_intervall;
	DWORD timeticks;
	
	DWORD dwMMISession;

	DWORD dwAuthSession;

	DWORD dwMacrovisionSession;
	
	DWORD dwHostControlSession;

	DWORD dwPinMgmtSession;
} CI_HANDLESTRUCT,*CI_HANDLE;
/*** ÂÆö‰πâÁªìÊùü ***/

static char dvbciManufacturer[20];
static char dvbciProduct[20];
static bool dvbcicard_status=FALSE;
cards_status all_card_status = {
                                       .dcas_cnt = 0,
                                       .max_dcas = 6,
                                       .card_status = 0};


static unsigned char create_link(uint32_t cardtype);
unsigned char  conn_id=CONN_ID; 
unsigned short res_mng_sid=1;
unsigned short app_inf_sid=2;
unsigned short ca_support_sid=3;
unsigned short host_ctrl_sid=4;
unsigned short date_time_sid=5;
unsigned short mmi_sid=6;
unsigned short low_speed_comm_sid=7;

//ø®≤Â»Î–≈∫≈µ∆
static sem_t ci_init_sem_a;
static sem_t ci_init_sem_b;


extern unsigned char wait_for_io_bit(uint32_t cardtype,unsigned char mask, unsigned int wait_time);
extern unsigned int read_card_io(uint32_t cardtype,unsigned char* buff);
extern unsigned short write_card_io(uint32_t cardtype,unsigned char* buff, 
                             unsigned short len);
extern unsigned int test_read_bit(uint32_t cardtype);
extern unsigned int test_write_bit(uint32_t cardtype);
unsigned short detect_ci_status();
void Set_Ci_CPLD_Mode(uint32_t mode);
//extern unsigned int GetFPGASwtichMode(void);
extern unsigned int fpgaswitchmode;
extern unsigned int ci_max_cnt;
extern bool ci_card_actived();
extern uint16_t ci_attr_mem_read_u16(int off);
extern bool ci_io_write_u8(int off, uint8_t v);
extern uint8_t ci_io_read_u8(int off);
extern bool ci_mem_write_u16(int off, uint16_t v);
extern uint16_t ci_mem_read_u16(int off);

static unsigned short get_ci_buff_size(uint32_t cardtype)
{
    char v = 0;
    unsigned short buffSize = 0;
    unsigned char ci_size[2] = {0};

    ci_io_write_u8(0x01, 0x04);
    while(!(ci_io_read_u8(0x01) & 0x80)){
        usleep(1000);
    }

    read_card_io(cardtype,ci_size);
    ci_io_write_u8(0x01, 0x00);

    buffSize = (ci_size[0]<<2)+ci_size[1];

    if(buffSize>0x100)
        buffSize = 0x100;

    return buffSize;
}

static void set_ci_buff_size(unsigned short size,uint32_t cardtype)
{
    unsigned char buff_size[2] = {size&0xFF, size>>8};
    int i;
    ci_io_write_u8(0x01, 0x02);
	
	while(!(ci_io_read_u8(0x01)&0x40))
        usleep(1000);

        printf("set_ci_buff_size 0x%x\n",size);
	 ci_io_write_u8(0x01, 0x01);
	 while(!(ci_io_read_u8(0x01)&0x40)){
   		;
    	}
	
        ci_io_write_u8(0x03, 0x00);
	ci_io_write_u8(0x02, 0x02);

    for (i=0; i<2; i++){
        ci_io_write_u8(0x00, buff_size[i]);
    }

    ci_io_write_u8(0x01, 0x00);
  // write_card_io(cardtype,buff_size, 2);
	ci_io_write_u8(0x01, 0x00);
}
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
			trace_new(TRACE_LEVEL_Mhp,"dvbcidemo card Manufacturer:%s !\n",pInfo->pszManufacturerName);
			trace_new(TRACE_LEVEL_Mhp,"dvbcidemo card Product:%s !\n",pInfo->pszProductName);
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

static void read_ci_mem()
{
    PCMCIA_INFO Info;
    unsigned short v = 0;
    unsigned char ci_mem[0x150] = {0};
    int i=0;
    int j=0;
    unsigned char dat;
    while(1){
        ci_mem[j] = (ci_mem_read_u16(i+0x0000000));
      // printf("ci_mem%d:0x%x\n",i,ci_mem[j]);
        i+=2;
        j++;

        if(i>0x150)
        {
            Parse_PCMCIA_AttribMem(ci_mem, &Info);
            show_ci_mem_info(&Info);

            usleep(500000);
            printf("\n************************************************\n");
            j = 0;
            i = 0;
            break;
        }
    }

    printf("[Info.ConfigOffset is :] 0x%X\n", Info.ConfigOffset);
    printf("[Info.Config[i].bConfigIndex is :] 0x%X\n", Info.Config[1].bConfigIndex);
    //usi_ebi_ci_write16(Info.ConfigOffset,Info.Config[1].bConfigIndex);
    
    dat=ci_mem_read_u16(Info.ConfigOffset);
	printf("CI memory read dat=0x%x\n",dat);
	ci_mem_write_u16(Info.ConfigOffset,0x0f);
}
#if 1
static void init_card_buff_size(uint32_t cardtype)
{
    unsigned short buffsize = 0;
    unsigned char dat=0;
	unsigned char flag=0;
	unsigned char Size_Reg[2] = {0x00};
    unsigned short bsize = 0x00;
	 unsigned char ci_size[256] = {0};
  int i;
  
   printf("init_card_buff_size start\n");
 // usi_ebi_ci_write8(0x00, 0x06);
// dat=usi_ebi_ci_read8(0x00);
 // printf("read reg00 dat=0x%x\n",dat);
    ci_io_write_u8(0x01, 0x08);
   printf("reg01:write dat=0x08\n");
   sleep(1);
   ci_io_write_u8(0x01, 0x00);
    printf("reg01:write dat=0x00\n");
    // usleep(100000);
  // dat= usi_ebi_ci_read8(0x01);
   //printf("ci reg0x01:%d\n",dat);
   #if 0
    while((usi_ebi_ci_read8(0x01) & 0x40)){
        usleep(1000);
	
    }
	#endif
	while(1)
	{
		flag=ci_io_read_u8(0x01) ;
		printf("wait 01reg=0x%x\n",flag);
		if((flag&0x40)==0x40)
	        break;
		usleep(1000);
	}

	 ci_io_write_u8(0x01, 0x04);
	  printf("reg01:write dat=0x04\n");
      //   usleep(100000);
	 while(1)
	{
		flag=ci_io_read_u8(0x01) ;
		//printf("wait 01reg=0x%x\n",flag);
		if((flag&0x80)==0x80)
		{
			flag=ci_io_read_u8(0x01) ;
	              //  printf("flag=0x%x\n",flag);
	        	break;
		}
	     usleep(1000);
	}
	 printf("reg01:read dat=0x%x\n",flag);
      //  usleep(100000);
	 flag=ci_io_read_u8(0x01) ;
	 printf("flag=0x%x\n",flag);
	if((flag&0x80)==0x80)
     {
       // sleep(2);
   	  Size_Reg[1] = ci_io_read_u8(0x03);
       Size_Reg[0] = ci_io_read_u8(0x02);
      
   
          bsize = Size_Reg[0] | Size_Reg[1] << 8;
          printf("size0=0x%2x,size1=0x%2x\n",Size_Reg[0],Size_Reg[1]);
   		}
        for (i=0; i<bsize; i++)
         ci_size[i] = (ci_io_read_u8(0x00)&0x00FF);
         ci_io_write_u8(0x01, 0x00);
    //get buffer size from card
  //  buffsize = get_ci_buff_size(cardtype);
     buffsize = (ci_size[0]<<8)+ci_size[1];
	 printf("buffsize=0x%2x\n",buffsize);
    //set card buffer
    //buffsize=0x100;
    set_ci_buff_size(buffsize,cardtype);
}
#endif

int waitforbit(unsigned char mask)
{
	int i;
	for(i=0;i<20;i++)
	{
		if(ci_io_read_u8(0x01)&mask)
		return 1;
		usleep(60000);
	}
	return 0;
}


static void init_card()
{
    printf("init card \n");


       read_ci_mem();

       init_card_buff_size(IO_CARDA);
	//create_link(IO_CARDA);
}


static unsigned char create_link(uint32_t cardtype)
{
    int ret = 0;
    unsigned char buff[0x100] = {0};
    ret = send_t_create_pdu(cardtype,conn_id);
    if(ret)
    {
        ret = read_ci(cardtype,buff, sizeof(buff));
    }
    
    return ret;
}


unsigned short detect_ci_board(){
	return ci_io_read_u8(CI_CPLD | CPLD_CIBOARDID);
}

unsigned short detect_ci_vccen(){
	return ci_io_read_u8(CI_CPLD | CPLD_CIENABLE);	
}

unsigned short detect_ci_status(){
	return ci_io_read_u8(CI_CPLD | CPLD_CARDSTATUS);	
}

void open_ci_vccen(){
unsigned short vccen_reg;
	vccen_reg = ci_io_write_u8(CI_CPLD | CPLD_CIENABLE,CI_EN);	
}
void Set_Ci_CPLD_Mode(uint32_t mode)
{
	ci_io_write_u8(CI_CPLD | CPLD_CONTROL,mode);
}
void Set_Ci_CPLD_RESET(uint32_t mode)
{
	ci_io_write_u8(CI_CPLD | CPLD_CIRESET,mode);
}
unsigned int init_ci()
{
    int sem_init_ret;
    unsigned int ret = CI_INIT_OVER;
    unsigned short ci_stus=0;

    Set_CARD_RESET(1);
    usleep(100000);
    Set_CARD_RESET(0);

   usleep(100000);
   if(ci_card_actived()==TRUE)
   {
   	all_card_status.card_status=CARD_IS_EXIST;
   	all_card_status.card_exist=CARDA_ONLY;
   }
   else
   {
   	return CI_NOT_EXIST;
   }
   init_card();
   if(CARD_IS_EXIST == all_card_status.card_status)
    {
        //sel_card_A();
	printf("sel_card_A!\n");	
		 

        if(!create_link(IO_CARDA))
        {
            ret = CI_INIT_FAILED;
             return ret;
        }

    }


    return ret;
}

unsigned int CIStart()
{
    unsigned ret = 0;
     printf("CIStartUp!\n");

    ret = init_ci();

    if(CI_INIT_OVER != ret)
    {
        return 0;
    }

    ret = start_query_ci();

    return ret;
}

cards_status* get_cards_status()
{
    return (&all_card_status);
}

