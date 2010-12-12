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

/*
 * PAT generate stuff
 */
struct pat_prog_info {
	uint16_t prog_num;
	uint16_t pmt_pid;
};
struct pat_gen_context {
	uint8_t nprogs;

	uv_pat_data tpat;
	uv_pat_pid_data tpid_data[PROGRAM_MAX_NUM + 1];
	struct pat_prog_info programs[PROGRAM_MAX_NUM];
};
void pat_gen_context_init(struct pat_gen_context *ctx);
void pat_gen_context_pack(struct pat_gen_context *ctx);
void pat_gen_context_add_program(struct pat_gen_context *ctx,
		uint16_t prog_num, uint16_t pmt_pid);
void pat_gen_context_free(struct pat_gen_context *ctx);

/*
 * PMT generate stuff
 */
struct pmt_gen_context {
	uv_pmt_data tpmt;

	uint8_t nes;
	uv_pmt_es_data tes[PROGRAM_MAX_NUM];
};
void pmt_gen_context_init(struct pmt_gen_context *ctx);
void pmt_gen_context_pack(struct pmt_gen_context *ctx);
void pmt_gen_context_add_program_info(struct pmt_gen_context *ctx,
		uint16_t prog_num, uint16_t pmt_pid, uint16_t pcr_pid);
void pmt_gen_context_add_es(struct pmt_gen_context *ctx,
		uint16_t pid, uint8_t type);
void pmt_gen_context_free(struct pmt_gen_context *ctx);

int psi_gen_output_psi_from_sections();
int psi_apply();


#endif /* _PSI_GEN_H_ */

