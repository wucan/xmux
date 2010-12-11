#include <string.h>

#include "xmux.h"
#include "xmux_config.h"
#include "psi_parse.h"
#include "psi_gen.h"
#include "section.h"
#include "gen_dvb_si.h"


extern uv_dvb_io hfpga_dev;

static int pkt_offset;

void fill_output_psi_data(int psi_type, uint8_t *ts_buf, int ts_len)
{
	struct xmux_output_psi_data *psi_data = &g_xmux_root_param.output_psi_area.output_psi;

	if (psi_type == 0) {
		memset(psi_data->psi_ents, 0, sizeof(psi_data->psi_ents));
		pkt_offset = 0;
	}

	if (!psi_data->psi_ents[psi_type].nr_ts_pkts) {
		psi_data->psi_ents[psi_type].offset = pkt_offset;
	}
	psi_data->psi_ents[psi_type].nr_ts_pkts += ts_len / TS_PACKET_BYTES;
	memcpy(&psi_data->ts_pkts[pkt_offset], ts_buf, ts_len);
	
	pkt_offset += ts_len / TS_PACKET_BYTES;
}

int psi_gen_output_psi_from_sections()
{
	int sec_len, ts_len;
	uint8_t ts_buf[188 * 7];
	int cc;

	/*
	 * PAT
	 */
	cc = 0;
	sec_len = sg_mib_xxx_len(sg_mib_pat[CHANNEL_MAX_NUM]);
	ts_len = section_to_ts_length(sec_len);
	ts_len = section_to_ts(sg_mib_pat[CHANNEL_MAX_NUM] + 2, sec_len, ts_buf, 0x00, &cc);
	fill_output_psi_data(0, ts_buf, ts_len);

	/*
	 * PMT
	 */
	{
	struct pid_trans_info_snmp_data *pid_trans_info;
	struct pid_trans_info_program_snmp_data *prog;
	uint8_t chan_idx, prog_idx;

	cc = 0;
	pid_trans_info = g_xmux_root_param.pid_trans_info_area.pid_trans_info;
	for (chan_idx = 0; chan_idx < CHANNEL_MAX_NUM; chan_idx++) {
		for (prog_idx = 0; prog_idx < pid_trans_info[chan_idx].nprogs; prog_idx++) {
			if (!(pid_trans_info[chan_idx].status & (1 << prog_idx)))
				continue;
			printf("pid_gen: gen pmt, use secion chan #%d, prog #%d\n",
				chan_idx, prog_idx);
			prog = &pid_trans_info[chan_idx].programs[prog_idx];
			sec_len = sg_mib_xxx_len(sg_mib_pmt[chan_idx][prog_idx]);
			ts_len = section_to_ts_length(sec_len);
			ts_len = section_to_ts(sg_mib_pmt[chan_idx][prog_idx] + 2,
				sec_len, ts_buf, prog->pmt.out, &cc);
			fill_output_psi_data(1, ts_buf, ts_len);
		}
	}
	}

	/*
	 * SDT
	 */
	cc = 0;
	sec_len = sg_mib_xxx_len(sg_mib_sdt[CHANNEL_MAX_NUM]);
	ts_len = section_to_ts_length(sec_len);
	ts_len = section_to_ts(sg_mib_sdt[CHANNEL_MAX_NUM] + 2,
		sec_len, ts_buf, 0x11, &cc);
	fill_output_psi_data(2, ts_buf, ts_len);

	/*
	 * at last save it to eeprom
	 */
	xmux_config_save_output_psi_data();

	return 0;
}

int psi_apply()
{
	struct xmux_output_psi_data *psi_data = &g_xmux_root_param.output_psi_area.output_psi;
	struct output_psi_data_entry *ent;
	uint8_t psi_type, howto = 0;

	/*
	 * FIXME: PAT, PMT, CAT: howto = 0; SDT, NIT: howto = 1
	 */
	dvbSI_Start(&hfpga_dev);
	for (psi_type = 0; psi_type < OUTPUT_PSI_TYPE_MAX_NUM; psi_type++) {
		ent = &psi_data->psi_ents[psi_type];
		if (ent->nr_ts_pkts) {
			hfpga_dev.write(&psi_data->ts_pkts[ent->offset], 188 * ent->nr_ts_pkts, howto);
		}
	}
	dvbSI_Stop(&hfpga_dev);

	return 0;
}

