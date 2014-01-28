#include <stdlib.h>

#include <gtk/gtk.h>

#include "../libtopmenu-server/topmenu-widget.h"

static GtkWindow *mainwin;
static TopMenuWidget *topmenu;

static gboolean handle_button_press(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	g_debug("Mainwin: button press");
	return FALSE;
}

static void construct_main_window()
{
	mainwin = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	topmenu = TOPMENU_WIDGET(topmenu_widget_new());
	gtk_container_add(GTK_CONTAINER(mainwin), GTK_WIDGET(topmenu));
}

int main(int argc, char **argv)
{
	gtk_set_locale();
	gtk_init(&argc, &argv);

	construct_main_window();

	g_signal_connect(mainwin, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(mainwin, "button-press-event", G_CALLBACK(handle_button_press), NULL);

	gtk_window_set_keep_above(mainwin, TRUE);
	gtk_window_set_accept_focus(mainwin, FALSE);
	gtk_widget_show_all(GTK_WIDGET(mainwin));

	gtk_main();

	return EXIT_SUCCESS;
}
