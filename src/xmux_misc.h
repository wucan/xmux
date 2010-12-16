#ifndef _XMUX_MISC_H_
#define _XMUX_MISC_H_

#include "wu/wu_base_type.h"


struct xmux_system_param;

void set_output_bitrate(uint32_t new_bitrate);
void set_packet_format(uint8_t new_fmt);
bool xmux_system_param_validate(struct xmux_system_param *sys);
void xmux_system_param_init_default(struct xmux_system_param *sys);
void xmux_system_param_dump(struct xmux_system_param *sys);


#endif /* _XMUX_MISC_H_ */

