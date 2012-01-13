#ifndef ST_OSLINUX 
	#include "stdlib.h"
	#include "stdio.h"
	#include "string.h"
#endif


#ifdef HOST_PC
	#include "windows.h" 
	static HANDLE ChipMutex = NULL;
	int PPortAdr;
#endif 		

#include "globaldefs.h" 
#include "chip.h"
//#include "i2c.h"
#include "stddefs.h"
//#include"heard.h"
#define  uchar unsigned char
#define FSTV6110_CO_DIV  0x000100c0
#define I2C_READ 0
#define I2C_WRITE 1
#define I2C_WRITENOSTOP 2
#define MAX_UART_LEN 200




typedef struct node 
{
	STCHIP_Handle_t hChip;
	struct node *pNextNode;
}NODE;

static NODE *pFirstNode = NULL;


#if 0
/* List routines	*/
static NODE *AppendNode(STCHIP_Handle_t hChip)
{
	NODE *pNode = pFirstNode;
	
	if(pNode == NULL)
	{   /* Allocation of the first node	*/
		pNode = calloc(1,sizeof(NODE));
		pFirstNode = pNode;
	}
	else
	{   /* Allocation of a new node */
		while(pNode->pNextNode != NULL)	/* Search of the last node	*/
			pNode = pNode->pNextNode;
			
		pNode->pNextNode = calloc(1,sizeof(NODE));	/* Memory allocation */
		
		if(pNode->pNextNode != NULL)				/* Check allocation */
			pNode = pNode->pNextNode;
		else
			pNode = NULL;
	}
	
	if(pNode != NULL)	/* if allocation ok	*/
	{
		/*	Fill the node	*/
		pNode->hChip = hChip;
		pNode->pNextNode = NULL;	
	}
	
	return pNode;
}

static void DeleteNode(NODE *pNode)
{
	NODE *pPrevNode = pFirstNode;
	
	if(pNode != NULL)
	{
		if(pNode == pFirstNode) 
		{
			/* Delete the first node	*/
			pFirstNode = pNode->pNextNode;
		}
		else
		{
			/* Delete a non-first node	*/
			while(pPrevNode->pNextNode != pNode)	/* search the node before the node to delete */
				pPrevNode = pPrevNode->pNextNode;
				
			if(pNode->pNextNode == NULL)
				pPrevNode->pNextNode = NULL;		/* the node to delete is the last */
			else	
				pPrevNode->pNextNode = pPrevNode->pNextNode->pNextNode;	/* the node to delete is not the last */
		}
		
		free(pNode);	/* memory deallocation */
	}
}

#endif
/*****************************************************
**FUNCTION	::	ChipGetFirst
**ACTION	::	Retrieve the first chip handle
**PARAMS IN	::	NONE
**PARAMS OUT::	NONE
**RETURN	::	STCHIP_Handle_t if ok, NULL otherwise
*****************************************************/
STCHIP_Handle_t	ChipGetFirst(void)
{
	if((pFirstNode != NULL) && (pFirstNode->hChip != NULL))
		return pFirstNode->hChip;	
	else
		return NULL;
}

/*****************************************************
**FUNCTION	::	ChipFindNode
**ACTION	::	Find that node that contains the chip 
**PARAMS IN	::	NONE
**PARAMS OUT::	NONE
**RETURN	::	STCHIP_Handle_t if ok, NULL otherwise
*****************************************************/
NODE *ChipFindNode(STCHIP_Handle_t hChip)
{
	NODE *pNode = pFirstNode;
	
	if(pNode != NULL)
	{
		while((pNode->hChip != hChip) && (pNode->pNextNode != NULL))
			pNode = pNode->pNextNode;
		
		if(pNode->hChip != hChip)
			pNode = NULL;	
		
	}
	
	return pNode;
}

/*****************************************************
**FUNCTION	::	ChipGetNext
**ACTION	::	Retrieve the handle of the next chip
**PARAMS IN	::	hPrevChip	==> handle of the previous chip
**PARAMS OUT::	NONE
**RETURN	::	STCHIP_Handle_t if ok, NULL otherwise
*****************************************************/
STCHIP_Handle_t	ChipGetNext(STCHIP_Handle_t hPrevChip)
{
	NODE *pNode;
	
	pNode = ChipFindNode(hPrevChip);
	if((pNode != NULL) && (pNode->pNextNode != NULL))
		return pNode->pNextNode->hChip;
	else
		return NULL; 
}

/*****************************************************
**FUNCTION	::	ChipGetHandleFromName
**ACTION	::	Retrieve the handle of chip with its name
**PARAMS IN	::	Name	==> name of the chip
**PARAMS OUT::	NONE
**RETURN	::	STCHIP_Handle_t if ok, NULL otherwise
*****************************************************/
STCHIP_Handle_t ChipGetHandleFromName(ST_String_t *Name)
{
	STCHIP_Handle_t hChip;
	
	hChip = ChipGetFirst();
	while((hChip != NULL) && (strcmp((char *)hChip->Name,(char *)Name) != 0))
	{
		hChip = ChipGetNext(hChip);
	}
	
	return hChip;
}



/*****************************************************
**FUNCTION	::	ChipOpen
**ACTION	::	Open a new chip
**PARAMS IN	::	Name	==> Name of the chip
**				I2cAddr	==> I2C address of the chip
**				NbRegs	==> number of register in the chip
**				NbFields==> number of field in the chip
**PARAMS OUT::	NONE
**RETURN	::	Handle to the chip, NULL if an error occur
*****************************************************/
#if 0
STCHIP_Handle_t ChipOpen(STCHIP_Info_t *hChipOpenParams)
{
	STCHIP_Handle_t hChip;
	S32 reg;
	#ifdef HOST_PC
		char mutex_name[100];
	#endif
	
	hChip = calloc (1,sizeof(STCHIP_Info_t));					/* Allocation of the chip structure	*/
	
	if((hChip != NULL) && (hChipOpenParams != NULL))
	{
		hChip->pRegMapImage = calloc(hChipOpenParams->NbRegs,sizeof(STCHIP_Register_t));		/* Allocation of the register map	*/
		
		if(hChip->pRegMapImage != NULL)
		{
			if((ChipGetHandleFromName(hChipOpenParams->Name)==NULL) && (AppendNode(hChip)!=NULL)) 
			{
				hChip->I2cAddr = hChipOpenParams->I2cAddr;
				strcpy((char *)hChip->Name,(char *)hChipOpenParams->Name);
				hChip->NbRegs = hChipOpenParams->NbRegs;
				hChip->NbFields = hChipOpenParams->NbFields;
				hChip->ChipMode = hChipOpenParams->ChipMode;
				hChip->Repeater = hChipOpenParams->Repeater;
				hChip->RepeaterHost = hChipOpenParams->RepeaterHost;
				hChip->RepeaterFn = hChipOpenParams->RepeaterFn;
				hChip->WrStart  = hChipOpenParams->WrStart;
			    hChip->WrSize   = hChipOpenParams->WrSize;     
			    hChip->RdStart  = hChipOpenParams->RdStart;     
			    hChip->RdSize   = hChipOpenParams->RdSize;     
				hChip->pData = hChipOpenParams->pData;
				hChip->Error = CHIPERR_NO_ERROR;
				
				for(reg=0;reg<hChip->NbRegs;reg++)
				{
					hChip->pRegMapImage[reg].Addr=0;
					hChip->pRegMapImage[reg].Value=0;
				}

				#if 0
				#ifdef HOST_PC
						if(ChipMutex == NULL)
						{
							sprintf(mutex_name,"I2C_%04x",PPortAdr);			/*	Create mutex name */
							ChipMutex = CreateMutex(NULL,FALSE,mutex_name);		/*Create mutex or use existing one*/
						}
				#endif
				#endif
			}
		}
		else
		{
			free(hChip);
			hChip = NULL;	
		}
	}
	
	return hChip;
}
#endif
/*****************************************************
**FUNCTION	::	ChipUpdateDefaultValues
**ACTION	::	update the default values of the RegMap chip
**PARAMS IN	::	hChip	==> handle to the chip
			::	pRegMap	==> pointer to 
**PARAMS OUT::	NONE
**RETURN	::	CHIPERR_NO_ERROR if ok, CHIPERR_INVALID_HANDLE otherwise
*****************************************************/
STCHIP_Error_t  ChipUpdateDefaultValues(STCHIP_Handle_t hChip,STCHIP_Register_t  *pRegMap)
{
	STCHIP_Error_t error = CHIPERR_NO_ERROR;
	S32 reg;
	
	if(hChip != NULL)
	{
		for(reg=0;reg<hChip->NbRegs;reg++)
		{
			hChip->pRegMapImage[reg].Addr=pRegMap[reg].Addr;
			hChip->pRegMapImage[reg].Value=pRegMap[reg].Value;
		}
	}
	else
		error = CHIPERR_INVALID_HANDLE;
	
	return error;
}

/*****************************************************
**FUNCTION	::	ChipClose
**ACTION	::	Close a chip
**PARAMS IN	::	hChip	==> handle to the chip
**PARAMS OUT::	NONE
**RETURN	::	CHIPERR_NO_ERROR if ok, CHIPERR_INVALID_HANDLE otherwise
*****************************************************/

STCHIP_Error_t	ChipClose(STCHIP_Handle_t hChip)
{
	STCHIP_Error_t error = CHIPERR_NO_ERROR;
	#if 1
	if(hChip != NULL)
	{
		#if 0
		#ifdef HOST_PC
			if(ChipFindNode(hChip) == pFirstNode)
			{
				CloseHandle(ChipMutex);
				ChipMutex = NULL;
			}
		#endif
		
		DeleteNode(ChipFindNode(hChip));
		free(hChip->pRegMapImage);
		free(hChip);
             #endif
	}
	else
		error = CHIPERR_INVALID_HANDLE;
	#endif
	return error; 
}


/*****************************************************
**FUNCTION	::	CreateMask
**ACTION	::	Create a mask according to its number of bits and position 
**PARAMS IN	::	field	==> Id of the field
**PARAMS OUT::	NONE
**RETURN	::	mask of the field
*****************************************************/
U8 CreateMask(U8 NbBits, U8 Pos)
{
	S32 i;
	U8 mask=0;
	
	/*	Create mask	*/
	for (i = 0; i < NbBits; i++)
	{
		mask <<= 1 ;
		mask +=  1 ;
	}
	  
	mask = mask << Pos;
	
	return mask;
}

/*****************************************************
**FUNCTION	::	ChipGetRegisterIndex
**ACTION	::	Get the index of a register from the pRegMapImage table
**PARAMS IN	::	hChip		==> Handle to the chip
**				RegId		==> Id of the register (adress)
**PARAMS OUT::	None 
**RETURN	::	Index of the register in the register map image
*****************************************************/
S32 ChipGetRegisterIndex(STCHIP_Handle_t hChip, U16 RegId)
{
	S32 reg=0,
		regIndex=-1;
 
	if(hChip)
	{
		while(reg < hChip->NbRegs) 
		{
			if(hChip->pRegMapImage[reg].Addr == RegId)
			{
				regIndex=reg;
			}

			reg++;
		}
	}
	return regIndex;
}

/*****************************************************
**FUNCTION	::	ChipGetFieldIndex
**ACTION	::	Get the index of a register from the pRegMapImage table
**PARAMS IN	::	hChip		==> Handle to the chip
**				RegId		==> Id of the fileld (adress)
**PARAMS OUT::	None
**RETURN	::	Index of the register in the register map image
*****************************************************/
S32 ChipGetFieldIndex(STCHIP_Handle_t hChip, U32 FieldId)
{
	S32 reg=0,
		regIndex=-1;
	U16 regAddress;
	
	if(hChip)
	{

		regAddress=(FieldId >> 16)&0xFFFF;
		while(reg < hChip->NbRegs)
		{
			if(hChip->pRegMapImage[reg].Addr == regAddress)
			{
				regIndex=reg;	
			}

			reg++;
		}
	}
	
	return regIndex;
}

/*****************************************************
**FUNCTION	::	ChipSetOneRegister
**ACTION	::	Set the value of one register 
**PARAMS IN	::	hChip	==> Handle to the chip
**				reg_id	==> Id of the register
**				Data	==> Data to store in the register
**PARAMS OUT::	NONE
**RETURN	::	Error
*****************************************************/
#if 0
STCHIP_Error_t ChipSetOneRegister(STCHIP_Handle_t hChip,U16 RegAddr,U8 Data)   
{
	S32 regIndex;
	
	if(hChip)
	{
		regIndex =ChipGetRegisterIndex(hChip, RegAddr);
		
		if((regIndex >= 0) && (regIndex  < hChip->NbRegs)) 
		{
			hChip->pRegMapImage[regIndex].Value=Data;
			ChipSetRegisters(hChip,RegAddr,1);
		}
		else
			hChip->Error = CHIPERR_INVALID_REG_ID;
	}
	else
		return CHIPERR_INVALID_HANDLE;
	
	return hChip->Error;
}
#endif

void ChipSetOneRegister_E(U8 chipaddress,STCHIP_Mode_t mode,BOOL Repeater,U16 RegAddr,U8 Data)   
{
	unsigned char dat[2];
	 I2cReadWrite(I2C_WRITE,chipaddress,RegAddr,&Data,1);  
	 //WAIT_N_MS(1000);
	 //dat[0]=(RegAddr>>8)&0x00ff;
	 //dat[1]=RegAddr;
	 //hex_out(dat, 2);
}	

void Set0903(unsigned char len,unsigned char* data,U16 addr)
{
	unsigned char i;
	unsigned char dat;
	if(len>MAX_UART_LEN)
	len=MAX_UART_LEN;
	for(i=0;i<len;i++)
	{
		ChipSetOneRegister_E(0xd0,STCHIP_MODE_SUBADR_16,FALSE,addr+i,data[i]);
	}
}

void Set6110(unsigned char len,unsigned char* data,unsigned char addr)
{
	unsigned char i;
	if(len>MAX_UART_LEN)
	len=MAX_UART_LEN;

	for(i=0;i<len;i++)
	{
		ChipSetOneRegister_E(0xc0,STCHIP_MODE_SUBADR_8,TRUE,addr+i,data[i]);
		
	}
}

U8 ChipGetOneRegister_E(U8 chipaddress,STCHIP_Mode_t mode,BOOL Repeater, U16 RegAddr,U16 RdStart,U16 RdSize)
{                                                              //                                                                                        //            
        U8 data = 0xFF;
        //U8 databuf[8];
//      U8 regIndex; 
        //U8 i;
//   printf("ChipGetOneRegister:0x%x,0x%x\n",chipaddress,RegAddr);
        if(chipaddress == 0xd0)
        {
                I2cReadWrite(I2C_READ,chipaddress,RegAddr,&data,1);
                        return data;
        }
        else
        {
                I2cReadWrite(I2C_READ,chipaddress,RegAddr,&data,1);
                return data;
        }
}



void Get0903(unsigned char len,unsigned char* data,U16 addr)
{
	unsigned char i;
	unsigned char dat=0;
	unsigned char reg=0;
	if(len>MAX_UART_LEN)
	len=MAX_UART_LEN;
	for(i=0;i<len;i++)
	{
		*data=ChipGetOneRegister_E(0xd0,STCHIP_MODE_SUBADR_16,FALSE,addr+i,0,0);
		//hex_out(data, 1);
		//reg=addr>>8;
		//display_hex(reg,8);
		//reg=addr&0x00ff;
		//display_hex(reg,10);
		//display_hex(*data,8); 
		data++;
	}
}



/*****************************************************
**FUNCTION	::	ChipGetOneRegister
**ACTION	::	Get the value of one register 
**PARAMS IN	::	hChip	==> Handle to the chip
**				reg_id	==> Id of the register
**PARAMS OUT::	NONE
**RETURN	::	Register's value
*****************************************************/
#if 0
S32 ChipGetOneRegister(STCHIP_Handle_t hChip, U16 RegAddr)         
{
	U8 data = 0xFF;
	S32 regIndex; 
	
	if(hChip)
	{
		regIndex =ChipGetRegisterIndex(hChip, RegAddr);
		
		if((regIndex >= 0) && (regIndex  < hChip->NbRegs))
		{
			if(hChip->ChipMode != STCHIP_MODE_NOSUBADR)
			{
				if(ChipGetRegisters(hChip,RegAddr,1) == CHIPERR_NO_ERROR)
					data = hChip->pRegMapImage[regIndex].Value;
			}
			else
			{
				if(ChipGetRegisters(hChip,hChip->RdStart,hChip->RdSize) == CHIPERR_NO_ERROR)  
					data = data = hChip->pRegMapImage[regIndex].Value;
			}
		}
		else
			hChip->Error = CHIPERR_INVALID_REG_ID;
	}
	else
		hChip->Error = CHIPERR_INVALID_HANDLE;
		
	return data;
		
	
}
#endif
//	carFreq=ChipGetOneRegister_E(STV0903_I2C_ADDRESS,STCHIP_MODE_SUBADR_16,FALSE,RSTV0903_CARFREQ,0,0);		



/*****************************************************
**FUNCTION	::	ChipSetRegisters
**ACTION	::	Set values of consecutive's registers (values are taken in RegMap)
**PARAMS IN	::	hChip		==> Handle to the chip
**				FirstReg	==> Id of the first register
**				NbRegs		==> Number of register to write		
**PARAMS OUT::	NONE
**RETURN	::	Error
*****************************************************/
#if 0
STCHIP_Error_t ChipSetRegisters(STCHIP_Handle_t hChip, U16 FirstReg, S32 NbRegs)
{
	U8 data[100],nbdata = 0;
	S32 i,
		firstRegIndex;

	if(hChip)
	{
		if(!hChip->Error)
		{
			if(NbRegs < 70)	
			{
				firstRegIndex =ChipGetRegisterIndex(hChip, FirstReg);
				if((firstRegIndex >= 0) && ((firstRegIndex + NbRegs - 1) < hChip->NbRegs)) 
				{
					switch(hChip->ChipMode)
					{
						case STCHIP_MODE_SUBADR_16:
							//data[nbdata++]=MSB(hChip->pRegMapImage[firstRegIndex].Addr);	/* 16 bits sub addresses */
						case STCHIP_MODE_SUBADR_8:
						case STCHIP_MODE_NOSUBADR_RD:
							//data[nbdata++]=LSB(hChip->pRegMapImage[firstRegIndex].Addr);	/* 8 bits sub addresses */
						break;
					}	
					#ifndef NO_I2C
					          #if 0
						#ifdef HOST_PC
						if(WaitForSingleObject(ChipMutex,2000) == WAIT_OBJECT_0)	/* Wait for other I2C access termination */ 
						{
						#endif
                                            #endif
						if(hChip->Repeater)
							hChip->RepeaterFn(hChip->RepeaterHost,TRUE);	/* Set repeater ON */
							
						if(I2cReadWrite(I2C_WRITE,hChip->I2cAddr,FirstReg,data,nbdata) != I2C_OK)	/* write data buffer */
							hChip->Error = CHIPERR_I2C_NO_ACK;
								
						if(hChip->Repeater)
							hChip->RepeaterFn(hChip->RepeaterHost,FALSE);	/* Set repeater OFF */
					         #if 0
						#ifdef HOST_PC
						ReleaseMutex(ChipMutex);								/*	Allow other I2C access */  
						}
						#endif
						#endif

					#endif
				}
				else
					hChip->Error = CHIPERR_INVALID_REG_ID;
			}
			else
				hChip->Error = CHIPERR_I2C_BURST;
		}
		else
			return hChip->Error;
		
	}
	else
		return CHIPERR_INVALID_HANDLE;
	
	return hChip->Error;
}
#endif
void ChipSetRegisters_E(U8 hChipAddress, STCHIP_Mode_t mode,BOOL Repeater,U16 FirstReg, S32 NbRegs,U8* databuf)
{
	U8 data[100],nbdata = 0;
	STCHIP_Error_t ret=0;
	S32 i,
	firstRegIndex;


		switch(mode)
		{
			case STCHIP_MODE_SUBADR_16:
			data[nbdata++]=MSB(FirstReg);	/* 16 bits sub addresses */
			case STCHIP_MODE_SUBADR_8:
			case STCHIP_MODE_NOSUBADR_RD:
			data[nbdata++]=LSB(FirstReg);	/* 8 bits sub addresses */
			case STCHIP_MODE_NOSUBADR:
			for(i=0;i<NbRegs;i++)
			data[nbdata++]=databuf[i];	/* fill data buffer */
			break;
		}	
					        
		if(Repeater)
		FE_STV0903_RepeaterFn_E(TRUE);	/* Set repeater ON */
							
		I2cReadWrite(I2C_WRITE,hChipAddress,FirstReg,data,nbdata);
								
		if(Repeater)
		FE_STV0903_RepeaterFn_E(FALSE);	/* Set repeater OFF */
					         
	
}
/*****************************************************
**FUNCTION	::	ChipGetRegisters
**ACTION	::	Get values of consecutive's registers (values are writen in RegMap)
**PARAMS IN	::	hChip		==> Handle to the chip
**				FirstReg	==> Id of the first register
**				NbRegs		==> Number of register to read		
**PARAMS OUT::	NONE
**RETURN	::	Error
*****************************************************/
#if 0
STCHIP_Error_t ChipGetRegisters(STCHIP_Handle_t hChip, U16 FirstReg, S32 NbRegs)
{
	U8 data[100] = {0},nbdata =0;
	S32 i,
		firstRegIndex;

	if(hChip)
	{
		if(!hChip->Error)
		{
			if(NbRegs < 70)
			{
				firstRegIndex =ChipGetRegisterIndex(hChip, FirstReg);
				if((firstRegIndex >= 0) && ((firstRegIndex + NbRegs - 1) < hChip->NbRegs))
				{
					#ifndef NO_I2C

						#if 0
						#ifdef HOST_PC
						if(WaitForSingleObject(ChipMutex,2000) == WAIT_OBJECT_0)	/* Wait for other I2C access termination */ 
						{
						#endif
				              #endif
						switch(hChip->ChipMode)
						{
							case STCHIP_MODE_SUBADR_16:	
								data[nbdata++]=MSB(hChip->pRegMapImage[firstRegIndex].Addr);	/* for 16 bits sub addresses */
							case STCHIP_MODE_SUBADR_8:
								data[nbdata++]=LSB(hChip->pRegMapImage[firstRegIndex].Addr);  /* for 8 bits sub addresses	*/
								
									if(hChip->Repeater)
										hChip->RepeaterFn(hChip->RepeaterHost,TRUE);	/* Set repeater ON */
							
									if(I2cReadWrite(I2C_WRITE,hChip->I2cAddr,FirstReg,data,nbdata) != I2C_OK)	/* Write sub address */
										hChip->Error = CHIPERR_I2C_NO_ACK;	
									
									if(hChip->Repeater)
										hChip->RepeaterFn(hChip->RepeaterHost,FALSE);	/* Set repeater OFF */
							
							case STCHIP_MODE_NOSUBADR:
							case STCHIP_MODE_NOSUBADR_RD:
									if(hChip->Repeater)
										hChip->RepeaterFn(hChip->RepeaterHost,TRUE);	/* Set repeater ON */
							
									if(I2cReadWrite(I2C_READ,hChip->I2cAddr,FirstReg,data,NbRegs) != I2C_OK)	/* Read data buffer */ 
										hChip->Error = CHIPERR_I2C_NO_ACK;	
								
									if(hChip->Repeater)
										hChip->RepeaterFn(hChip->RepeaterHost,FALSE);	/* Set repeater OFF */
							break;
						}
					
						/*	Update RegMap structure	*/
						for(i=0;i<NbRegs;i++)
							if(!hChip->Error)
								hChip->pRegMapImage[firstRegIndex+i].Value = data[i];
							else
								hChip->pRegMapImage[firstRegIndex+i].Value = 0xFF; 
					       #if 0
						#ifdef HOST_PC
						ReleaseMutex(ChipMutex);								/*	Allow other I2C access */          	
						}
						#endif
						#endif
				
					#endif
				}
				else
					hChip->Error = CHIPERR_INVALID_REG_ID;
				  
			}
			else 
				hChip->Error = CHIPERR_I2C_BURST;
		}
	}
	else
		return CHIPERR_INVALID_HANDLE;
	
	return hChip->Error;
}
#endif
#if 0
I2cReadWrite(int mode,unsigned char chipaddr,unsigned int  regaddr,unsigned char *data,unsigned int  numb)
{
     unsigned char tmp;
    //  printf("regaddr:0x%x\n",regaddr);
     if(chipaddr == 0xd0)
     {
	 if(mode==I2C_WRITE)
         {
             for(tmp=0;tmp<numb;tmp++)
             {
              tuner_device_write_register(regaddr+tmp,data[tmp],1);
  //            printf("regaddr:0x%x\n",regaddr+tmp);
             }
         }
         else
         {
             unsigned char v;
              printf("i2c read reg:0x%x\n",regaddr);
	      for(tmp=0;tmp<numb;tmp++)
             {
                 tuner_device_read_register(regaddr+tmp,&v,1);
                 data[tmp]=v;	         
             }
         }
      }
     else
     {
	 unsigned char dat;
         unsigned char v;
         if(mode==I2C_WRITE)
         {
	     for(tmp=0;tmp<numb;tmp++)
             {
                 v=0xa0;
                 tuner_device_write_register(0xf12a,&v,1);
                 tuner_device_6110_write_register(regaddr%256+tmp,data[tmp],1);
                 v=0x0;
                 tuner_device_write_register(0xf12a,&v,1);

             }
         }
         else
         {   
           
             for(tmp=0;tmp<numb;tmp++)
             {
                 v=0xa0;
                 tuner_device_write_register(0xf12a,&v,1);
                 tuner_device_6110_read_register(regaddr%256+tmp,&dat,1);
                 data[tmp]=dat;
                 v=0x0;
                 tuner_device_write_register(0xf12a,&v,1);
                 printf("read 6110:%d\n",dat);
             }
         }
     }
}
#endif
void I2cReadWrite(unsigned int  mode,unsigned char chipaddr,unsigned int  regaddr,unsigned char *data,unsigned int  numb)
{
 	 uchar tmp;
//	 I2C_RESULT status=I2C_OK;

	 #if 0
    if( (chipaddr != 0xd0)&&(chipaddr != 0xc0) || (data == NULL) || numb <= 0) 
				 return I2C_BUSERROR; 	
	#endif

	if(chipaddr == 0xd0)
	{
		 if( mode == I2C_WRITE)
		 	for(tmp=0;tmp<numb;tmp++)
		 	 	Write_Stv0903(chipaddr,regaddr+tmp,data[tmp]);
		 else
		 	for(tmp=0;tmp<numb;tmp++)
		    		data[tmp]= Read_Stv0903(chipaddr,regaddr+tmp);
	}
	else
	{
	       if( mode == I2C_WRITE)
	  	{
	  		for(tmp=0;tmp<numb;tmp++)
	  			{
	  	  			Write_Stv0903(0xd0,0xf12a,0xa0);
	  	  			Write_6110(chipaddr,regaddr%256+tmp,data[tmp]);
		  			Write_Stv0903(0xd0,0xf12a,0x00);
	  			}
	  	}
	       else
	  	{
	  	       for(tmp=0;tmp<numb;tmp++)
	  	  	{
	  	  		Write_Stv0903(0xd0,0xf12a,0xa0);
	  	  		data[tmp]=Read_6110(chipaddr,regaddr%256+tmp);
		  		Write_Stv0903(0xd0,0xf12a,0x00);
	  	  	}
	  	}
	  }
	
}


STCHIP_Error_t ChipGetRegisters_E(U8 hChipAddress, STCHIP_Mode_t mode,BOOL Repeater, U16 FirstReg, S32 NbRegs,U8* rdbuf)
{
	U8 data[100] = {0},nbdata =0;
	S32 i;
	STCHIP_Error_t ret=CHIPERR_NO_ERROR;
          //display_hex(0x11,0); 
	  //display_hex((uchar)NbRegs,2); 
		
		
		switch(mode)
		{
			case STCHIP_MODE_SUBADR_16:	
				//data[nbdata++]=MSB(FirstReg);	/* for 16 bits sub addresses */
				 I2cReadWrite(I2C_READ,hChipAddress,FirstReg,data,NbRegs);
				
				 break;
			case STCHIP_MODE_SUBADR_8:
				I2cReadWrite(I2C_READ,hChipAddress,FirstReg,data,NbRegs);
				break;

                      #if 0
			case STCHIP_MODE_NOSUBADR:
			case STCHIP_MODE_NOSUBADR_RD:
				if(Repeater)
				FE_STV0903_RepeaterFn_E(TRUE);	/* Set repeater ON */
					
				if(I2cReadWrite(I2C_READ,hChipAddress,FirstReg,data,NbRegs) != I2C_OK)	/* Read data buffer */ 
				ret = CHIPERR_I2C_NO_ACK;	
								
				if(Repeater)
				FE_STV0903_RepeaterFn_E(TRUE);	/* Set repeater OFF */
				break;
			#endif
		}
		//display_hex((uchar)NbRegs,0); 
		//display_hex(data[0],2); 

		if(NbRegs==1)
		{
			rdbuf[0]=data[0];
			//display_hex(rdbuf[0],0); 		
		}
		else
		{
			/*	Update RegMap structure	*/
			for(i=0;i<NbRegs;i++)
			{
				rdbuf[i] = data[i];
			}
		}
		//display_hex(rdbuf[0],4); 		
	return ret;
}

/*****************************************************
**FUNCTION	::	ChipApplyDefaultValues
**ACTION	::	Write default values in all the registers
**PARAMS IN	::	hChip	==> Handle of the chip	
**PARAMS OUT::	NONE
**RETURN	::	Error
*****************************************************/
#if 0
STCHIP_Error_t ChipApplyDefaultValues(STCHIP_Handle_t hChip,U16 RegAddr, U8 RegsVal)
{
	if(hChip != NULL) 
	{
		if(hChip->ChipMode != STCHIP_MODE_NOSUBADR)   
		{
			if((!hChip->Error))
			{
				ChipSetOneRegister(hChip,RegAddr,RegsVal);     
			}
		}
		 
	}
	else
		return CHIPERR_INVALID_HANDLE;
		
	return hChip->Error;
}
#endif

/*****************************************************
**FUNCTION	::	ChipGetError
**ACTION	::	Return the current error of the chip
**PARAMS IN	::	hChip	==> Handle of the chip
**PARAMS OUT::	NONE
**RETURN	::	Current error, CHIPERR_INVALID_HANDLE otherwise
*****************************************************/
STCHIP_Error_t ChipGetError(STCHIP_Handle_t hChip)
{
	if(hChip != NULL)     
		return hChip->Error;
	else
		return CHIPERR_INVALID_HANDLE;
}

/*****************************************************
**FUNCTION	::	ChipResetError
**ACTION	::	Reset the error of the chip
**PARAMS IN	::	hChip	==> Handle of the chip
**PARAMS OUT::	NONE
**RETURN	::	Error
*****************************************************/
STCHIP_Error_t ChipResetError(STCHIP_Handle_t hChip)    
{
	if(hChip != NULL) 
		hChip->Error = CHIPERR_NO_ERROR;
	else
		return CHIPERR_INVALID_HANDLE;
		
	return hChip->Error;
}

/*****************************************************
**FUNCTION	::	ChipGetFieldMask
**ACTION	::	get the mask of a field in the chip
**PARAMS IN	::	
**PARAMS OUT::	mask
**RETURN	::	mask
*****************************************************/
S32 ChipGetFieldMask(U32 FieldId)
{
	S32 mask;
	mask = FieldId & 0xFF; /*FieldId is [reg address][reg address][sign][mask] --- 4 bytes */ 
	
	return mask;
}

/*****************************************************
**FUNCTION	::	ChipGetFieldSign
**ACTION	::	get the sign of a field in the chip
**PARAMS IN	::	
**PARAMS OUT::	sign
**RETURN	::	sign
*****************************************************/
S32 ChipGetFieldSign(U32 FieldId)
{
	S32 sign;
	sign = (FieldId>>8) & 0x01; /*FieldId is [reg address][reg address][sign][mask] --- 4 bytes */ 
	
	return sign;
}

/*****************************************************
**FUNCTION	::	ChipGetFieldPosition
**ACTION	::	get the position of a field in the chip
**PARAMS IN	::	
**PARAMS OUT::	position
**RETURN	::	position
*****************************************************/
S32 ChipGetFieldPosition(U8 Mask)
{
	S32 position=0, i=0;

	while((position == 0)&&(i < 8))
	{
		position = (Mask >> i) & 0x01;
		i++;
	}
  
	return (i-1);
}

/*****************************************************
**FUNCTION	::	ChipGetFieldBits
**ACTION	::	get the number of bits of a field in the chip
**PARAMS IN	::	
**PARAMS OUT::	number of bits
**RETURN	::	number of bits
*****************************************************/
S32 ChipGetFieldBits(S32 mask, S32 Position)
{
 S32 bits,bit;
 S32 i =0;
 
 bits = mask >> Position;
 bit = bits ;
 while ((bit > 0)&&(i<8))
 {
 	i++;
 	bit = bits >> i;
 	
 }
 return i;
}

/*****************************************************
**FUNCTION	::	ChipSetFieldImage
**ACTION	::	Set value of a field in RegMap
**PARAMS IN	::	hChip	==> Handle to the chip
**				FieldId	==> Id of the field
**				Value	==> Value of the field
**PARAMS OUT::	NONE
**RETURN	::	Error
*****************************************************/
STCHIP_Error_t ChipSetFieldImage(STCHIP_Handle_t hChip,U32 FieldId, S32 Value)
{
	S32 regIndex,
		mask,
		sign,
		bits,
		pos;
	
	if(hChip != NULL)
	{
		if(!hChip->Error)
		{
			regIndex=ChipGetFieldIndex(hChip,FieldId);						/*	Just for code simplification	*/
			
			if((regIndex >= 0) && (regIndex  < hChip->NbRegs))
			{

				sign = ChipGetFieldSign(FieldId);
				mask = ChipGetFieldMask(FieldId);
				pos = ChipGetFieldPosition(mask);
				bits = ChipGetFieldBits(mask,pos);
			
				if(sign == CHIP_SIGNED)
					Value = (Value > 0 ) ? Value : Value + (1<<bits);	/*	compute signed fieldval	*/

				Value = mask & (Value << pos);	/*	Shift and mask value	*/
				hChip->pRegMapImage[regIndex].Value = (hChip->pRegMapImage[regIndex].Value & (~mask)) + Value;	/*	Concat register value and fieldval	*/
			}
			else
				hChip->Error = CHIPERR_INVALID_FIELD_ID;
		}
	}
	else
		return CHIPERR_INVALID_HANDLE;
	
	return hChip->Error;
}


/*****************************************************
**FUNCTION	::	ChipSetField
**ACTION	::	Set value of a field in the chip
**PARAMS IN	::	hChip	==> Handle to the chip
**				FieldId	==> Id of the field
**				Value	==> Value to write
**PARAMS OUT::	NONE
**RETURN	::	Error
*****************************************************/
#if 0
STCHIP_Error_t ChipSetField(STCHIP_Handle_t hChip,U32 FieldId,S32 Value)
{
	
	S32 regValue,
		mask,
		sign,
		bits,
		pos;
	
	if(hChip)
	{
		if(!hChip->Error)
		{

			regValue=ChipGetOneRegister(hChip,(FieldId >> 16)&0xFFFF);		/*	Read the register	*/
			sign = ChipGetFieldSign(FieldId);
			mask = ChipGetFieldMask(FieldId);
			pos = ChipGetFieldPosition(mask);
			bits = ChipGetFieldBits(mask,pos);
			
			if(sign == CHIP_SIGNED)
				Value = (Value > 0 ) ? Value : Value + (bits);	/*	compute signed fieldval	*/

			Value = mask & (Value << pos);						/*	Shift and mask value	*/

			regValue=(regValue & (~mask)) + Value;		/*	Concat register value and fieldval	*/
			ChipSetOneRegister(hChip,(FieldId >> 16)&0xFFFF,regValue);		/*	Write the register */

		}
		else
			hChip->Error = CHIPERR_INVALID_FIELD_ID;
	}
	else
		return CHIPERR_INVALID_HANDLE;
		
	return hChip->Error;
}
#endif
STCHIP_Error_t ChipSetField_E(U8 chipaddress,STCHIP_Mode_t mode,BOOL Repeater, U32 FieldId,S32 Value)
{
	
	S32 regValue,
		mask,
		sign,
		bits,
		pos;
		U8 dat=0;
		//fieldID    
			//0xf4e0   00      70
			//addr      sign    mask 
										//芯片地址地址类型	//寄存器地址
			regValue=ChipGetOneRegister_E(chipaddress,mode,Repeater,(FieldId >> 16)&0xFFFF,0,0);		/*	Read the register	*/
			sign = ChipGetFieldSign(FieldId);
			mask = ChipGetFieldMask(FieldId);
			pos = ChipGetFieldPosition(mask);//mask 中为1的位置，比如  0x80  pos = 7
			bits = ChipGetFieldBits(mask,pos);
			//从pos 开始到bits 之间为1 比如  1101 0000 pos = 4 bits =4
			if(sign == CHIP_SIGNED)
				Value = (Value > 0 ) ? Value : Value + (bits);	/*	compute signed fieldval	*/

			Value = mask & (Value << pos);						/*	Shift and mask value	*/

			regValue=(regValue & (~mask)) + Value;		/*	Concat register value and fieldval	*/
			 dat=regValue;
//                     display_hex(dat, 2);
			ChipSetOneRegister_E(chipaddress,mode,Repeater,(FieldId >> 16)&0xFFFF,regValue);		/*Write the register */
	
   			return CHIPERR_NO_ERROR;
}

U32	 ChipGetField_E(U8 chipaddress,STCHIP_Mode_t mode,BOOL Repeater, U32 FieldId)
{
	U32 value = 0xFF;
	U32 sign, mask, pos, bits;
	U8 dat=0;

			/* I2C Read : register address set-up */
			sign = ChipGetFieldSign(FieldId);
			mask = ChipGetFieldMask(FieldId);
			pos = ChipGetFieldPosition(mask);
			bits = ChipGetFieldBits(mask,pos);					
			
			value=ChipGetOneRegister_E(chipaddress,mode,Repeater,FieldId >> 16,0,0);		/*	Read the register	*/     
			//dat=value;
		//	display_hex(dat, 0);
		
			value=(value & mask)  >> pos;	/*	Extract field	*/
			//dat=value;
                     //display_hex(dat, 2);
			if((sign == CHIP_SIGNED)&&(value & (1<<(bits-1))))
				value = value - (1<<bits);			/*	Compute signed value	*/
                     //dat=value;
                     //display_hex(dat, 4);
                     
        	return value;//锁的状态
}



/*****************************************************
**FUNCTION	::	ChipCheckAck
**ACTION	::	Test if the chip acknowledge at its supposed address
**PARAMS IN	::	ChipId	==> Id of the chip
**PARAMS OUT::	NONE
**RETURN	::	ACK if acknowledge is OK, NOACK otherwise
*****************************************************/
#if 0
S32 ChipCheckAck(STCHIP_Handle_t hChip)
{
	I2C_RESULT status = I2C_NOACK;
	U8 data = 0;
	
	if(hChip)  
	{
		hChip->Error = CHIPERR_NO_ERROR;

		#ifndef NO_I2C
		        #if 0
			#ifdef HOST_PC		
				if(WaitForSingleObject(ChipMutex,2000) == WAIT_OBJECT_0)	/* Wait for other I2C access termination */ 
				{
			#endif
			#endif
					if(hChip->Repeater)
						hChip->RepeaterFn(hChip->RepeaterHost,TRUE);	/* Set repeater ON */
			
					status = I2cReadWrite(I2C_READ,hChip->I2cAddr,FirstReg,&data,1);
		
					if(hChip->Repeater)
						hChip->RepeaterFn(hChip->RepeaterHost,FALSE);	/* Set repeater OFF */
				#if 0
				#ifdef HOST_PC
				}
				ReleaseMutex(ChipMutex);								/*	Allow other I2C access */
			#endif
			#endif
		#else
			status = I2C_OK;
		#endif
	
		if(status != I2C_OK)
			hChip->Error = CHIPERR_I2C_NO_ACK;
	}
	
	return status;  
}

#endif







