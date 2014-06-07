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

#include "../global.h"

#if GTK_MAJOR_VERSION == 3
#include <gtk/gtkx.h>
#endif

#include "../libtopmenu-client/topmenu-monitor.h"

#include "data.h"
#include "appmenu.h"

G_DEFINE_QUARK(topmenu-window-data, window_data)
G_DEFINE_QUARK(topmenu-menu-shell-data, menu_shell_data)
G_DEFINE_QUARK(topmenu-menu-item-data, menu_item_data)

gboolean
topmenu_is_blacklisted (void)
{
	return FALSE;
}

gboolean
topmenu_is_window_blacklisted (GtkWindow *window)
{
	if (gtk_window_get_window_type (window) != GTK_WINDOW_TOPLEVEL)
		return TRUE;

	if (GTK_IS_PLUG (window))
		return TRUE;

	return FALSE;
}

static WindowData *
window_data_new (void)
{
	return g_slice_new0 (WindowData);
}

static void
window_data_free (gpointer data)
{
	WindowData *window_data = data;

	if (window_data != NULL)
	{
		if (window_data->menus != NULL)
			g_slist_free_full (window_data->menus, g_object_unref);

		if (window_data->monitor_connection_id)
			g_signal_handler_disconnect(topmenu_monitor_get_instance(),
			                            window_data->monitor_connection_id);

		if (window_data->appmenu.menu)
			topmenu_appmenu_destroy(&window_data->appmenu);

		if (window_data->appmenubar)
			gtk_widget_destroy(GTK_WIDGET(window_data->appmenubar));

		g_slice_free (WindowData, window_data);
	}
}

static MenuShellData *
menu_shell_data_new (void)
{
	return g_slice_new0 (MenuShellData);
}

static void
menu_shell_data_free (gpointer data)
{
	if (data != NULL)
		g_slice_free (MenuShellData, data);
}

static MenuItemData *
menu_item_data_new (void)
{
	return g_slice_new0 (MenuItemData);
}

static void
menu_item_data_free (gpointer data)
{
	if (data != NULL) {
		MenuItemData *item_data = data;

		if (item_data->proxy)
			gtk_widget_destroy (GTK_WIDGET (item_data->proxy));

		g_slice_free (MenuItemData, data);
	}
}

MenuItemData *
topmenu_get_menu_item_data (GtkMenuItem *menu_item)
{
	MenuItemData *menu_item_data;

	g_return_val_if_fail (GTK_IS_MENU_ITEM (menu_item), NULL);

	menu_item_data = g_object_get_qdata (G_OBJECT (menu_item), menu_item_data_quark ());

	if (menu_item_data == NULL)
	{
		menu_item_data = menu_item_data_new ();

		g_object_set_qdata_full (G_OBJECT (menu_item), menu_item_data_quark (), menu_item_data, menu_item_data_free);
	}

	return menu_item_data;
}

MenuShellData *
topmenu_get_menu_shell_data (GtkMenuShell *menu_shell)
{
	MenuShellData *menu_shell_data;

	g_return_val_if_fail (GTK_IS_MENU_SHELL (menu_shell), NULL);

	menu_shell_data = g_object_get_qdata (G_OBJECT (menu_shell), menu_shell_data_quark ());

	if (menu_shell_data == NULL)
	{
		menu_shell_data = menu_shell_data_new ();

		g_object_set_qdata_full (G_OBJECT (menu_shell), menu_shell_data_quark (), menu_shell_data, menu_shell_data_free);
	}

	return menu_shell_data;
}

WindowData *
topmenu_get_window_data (GtkWindow *window)
{
	WindowData *window_data;

	g_return_val_if_fail (GTK_IS_WINDOW (window), NULL);

	window_data = g_object_get_qdata (G_OBJECT (window), window_data_quark ());

	if (window_data == NULL)
	{
		if (topmenu_is_window_blacklisted (window))
			return NULL;

		window_data = window_data_new ();

		// Nothing to initialize right now.

		g_object_set_qdata_full (G_OBJECT (window), window_data_quark (), window_data, window_data_free);
	}

	return window_data;
}

void
topmenu_remove_window_data (GtkWindow *window)
{
	g_object_set_qdata (G_OBJECT (window), window_data_quark (), NULL);
}
