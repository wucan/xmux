#ifndef _HFPGA_H_
#define _HFPGA_H_

#include "xmux.h"
#include "fpga_api.h"


int hfpga_write_pid_map(ACCESS_HFPGA_PID_MAP *pid_map);
int hfpga_get_ts_status(int chan_idx, uint16_t *ts_status_para);
int hfpga_write_sys_output_bitrate(uint32_t bitrate);
int hfpga_write_sys_packet_length(uint16_t pkt_len);
int hfpga_write_select_channel(uint8_t chan);


#endif /* _HFPGA_H_ */

