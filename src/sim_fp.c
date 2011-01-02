#include <gtk/gtk.h>
#include <glib.h>
#include <gdk/gdktypes.h>

#include "wu/wu_byte_stream.h"
#include "wu/wu_csc.h"

#include "front_panel.h"
#include "front_panel_intstr.h"
#include "front_panel_define.h"


static uint8_t req_buf[1024], resp_buf[1024];
static uint16_t resp_len;

static gboolean window_delete_event(GtkWidget *widget,
		GdkEvent *event, gpointer data)
{
	return FALSE;
}

static void window_destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

static void window_key_press_event(GtkWidget *w, GdkEventKey *event)
{
	guint keyval = event->keyval;
}

static void enter_fp_management_mode_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	uint16_t sys_cmd = FP_SYS_CMD_ENTER_FP_MANAGEMENT_MODE;

	sys_cmd = htons(sys_cmd);
	fp_build_cmd(req_buf, true, 0x103, &sys_cmd, sizeof(sys_cmd));
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
}

static void leave_fp_management_mode_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	uint16_t sys_cmd = FP_SYS_CMD_LEAVE_FP_MANAGEMENT_MODE;

	sys_cmd = htons(sys_cmd);
	fp_build_cmd(req_buf, true, 0x103, &sys_cmd, sizeof(sys_cmd));
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
}

static void analyse_psi_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	uint16_t sys_cmd = FP_SYS_CMD_START_ANALYSE_PSI;

	sys_cmd = htons(sys_cmd);
	fp_build_cmd(req_buf, true, 0x103, &sys_cmd, sizeof(sys_cmd));
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
}

static void read_map_analyse_status_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	uint16_t sys_cmd = FP_SYS_CMD_READ_MAP_ANALYSE_STATUS;
	CHN_NUM_T chnum;

	sys_cmd = htons(sys_cmd);
	fp_build_cmd(req_buf, true, 0x103, &sys_cmd, sizeof(sys_cmd));
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
	fp_build_cmd(req_buf, true, 0x103, &sys_cmd, sizeof(sys_cmd));
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
	fp_build_cmd(req_buf, true, 0x103, &sys_cmd, sizeof(sys_cmd));
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
}

static void reset_net_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	uint16_t sys_cmd = FP_SYS_CMD_RESET_NET;

	sys_cmd = htons(sys_cmd);
	fp_build_cmd(req_buf, true, 0x103, &sys_cmd, sizeof(sys_cmd));
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
}

static void prog_0_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
	fp_build_cmd(req_buf, true, 0, NULL, 0);
	hex_dump("req", req_buf, 6);
	__parse_mcu_cmd(req_buf, resp_buf, &resp_len);
	hex_dump("resp", resp_buf, resp_len);
}

static void build_control_ui(GtkWidget *vbox)
{
	GtkWidget *btn, *radio1, *radio2;
	GtkWidget *hbox;

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
	/* set the event */
	gtk_signal_connect(GTK_OBJECT(window), "key_press_event",
		GTK_SIGNAL_FUNC(window_key_press_event), NULL);
	/* show all */
	gtk_widget_show_all(window);
}

void sim_fp_run()
{
	g_print("sim_fp: run\n");
	g_thread_init(NULL);
	gtk_init(NULL, NULL);
	fp_gtk_ui_create();
	gtk_main();
	g_print("sim_fp: main loop quit\n");
}

