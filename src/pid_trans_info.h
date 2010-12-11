#ifndef _PID_TRANS_INFO_H
#define _PID_TRANS_INFO_H

#include "xmux.h"
#include "xmux_snmp_intstr.h"


extern struct pid_trans_info_snmp_data sg_mib_pid_trans_info[CHANNEL_MAX_NUM];
extern uint8_t sg_mib_trans[];

void pid_trans_info_read_data_snmp(uint8_t trans_idx, struct wu_snmp_value *v);
void pid_trans_info_write_data_snmp(uint8_t trans_idx, struct wu_snmp_value *v);


#endif /* _PID_TRANS_INFO_H */

