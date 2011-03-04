#ifndef _FRONT_PANEL_H_
#define _FRONT_PANEL_H_

#include "wu/wu_base_type.h"


struct fp_cmd_header;
struct fp_cmd;

int front_panel_open();
int front_panel_close();
int front_panel_run();
struct fp_cmd * front_panel_send_cmd(struct fp_cmd *cmd, int expect_cmd);
int front_panel_check_recv_cmd(struct fp_cmd *recv_cmd);

int fp_create_response_cmd(uint8_t *buf,
	struct fp_cmd_header *req_cmd_header, void *param, int size);
int fp_build_cmd(uint8_t *buf, bool is_read, int cmd,
		void *param, int param_size);
bool fp_validate_cmd(uint8_t *buf, int size, int expect_param_size);
void fp_cmd_copy(struct fp_cmd *dst, struct fp_cmd *src);
void fp_select_program(uint8_t prog_idx);
void fp_deselect_program(uint8_t prog_idx);


#endif /* _FRONT_PANEL_H_ */

