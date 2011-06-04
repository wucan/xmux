#ifndef _PID_TRANS_INFO_H
#define _PID_TRANS_INFO_H

#include "xmux.h"
#include "xmux_snmp_intstr.h"


struct wu_snmp_value;

extern struct pid_trans_info_snmp_data sg_mib_pid_trans_info[CHANNEL_MAX_NUM];

void pid_trans_info_read_data_snmp(uint8_t trans_idx, struct wu_snmp_value *v);
void pid_trans_info_write_data_snmp(uint8_t trans_idx, struct wu_snmp_value *v);
bool pid_trans_info_validate(struct pid_trans_info_snmp_data *data);
void pid_trans_info_dump(uint8_t chan_idx, struct pid_trans_info_snmp_data *data);


#endif /* _PID_TRANS_INFO_H */

