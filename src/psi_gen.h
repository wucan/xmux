#ifndef _PSI_GEN_H_
#define _PSI_GEN_H_

#include "xmux.h"

#include "gen_dvb_si.h"


#define UV_DESCR_LEN    		33

/*
 * SDT generate stuff
 */
struct sdt_service_info {
	int service_id;
	char name[PROGRAM_NAME_SIZE];
	uint8_t name_len;

	char provider[PROGRAM_NAME_SIZE];
	uint8_t provider_len;
};
struct sdt_gen_context {
	uint8_t serv_num;
	uint8_t desc_num;

	struct sdt_service_info serv_info[PROGRAM_MAX_NUM];

	uv_sdt_data sdt_data;
	uv_sdt_serv_data sdt_serv_data[PROGRAM_MAX_NUM];
};
static inline void sdt_gen_context_init(struct sdt_gen_context *ctx)
{
	ctx->serv_num = 0;
	ctx->desc_num = 1;

	ctx->sdt_data.i_table_id = 0x42;
	ctx->sdt_data.i_tran_stream_id = 0x1234;
	ctx->sdt_data.i_orig_net_id = 0xEAEA;

}
static inline void sdt_gen_context_pack(struct sdt_gen_context *ctx)
{
	int i, j;
	uv_descriptor *p_descr;
	uv_sdt_serv_data *sdt_serv_data = ctx->sdt_serv_data;
	static uint8_t sdbuf[PROGRAM_MAX_NUM][5][UV_DESCR_LEN];

	for (i = 0; i < ctx->serv_num; i++) {
		sdt_serv_data[i].i_serv_id = ctx->serv_info[i].service_id;
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
			p_descr[j].i_length = 3 + ctx->serv_info[i].provider_len + ctx->serv_info[i].name_len;
			p_descr[j].p_data = sdbuf[i][j];
			p_descr[j].p_data[0] = 0x01; // serivece type: DTV
			p_descr[j].p_data[1] = ctx->serv_info[i].provider_len;
			memcpy(p_descr[j].p_data + 2, ctx->serv_info[i].provider, ctx->serv_info[i].provider_len);
			p_descr[j].p_data[6] = ctx->serv_info[i].name_len;
			memcpy(p_descr[j].p_data + 3 + ctx->serv_info[i].provider_len, ctx->serv_info[i].name, ctx->serv_info[i].name_len);
		}
	}
}
static inline void sdt_gen_context_add_service(struct sdt_gen_context *ctx,
		const char *serv_name, int service_id, const char *provider)
{
	struct sdt_service_info *info = &ctx->serv_info[ctx->serv_num];

	info->service_id = service_id;
	info->name_len = strlen(serv_name);
	memcpy(info->name, serv_name, info->name_len);
	info->provider_len = strlen(provider);
	memcpy(info->provider, provider, info->provider_len);
	ctx->serv_num++;
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

