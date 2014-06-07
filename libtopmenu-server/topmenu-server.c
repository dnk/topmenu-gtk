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

#include <gtk/gtk.h>

#include "topmenu-server.h"

#include "../global.h"

static GdkAtom selection_atom = GDK_NONE;
static GtkClipboard *selection_clipboard = NULL;
static GList *server_widgets = NULL;

static void handle_selection_owner_change(GtkClipboard *clipboard, GdkEvent *event, gpointer user_data);

static void init_selection_monitor()
{
	if (!selection_clipboard || selection_atom == GDK_NONE) {
		selection_atom = gdk_atom_intern_static_string(ATOM_TOPMENU_SERVER_SELECTION);
		selection_clipboard = gtk_clipboard_get(selection_atom);
		// Used to monitor the current owner of the server selection
		g_signal_connect(selection_clipboard, "owner-change",
		                 G_CALLBACK(handle_selection_owner_change), NULL);
	}
}

/** Returns the current stub this process wants to set as owner of the server_selection. */
static GdkWindow *get_front_server_stub()
{
	if (server_widgets) {
		GtkWidget *widget = server_widgets->data;
		gpointer data = g_object_get_data(G_OBJECT(widget), OBJECT_DATA_KEY_SERVER_STUB);
		g_return_val_if_fail(data, NULL);
		return GDK_WINDOW(data);
	} else {
		return NULL;
	}
}

static void update_selection_owner(guint32 time)
{
	GdkWindow *our_owner = get_front_server_stub();
	if (our_owner == NULL) return; // Nothing to do

	GdkWindow *cur_owner = gdk_selection_owner_get(selection_atom);
	if (cur_owner != our_owner) {
		g_debug("Setting this process as owner of the selection");
		int res = gdk_selection_owner_set(our_owner, selection_atom, time, TRUE);
		g_debug("Result = %d", res);
	}
}

static void handle_selection_owner_change(GtkClipboard *clipboard, GdkEvent *event, gpointer user_data)
{
	update_selection_owner(event->selection.time);
}

void topmenu_server_register_server_widget(GtkWidget *widget)
{
	GdkWindow *window = gtk_widget_get_window(widget);
	g_return_if_fail(window);

	init_selection_monitor();

	g_return_if_fail(g_object_get_data(G_OBJECT(widget), OBJECT_DATA_KEY_SERVER_STUB) == NULL);

	GdkWindowAttr stub_attr = { 0 };
	stub_attr.wclass = GDK_INPUT_ONLY;
	stub_attr.override_redirect = TRUE;
	GdkWindow *stub = gdk_window_new(window, &stub_attr, GDK_WA_NOREDIR);

	g_object_set_data_full(G_OBJECT(widget), OBJECT_DATA_KEY_SERVER_STUB, stub,
	                       (GDestroyNotify) &gdk_window_destroy);

	server_widgets = g_list_prepend(server_widgets, widget);
	update_selection_owner(GDK_CURRENT_TIME);
}

void topmenu_server_unregister_server_widget(GtkWidget *widget)
{
	g_return_if_fail(g_object_get_data(G_OBJECT(widget), OBJECT_DATA_KEY_SERVER_STUB) != NULL);
	server_widgets = g_list_remove_all(server_widgets, widget);
	gpointer data = g_object_steal_data(G_OBJECT(widget), OBJECT_DATA_KEY_SERVER_STUB);
	GdkWindow *stub = GDK_WINDOW(data);
	gdk_window_destroy(stub);
}
