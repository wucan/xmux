#include "wu/wu_base_type.h"

#include "xmux.h"
#include "pid_map_rule.h"


#define defProgPidBgn		0x100


uint16_t pid_map_rule_map_psi_pid(uint8_t chan_idx, uint8_t prog_idx, enmDswPsiPid psi_pid)
{
	return defProgPidBgn + chan_idx * PROGRAM_PID_MAX_NUM +
		PROGRAM_MAX_NUM * prog_idx + psi_pid;
}

