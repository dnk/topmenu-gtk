/*
 * Copyright 2014 Javier S. Pedro <maemo@javispedro.com>
 *
 * This file is part of TopMenu.
 *
 * TopMenu is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TopMenu is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TopMenu.  If not, see <http://www.gnu.org/licenses/>.
 */

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
