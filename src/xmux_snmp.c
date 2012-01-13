#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "wu/message.h"
#include "wu/wu_csc.h"

#include "xmux_net.h"
#include "xmux_config.h"
#include "wu_snmp_agent.h"

#include "xmux_config.h"
#include "xmux_misc.h"
#include "psi_worker.h"
#include "psi_parse.h"
#include "psi_gen.h"
#include "gen_dvb_si_api.h"
#include "hfpga.h"
#include "pid_map_table.h"
#include "pid_trans_info.h"
#include "xmux_snmp_intstr.h"
#include "xmux_tunner.h"
#include "xmux_bcm3033.h"
#include "xmux_ci.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "xmux_snmp"};

#define OID_CHAN_IDX				6
#define OID_CHAN_PMT_PROG_IDX		8
#define OID_SDT_SEC_IDX				8
#define OID_EIT_SEC_IDX				8
#define OID_TUNNER_ID_IDX			6


#define LOAD_INFO_SIZE				USER_INFO_SIZE

struct pid_trans_snmp_data sg_mib_trans;
static uint8_t sg_mib_loadinfo[LOAD_INFO_SIZE];
struct heart_device_snmp_data sg_mib_heartDevice = {SNMP_LOGIN_STATUS_IDLE};
static struct ip_info_snmp_data sg_mib_IP_info;
static uint8_t sg_mib_User_info[USER_INFO_SIZE];
struct apply_psi_snmp_data sg_mib_apply_psi;


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
 * PID_TRANS_INFO
 */
static int pid_trans_info_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t trans_idx = obj->oid[obj->oid_len - 1] - 1;

	pid_trans_info_read_data_snmp(trans_idx, v);

	return 0;
}
static int pid_trans_info_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint8_t trans_idx = obj->oid[obj->oid_len - 1] - 1;

	pid_trans_info_write_data_snmp(trans_idx, v);

	return 0;
}
/*
 * PID_TRANS
 */
static int pid_trans_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	sg_mib_trans.output_bitrate = g_eeprom_param.sys.output_bitrate;
	sg_mib_trans.format = g_eeprom_param.sys.format;

	v->size = PID_TRANS_SIZE;
	v->data = &sg_mib_trans;

	return 0;
}
static int pid_trans_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	memcpy(&sg_mib_trans, v->data, v->size);
	if (!pid_map_table_validate(&sg_mib_trans.table)) {
		trace_err("pid map table invalidate set!");
		return -1;
	}
	pid_map_table_dump(&sg_mib_trans.table);
	xmux_config_save_pid_map_table(&sg_mib_trans.table);
	pid_map_table_apply(&sg_mib_trans.table);
	set_output_bitrate(sg_mib_trans.output_bitrate);
	set_packet_format(sg_mib_trans.format);

	return 0;
}
/*
 * NET
 */
static int net_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	sg_mib_IP_info.len = sizeof(struct ip_info_snmp_data) - 2;
	memcpy(&sg_mib_IP_info.server_ip, &g_eeprom_param.net, sg_mib_IP_info.len);
	v->size = IP_INFO_SIZE;
	v->data = &sg_mib_IP_info;

	return 0;
}
static int net_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	struct xmux_net_param net;

	memcpy(&sg_mib_IP_info, v->data, v->size);
	memcpy(&net, &sg_mib_IP_info.server_ip,
		sizeof(struct ip_info_snmp_data) - 2);
	xmux_net_set(&net);

	return 0;
}
/*
 * USER
 */
static int user_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	memcpy(sg_mib_User_info, &g_eeprom_param.user, USER_INFO_SIZE);
	v->size = USER_INFO_SIZE;
	v->data = sg_mib_User_info;

	return 0;
}
static int user_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	memcpy(sg_mib_User_info, v->data, v->size);
	memcpy(&g_eeprom_param.user, sg_mib_User_info, USER_INFO_SIZE);
	g_eeprom_param.user.csc = wu_csc(&g_eeprom_param.user, offsetof(struct xmux_user_param, csc));
	eeprom_write(EEPROM_OFF_USER, &g_eeprom_param.user, sizeof(struct xmux_user_param));

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
	oper = htons(oper);
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
		case 0x3232: // for my test case, trigger parse from fp
			psi_worker_request_parse_psi(0, NULL, MANAGEMENT_MODE_FP);
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
 * APPLY PSI
 */
extern int save_pid_trans_info;
static int apply_psi_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	trace_info("apply psi...");
	memcpy(&sg_mib_apply_psi, v->data, v->size);
	psi_gen_output_psi_from_sections();
	psi_apply_from_output_psi();

	save_pid_trans_info = 1;

#if CHANNEL_MAX_NUM == 1
	/*
	 * save raw input pmt section for descramble use
	 */
	xmux_config_save_input_pmt_section();
	xmux_ci_apply();
#endif

	return 0;
}
/*
 * output mux program info
 */
static int mux_program_info_get(struct wu_oid_object *obj,
		struct wu_snmp_value *v)
{
	v->size = sizeof(struct xmux_mux_program_info);
	v->data = &g_eeprom_param.mux_prog_info;
	xmux_config_save_mux_program_info(&g_eeprom_param.mux_prog_info);

	return 0;
}
static int mux_program_info_set(struct wu_oid_object *obj,
		struct wu_snmp_value *v)
{
	memcpy(&g_eeprom_param.mux_prog_info, v->data, v->size);

	return 0;
}
/*
 * tunner param
 */
static int tunner_param_get(struct wu_oid_object *obj,
		struct wu_snmp_value *v)
{
	static struct tunner_param p;
	int tunner_id = 0;

	switch (obj->oid[OID_TUNNER_ID_IDX]) {
		default:
		case 19: tunner_id = 0; break;
		case 21: tunner_id = 1; break;
	}
	tunner_get_param(tunner_id, &p);
	v->size = sizeof(struct tunner_param);
	v->data = &p;

	return 0;
}
static int tunner_param_set(struct wu_oid_object *obj,
		struct wu_snmp_value *v)
{
	int tunner_id = 0;

	switch (obj->oid[OID_TUNNER_ID_IDX]) {
		default:
		case 19: tunner_id = 0; break;
		case 21: tunner_id = 1; break;
	}

	tunner_set_param(tunner_id, v->data);

	return 0;
}
/*
 * tunner status
 */
static int tunner_status_get(struct wu_oid_object *obj,
		struct wu_snmp_value *v)
{
	static struct tunner_status s;
	int tunner_id = 0;

	switch (obj->oid[OID_TUNNER_ID_IDX]) {
		default:
		case 20: tunner_id = 0; break;
		case 22: tunner_id = 1; break;
	}
	tunner_get_status(tunner_id, &s);
	v->size = sizeof(struct tunner_status);
	v->data = &s;

	return 0;
}
/*
 * bcm3033 param
 */
static int bcm3033_param_get(struct wu_oid_object *obj,
		struct wu_snmp_value *v)
{
	static struct bcm3033_param p;

	bcm3033_get_param(&p);
	v->size = sizeof(struct bcm3033_param);
	v->data = &p;

	return 0;
}
static int bcm3033_param_set(struct wu_oid_object *obj,
		struct wu_snmp_value *v)
{
	bcm3033_set_param(v->data);

	return 0;
}
/*
 * ci info
 */
static int ci_info_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	get_ci_name(g_eeprom_param.misc.ci_info.ci_name);
	v->size = sizeof(struct ci_info_param);
	v->data = &g_eeprom_param.misc.ci_info;

	return 0;
}
static int ci_info_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	struct ci_info_param ci_info;

	memcpy(&ci_info, v->data, sizeof(ci_info));
	xmux_config_save_ci_info(&ci_info);
	xmux_ci_info_update(&ci_info);

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
	struct user_param_snmp_data *user = &sg_mib_loadinfo;

	memcpy(sg_mib_loadinfo, v->data, v->size);
	if (management_mode == MANAGEMENT_MODE_FP) {
		trace_err("busy in fp mode!");
		sg_mib_heartDevice.flag = SNMP_LOGIN_STATUS_BUSY;
		return -1;
	}
	if (user->user_len != g_eeprom_param.user.user_len ||
		memcmp(user->user, g_eeprom_param.user.user, user->user_len)) {
		trace_err("user error!");
		sg_mib_heartDevice.flag = SNMP_LOGIN_STATUS_USER_NAME_INVALID;
		return -1;
	}
	if (user->password_len != g_eeprom_param.user.password_len ||
		memcmp(user->password, g_eeprom_param.user.password, user->password_len)) {
		trace_err("password error!");
		sg_mib_heartDevice.flag = SNMP_LOGIN_STATUS_PASSWORD_INVALID;
		return -1;
	}
	sg_mib_heartDevice.flag = SNMP_LOGIN_STATUS_SUCCESS;

	return 0;
}
/*
 * HEART DEVICE
 */
static int heart_device_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	if (sg_mib_heartDevice.flag == SNMP_LOGIN_STATUS_SUCCESS) {
		uint16_t ts_status = 0;
		hfpga_get_ts_status(0, &ts_status);
		sg_mib_heartDevice.mode = management_mode;
		sg_mib_heartDevice.lock_state = ts_status & CHANNEL_ALL_BITMAP;
	} else {
		sg_mib_heartDevice.mode = 0;
		sg_mib_heartDevice.lock_state = 0;
	}
	v->size = HEART_DEVICE_SIZE;
	v->data = &sg_mib_heartDevice;

	xmux_snmp_update_heart_device_time();

	return 0;
}
static int heart_device_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	memcpy(&sg_mib_heartDevice, v->data, v->size);

	return 0;
}
/*
 * TEST DATA SIZE
 */
static uint8_t test_buf[2 + 1024];
static int test_data_size_get(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	uint16_t len = 1024;

	memcpy(test_buf, &len, sizeof(len));
	v->size = 2 + 1024;
	v->data = test_buf;

	return 0;
}
static int test_data_size_set(struct wu_oid_object *obj, struct wu_snmp_value *v)
{
	memcpy(test_buf, v->data, v->size);
	trace_info("test_data_size set, size %d", v->size);

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
static struct wu_oid_object pid_trans_info_obj = {
	"PID_TRANS_INFO", {XMUX_ROOT_OID, 12, 1}, 8,
	0, OID_STATUS_RWRITE,
	pid_trans_info_get, pid_trans_info_set, PID_TRANS_INFO_SIZE,
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
	// APPLY psi
	{"APPLY_PSI", {XMUX_ROOT_OID, 17}, 7,
	 0, OID_STATUS_WRITE,
	 NULL, apply_psi_set, sizeof(struct apply_psi_snmp_data),
	},
	// output mux program info
	{"MUX_PROGRAM_INFO", {XMUX_ROOT_OID, 18}, 7,
	 0, OID_STATUS_RWRITE,
	 mux_program_info_get, mux_program_info_set,
	 sizeof(struct xmux_mux_program_info),
	},
	// tunner 0 param
	{"TUNNER_0_PARAM", {XMUX_ROOT_OID, 19}, 7,
	 0, OID_STATUS_RWRITE,
	 tunner_param_get, tunner_param_set,
	 sizeof(struct tunner_param),
	},
	// tunner 0 status
	{"TUNNER_0_STATUS", {XMUX_ROOT_OID, 20}, 7,
	 0, OID_STATUS_READ,
	 tunner_status_get, NULL,
	 sizeof(struct tunner_status),
	},
	// tunner 1 param
	{"TUNNER_1_PARAM", {XMUX_ROOT_OID, 21}, 7,
	 0, OID_STATUS_RWRITE,
	 tunner_param_get, tunner_param_set,
	 sizeof(struct tunner_param),
	},
	// tunner 1 status
	{"TUNNER_1_STATUS", {XMUX_ROOT_OID, 22}, 7,
	 0, OID_STATUS_READ,
	 tunner_status_get, NULL,
	 sizeof(struct tunner_status),
	},
	// bcm3033 param
	{"BCM3033_PARAM", {XMUX_ROOT_OID, 23}, 7,
	 0, OID_STATUS_RWRITE,
	 bcm3033_param_get, bcm3033_param_set,
	 sizeof(struct bcm3033_param),
	},
	// ci info
	{"CI_INFO", {XMUX_ROOT_OID, 24}, 7,
	 0, OID_STATUS_RWRITE,
	 ci_info_get, ci_info_set,
	 sizeof(struct ci_info_param),
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
	// test node for max data size test
	{"TEST_DATA_SIZE", {XMUX_ROOT_OID, 200}, 7,
	 0, OID_STATUS_RWRITE,
	 test_data_size_get, test_data_size_set, 0,
	},
};

#define ARRAY_ESIZE(a)		(sizeof(a) / sizeof(a[0]))

static void register_channel_pmt_oids(struct wu_oid_object *obj)
{
	int i;

	for (i = 0; i < PROGRAM_MAX_NUM; i++) {
		obj->oid[OID_CHAN_PMT_PROG_IDX] = i + 1;
		wu_snmp_agent_register(obj);
	}
}
static void register_channel_sdt_oids(struct wu_oid_object *obj)
{
	int i;

	for (i = 0; i < SDT_SECTION_NUM; i++) {
		obj->oid[OID_SDT_SEC_IDX] = i + 1;
		wu_snmp_agent_register(obj);
	}
}
static void register_channel_eit_oids(struct wu_oid_object *obj)
{
	int i;

	for (i = 0; i < EIT_SECTION_NUM; i++) {
		obj->oid[OID_EIT_SEC_IDX] = i + 1;
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

	for (i = 0; i < (CHANNEL_MAX_NUM * NODE_NUM); i++) {
		pid_trans_info_obj.oid[7] = i + 1;
		wu_snmp_agent_register(&pid_trans_info_obj);
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

static time_t heart_device_time;
static int disable_check;
void xmux_snmp_update_heart_device_time()
{
	heart_device_time = time(NULL);
}
void xmux_snmp_check_connection()
{
	if (disable_check)
		return;

	if ((sg_mib_heartDevice.flag == SNMP_LOGIN_STATUS_SUCCESS) &&
		(time(NULL) - heart_device_time > 15)) {
		trace_warn("timeout of heart beat!");
		sg_mib_heartDevice.flag = SNMP_LOGIN_STATUS_IDLE;
	}
}

void disable_snmp_connection_check()
{
	disable_check = 1;
}

void enable_snmp_connection_check()
{
	xmux_snmp_update_heart_device_time();
	disable_check = 0;
}

