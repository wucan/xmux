#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "wu/message.h"

#include "up_support.h"
#include "wu_snmp_agent.h"

#include "psi_worker.h"
#include "psi_parse.h"
#include "psi_gen.h"
#include "gen_dvb_si.h"
#include "hfpga.h"
#include "pid_map_table.h"
#include "xmux_snmp_intstr.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "xmux_snmp"};

#define OID_CHAN_IDX				6
#define OID_CHAN_PMT_PROG_IDX		8
#define OID_SDT_SEC_IDX				8
#define OID_EIT_SEC_IDX				8


#define TRAN_PID_OFFSET				0
#define CHANNEL_INFO_OFFSET			1024

#define PID_TRANS_SIZE				1048
#define INPUT_TRANS_INFO_NUM		24
#define INPUT_TRANS_INFO_SIZE		1082
#define LOAD_INFO_SIZE				34
#define HEART_DEVICE_SIZE			4
#define USER_INFO_SIZE				6

static uint8_t sg_mib_trans[PID_TRANS_SIZE];
static uint8_t sg_mib_loadinfo[LOAD_INFO_SIZE];
static uint8_t sg_mib_heartDevice[HEART_DEVICE_SIZE];
static uint8_t sg_mib_Inptu_Trans_info[INPUT_TRANS_INFO_NUM][INPUT_TRANS_INFO_SIZE];
static struct ip_info_snmp_data sg_mib_IP_info;
static uint8_t sg_mib_User_info[USER_INFO_SIZE];


/*
 * PAT
 */
static int pat_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t chan_idx = obj->oid[OID_CHAN_IDX] - 1;
	uint16_t sec_len;

	sec_len = sg_mib_xxx_len(sg_mib_pat[chan_idx]);
	v->size = sec_len + 2;
	v->data = sg_mib_pat[chan_idx];

	return 0;
}
static int pat_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t chan_idx = obj->oid[OID_CHAN_IDX] - 1;

	memcpy(sg_mib_pat[chan_idx], v->data, v->size);

	return 0;
}
/*
 * PMT
 */
static int pmt_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t chan_idx = obj->oid[OID_CHAN_IDX] - 1;
	uint8_t prog_idx = obj->oid[OID_CHAN_PMT_PROG_IDX] - 1;
	uint16_t sec_len;

	sec_len = sg_mib_xxx_len(sg_mib_pmt[chan_idx][prog_idx]);
	v->size = sec_len + 2;
	v->data = sg_mib_pmt[chan_idx][prog_idx];

	return 0;
}
static int pmt_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t chan_idx = obj->oid[OID_CHAN_IDX] - 1;
	uint8_t prog_idx = obj->oid[OID_CHAN_PMT_PROG_IDX] - 1;

	memcpy(sg_mib_pmt[chan_idx][prog_idx], v->data, v->size);

	return 0;
}
/*
 * CAT
 */
static int cat_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t chan_idx = obj->oid[OID_CHAN_IDX] - 1;
	uint16_t sec_len;

	sec_len = sg_mib_xxx_len(sg_mib_cat[chan_idx]);
	v->size = sec_len + 2;
	v->data = sg_mib_cat[chan_idx];

	return 0;
}
static int cat_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t chan_idx = obj->oid[OID_CHAN_IDX] - 1;

	memcpy(sg_mib_cat[chan_idx], v->data, v->size);

	return 0;
}
/*
 * NIT
 */
static int nit_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t chan_idx = obj->oid[OID_CHAN_IDX] - 1;
	uint16_t sec_len;

	sec_len = sg_mib_xxx_len(sg_mib_nit[chan_idx]);
	v->size = sec_len + 2;
	v->data = sg_mib_nit[chan_idx];

	return 0;
}
static int nit_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t chan_idx = obj->oid[OID_CHAN_IDX] - 1;

	memcpy(sg_mib_nit[chan_idx], v->data, v->size);

	return 0;
}
/*
 * SDT
 */
static int sdt_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t chan_idx = obj->oid[OID_CHAN_IDX] - 1;
	uint8_t sec_idx = obj->oid[OID_SDT_SEC_IDX] - 1;
	uint16_t sec_len;

	sec_len = sg_mib_xxx_len(sg_mib_sdt[chan_idx][sec_idx]);
	v->size = sec_len + 2;
	v->data = sg_mib_sdt[chan_idx][sec_idx];

	return 0;
}
static int sdt_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t chan_idx = obj->oid[OID_CHAN_IDX] - 1;
	uint8_t sec_idx = obj->oid[OID_SDT_SEC_IDX] - 1;

	memcpy(sg_mib_sdt[chan_idx][sec_idx], v->data, v->size);

	return 0;
}
/*
 * EIT
 */
static int eit_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t chan_idx = obj->oid[OID_CHAN_IDX] - 1;
	uint8_t sec_idx = obj->oid[OID_EIT_SEC_IDX] - 1;
	uint16_t sec_len;

	sec_len = sg_mib_xxx_len(sg_mib_eit[chan_idx][sec_idx]);
	v->size = sec_len + 2;
	v->data = sg_mib_eit[chan_idx][sec_idx];

	return 0;
}
static int eit_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t chan_idx = obj->oid[OID_CHAN_IDX] - 1;
	uint8_t sec_idx = obj->oid[OID_EIT_SEC_IDX] - 1;

	memcpy(sg_mib_eit[chan_idx][sec_idx], v->data, v->size);

	return 0;
}
/*
 * INPUT_TRANS
 */
static int input_trans_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t trans_idx = obj->oid[obj->oid_len - 1];

	v->size = INPUT_TRANS_INFO_SIZE;
	v->data = sg_mib_Inptu_Trans_info[trans_idx];

	return 0;
}
static int input_trans_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t trans_idx = obj->oid[obj->oid_len - 1];

	memcpy(sg_mib_Inptu_Trans_info[trans_idx], v->data, v->size);
	eeprom_write(sg_mib_Inptu_Trans_info[trans_idx], INPUT_TRANS_INFO_SIZE,
		CHANNEL_INFO_OFFSET + INPUT_TRANS_INFO_SIZE * trans_idx);

	return 0;
}
/*
 * PID_TRANS
 */
static int pid_trans_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	v->size = PID_TRANS_SIZE;
	v->data = sg_mib_trans;

	return 0;
}
static int pid_trans_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	memcpy(sg_mib_trans, v->data, v->size);
	eeprom_write(sg_mib_trans, PID_TRANS_SIZE, TRAN_PID_OFFSET);
	pid_map_table_apply(sg_mib_trans + 7, 1024);

	return 0;
}
/*
 * NET
 */
static int net_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	v->size = IP_INFO_SIZE;
	v->data = &sg_mib_IP_info;

	return 0;
}
static int net_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	memcpy(&sg_mib_IP_info, v->data, v->size);
	up_set_net_param(0, sg_mib_IP_info.ip, sg_mib_IP_info.netmask);
	up_set_gateway(sg_mib_IP_info.gateway);
	up_set_mac(0, sg_mib_IP_info.mac);

	return 0;
}
/*
 * USER
 */
static int user_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	v->size = USER_INFO_SIZE;
	v->data = sg_mib_User_info;

	return 0;
}
static int user_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	memcpy(sg_mib_User_info, v->data, v->size);

	return 0;
}
/*
 * START PARSE PSI
 */
static uint8_t parse_psi_status;
static int start_parse_psi_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	v->size = 1;
	v->data = &parse_psi_status;

	return 0;
}
static int start_parse_psi_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint16_t oper;

	if (v->size != 2) {
		trace_err("start parse psi want 2 bytes data, but got %d!", v->size);
		return -1;
	}
	memcpy(&oper, v->data, v->size);
	switch (oper) {
		case 0x3131: // for my test case
		case 0x55AA:
			parse_psi_status++;
			psi_worker_request_parse_psi(0, NULL, MANAGEMENT_MODE_SNMP);
			break;
		case 0x3030: // for my test case
		case 0x0000:
			psi_worker_stop_parse_psi();
			break;
		default:
			trace_err("unknow start parse psi oper %#x!", oper);
			return -1;
			break;
	}

	return 0;
}
/*
 * PSI STATUS
 */
extern uv_cha_si_stat All_Channel_Psi_Status;
static int psi_status_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	v->size = sizeof(All_Channel_Psi_Status);
	v->data = &All_Channel_Psi_Status;

	return 0;
}
/*
 * LOAD INFO
 */
static int load_info_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	v->size = LOAD_INFO_SIZE;
	v->data = sg_mib_loadinfo;

	return 0;
}
static int load_info_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	memcpy(sg_mib_loadinfo, v->data, v->size);

	psi_gen_output_psi_from_sections();

	return 0;
}
/*
 * HEART DEVICE
 */
static int heart_device_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	v->size = HEART_DEVICE_SIZE;
	v->data = sg_mib_heartDevice;

	return 0;
}
static int heart_device_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	memcpy(sg_mib_heartDevice, v->data, v->size);

	return 0;
}

static struct wu_oid_object chan_oid_objs[] = {
	// PAT
	{"PAT", {XMUX_ROOT_OID, 1, 1}, 8,
	 0, OID_STATUS_RWRITE,
	 pat_get, pat_set, SECTION_MAX_SIZE,
	},
	// PMT
	{"PMT", {XMUX_ROOT_OID, 1, 2, 1}, 9,
	 0, OID_STATUS_RWRITE,
	 pmt_get, pmt_set, SECTION_MAX_SIZE,
	},
	// CAT
	{"CAT", {XMUX_ROOT_OID, 1, 3}, 8,
	 0, OID_STATUS_RWRITE,
	 cat_get, cat_set, SECTION_MAX_SIZE,
	},
	// NIT
	{"NIT", {XMUX_ROOT_OID, 1, 4}, 8,
	 0, OID_STATUS_RWRITE,
	 nit_get, nit_set, SECTION_MAX_SIZE,
	},
	// SDT
	{"SDT", {XMUX_ROOT_OID, 1, 5, 1}, 9,
	 0, OID_STATUS_RWRITE,
	 sdt_get, sdt_set, SECTION_MAX_SIZE,
	},
	// EIT
	{"EIT", {XMUX_ROOT_OID, 1, 6, 1}, 9,
	 0, OID_STATUS_RWRITE,
	 sdt_get, sdt_set, SECTION_MAX_SIZE,
	},
};
static struct wu_oid_object input_trans_obj = {
	"INPUT_TRANS", {XMUX_ROOT_OID, 12, 1}, 8,
	0, OID_STATUS_RWRITE,
	input_trans_get, input_trans_set, INPUT_TRANS_INFO_SIZE,
};
static struct wu_oid_object solo_oid_objs[] = {
	// PID_TRANS
	{"PID_TRANS", {XMUX_ROOT_OID, 11}, 7,
	 0, OID_STATUS_RWRITE,
	 pid_trans_get, pid_trans_set, PID_TRANS_SIZE,
	},
	// NET
	{"NET", {XMUX_ROOT_OID, 13}, 7,
	 0, OID_STATUS_RWRITE,
	 net_get, net_set, IP_INFO_SIZE,
	},
	// USER
	{"USER", {XMUX_ROOT_OID, 14}, 7,
	 0, OID_STATUS_RWRITE,
	 user_get, user_set, USER_INFO_SIZE,
	},
	// START PARSE PSI
	{"START_PARSE_PSI", {XMUX_ROOT_OID, 15}, 7,
	 0, OID_STATUS_RWRITE,
	 start_parse_psi_get, start_parse_psi_set, 2,
	},
	// PSI STATUS
	{"PSI_STATUS", {XMUX_ROOT_OID, 16}, 7,
	 0, OID_STATUS_READ,
	 psi_status_get, NULL,
	},
	// LOAD INFO
	{"LOAD_INFO", {XMUX_ROOT_OID, 100}, 7,
	 0, OID_STATUS_RWRITE,
	 load_info_get, load_info_set, LOAD_INFO_SIZE,
	},
	// HEART DEVICE
	{"HEART_DEVICE", {XMUX_ROOT_OID, 101}, 7,
	 0, OID_STATUS_RWRITE,
	 heart_device_get, heart_device_set, HEART_DEVICE_SIZE,
	},
};

#define ARRAY_ESIZE(a)		(sizeof(a) / sizeof(a[0]))

static void register_channel_pmt_oids(struct wu_oid_object *obj)
{
	int i;

	for (i = 0; i < PROGRAM_MAX_NUM; i++) {
		obj->oid[8] = i + 1;
		wu_snmp_agent_register(obj);
	}
}
static void register_channel_sdt_oids(struct wu_oid_object *obj)
{
	int i;

	for (i = 0; i < SDT_SECTION_NUM; i++) {
		obj->oid[8] = i + 1;
		wu_snmp_agent_register(obj);
	}
}
static void register_channel_eit_oids(struct wu_oid_object *obj)
{
	int i;

	for (i = 0; i < EIT_SECTION_NUM; i++) {
		obj->oid[8] = i + 1;
		wu_snmp_agent_register(obj);
	}
}
/*
 * @chan_idx: channel index, from 0
 */
static void register_channel_oids(uint8_t chan_idx)
{
	int i;

	for (i = 0; i < ARRAY_ESIZE(chan_oid_objs); i++) {
		chan_oid_objs[i].oid[6] = chan_idx + 1;
		if (chan_oid_objs[i].oid[7] == 2) {
			// PMT
			register_channel_pmt_oids(&chan_oid_objs[i]);
		} else if (chan_oid_objs[i].oid[7] == 5) {
			// SDT
			register_channel_sdt_oids(&chan_oid_objs[i]);
		} else if (chan_oid_objs[i].oid[7] == 6) {
			// EIT
			register_channel_eit_oids(&chan_oid_objs[i]);
		} else {
			wu_snmp_agent_register(&chan_oid_objs[i]);
		}
	}
}
static void register_input_trans_oids()
{
	int i;

	for (i = 0; i < INPUT_TRANS_INFO_NUM; i++) {
		input_trans_obj.oid[7] = i + 1;
		wu_snmp_agent_register(&input_trans_obj);
	}
}
static void register_solo_oids()
{
	int i;

	for (i = 0; i < ARRAY_ESIZE(solo_oid_objs); i++) {
		wu_snmp_agent_register(&solo_oid_objs[i]);
	}
}
int xmux_snmp_register_all_oids()
{
	int chan_idx;

	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		register_channel_oids(chan_idx);
	}
	/*
	 * register output stuff
	 */
	register_channel_oids(CHANNEL_MAX_NUM);

	register_input_trans_oids();
	register_solo_oids();

	return 0;
}

