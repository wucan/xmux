#include "wu/wu_base_type.h"

#include "xmux.h"
#include "pid_map_rule.h"


#define defProgPidBgn		0x100

#define PID_MAP_MAX_PID_NUM_PER_CHAN		0x200
#define PID_MAP_MAX_PID_NUM_PER_PROG		0x10

uint16_t pid_map_rule_map_psi_pid(uint8_t chan_idx, uint8_t prog_idx, enmDswPsiPid psi_pid)
{
	return defProgPidBgn + chan_idx * PID_MAP_MAX_PID_NUM_PER_CHAN +
		PID_MAP_MAX_PID_NUM_PER_PROG * prog_idx + psi_pid;
}

