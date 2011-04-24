#ifndef _OUTPUT_PSI_DATA_H_
#define _OUTPUT_PSI_DATA_H_

#include "wu/wu_base_type.h"


struct xmux_output_psi_data;


/*
 * psi type store in output_psi_area
 * PAT, PMT, CAT: (hfpga write) howto = 0; SDT, NIT: howto = 1
 */
enum {
	PSI_TYPE_PAT = 0,
	PSI_TYPE_PMT,
	PSI_TYPE_CAT,
	PSI_TYPE_SDT,
	PSI_TYPE_NIT,
};
static inline int psi_type_2_howto(type)
{
	if (type == PSI_TYPE_NIT)
		return 0;
	if (type >= PSI_TYPE_PAT && type <= PSI_TYPE_CAT)
		return 0;

	return 1;
}

bool output_psi_data_validate(struct xmux_output_psi_data *psi);
void output_psi_data_dump(struct xmux_output_psi_data *psi);
void output_psi_data_clear(struct xmux_output_psi_data *psi);
void begin_fill_output_psi_data();
void fill_output_psi_data(int psi_type, uint8_t *ts_buf, int ts_len);
int psi_apply_from_output_psi();


#endif /* _OUTPUT_PSI_DATA_H_ */

