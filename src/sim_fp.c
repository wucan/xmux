#include <gtk/gtk.h>
#include <glib.h>
#include <gdk/gdktypes.h>


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

static void left_key_btn_press(GtkWidget *widget,
		GdkEventButton *event, gpointer *user_data)
{
}

static void build_control_ui(GtkWidget *vbox)
{
	GtkWidget *btn, *radio1, *radio2;
	GtkWidget *hbox;

	hbox = gtk_hbox_new(TRUE, 2);
	btn = gtk_button_new_with_label("Left");
	gtk_signal_connect(GTK_OBJECT(btn), "button_press_event",
		GTK_SIGNAL_FUNC(left_key_btn_press), NULL);
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

