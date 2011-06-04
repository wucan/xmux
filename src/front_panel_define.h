#ifndef _FRONT_PANEL_DEFINE_H_
#define _FRONT_PANEL_DEFINE_H_


#define defPidIdler		DATA_PID_PAD_VALUE
#define defProviderDsw		"souka"
#define defMcuBufMax		4096
#define defProgPidMin		0x0020
#define defProgPidMax		(NULL_PID - 1)
#define defSelectedProgFpga	0x20
#define defMcuSyncFlag		0x74

/*
 * commands
 */
enum {
	FP_CMD_OUT_RATE = ((CHANNEL_MAX_NUM * PROGRAM_MAX_NUM) + 1),
	FP_CMD_NET,
	FP_CMD_SYS,
	FP_CMD_TUNNER1_PARAM,
	FP_CMD_TUNNER1_STATUS,
	FP_CMD_TUNNER2_PARAM,
	FP_CMD_TUNNER2_STATUS,
	FP_CMD_BCM3033,
};

enum FP_SYS_CMD {
	FP_SYS_CMD_ENTER_FP_MANAGEMENT_MODE = 1,
	FP_SYS_CMD_LEAVE_FP_MANAGEMENT_MODE,
	FP_SYS_CMD_START_ANALYSE_PSI,
	FP_SYS_CMD_READ_MAP_ANALYSE_STATUS,
	FP_SYS_CMD_READ_TS_STATUS,
	FP_SYS_CMD_APPLY_MAP_ANALYSE,
	FP_SYS_CMD_RESET_NET = 7,
};

typedef enum {
	enm_prog_pid_valid = 0,
	enm_prog_pid_val_err,
	enm_prog_pid_pmt_pcr,
	enm_prog_pid_pmt_other,
	enm_prog_pid_pcr_other,
	enm_prog_pid_other_other,
	enm_prog_pid_program,
	enm_prog_pid_overpid,
	enm_prog_pid_overprog,
} enm_prog_pid_invalid;


#endif /* _FRONT_PANEL_DEFINE_H_ */

