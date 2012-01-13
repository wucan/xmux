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
	memcpy(&dvbstunerinfo, param, sizeof(tunerinfo));
	Set_Polarization(dvbstunerinfo.Polarization);
	Set_Lnb(dvbstunerinfo.LNB);
	Set_F22K(dvbstunerinfo.F22K);
	FE_TUNER_SET();
	GetTunerRegister_E();
	Set_Tuner_Dvbs(dvbstunerinfo.Frequency, dvbstunerinfo.LOFrequency, dvbstunerinfo.SymbolRate);
	tuner_port_test();

	return 0;
}

int tunner_device_set_param(int id, struct tunner_param *param)
{
	/* set param to tunner chip */
	tunner_device_do_set_param(id, param);

	/* save to storage */
	xmux_config_put_tunner_param(id, param);

	return 0;
}

int tunner_device_get_status(int id, struct tunner_status *status)
{
	/* get status from tunner chip  */
	GetSignalInfo();
	memcpy(status, &dvbsstatus, sizeof(tunerstatus));

	return 0;
}

void tunner_device_check()
{
	struct tunner_status status;

	tunner_device_get_status(0, &status);
	if (!status.lock) {
		tuner_device_reset();
		tunner_device_init();
		tunner_device_do_set_param(0, &g_eeprom_param.tunner[0]);
	}
}

