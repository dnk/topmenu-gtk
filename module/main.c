/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Ryan Lortie <desrt@desrt.ca>
 *          William Hua <william.hua@canonical.com>
 */

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include "../global.h"
#include "../libtopmenu-client/topmenu-client.h"
#include "../libtopmenu-client/topmenu-monitor.h"

#include "menuitem-proxy.h"
#include "appmenu.h"
#include "data.h"

static gboolean already_initialized = FALSE;

static void (* pre_hijacked_window_realize)                          (GtkWidget      *widget);

static void (* pre_hijacked_window_unrealize)                        (GtkWidget      *widget);

#if GTK_MAJOR_VERSION == 3
static void (* pre_hijacked_application_window_realize)              (GtkWidget      *widget);
#endif

static void (* pre_hijacked_menu_bar_realize)                        (GtkWidget      *widget);

static void (* pre_hijacked_menu_bar_unrealize)                      (GtkWidget      *widget);

static void (* pre_hijacked_widget_size_allocate)                    (GtkWidget      *widget,
                                                                      GtkAllocation  *allocation);

static void (* pre_hijacked_menu_bar_size_allocate)                  (GtkWidget      *widget,
                                                                      GtkAllocation  *allocation);

#if GTK_MAJOR_VERSION == 2
static void (* pre_hijacked_menu_bar_size_request)                   (GtkWidget      *widget,
                                                                      GtkRequisition *requisition);
#elif GTK_MAJOR_VERSION == 3
static void (* pre_hijacked_menu_bar_get_preferred_width)            (GtkWidget      *widget,
                                                                      gint           *minimum_width,
                                                                      gint           *natural_width);

static void (* pre_hijacked_menu_bar_get_preferred_height)           (GtkWidget      *widget,
                                                                      gint           *minimum_height,
                                                                      gint           *natural_height);

static void (* pre_hijacked_menu_bar_get_preferred_width_for_height) (GtkWidget      *widget,
                                                                      gint            height,
                                                                      gint           *minimum_width,
                                                                      gint           *natural_width);

static void (* pre_hijacked_menu_bar_get_preferred_height_for_width) (GtkWidget      *widget,
                                                                      gint            width,
                                                                      gint           *minimum_height,
                                                                      gint           *natural_height);
#endif

static void
handle_should_hide_menubar_updated (GObject    *object,
                                    GParamSpec *pspec,
                                    gpointer    user_data);

static void
count_container_items_helper (GtkWidget *widget, gpointer data)
{
	gint *count = data;
	(*count)++;
}

static gint
count_container_items (GtkContainer *container)
{
	gint count = 0;
	gtk_container_foreach (container, count_container_items_helper, &count);
	return count;
}




static gboolean
topmenu_should_hide_menubar_on_window (GtkWindow *window)
{
	TopMenuMonitor *monitor = topmenu_monitor_get_instance ();

	if (topmenu_is_window_blacklisted (window))
		return FALSE;

	return monitor->available;
}

static gboolean
topmenu_should_hide_menubar (GtkWidget *widget)
{
	GtkWindow *window = GTK_WINDOW (gtk_widget_get_toplevel (widget));

	g_return_val_if_fail (GTK_IS_MENU_SHELL (widget), FALSE);

	return topmenu_should_hide_menubar_on_window (window);
}

static void
topmenu_prepare_window (GtkWindow *window)
{
	WindowData *window_data;

	g_return_if_fail (GTK_IS_WINDOW (window));
	g_return_if_fail (gtk_widget_get_realized (GTK_WIDGET (window)));

	window_data = topmenu_get_window_data (window);

	if (window_data == NULL)
		return; // Window is ignored

	if (window_data->appmenubar != 0)
		return; // Already prepared

	window_data->appmenubar = TOPMENU_APP_MENU_BAR (topmenu_app_menu_bar_new ());
	gtk_widget_show(GTK_WIDGET(window_data->appmenubar));

	topmenu_app_menu_bar_set_app_menu (window_data->appmenubar,
	                                   topmenu_appmenu_build(&window_data->appmenu));
}

static void
topmenu_connect_window (GtkWindow *window)
{
	WindowData *window_data;

	g_return_if_fail (GTK_IS_WINDOW (window));
	g_return_if_fail (gtk_widget_get_realized (GTK_WIDGET (window)));

	window_data = topmenu_get_window_data (window);

	if (window_data == NULL)
		return; // Window is ignored

	if (window_data->monitor_connection_id != 0)
		return; // Already connected

	g_return_if_fail (window_data->menus != NULL); // Must contain one menu at least.
	g_return_if_fail (window_data->appmenubar != NULL); // Must be prepared

	TopMenuMonitor *monitor = topmenu_monitor_get_instance();
	window_data->monitor_connection_id = g_signal_connect(monitor, "notify::available",
	                                                      G_CALLBACK (handle_should_hide_menubar_updated), window);

	topmenu_client_connect_window_widget (gtk_widget_get_window (GTK_WIDGET (window)),
	                                      GTK_WIDGET (window_data->appmenubar));
}

static void
topmenu_disconnect_window (GtkWindow *window)
{
	WindowData *window_data;

	g_return_if_fail (GTK_IS_WINDOW (window));

	window_data = topmenu_get_window_data (window);

	if (window_data == NULL)
		return; // Already disconnected or ignored

	if (window_data->monitor_connection_id == 0)
		return; // Already disconnected

	TopMenuMonitor *monitor = topmenu_monitor_get_instance();
	g_signal_handler_disconnect(monitor, window_data->monitor_connection_id);
	window_data->monitor_connection_id = 0;

	if (window_data->appmenu.menu)
		topmenu_appmenu_destroy(&window_data->appmenu);

	if (window_data->appmenubar)
	{
		gtk_widget_destroy (GTK_WIDGET (window_data->appmenubar));
		window_data->appmenubar = NULL;
	}

	if (gtk_widget_get_realized (GTK_WIDGET(window)))
	{
		topmenu_client_disconnect_window (gtk_widget_get_window (GTK_WIDGET (window)));
	}
}

static gint
compute_shell_position_in_appmenu (WindowData *window_data, GtkMenuShell *menu_shell)
{
	GSList *iter;
	gint position = 1; // Skip app_menu_item

	for (iter = window_data->menus; iter; iter = g_slist_next (iter))
	{
		if (iter->data == menu_shell) {
			return position;
		}

		position += count_container_items (GTK_CONTAINER (iter->data));
	}

	return -1;
}

static void
add_menu_item_to_appmenu (WindowData *window_data, GtkMenuItem *item, gint position)
{
	MenuItemData *item_data = topmenu_get_menu_item_data (item);
	item_data->proxy = topmenu_create_proxy_menu_item (item);

	gtk_menu_shell_insert (GTK_MENU_SHELL (window_data->appmenubar),
	                       GTK_WIDGET (item_data->proxy), position);
}

static void
remove_menu_item_from_appmenu (WindowData *window_data, GtkMenuItem *item)
{
	MenuItemData *item_data = topmenu_get_menu_item_data (item);

	if (item_data->proxy) {
		gtk_widget_destroy (GTK_WIDGET (item_data->proxy));
		item_data->proxy = NULL;
	}
}

static void
handle_shell_insert (GtkMenuShell *menu_shell, GtkWidget *child, gint position, WindowData *window_data)
{
	GtkMenuItem *item = GTK_MENU_ITEM (child);
	gint offset = compute_shell_position_in_appmenu (window_data, menu_shell);
	g_return_if_fail (offset >= 0);

	add_menu_item_to_appmenu (window_data, item, offset + position);
}

static void
handle_shell_remove (GtkMenuShell *menu_shell, GtkWidget *widget, WindowData *window_data)
{
	GtkMenuItem *item = GTK_MENU_ITEM (widget);
	remove_menu_item_from_appmenu (window_data, item);
}

typedef struct _AddShellCbData
{
	WindowData *window_data;
	gint position;
} AddShellCbData;

static void
add_shell_cb (GtkWidget *widget, gpointer user_data)
{
	AddShellCbData *data = user_data;
	GtkMenuItem *item = GTK_MENU_ITEM (widget);
	add_menu_item_to_appmenu (data->window_data, item, data->position);

	data->position++;
}

static void
add_shell_to_appmenu (WindowData *window_data, GtkMenuShell *menu_shell)
{
	AddShellCbData data;
	data.window_data = window_data;
	data.position = compute_shell_position_in_appmenu (window_data, menu_shell);

	g_warn_if_fail (data.position >= 0);

	gtk_container_foreach (GTK_CONTAINER (menu_shell),
	                       add_shell_cb, &data);

	topmenu_appmenu_scan_for_items (&window_data->appmenu, menu_shell);

	g_signal_connect (menu_shell, "insert",
	                  G_CALLBACK (handle_shell_insert), window_data);
	g_signal_connect (menu_shell, "remove",
	                  G_CALLBACK (handle_shell_remove), window_data);
}

static void
remove_shell_cb (GtkWidget *widget, gpointer user_data)
{
	WindowData *window_data = user_data;
	GtkMenuItem *item = GTK_MENU_ITEM (widget);
	remove_menu_item_from_appmenu (window_data, item);
}

static void
remove_shell_from_appmenu (WindowData *window_data, GtkMenuShell *menu_shell)
{
	gtk_container_foreach (GTK_CONTAINER (menu_shell),
	                       remove_shell_cb, window_data);

	g_signal_handlers_disconnect_by_data (menu_shell, window_data);
}

static void
topmenu_disconnect_menu_shell (GtkWindow    *window,
                               GtkMenuShell *menu_shell)
{
	WindowData *window_data;
	MenuShellData *menu_shell_data;

	g_return_if_fail (GTK_IS_WINDOW (window));
	g_return_if_fail (GTK_IS_MENU_SHELL (menu_shell));

	menu_shell_data = topmenu_get_menu_shell_data (menu_shell);

	g_warn_if_fail (window == menu_shell_data->window);

	window_data = topmenu_get_window_data (menu_shell_data->window);

	if (window_data != NULL)
	{
		GSList *iter;

		for (iter = window_data->menus; iter != NULL; iter = g_slist_next (iter))
			if (GTK_MENU_SHELL(iter->data) == menu_shell)
				break;

		if (iter != NULL)
		{
			GtkMenuShell *menu_shell = GTK_MENU_SHELL(iter->data);

			remove_shell_from_appmenu (window_data, menu_shell);

			window_data->menus = g_slist_delete_link (window_data->menus, iter);
		}
	}

	menu_shell_data->window = NULL;
}

static void
topmenu_connect_menu_shell (GtkWindow    *window,
                            GtkMenuShell *menu_shell)
{
	MenuShellData *menu_shell_data;

	g_return_if_fail (GTK_IS_WINDOW (window));
	g_return_if_fail (GTK_IS_MENU_SHELL (menu_shell));

	menu_shell_data = topmenu_get_menu_shell_data (menu_shell);

	if (window != menu_shell_data->window)
	{
		WindowData *window_data;

		if (menu_shell_data->window != NULL)
			topmenu_disconnect_menu_shell (menu_shell_data->window, menu_shell);

		window_data = topmenu_get_window_data (window);

		if (window_data != NULL)
		{
			GSList *iter;

			for (iter = window_data->menus; iter != NULL; iter = g_slist_next (iter))
				if (GTK_MENU_SHELL(iter->data) == menu_shell)
					break;

			if (iter == NULL)
			{
				topmenu_prepare_window (window);

				window_data->menus = g_slist_append (window_data->menus, menu_shell);

				add_shell_to_appmenu (window_data, menu_shell);

				topmenu_connect_window (window); // Does nothing if already connected
			}
		}

		menu_shell_data->window = window;
	}
}

static void
handle_should_hide_menubar_updated (GObject    *object,
                                    GParamSpec *pspec,
                                    gpointer    user_data)
{
	g_return_if_fail (GTK_IS_WINDOW (user_data));

	GtkWindow *window = GTK_WINDOW (user_data);
	WindowData *window_data = topmenu_get_window_data (window);
	GSList *iter;

	for (iter = window_data->menus; iter != NULL; iter = g_slist_next (iter))
	{
		gtk_widget_queue_resize (GTK_WIDGET (iter->data));
	}
}

static void
hijacked_window_realize (GtkWidget *widget)
{
	g_return_if_fail (GTK_IS_WINDOW (widget));

	if (pre_hijacked_window_realize != NULL)
		(* pre_hijacked_window_realize) (widget);

#if GTK_MAJOR_VERSION == 3
	if (!GTK_IS_APPLICATION_WINDOW (widget))
#endif
		topmenu_get_window_data (GTK_WINDOW (widget));
}

static void
hijacked_window_unrealize (GtkWidget *widget)
{
	g_return_if_fail (GTK_IS_WINDOW (widget));

	if (pre_hijacked_window_unrealize != NULL)
		(* pre_hijacked_window_unrealize) (widget);

	topmenu_disconnect_window (GTK_WINDOW (widget));
	topmenu_remove_window_data (GTK_WINDOW (widget));
}

#if GTK_MAJOR_VERSION == 3
static void
hijacked_application_window_realize (GtkWidget *widget)
{
	g_return_if_fail (GTK_IS_APPLICATION_WINDOW (widget));

	if (pre_hijacked_application_window_realize != NULL)
		(* pre_hijacked_application_window_realize) (widget);

	topmenu_get_window_data (GTK_WINDOW (widget));
}
#endif

static void
hijacked_menu_bar_realize (GtkWidget *widget)
{
	GtkWidget *window;

	g_return_if_fail (GTK_IS_MENU_BAR (widget));

	if (pre_hijacked_menu_bar_realize != NULL)
		(* pre_hijacked_menu_bar_realize) (widget);

	window = gtk_widget_get_toplevel (widget);

	if (GTK_IS_WINDOW (window))
		topmenu_connect_menu_shell (GTK_WINDOW (window), GTK_MENU_SHELL (widget));
}

static void
hijacked_menu_bar_unrealize (GtkWidget *widget)
{
	MenuShellData *menu_shell_data;

	g_return_if_fail (GTK_IS_MENU_BAR (widget));

	if (pre_hijacked_menu_bar_unrealize != NULL)
		(* pre_hijacked_menu_bar_unrealize) (widget);

	menu_shell_data = topmenu_get_menu_shell_data (GTK_MENU_SHELL (widget));

	if (menu_shell_data->window != NULL)
		topmenu_disconnect_menu_shell (menu_shell_data->window, GTK_MENU_SHELL (widget));
}

static void
hijacked_menu_bar_size_allocate (GtkWidget     *widget,
                                 GtkAllocation *allocation)
{
	GtkAllocation zero = { 0, 0, 0, 0 };
	GdkWindow *window;

	g_return_if_fail (GTK_IS_MENU_BAR (widget));

	if (topmenu_should_hide_menubar (widget))
	{
		/*
	   * We manually assign an empty allocation to the menu bar to
	   * prevent the container from attempting to draw it at all.
	   */
		if (pre_hijacked_widget_size_allocate != NULL)
			(* pre_hijacked_widget_size_allocate) (widget, &zero);

		/*
	   * Then we move the GdkWindow belonging to the menu bar outside of
	   * the clipping rectangle of the parent window so that we can't
	   * see it.
	   */
		window = gtk_widget_get_window (widget);

		if (window != NULL)
			gdk_window_move_resize (window, -1, -1, 1, 1);
	}
	else if (pre_hijacked_menu_bar_size_allocate != NULL)
		(* pre_hijacked_menu_bar_size_allocate) (widget, allocation);
}

#if GTK_MAJOR_VERSION == 2
static void
hijacked_menu_bar_size_request (GtkWidget      *widget,
                                GtkRequisition *requisition)
{
	g_return_if_fail (GTK_IS_MENU_BAR (widget));

	if (pre_hijacked_menu_bar_size_request != NULL)
		(* pre_hijacked_menu_bar_size_request) (widget, requisition);

	if (topmenu_should_hide_menubar(widget))
	{
		requisition->width = 0;
		requisition->height = 0;
	}
}
#elif GTK_MAJOR_VERSION == 3
static void
hijacked_menu_bar_get_preferred_width (GtkWidget *widget,
                                       gint      *minimum_width,
                                       gint      *natural_width)
{
	g_return_if_fail (GTK_IS_MENU_BAR (widget));

	if (pre_hijacked_menu_bar_get_preferred_width != NULL)
		(* pre_hijacked_menu_bar_get_preferred_width) (widget, minimum_width, natural_width);

	if (topmenu_should_hide_menubar(widget))
	{
		*minimum_width = 0;
		*natural_width = 0;
	}
}

static void
hijacked_menu_bar_get_preferred_height (GtkWidget *widget,
                                        gint      *minimum_height,
                                        gint      *natural_height)
{
	g_return_if_fail (GTK_IS_MENU_BAR (widget));

	if (pre_hijacked_menu_bar_get_preferred_height != NULL)
		(* pre_hijacked_menu_bar_get_preferred_height) (widget, minimum_height, natural_height);

	if (topmenu_should_hide_menubar(widget))
	{
		*minimum_height = 0;
		*natural_height = 0;
	}
}

static void
hijacked_menu_bar_get_preferred_width_for_height (GtkWidget *widget,
                                                  gint       height,
                                                  gint      *minimum_width,
                                                  gint      *natural_width)
{
	g_return_if_fail (GTK_IS_MENU_BAR (widget));

	if (pre_hijacked_menu_bar_get_preferred_width_for_height != NULL)
		(* pre_hijacked_menu_bar_get_preferred_width_for_height) (widget, height, minimum_width, natural_width);

	if (topmenu_should_hide_menubar(widget))
	{
		*minimum_width = 0;
		*natural_width = 0;
	}
}

static void
hijacked_menu_bar_get_preferred_height_for_width (GtkWidget *widget,
                                                  gint       width,
                                                  gint      *minimum_height,
                                                  gint      *natural_height)
{
	g_return_if_fail (GTK_IS_MENU_BAR (widget));

	if (pre_hijacked_menu_bar_get_preferred_height_for_width != NULL)
		(* pre_hijacked_menu_bar_get_preferred_height_for_width) (widget, width, minimum_height, natural_height);

	if (topmenu_should_hide_menubar(widget))
	{
		*minimum_height = 0;
		*natural_height = 0;
	}
}
#endif

static void
hijack_window_class_vtable (GType type)
{
	GtkWidgetClass *widget_class = g_type_class_ref (type);
	GType *children;
	guint n;
	guint i;

	if (widget_class->realize == pre_hijacked_window_realize)
		widget_class->realize = hijacked_window_realize;

#if GTK_MAJOR_VERSION == 3
	if (widget_class->realize == pre_hijacked_application_window_realize)
		widget_class->realize = hijacked_application_window_realize;
#endif

	if (widget_class->unrealize == pre_hijacked_window_unrealize)
		widget_class->unrealize = hijacked_window_unrealize;

	children = g_type_children (type, &n);

	for (i = 0; i < n; i++)
		hijack_window_class_vtable (children[i]);

	g_free (children);
}

static void
hijack_menu_bar_class_vtable (GType type)
{
	GtkWidgetClass *widget_class = g_type_class_ref (type);
	GType *children;
	guint n;
	guint i;

	/* This fixes lp:1113008. */
	widget_class->hierarchy_changed = NULL;

	if (widget_class->realize == pre_hijacked_menu_bar_realize)
		widget_class->realize = hijacked_menu_bar_realize;

	if (widget_class->unrealize == pre_hijacked_menu_bar_unrealize)
		widget_class->unrealize = hijacked_menu_bar_unrealize;

	if (widget_class->size_allocate == pre_hijacked_menu_bar_size_allocate)
		widget_class->size_allocate = hijacked_menu_bar_size_allocate;

#if GTK_MAJOR_VERSION == 2
	if (widget_class->size_request == pre_hijacked_menu_bar_size_request)
		widget_class->size_request = hijacked_menu_bar_size_request;
#elif GTK_MAJOR_VERSION == 3
	if (widget_class->get_preferred_width == pre_hijacked_menu_bar_get_preferred_width)
		widget_class->get_preferred_width = hijacked_menu_bar_get_preferred_width;

	if (widget_class->get_preferred_height == pre_hijacked_menu_bar_get_preferred_height)
		widget_class->get_preferred_height = hijacked_menu_bar_get_preferred_height;

	if (widget_class->get_preferred_width_for_height == pre_hijacked_menu_bar_get_preferred_width_for_height)
		widget_class->get_preferred_width_for_height = hijacked_menu_bar_get_preferred_width_for_height;

	if (widget_class->get_preferred_height_for_width == pre_hijacked_menu_bar_get_preferred_height_for_width)
		widget_class->get_preferred_height_for_width = hijacked_menu_bar_get_preferred_height_for_width;
#endif

	children = g_type_children (type, &n);

	for (i = 0; i < n; i++)
		hijack_menu_bar_class_vtable (children[i]);

	g_free (children);
}

G_MODULE_EXPORT
void gtk_module_init(void)
{
	if (!topmenu_is_blacklisted())
	{
		GtkWidgetClass *widget_class;

		/* gtk_module_init may be called more than once in a resident module */
		g_return_if_fail(!already_initialized);
		already_initialized = TRUE;

		/* store the base GtkWidget size_allocate vfunc */
		widget_class = g_type_class_ref (GTK_TYPE_WIDGET);
		pre_hijacked_widget_size_allocate = widget_class->size_allocate;

#if GTK_MAJOR_VERSION == 3
		/* store the base GtkApplicationWindow realize vfunc */
		widget_class = g_type_class_ref (GTK_TYPE_APPLICATION_WINDOW);
		pre_hijacked_application_window_realize = widget_class->realize;
#endif

		/* intercept window realize vcalls on GtkWindow */
		widget_class = g_type_class_ref (GTK_TYPE_WINDOW);
		pre_hijacked_window_realize = widget_class->realize;
		pre_hijacked_window_unrealize = widget_class->unrealize;
		hijack_window_class_vtable (GTK_TYPE_WINDOW);

		/* intercept size request and allocate vcalls on GtkMenuBar (for hiding) */
		widget_class = g_type_class_ref (GTK_TYPE_MENU_BAR);
		pre_hijacked_menu_bar_realize = widget_class->realize;
		pre_hijacked_menu_bar_unrealize = widget_class->unrealize;
		pre_hijacked_menu_bar_size_allocate = widget_class->size_allocate;
#if GTK_MAJOR_VERSION == 2
		pre_hijacked_menu_bar_size_request = widget_class->size_request;
#elif GTK_MAJOR_VERSION == 3
		pre_hijacked_menu_bar_get_preferred_width = widget_class->get_preferred_width;
		pre_hijacked_menu_bar_get_preferred_height = widget_class->get_preferred_height;
		pre_hijacked_menu_bar_get_preferred_width_for_height = widget_class->get_preferred_width_for_height;
		pre_hijacked_menu_bar_get_preferred_height_for_width = widget_class->get_preferred_height_for_width;
#endif
		hijack_menu_bar_class_vtable (GTK_TYPE_MENU_BAR);
	}
}

G_MODULE_EXPORT
const gchar * g_module_check_init(GModule *module)
{
	/* It is hard to unhijack Gtk's vtables, specially if some other module
	 * has decided to also hijack them.
	 * Thus, we just prevent unloading of this module. */
	g_module_make_resident(module);
	return NULL;
}
