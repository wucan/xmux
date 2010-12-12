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
void sdt_gen_context_init(struct sdt_gen_context *ctx);
void sdt_gen_context_pack(struct sdt_gen_context *ctx);
void sdt_gen_context_add_service(struct sdt_gen_context *ctx,
		const char *serv_name, int service_id, const char *provider);
void sdt_gen_context_free(struct sdt_gen_context *ctx);

int psi_gen_output_psi_from_sections();
int psi_apply();


#endif /* _PSI_GEN_H_ */

