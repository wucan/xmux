#ifndef _FRONT_PANEL_H_
#define _FRONT_PANEL_H_

#include "wu/wu_base_type.h"


int front_panel_open();
int front_panel_close();
int front_panel_run();

int fp_build_cmd(uint8_t *buf, bool is_read, int cmd,
		void *param, int param_size);
bool fp_validate_cmd(uint8_t *buf, int size, int expect_param_size);


#endif /* _FRONT_PANEL_H_ */

