#include "wu/message.h"

#include "xmux_config.h"
#include "ci_card.h"

static msgobj mo = {MSG_INFO, ENCOLOR, "ci"};


static bool xmux_ci_download_pmt_section(uint8_t *sec, int sec_len)
{
	/* TODO */

	return true;
}

void xmux_ci_apply()
{
#if CHANNEL_MAX_NUM == 1
	struct pid_trans_info_snmp_data *pid_trans_info;
	uint8_t prog_idx;
	uint16_t sec_len;
	int down_cnt = 0;

	pid_trans_info = &g_eeprom_param.pid_trans_info_area.table[0].data;
	for (prog_idx = 0; prog_idx < pid_trans_info->nprogs; prog_idx++) {
		if (wu_bitmap_test_bit(pid_trans_info->scramble_status, prog_idx) &&
			PROGRAM_SELECTED(pid_trans_info->sel_status, prog_idx)) {
			memcpy(&sec_len, g_eeprom_param.input_pmt_sec[prog_idx], 2);
			if (sec_len > 0 && sec_len < (INPUT_PMT_SEC_MAX_LEN - 2)) {
				trace_info("program #%d download pmt section, len %d",
					prog_idx, sec_len);
				xmux_ci_download_pmt_section(
					g_eeprom_param.input_pmt_sec[prog_idx] + 2, sec_len);
				down_cnt++;
			} else {
				trace_err("program #%d pmt section invalide!, len %d",
					prog_idx, sec_len);
			}
		}
	}
	trace_info("total %d pmt section had download to ci", down_cnt);
#endif
}

void xmux_ci_info_update(struct ci_info_param *ci_info)
{
	// TODO
}

