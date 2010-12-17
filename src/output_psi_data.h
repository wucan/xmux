#ifndef _OUTPUT_PSI_DATA_H_
#define _OUTPUT_PSI_DATA_H_

#include "wu/wu_base_type.h"


struct xmux_output_psi_data;


bool output_psi_data_validate(struct xmux_output_psi_data *psi);
void output_psi_data_dump(struct xmux_output_psi_data *psi);
void output_psi_data_clear(struct xmux_output_psi_data *psi);


#endif /* _OUTPUT_PSI_DATA_H_ */

