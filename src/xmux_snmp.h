#ifndef _XMUX_SNMP_H_
#define _XMUX_SNMP_H_

#include "xmux_snmp_intstr.h"


extern struct pid_trans_snmp_data sg_mib_trans;
extern struct heart_device_snmp_data sg_mib_heartDevice;

int xmux_snmp_register_all_oids();


#endif /* _XMUX_SNMP_H_ */

