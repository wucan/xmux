#include <gtk/gtk.h>
#include <glib.h>
#include <gdk/gdktypes.h>

#include "wu/wu_byte_stream.h"
#include "wu/wu_csc.h"
#include "wu/thread.h"

#include "front_panel.h"
#include "front_panel_intstr.h"
#include "front_panel_define.h"


static uint8_t req_buf[1024], resp_buf[1024];
static uint16_t resp_len;

static GtkComboBox *chan_combo_box;
static GtkComboBox *prog_combo_box;
static GtkLabel *pmt_in_label;
static GtkEntry *pmt_entry;
static GtkLabel *pcr_in_label;
static GtkEntry *pcr_entry;
static GtkLabel *data_0_in_label;
static GtkEntry *data_0_entry;
static GtkLabel *data_1_in_label;
static GtkEntry *data_1_entry;
static GtkLabel *data_2_in_label;
static GtkEntry *data_2_entry;

static gboolean window_delete_event(GtkWidget *widget,
		GdkEvent *event, gpointer data)
{
	return FALSE;
}

static void window_destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

static void enter_fp_management_mode_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	uint16_t sys_cmd = FP_SYS_CMD_ENTER_FP_MANAGEMENT_MODE;

	sys_cmd = htons(sys_cmd);
	fp_build_cmd(req_buf, true, FP_CMD_SYS, &sys_cmd, sizeof(sys_cmd));
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
}

static void leave_fp_management_mode_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	uint16_t sys_cmd = FP_SYS_CMD_LEAVE_FP_MANAGEMENT_MODE;

	sys_cmd = htons(sys_cmd);
	fp_build_cmd(req_buf, true, FP_CMD_SYS, &sys_cmd, sizeof(sys_cmd));
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
}

static void analyse_psi_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	uint16_t sys_cmd = FP_SYS_CMD_START_ANALYSE_PSI;

	sys_cmd = htons(sys_cmd);
	fp_build_cmd(req_buf, true, FP_CMD_SYS, &sys_cmd, sizeof(sys_cmd));
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
}

static void read_map_analyse_status_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	uint16_t sys_cmd = FP_SYS_CMD_READ_MAP_ANALYSE_STATUS;
	CHN_NUM_T chnum;

	sys_cmd = htons(sys_cmd);
	fp_build_cmd(req_buf, true, FP_CMD_SYS, &sys_cmd, sizeof(sys_cmd));
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
	if (!fp_validate_cmd(resp_buf, resp_len, sizeof(CHN_NUM_T))) {
		g_print("response cmd invalidate!\n");
		return;
	}
	memcpy(&chnum, &resp_buf[sizeof(struct fp_cmd_header)], sizeof(chnum));
	hex_dump("chan num", &chnum, sizeof(chnum));
}

static void get_ts_status_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	uint16_t sys_cmd = FP_SYS_CMD_READ_TS_STATUS;

	sys_cmd = htons(sys_cmd);
	fp_build_cmd(req_buf, true, FP_CMD_SYS, &sys_cmd, sizeof(sys_cmd));
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
	if (!fp_validate_cmd(resp_buf, resp_len, 1)) {
		g_print("response cmd invalidate!\n");
		return;
	}
	uint8_t ts_status = resp_buf[sizeof(struct fp_cmd_header)];
	g_print("ts status: %#x\n", ts_status);
}

static void apply_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	uint16_t sys_cmd = FP_SYS_CMD_APPLY_MAP_ANALYSE;

	sys_cmd = htons(sys_cmd);
	fp_build_cmd(req_buf, true, FP_CMD_SYS, &sys_cmd, sizeof(sys_cmd));
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
}

static void reset_net_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	uint16_t sys_cmd = FP_SYS_CMD_RESET_NET;

	sys_cmd = htons(sys_cmd);
	fp_build_cmd(req_buf, true, FP_CMD_SYS, &sys_cmd, sizeof(sys_cmd));
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
}

static void prog_0_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	static int cnt = 1;

	fp_build_cmd(req_buf, true, 0, NULL, 0);
	hex_dump("req", req_buf, 6);
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
	hex_dump("resp", resp_buf, resp_len);

	if (cnt % 2) {
		fp_select_program(0);
		fp_select_program(1);
		fp_select_program(2);
		fp_select_program(3);
	} else {
		fp_deselect_program(0);
		fp_deselect_program(1);
		fp_deselect_program(2);
		fp_deselect_program(3);
	}
	cnt++;
}

static PROG_INFO_T cur_prog, local_prog;
static void get_program_info_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	gint chan, prog, prog_idx;
	struct fp_cmd *cmd;

	// build program index
	chan = gtk_combo_box_get_active(chan_combo_box);
	prog = gtk_combo_box_get_active(prog_combo_box);
	prog_idx = chan * PROGRAM_MAX_NUM + prog;

	fp_build_cmd(req_buf, true, prog_idx, NULL, 0);
	hex_dump("req", req_buf, 6);
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
	hex_dump("resp", resp_buf, resp_len);

	// cache program info
	memcpy(&cur_prog, resp_buf + sizeof(struct fp_cmd_header), sizeof(cur_prog));

	char text_buf[10];
	buf_2_prog_info(&local_prog, &cur_prog);

	sprintf(text_buf, "%#x", local_prog.info.pmt.in);
	gtk_label_set_text(pmt_in_label, text_buf);
	sprintf(text_buf, "%#x", local_prog.info.pmt.out);
	gtk_entry_set_text(pmt_entry, text_buf);

	sprintf(text_buf, "%#x", local_prog.info.pcr.in);
	gtk_label_set_text(pcr_in_label, text_buf);
	sprintf(text_buf, "%#x", local_prog.info.pcr.out);
	gtk_entry_set_text(pcr_entry, text_buf);

	sprintf(text_buf, "%#x", local_prog.info.data[0].in);
	gtk_label_set_text(data_0_in_label, text_buf);
	sprintf(text_buf, "%#x", local_prog.info.data[0].out);
	gtk_entry_set_text(data_0_entry, text_buf);

	sprintf(text_buf, "%#x", local_prog.info.data[1].in);
	gtk_label_set_text(data_1_in_label, text_buf);
	sprintf(text_buf, "%#x", local_prog.info.data[1].out);
	gtk_entry_set_text(data_1_entry, text_buf);

	sprintf(text_buf, "%#x", local_prog.info.data[2].in);
	gtk_label_set_text(data_2_in_label, text_buf);
	sprintf(text_buf, "%#x", local_prog.info.data[2].out);
	gtk_entry_set_text(data_2_entry, text_buf);
}

static void select_program_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	gint chan, prog, prog_idx;

	chan = gtk_combo_box_get_active(chan_combo_box);
	prog = gtk_combo_box_get_active(prog_combo_box);
	prog_idx = chan * PROGRAM_MAX_NUM + prog;

	char *text = gtk_entry_get_text(pmt_entry);
	local_prog.info.pmt.out = strtol(text, NULL, 16);
	text = gtk_entry_get_text(pcr_entry);
	local_prog.info.pcr.out = strtol(text, NULL, 16);
	if (local_prog.info.data[0].type != PID_TYPE_PAD) {
		text = gtk_entry_get_text(data_0_entry);
		local_prog.info.data[0].out = strtol(text, NULL, 16);
	}
	if (local_prog.info.data[1].type != PID_TYPE_PAD) {
		text = gtk_entry_get_text(data_1_entry);
		local_prog.info.data[1].out = strtol(text, NULL, 16);
	}
	if (local_prog.info.data[2].type != PID_TYPE_PAD) {
		text = gtk_entry_get_text(data_2_entry);
		local_prog.info.data[2].out = strtol(text, NULL, 16);
	}
	buf_2_prog_info(&cur_prog, &local_prog);

	FP_SELECT_PROG(&cur_prog);
	fp_build_cmd(req_buf, false, prog_idx, &cur_prog, sizeof(cur_prog));
	hex_dump("req", req_buf, 16);
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
	hex_dump("resp", resp_buf, resp_len);
}

static void deselect_program_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	gint chan, prog, prog_idx;

	chan = gtk_combo_box_get_active(chan_combo_box);
	prog = gtk_combo_box_get_active(prog_combo_box);
	prog_idx = chan * PROGRAM_MAX_NUM + prog;

	FP_DESELECT_PROG(&cur_prog);
	fp_build_cmd(req_buf, false, prog_idx, &cur_prog, sizeof(cur_prog));
	hex_dump("req", req_buf, 16);
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
	hex_dump("resp", resp_buf, resp_len);
}

static void build_control_ui(GtkWidget *vbox)
{
	GtkWidget *btn, *radio1, *radio2;
	GtkWidget *hbox;
	GtkWidget *label;
	int i;

	hbox = gtk_hbox_new(TRUE, 2);
	/* FP_SYS_CMD_ENTER_FP_MANAGEMENT_MODE */
	btn = gtk_button_new_with_label("Enter Manage");
	gtk_signal_connect(GTK_OBJECT(btn), "button_press_event",
		GTK_SIGNAL_FUNC(enter_fp_management_mode_btn_press), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
	/* FP_SYS_CMD_LEAVE_FP_MANAGEMENT_MODE */
	btn = gtk_button_new_with_label("Leave Manage");
	gtk_signal_connect(GTK_OBJECT(btn), "button_press_event",
		GTK_SIGNAL_FUNC(leave_fp_management_mode_btn_press), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
	/* FP_SYS_CMD_START_ANALYSE_PSI */
	btn = gtk_button_new_with_label("Analyse PSI");
	gtk_signal_connect(GTK_OBJECT(btn), "button_press_event",
		GTK_SIGNAL_FUNC(analyse_psi_btn_press), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
	/* FP_SYS_CMD_READ_MAP_ANALYSE_STATUS */
	btn = gtk_button_new_with_label("Analyse Status");
	gtk_signal_connect(GTK_OBJECT(btn), "button_press_event",
		GTK_SIGNAL_FUNC(read_map_analyse_status_btn_press), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
	/* FP_SYS_CMD_READ_TS_STATUS */
	btn = gtk_button_new_with_label("Get TS Status");
	gtk_signal_connect(GTK_OBJECT(btn), "button_press_event",
		GTK_SIGNAL_FUNC(get_ts_status_btn_press), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
	/* FP_SYS_CMD_APPLY_MAP_ANALYSE */
	btn = gtk_button_new_with_label("Apply");
	gtk_signal_connect(GTK_OBJECT(btn), "button_press_event",
		GTK_SIGNAL_FUNC(apply_btn_press), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
	/* FP_SYS_CMD_RESET_NET */
	btn = gtk_button_new_with_label("Reset Net");
	gtk_signal_connect(GTK_OBJECT(btn), "button_press_event",
		GTK_SIGNAL_FUNC(reset_net_btn_press), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
	/* get program info */
	btn = gtk_button_new_with_label("Prog 0");
	gtk_signal_connect(GTK_OBJECT(btn), "button_press_event",
		GTK_SIGNAL_FUNC(prog_0_btn_press), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	/*
	 * next row hbox
	 */
	hbox = gtk_hbox_new(TRUE, 2);
	/* channel */
	chan_combo_box = gtk_combo_box_new_text();
	for (i = 0; i < CHANNEL_MAX_NUM; i++) {
		char s[10];
		sprintf(s, "%d", i);
		gtk_combo_box_append_text(chan_combo_box, s);
	}
	gtk_combo_box_set_active(chan_combo_box, 0);
	gtk_box_pack_start(GTK_BOX(hbox), chan_combo_box, FALSE, FALSE, 0);
	/* program */
	prog_combo_box = gtk_combo_box_new_text();
	for (i = 0; i < PROGRAM_MAX_NUM; i++) {
		char s[10];
		sprintf(s, "%d", i);
		gtk_combo_box_append_text(prog_combo_box, s);
	}
	gtk_combo_box_set_active(prog_combo_box, 0);
	gtk_box_pack_start(GTK_BOX(hbox), prog_combo_box, FALSE, FALSE, 0);
	/* get speciy program info */
	btn = gtk_button_new_with_label("Get Program Info");
	gtk_signal_connect(GTK_OBJECT(btn), "button_press_event",
		GTK_SIGNAL_FUNC(get_program_info_btn_press), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
	/* select current program */
	btn = gtk_button_new_with_label("Select Program");
	gtk_signal_connect(GTK_OBJECT(btn), "button_press_event",
		GTK_SIGNAL_FUNC(select_program_btn_press), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
	/* deselect current program */
	btn = gtk_button_new_with_label("Deselect Program");
	gtk_signal_connect(GTK_OBJECT(btn), "button_press_event",
		GTK_SIGNAL_FUNC(deselect_program_btn_press), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	hbox = gtk_hbox_new(TRUE, 2);
	label = gtk_label_new("PMT:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	pmt_in_label = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(hbox), pmt_in_label, FALSE, FALSE, 0);
	pmt_entry = gtk_entry_new();
	gtk_editable_set_editable(GTK_EDITABLE(pmt_entry), TRUE);
	gtk_entry_set_width_chars(GTK_ENTRY(pmt_entry), 8);
	gtk_box_pack_start(GTK_BOX(hbox), pmt_entry, FALSE, FALSE, 0);
	label = gtk_label_new("PCR:");
	pcr_in_label = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(hbox), pcr_in_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	pcr_entry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(pcr_entry), 8);
	gtk_box_pack_start(GTK_BOX(hbox), pcr_entry, FALSE, FALSE, 0);
	label = gtk_label_new("DATA 0:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	data_0_in_label = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(hbox), data_0_in_label, FALSE, FALSE, 0);
	data_0_entry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(data_0_entry), 8);
	gtk_box_pack_start(GTK_BOX(hbox), data_0_entry, FALSE, FALSE, 0);
	label = gtk_label_new("DATA 1:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	data_1_in_label = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(hbox), data_1_in_label, FALSE, FALSE, 0);
	data_1_entry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(data_1_entry), 8);
	gtk_box_pack_start(GTK_BOX(hbox), data_1_entry, FALSE, FALSE, 0);
	label = gtk_label_new("DATA 2:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	data_2_in_label = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(hbox), data_2_in_label, FALSE, FALSE, 0);
	data_2_entry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(data_2_entry), 8);
	gtk_box_pack_start(GTK_BOX(hbox), data_2_entry, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
}

static void fp_gtk_ui_create()
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *btnQuit;
	GdkFont *font;

	font = gdk_font_load("-misc-New-medium-r-normal--0-0-0-0-p-0-iso8859-1");
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_border_width(GTK_CONTAINER(window), 10);
	g_signal_connect(G_OBJECT(window), "delete_event",
		G_CALLBACK(window_delete_event), NULL);
	g_signal_connect(G_OBJECT(window), "destroy",
		G_CALLBACK(window_destroy), NULL);

	vbox = gtk_vbox_new(FALSE, 10);
	build_control_ui(vbox);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_window_set_title(GTK_WINDOW(window), "XMUX FP");
	/* show all */
	gtk_widget_show_all(window);
}

static int gtk_main_thread(void *data)
{
	gtk_main();
	g_print("sim_fp: main loop quit\n");

	return 0;
}

void sim_fp_run()
{
	g_print("sim_fp: run\n");
	g_thread_init(NULL);
	gtk_init(NULL, NULL);
	fp_gtk_ui_create();
	thread_create(gtk_main_thread, NULL);
}

