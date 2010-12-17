#include <string.h>

#include "wu/message.h"

#include "xmux.h"
#include "xmux_config.h"
#include "output_psi_data.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "output_psi_data"};


bool output_psi_data_validate(struct xmux_output_psi_data *psi)
{
	/* TODO */

	return true;
}

void output_psi_data_dump(struct xmux_output_psi_data *psi)
{
	uint8_t i;
	struct output_psi_data_entry *e;
	int total_pkts = 0;

	for (i = 0; i < OUTPUT_PSI_TYPE_MAX_NUM; i++) {
		e = &psi->psi_ents[i];
		if (e->offset == 0 && e->nr_ts_pkts == 0)
			break;
		trace_info("psi type #%d, offset %d, %d packets",
			i, e->offset, e->nr_ts_pkts);
		total_pkts += e->nr_ts_pkts;
	}
	trace_info("there are total %d psi ts packets", total_pkts);
}

void output_psi_data_clear(struct xmux_output_psi_data *psi)
{
	memset(psi, 0, sizeof(*psi));
}

