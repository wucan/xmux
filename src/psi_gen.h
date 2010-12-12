#ifndef _PSI_GEN_H_
#define _PSI_GEN_H_

#include "gen_dvb_si.h"


/*
 * SDT generate stuff
 */
struct sdt_gen_context {
	uv_sdt_data sdt_data;
};
static inline void sdt_gen_context_init(struct sdt_gen_context *ctx)
{
	ctx->sdt_data.i_table_id = 0x42;
	ctx->sdt_data.i_tran_stream_id = 0x1234;
	ctx->sdt_data.i_orig_net_id = 0xEAEA;
}

int psi_gen_output_psi_from_sections();
int psi_apply();


#endif /* _PSI_GEN_H_ */

