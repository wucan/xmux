#ifndef _OUTPUT_PSI_DATA_H_
#define _OUTPUT_PSI_DATA_H_

#include "wu/wu_base_type.h"


struct xmux_output_psi_data;


bool output_psi_data_validate(struct xmux_output_psi_data *psi);
void output_psi_data_dump(struct xmux_output_psi_data *psi);
void output_psi_data_clear(struct xmux_output_psi_data *psi);
void fill_output_psi_data(int psi_type, uint8_t *ts_buf, int ts_len);
int psi_apply_from_output_psi();


#endif /* _OUTPUT_PSI_DATA_H_ */

