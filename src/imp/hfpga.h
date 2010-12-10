#ifndef _HFPGA_H_
#define _HFPGA_H_

#include "xmux.h"
#include "fpga_api.h"


int hfpga_write_pid_map(ACCESS_HFPGA_PID_MAP pid_map);
int hfpga_get_ts_status(int chan_idx, uint16_t *ts_status_para);


#endif /* _HFPGA_H_ */

