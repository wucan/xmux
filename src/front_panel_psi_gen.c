#include "wu/message.h"

#include "xmux.h"
#include "psi_gen.h"

#include "front_panel_intstr.h"
#include "front_panel_define.h"


static msgobj mo = {MSG_INFO, ENCOLOR, "fp-psi-gen"};


static uint16_t prog_num_table[PROGRAM_MAX_NUM];
static int prog_num_cnt;
static uint16_t pick_free_prog_num()
{
	uint16_t i, j;

	for (i = 1; i <= 0xFFFF; i++) {
		for (j = 0; j < prog_num_cnt; j++) {
			if (prog_num_table[j] == i) {
				break;
			}
		}
		if (j == prog_num_cnt) {
			return i;
		}
	}

	return 0xFFFF;
}
static void fix_and_push_program_number(int index, uint16_t prog_num)
{
	int i;

	/*
	 * fix program number
	 */
	for (i = 0; i < index; i++) {
		if (prog_num == prog_num_table[i]) {
			uint16_t new_prog_num = pick_free_prog_num();
			trace_warn("fix program number from %d => %d",
				prog_num, new_prog_num);
			prog_num = new_prog_num;
			break;
		}
	}

	prog_num_table[index] = prog_num;
}
void build_program_number_table()
{
	int i;

	prog_num_cnt = 0;
	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		PROG_INFO_T *prog = &g_prog_info_table[i];
		if (FP_PROG_SELECTED(prog)) {
			fix_and_push_program_number(prog_num_cnt, prog->info.prog_num);
			prog_num_cnt++;
		}
		if (prog_num_cnt >= PROGRAM_MAX_NUM)
			break;
	}
}

int gen_nit_from_fp()
{
	struct nit_gen_context nit_gen_ctx;
	int i;

	trace_info("generate NIT ...");
	nit_gen_context_init(&nit_gen_ctx);
	for (i = 0; i < 2; i++) {
		uint32_t tsid = i + 1;
		nit_gen_context_add_stream(&nit_gen_ctx, tsid, 1);
	}
	nit_gen_context_pack(&nit_gen_ctx);
	dvbSI_Gen_NIT(&nit_gen_ctx.nit_data, nit_gen_ctx.nit_stream_data, nit_gen_ctx.stream_num);
	nit_gen_context_free(&nit_gen_ctx);

	return 0;
}

int gen_cat_from_fp()
{
	struct cat_gen_context cat_gen_ctx;

	trace_info("generate CAT ...");
	cat_gen_context_init(&cat_gen_ctx);
	cat_gen_context_add_ca_system(&cat_gen_ctx, 0x10, 0x20);
	cat_gen_context_pack(&cat_gen_ctx);
	dvbSI_Gen_CAT(cat_gen_ctx.cat_desc, cat_gen_ctx.desc_num);
	cat_gen_context_free(&cat_gen_ctx);

	return 0;
}

static struct sdt_gen_context gen_ctx;
int gen_sdt_from_fp(uint8_t *packpara, const PROG_INFO_T *pProgpara)
{
	int ncount;
	int nProgSel = 0;
	PROG_INFO_T *pProg;
	char name[PROGRAM_NAME_SIZE] = {0};

	trace_info("generate SDT ...");
	sdt_gen_context_init(&gen_ctx);
	for (ncount = 0; ncount < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; ncount++) {
		pProg = (PROG_INFO_T *) pProgpara + ncount;
		if (FP_PROG_SELECTED(pProg)) {
			memcpy(name, &pProg->info.prog_name[1][1], pProg->info.prog_name[1][0]);
			sdt_gen_context_add_service(&gen_ctx, name, prog_num_table[nProgSel], defProviderDsw);
			nProgSel++;
			if (nProgSel >= PROGRAM_MAX_NUM)
				break;
		}
	}
	sdt_gen_context_pack(&gen_ctx);
	dvbSI_Gen_SDT(&gen_ctx.sdt_data, gen_ctx.sdt_serv_data, gen_ctx.serv_num);
	sdt_gen_context_free(&gen_ctx);

	return 0;
}

static struct pat_gen_context pat_gen_ctx;
int gen_pat_from_fp(uint8_t *packpara, const PROG_INFO_T *pProgpara)
{
	int i, j;

	int nProgSel = 0;
	int nProgNum = 0;
	PROG_INFO_T *pProg = (PROG_INFO_T *) pProgpara;

	trace_info("generate PAT ...");
	// Begin Set Values
	pat_gen_context_init(&pat_gen_ctx);
	nProgSel = 0;
	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		pProg = (PROG_INFO_T *) pProgpara + i;
		if (FP_PROG_SELECTED(pProg)) {
			pat_gen_context_add_program(&pat_gen_ctx, prog_num_table[nProgSel], pProg->info.pmt.out);
			nProgSel++;
			if (nProgSel >= PROGRAM_MAX_NUM)
				break;
		}
	}
	pat_gen_context_pack(&pat_gen_ctx);
	trace_info("dvbsi generate and download PAT ...");
	dvbSI_Gen_PAT(&pat_gen_ctx.tpat, pat_gen_ctx.tpid_data, pat_gen_ctx.nprogs);

	return 0;
}

static struct pmt_gen_context pmt_gen_ctx;
int gen_pmt_from_fp(uint8_t *packpara, const PROG_INFO_T *pProgpara)
{
	int i, j;
	int nProgSel = 0;
	PROG_INFO_T *pProg;

	trace_info("generate PMT ...");
	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		int j;
		pProg = (PROG_INFO_T *) pProgpara + i;
		if (FP_PROG_SELECTED(pProg)) {
			pmt_gen_context_init(&pmt_gen_ctx);
			pmt_gen_context_add_program_info(&pmt_gen_ctx,
				prog_num_table[nProgSel], pProg->info.pmt.out, pProg->info.pcr.out);
			for (j = 0; j < PROGRAM_DATA_PID_MAX_NUM; j++)	// Video Audio
			{
				uint16_t out_pid = pProg->info.data[j].out;
				if (out_pid != 0x00 && out_pid != DATA_PID_PAD_VALUE) {
					pmt_gen_context_add_es(&pmt_gen_ctx,
						out_pid, pProg->info.data[j].type);
				}
			}
			pmt_gen_context_pack(&pmt_gen_ctx);
			trace_info("generate PMT of program #%d ...", i);
			dvbSI_Gen_PMT(&pmt_gen_ctx.tpmt, pmt_gen_ctx.tes, pmt_gen_ctx.nes);
			nProgSel++;

			if (nProgSel >= PROGRAM_MAX_NUM)
				break;
		}
	}

	return 0;
}

