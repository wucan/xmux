#include <string.h>

#include "xmux.h"
#include "xmux_4_chan_rf.h"
#include "front_panel.h"
#include "front_panel_intstr.h"
#include "front_panel_define.h"


struct xmux_4_chan_rf_param g_4_chan_rf_param;

/*
 * data churnning
 */
void buf_2_tunner_param(void *buf, struct tunner_param *p)
{
	memcpy(p, buf, sizeof(*p));
}
void tunner_status_2_buf(struct tunner_status *s, void *buf)
{
	memcpy(buf, s, sizeof(*s));
}
void buf_2_tunner_status(void *buf, struct tunner_status *s)
{
	memcpy(s, buf, sizeof(*s));
}
void bcm3033_param_2_buf(struct bcm3033_param *p, void *buf)
{
	memcpy(buf, p, sizeof(*p));
}
void buf_2_bcm3033_param(void *buf, struct bcm3033_param *p)
{
	memcpy(p, buf, sizeof(*p));
}

int xmux_4_channel_rf_cmds_handler(struct fp_cmd_header *cmd_header,
	int is_read, uint8_t *recv_msg_buf,
	uint8_t *resp_msg_buf,	uint16_t *p_resp_msg_len)
{
	uint16_t cmd = cmd_header->seq & 0x7FFF;
	int tunner_id = 0;

switch_again:
	switch (cmd) {
	case FP_CMD_TUNNER1_PARAM:
	{
		struct tunner_param tp;
		if (is_read) {
			tunner_get_param(tunner_id, &tp);
			*p_resp_msg_len = fp_create_response_cmd(resp_msg_buf, cmd_header,
				&tp, sizeof(tp));
		} else {
			buf_2_tunner_param(recv_msg_buf + sizeof(struct fp_cmd_header), &tp);
			tunner_set_param(tunner_id, &tp);
		}
		return 1;
	}
		break;
	case FP_CMD_TUNNER1_STATUS:
	{
		struct tunner_status ts;
		if (is_read) {
			tunner_get_status(tunner_id, &g_4_chan_rf_param.tunner[tunner_id].status);
			tunner_status_2_buf(&g_4_chan_rf_param.tunner[tunner_id].status, &ts);
			*p_resp_msg_len = fp_create_response_cmd(resp_msg_buf, cmd_header,
				&ts, sizeof(ts));
		} else {
			// not support!
			return 0;
		}
		return 1;
	}
		break;
	case FP_CMD_TUNNER2_PARAM:
	case FP_CMD_TUNNER2_STATUS:
		// reuse TUNNER1 codes
		tunner_id = 1;
		cmd -= 2;
		goto switch_again;
		break;
	case FP_CMD_BCM3033:
	{
		struct bcm3033_param bp;
		if (is_read) {
			bcm3033_get_param(&bp);
			bcm3033_param_2_buf(&g_4_chan_rf_param.bcm, &bp);
			*p_resp_msg_len = fp_create_response_cmd(resp_msg_buf, cmd_header,
				&bp, sizeof(bp));
		} else {
			buf_2_bcm3033_param(recv_msg_buf + sizeof(struct fp_cmd_header), &bp);
			bcm3033_set_param(&bp);
		}
		return 1;
	}
		break;
	default:
		break;
	}

	return 0;
}

