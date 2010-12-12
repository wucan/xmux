#ifndef _PSI_GEN_H_
#define _PSI_GEN_H_

#include "xmux.h"

#include "gen_dvb_si.h"


#define UV_DESCR_LEN    		33

/*
 * SDT generate stuff
 */
struct sdt_gen_context {
	uint8_t serv_num;
	uint8_t desc_num;

	uv_sdt_data sdt_data;
	uv_sdt_serv_data sdt_serv_data[PROGRAM_MAX_NUM];
};
static inline void sdt_gen_context_init(struct sdt_gen_context *ctx)
{
	int i, j;
	uv_descriptor *p_descr;
	uv_sdt_serv_data *sdt_serv_data = ctx->sdt_serv_data;
	static uint8_t sdbuf[PROGRAM_MAX_NUM][5][UV_DESCR_LEN];

	ctx->serv_num = PROGRAM_MAX_NUM;
	ctx->desc_num = 2;

	ctx->sdt_data.i_table_id = 0x42;
	ctx->sdt_data.i_tran_stream_id = 0x1234;
	ctx->sdt_data.i_orig_net_id = 0xEAEA;

	for (i = 0; i < ctx->serv_num; i++) {
		sdt_serv_data[i].i_serv_id = i + 1;
		sdt_serv_data[i].i_eit_pres_foll_flag = 0;
		sdt_serv_data[i].i_eit_sched_flag = 0;
		sdt_serv_data[i].i_free_ca_mode = 0;
		sdt_serv_data[i].i_running_status = 1;
		sdt_serv_data[i].i_descr_num = ctx->desc_num;
		sdt_serv_data[i].p_descr = (uv_descriptor *)malloc(
			sdt_serv_data[i].i_descr_num * sizeof(uv_descriptor));
		p_descr = sdt_serv_data[i].p_descr;
		for (j = 0; j < sdt_serv_data[i].i_descr_num; j++) {
			char name[32];
			p_descr[j].i_tag = 0x48;
			p_descr[j].i_length = 14;
			p_descr[j].p_data = sdbuf[i][j];
			p_descr[j].p_data[0] = 0x01; // serivece type: DTV
			p_descr[j].p_data[1] = 4;
			p_descr[j].p_data[6] = 7;
			if (j == 0) {
				sprintf(name, "CCTV-%02d", i);
				memcpy(p_descr[j].p_data + 2, "CCTV", 4);
				memcpy(p_descr[j].p_data + 7, name, 7);
			} else {
				sprintf(name, "SZTV-%02d", i);
				memcpy(p_descr[j].p_data + 2, "SZTV", 4);
				memcpy(p_descr[j].p_data + 7, name, 7);
			}
		}
	}

}
static inline void sdt_gen_context_free(struct sdt_gen_context *ctx)
{
	int i;

	for (i = 0; i < ctx->serv_num; i++) {
		free(ctx->sdt_serv_data[i].p_descr);
	}
}

int psi_gen_output_psi_from_sections();
int psi_apply();


#endif /* _PSI_GEN_H_ */

