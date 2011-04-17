#include <string.h>

#include "wu/message.h"

#include "xmux.h"
#include "xmux_config.h"
#include "output_psi_data.h"
#include "gen_dvb_si_api.h"
#include "hfpga.h"
#include "section.h"


extern uv_dvb_io hfpga_dev;

static msgobj mo = {MSG_INFO, ENCOLOR, "output_psi_data"};


bool output_psi_data_validate(struct xmux_output_psi_data *psi)
{
	uint8_t i;
	struct output_psi_data_entry *e;
	uint32_t next_expect_offset = 0;

	for (i = 0; i < OUTPUT_PSI_TYPE_MAX_NUM; i++) {
		e = &psi->psi_ents[i];
		if (e->offset == 0 && e->nr_ts_pkts == 0)
			continue;
		if (e->offset != next_expect_offset) {
			trace_err("new psi type #%d offset %d != expect offset %d!",
				i, e->offset, next_expect_offset);
			return false;
		}
		if (e->offset + e->nr_ts_pkts >= OUTPUT_PSI_PACKET_MAX_NUM) {
			trace_err("new psi type #%d exceed max packet(%d)!",
				i, OUTPUT_PSI_PACKET_MAX_NUM);
			return false;
		}
		next_expect_offset = e->offset + e->nr_ts_pkts;
	}

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
			continue;
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

static int pkt_offset;
void begin_fill_output_psi_data()
{
	struct xmux_output_psi_data *psi_data = &g_eeprom_param.output_psi_area.output_psi;

	memset(psi_data->psi_ents, 0, sizeof(psi_data->psi_ents));
	pkt_offset = 0;
}

void fill_output_psi_data(int psi_type, uint8_t *ts_buf, int ts_len)
{
	struct xmux_output_psi_data *psi_data = &g_eeprom_param.output_psi_area.output_psi;

	if (!psi_data->psi_ents[psi_type].nr_ts_pkts) {
		psi_data->psi_ents[psi_type].offset = pkt_offset;
	}
	psi_data->psi_ents[psi_type].nr_ts_pkts += ts_len / TS_PACKET_BYTES;
	memcpy(&psi_data->ts_pkts[pkt_offset], ts_buf, ts_len);

	pkt_offset += ts_len / TS_PACKET_BYTES;
}

int psi_apply_from_output_psi()
{
	struct xmux_output_psi_data *psi_data = &g_eeprom_param.output_psi_area.output_psi;
	struct output_psi_data_entry *ent;
	uint8_t psi_type, howto = 0;
	int i;

	trace_info("apply psi to fpga...");
	dvbSI_Start(&hfpga_dev);
	dvbSI_GenSS(HFPGA_CMD_SI_STOP);
	for (psi_type = 0; psi_type < OUTPUT_PSI_TYPE_MAX_NUM; psi_type++) {
		ent = &psi_data->psi_ents[psi_type];
		if (ent->nr_ts_pkts) {
			howto = psi_type_2_howto(psi_type);
			trace_info("write type %d, offset %d, %d packets",
				psi_type, ent->offset, ent->nr_ts_pkts);
			hex_dump("ts", &psi_data->ts_pkts[ent->offset],
				MIN(188 * ent->nr_ts_pkts, 48));
			hfpga_dev.write(&psi_data->ts_pkts[ent->offset], 188 * ent->nr_ts_pkts, howto);
		}
	}
	dvbSI_GenSS(HFPGA_CMD_SI_START);
	dvbSI_Stop(&hfpga_dev);

	return 0;
}

