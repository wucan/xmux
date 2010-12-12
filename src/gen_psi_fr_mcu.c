#include <fcntl.h>

#include "wu/message.h"

#include "xmux.h"
#include "fpga_api.h"
#include "psi_gen.h"

#include "front_panel_intstr.h"
#include "front_panel_define.h"


extern uv_dvb_io hfpga_dev;

static msgobj mo = {MSG_INFO, ENCOLOR, "fp-psi-gen"};

static int GenNIT(void)
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

static int GenCAT(void)
{
	struct cat_gen_context cat_gen_ctx;

	trace_info("generate CAT ...");
	cat_gen_context_init(&cat_gen_ctx);
	cat_gen_context_add_ca_system(&cat_gen_ctx, 0x10, 0x20);
	cat_gen_context_pack(&cat_gen_ctx);
	dvbSI_Gen_CAT(cat_gen_ctx.cat_desc, cat_gen_ctx.desc_num);
	cat_gen_context_free(&cat_gen_ctx);
}

int gen_sdt_fr_mcu(uint8_t * packpara, const PROG_INFO_T * pProgpara)
{
	struct sdt_gen_context gen_ctx;
	int ncount;
	int nProgSel = 0;
	PROG_INFO_T *pProg;

	trace_info("generate SDT ...");
	sdt_gen_context_init(&gen_ctx);
	for (ncount = 0; ncount < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; ncount++) {
		pProg = (PROG_INFO_T *) pProgpara + ncount;
		if (pProg->status == 1) {
			pProg->info.prog_name[1][pProg->info.prog_name[1][0]] = 0;
			sdt_gen_context_add_service(&gen_ctx, pProg->info.prog_name[1][1], nProgSel + 1, defProviderDsw);
			nProgSel++;
			if (nProgSel >= PROGRAM_MAX_NUM)
				break;
		}
	}
	sdt_gen_context_pack(&gen_ctx);
	dvbSI_Gen_SDT(&gen_ctx.sdt_data, gen_ctx.sdt_serv_data, gen_ctx.serv_num);
	sdt_gen_context_pack(&gen_ctx);

	return 0;
}

int gen_pat_pmt_fr_mcu(uint8_t * packpara, const PROG_INFO_T * pProgpara)
{
	int i, j;

	int nProgSel = 0;
	int nProgNum = 0;
	PROG_INFO_T *pProg = (PROG_INFO_T *) pProgpara;
	struct pat_gen_context pat_gen_ctx;

	trace_info("generate PAT&PMT ...");

	// Begin Set Values
	pat_gen_context_init(&pat_gen_ctx);
	nProgSel = 0;
	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		pProg = (PROG_INFO_T *) pProgpara + i;
		if (pProg->status == 1) {
			pat_gen_context_add_program(&pat_gen_ctx, nProgSel + 1, pProg->info.pmt.out);
			nProgSel++;
			if (nProgSel >= PROGRAM_MAX_NUM)
				break;
		}
	}
	pat_gen_context_pack(&pat_gen_ctx);
	trace_info("generate PAT ...");
	dvbSI_Gen_PAT(&pat_gen_ctx.tpat, pat_gen_ctx.tpid_data, pat_gen_ctx.nprogs);

	nProgSel = 0;
	for (i = 0; i < CHANNEL_MAX_NUM * PROGRAM_MAX_NUM; i++) {
		int j;
		pProg = (PROG_INFO_T *) pProgpara + i;
		if (pProg->status == 1) {
			struct pmt_gen_context pmt_gen_ctx;
			pmt_gen_context_init(&pmt_gen_ctx);
			pmt_gen_context_add_program_info(&pmt_gen_ctx,
				nProgSel + 1, pProg->info.pmt.out, pProg->info.pcr.out);
			for (j = 0; j < PROGRAM_DATA_PID_MAX_NUM; j++)	// Video Audio
			{
				uint16_t out_pid = pProg->info.data[j].out;
				if (out_pid != 0x00 && out_pid != 0x0F) {
					pmt_gen_context_add_es(&pmt_gen_ctx,
						out_pid, pProg->info.data[j].type);
				}
			}
			pmt_gen_context_pack(&pmt_gen_ctx);
			trace_info("generate PMT of program $#%d ...", i);
			dvbSI_Gen_PMT(&pmt_gen_ctx.tpmt, pmt_gen_ctx.tes, pmt_gen_ctx.nes);
			nProgSel++;

			if (nProgSel >= PROGRAM_MAX_NUM)
				break;
		}
	}

	return 0;
}

