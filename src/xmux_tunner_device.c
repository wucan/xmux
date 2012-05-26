#include "xmux.h"
#include "xmux_config.h"
#include "xmux_tunner.h"
#include "tuner_device.h"
/*
 * struct ang variable ref libtuner
 */
typedef struct {
	unsigned long Frequency;
	unsigned long SymbolRate;
	unsigned long LOFrequency;
	unsigned char Polarization;
	unsigned char LNB;
	unsigned char F22K;
} tunerinfo;

typedef struct {
	unsigned char Lock;
	unsigned char Standard;
	char Level;
	unsigned long CN;
	unsigned char PunctureRate;
	unsigned long Ber;
} tunerstatus __attribute__ ((__packed__));

extern tunerinfo dvbstunerinfo;
extern tunerstatus dvbsstatus;
unsigned tunergetflag=0;
unsigned int GettunerI2cbusy(void)
{
	printf("getbusy:%d\n",tunergetflag);
	return tunergetflag;
}
void Set_TunerI2c_Busy(void)
{
	tunergetflag=1;
       printf("tunergetflag:%d\n",tunergetflag);
}
void Set_TunerI2c_Free(void)
{
	tunergetflag=0;
        printf("tunergetflag:%d\n",tunergetflag);
}
int tunner_device_init()
{
	tuner_device_open();
	tuner_init();
	tuner_port_test();
	T6110_TEST();

	return 0;
}

int tunner_device_get_param(int id, struct tunner_param *param)
{
	/* direct get from storage */
	xmux_config_get_tunner_param(id, param);

	return 0;
}

int tunner_device_do_set_param(int id, struct tunner_param *param)
{
       unsigned int flag;
//       flag=GettunerI2cbusy();
 //      while(flag)
  //     {
   //        flag=GettunerI2cbusy();
    //       usleep(1000);
     //      if(flag==0)
      //     break;     
       // }
      // Set_TunerI2c_Busy();

	memcpy(&dvbstunerinfo, param, sizeof(tunerinfo));
	Set_Polarization(dvbstunerinfo.Polarization);
	Set_Lnb(dvbstunerinfo.LNB);
	Set_F22K(dvbstunerinfo.F22K);
	FE_TUNER_SET();
	GetTunerRegister_E();
	Set_Tuner_Dvbs(dvbstunerinfo.Frequency, dvbstunerinfo.LOFrequency, dvbstunerinfo.SymbolRate);
	tuner_port_test();
       // Set_TunerI2c_Free();
	return 0;
}

int tunner_device_do_set_info(int id, struct tunner_param *param)
{
        memcpy(&dvbstunerinfo, param, sizeof(tunerinfo));
        FE_TUNER_SET();
        GetTunerRegister_E();
        Set_Tuner_Dvbs(dvbstunerinfo.Frequency, dvbstunerinfo.LOFrequency,
         dvbstunerinfo.SymbolRate);
        tuner_port_test();
        return 0;
}

int tunner_device_set_param(int id, struct tunner_param *param)
{
	/* set param to tunner chip */
        tunergetflag=1;
	tunner_device_do_set_param(id, param);

	/* save : storage */
	xmux_config_put_tunner_param(id, param);
         tunergetflag=0;
	return 0;
}

int tunner_device_get_status(int id, struct tunner_status *status)
{
        unsigned int flag=0;
	/* get status from tunner chip  */
      tunergetflag=1;

	GetSignalInfo();
      //  Set_TunerI2c_Free();
        tunergetflag=0;
	memcpy(status, &dvbsstatus, sizeof(tunerstatus));
	return 0;
}

void tunner_device_check()
{
	struct tunner_status status;
       unsigned int flag=0;
       unsigned int berval;
       unsigned char cnval[4];
       #if 0
       flag=GettunerI2cbusy();
       while(flag)
       {
	   flag=GettunerI2cbusy();
           usleep(1000);
           if(flag==0)
           break;
       }
       #endif
      // Set_TunerI2c_Busy();
        if(tunergetflag==1)
        return;
	tunner_device_get_status(0, &status);
//        printf("LockStatus:%d,Ber:%f\n",status.lock,status.ber);
	if(!status.lock) {
		tuner_device_reset();
		tunner_device_init();
		tunner_device_do_set_info(0, &g_eeprom_param.tunner[0]);
	}
       // berval=(unsigned int )status.ber;
        memcpy(cnval,&status.cn,4);
       // printf("LockStatus:%d,Ber:%d\n",status.lock,status.ber);
#if 0
         if(status.ber==0.000000) {
                printf("tuner set repeat!\n");
                tuner_device_reset();
                tunner_device_init();
                tunner_device_do_set_info(0, &g_eeprom_param.tunner[0]);
        }
        printf("cnval:%d,%d,%d,%d\n",cnval[0],cnval[1],cnval[2],cnval[3]);
         if((cnval[0]==0x30)&&(cnval[1]==0x30)) {
                printf("tuner set repeat!\n");
                tuner_device_reset();
                tunner_device_init();
                tunner_device_do_set_info(0, &g_eeprom_param.tunner[0]);
        }
#endif
      // Set_TunerI2c_Free();
     //  tunergetflag=0;
 }

