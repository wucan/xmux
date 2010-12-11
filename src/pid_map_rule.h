#ifndef _PID_MAP_RULE_H_
#define _PID_MAP_RULE_H_

#include "wu/wu_base_type.h"


typedef enum {
	DSW_PID_PMT = 0,
	DSW_PID_PCR = 1,
	DSW_PID_VIDEO = 2,
	DSW_PID_AUDIO = 3,

	DSW_PID_MAX = 0x0F
} enmDswPsiPid;

typedef enum {
	DSW_PSI_TYPE_VIDEO = 2,
	DSW_PSI_TYPE_AUDIO = 4,

	DSW_PSI_TYPE_MAX = 0x0F
} enmDswPsiType;

uint16_t pid_map_rule_map_psi_pid(uint8_t chan_idx, uint8_t prog_idx, enmDswPsiPid psi_pid, uint16_t in_pid, uint16_t *pids, int nr_pids);


#endif /* _PID_MAP_RULE_H_ */

