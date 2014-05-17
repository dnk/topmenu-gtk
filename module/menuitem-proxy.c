/* Contains code from GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "../libtopmenu-client/topmenu-monitor.h"

#include "menuitem-proxy.h"

static gboolean static_data_ok = FALSE;
static const gchar *pname_visible;
static const gchar *pname_sensitive;
static const gchar *pname_label;
static const gchar *pname_submenu;

static void init_static_data()
{
	if (!static_data_ok) {
		pname_visible = g_intern_string("visible");
		pname_sensitive = g_intern_string("sensitive");
		pname_label = g_intern_string("label");
		pname_submenu = g_intern_string("submenu");

		static_data_ok = TRUE;
	}
}

#if GTK_MAJOR_VERSION == 3
/* What could possibly go wrong? */
typedef enum
{
  GTK_DIRECTION_LEFT,
  GTK_DIRECTION_RIGHT
} GtkSubmenuDirection;

typedef enum
{
  GTK_TOP_BOTTOM,
  GTK_LEFT_RIGHT
} GtkSubmenuPlacement;

struct _GtkMenuPrivate
{
	GtkWidget *parent_menu_item;
	GtkWidget *old_active_menu_item;

	GtkAccelGroup *accel_group;
	gchar         *accel_path;

	GtkMenuPositionFunc position_func;
	gpointer            position_func_data;
	GDestroyNotify      position_func_data_destroy;
	gint                position_x;
	gint                position_y;

	guint toggle_size;
	guint accel_size;

	GtkWidget *toplevel;

	GtkWidget     *tearoff_window;
	GtkWidget     *tearoff_hbox;
	GtkWidget     *tearoff_scrollbar;
	GtkAdjustment *tearoff_adjustment;

	GdkWindow *view_window;
	GdkWindow *bin_window;

	gint scroll_offset;
	gint saved_scroll_offset;
	gint scroll_step;

	guint scroll_timeout;

	guint needs_destruction_ref : 1;
	guint torn_off              : 1;
	/* The tearoff is active when it is torn off and the not-torn-off
   * menu is not popped up.
   */
	guint tearoff_active        : 1;
	guint scroll_fast           : 1;

	guint upper_arrow_visible   : 1;
	guint lower_arrow_visible   : 1;
	guint upper_arrow_prelight  : 1;
	guint lower_arrow_prelight  : 1;

	guint have_position         : 1;
	guint have_layout           : 1;
	guint seen_item_enter       : 1;
	guint ignore_button_release : 1;
	guint no_toggle_size        : 1;
	guint drag_already_pressed  : 1;
	guint drag_scroll_started   : 1;

	/* info used for the table */
	guint *heights;
	gint heights_length;
	gint requested_height;

	gboolean initially_pushed_in;
	gint monitor_num;

	/* Cached layout information */
	gint n_rows;
	gint n_columns;

	gchar *title;

	/* Arrow states */
	GtkStateFlags lower_arrow_state;
	GtkStateFlags upper_arrow_state;

	/* navigation region */
	gint navigation_x;
	gint navigation_y;
	gint navigation_width;
	gint navigation_height;

	guint navigation_timeout;

	gdouble drag_start_y;
	gint initial_drag_offset;
};

struct _GtkMenuShellPrivate
{
	GList *children;

	GtkWidget *active_menu_item;
	GtkWidget *parent_menu_shell;
	void *tracker;

	guint button;
	guint32 activate_time;

	guint active               : 1;
	guint have_grab            : 1;
	guint have_xgrab           : 1;
	guint ignore_enter         : 1;
	guint keyboard_mode        : 1;

	guint take_focus           : 1;
	guint activated_submenu    : 1;
	guint in_unselectable_item : 1;
};

struct _GtkMenuItemPrivate
{
	GtkWidget *submenu;
	GdkWindow *event_window;

	guint16 toggle_size;
	guint16 accelerator_width;

	guint timer;

	gchar  *accel_path;

	GtkAction *action;
	void *action_helper;

	guint show_submenu_indicator : 1;
	guint submenu_placement      : 1;
	guint submenu_direction      : 1;
	guint right_justify          : 1;
	guint timer_from_keypress    : 1;
	guint from_menubar           : 1;
	guint use_action_appearance  : 1;
	guint reserve_indicator      : 1;
};
#endif

static void
free_timeval (GTimeVal *val)
{
	g_slice_free (GTimeVal, val);
}

static void
get_offsets (GtkMenu *menu,
             gint    *horizontal_offset,
             gint    *vertical_offset)
{
#if GTK_MAJOR_VERSION == 3
	GtkStyleContext *context;
	GtkStateFlags state;
	GtkBorder padding;

	gtk_widget_style_get (GTK_WIDGET (menu),
	                      "horizontal-offset", horizontal_offset,
	                      "vertical-offset", vertical_offset,
	                      NULL);

	context = gtk_widget_get_style_context (GTK_WIDGET (menu));
	state = gtk_widget_get_state_flags (GTK_WIDGET (menu));
	gtk_style_context_get_padding (context, state, &padding);

	*vertical_offset -= padding.top;
	*horizontal_offset += padding.left;
#else
	gint vertical_padding;
	gint horizontal_padding;

	gtk_widget_style_get (GTK_WIDGET (menu),
	                      "horizontal-offset", horizontal_offset,
	                      "vertical-offset", vertical_offset,
	                      "horizontal-padding", &horizontal_padding,
	                      "vertical-padding", &vertical_padding,
	                      NULL);

	*vertical_offset -= GTK_WIDGET (menu)->style->ythickness;
	*vertical_offset -= vertical_padding;
	*horizontal_offset += horizontal_padding;
#endif
}

static void
menu_item_position_menu (GtkMenu  *menu,
                         gint     *x,
                         gint     *y,
                         gboolean *push_in,
                         gpointer  user_data)
{
#if GTK_MAJOR_VERSION == 3
	GtkMenuItem *menu_item = GTK_MENU_ITEM (user_data);
	GtkMenuItemPrivate *priv = menu_item->priv;
	GtkAllocation allocation;
	GtkWidget *widget;
	GtkMenuItem *parent_menu_item;
	GtkWidget *parent;
	GdkScreen *screen;
	gint twidth, theight;
	gint tx, ty;
	GtkTextDirection direction;
	GdkRectangle monitor;
	gint monitor_num;
	gint horizontal_offset;
	gint vertical_offset;
	gint available_left, available_right;
	GtkStyleContext *context;
	GtkStateFlags state;
	GtkBorder parent_padding;

	g_return_if_fail (menu != NULL);
	g_return_if_fail (x != NULL);
	g_return_if_fail (y != NULL);

	widget = GTK_WIDGET (user_data);

	if (push_in)
		*push_in = FALSE;

	direction = gtk_widget_get_direction (widget);

	twidth = gtk_widget_get_allocated_width (GTK_WIDGET (menu));
	theight = gtk_widget_get_allocated_height (GTK_WIDGET (menu));

	screen = gtk_widget_get_screen (GTK_WIDGET (menu));
	monitor_num = gdk_screen_get_monitor_at_window (screen, priv->event_window);
	if (monitor_num < 0)
		monitor_num = 0;
	gdk_screen_get_monitor_workarea (screen, monitor_num, &monitor);

	if (!gdk_window_get_origin (gtk_widget_get_window (widget), &tx, &ty))
	{
		g_warning ("Menu not on screen");
		return;
	}

	gtk_widget_get_allocation (widget, &allocation);

	tx += allocation.x;
	ty += allocation.y;

	get_offsets (menu, &horizontal_offset, &vertical_offset);

	available_left = tx - monitor.x;
	available_right = monitor.x + monitor.width - (tx + allocation.width);

	parent = gtk_widget_get_parent (widget);
	priv->from_menubar = GTK_IS_MENU_BAR (parent);

	switch (priv->submenu_placement)
	{
	case GTK_TOP_BOTTOM:
		if (direction == GTK_TEXT_DIR_LTR)
			priv->submenu_direction = GTK_DIRECTION_RIGHT;
		else
		{
			priv->submenu_direction = GTK_DIRECTION_LEFT;
			tx += allocation.width - twidth;
		}
		if ((ty + allocation.height + theight) <= monitor.y + monitor.height)
			ty += allocation.height;
		else if ((ty - theight) >= monitor.y)
			ty -= theight;
		else if (monitor.y + monitor.height - (ty + allocation.height) > ty)
			ty += allocation.height;
		else
			ty -= theight;
		break;

	case GTK_LEFT_RIGHT:
		if (GTK_IS_MENU (parent))
			parent_menu_item = GTK_MENU_ITEM (GTK_MENU (parent)->priv->parent_menu_item);
		else
			parent_menu_item = NULL;

		context = gtk_widget_get_style_context (parent);
		state = gtk_widget_get_state_flags (parent);
		gtk_style_context_get_padding (context, state, &parent_padding);

		if (parent_menu_item && !GTK_MENU (parent)->priv->torn_off)
		{
			priv->submenu_direction = parent_menu_item->priv->submenu_direction;
		}
		else
		{
			if (direction == GTK_TEXT_DIR_LTR)
				priv->submenu_direction = GTK_DIRECTION_RIGHT;
			else
				priv->submenu_direction = GTK_DIRECTION_LEFT;
		}

		switch (priv->submenu_direction)
		{
		case GTK_DIRECTION_LEFT:
			if (tx - twidth - parent_padding.left - horizontal_offset >= monitor.x ||
			        available_left >= available_right)
				tx -= twidth + parent_padding.left + horizontal_offset;
			else
			{
				priv->submenu_direction = GTK_DIRECTION_RIGHT;
				tx += allocation.width + parent_padding.right + horizontal_offset;
			}
			break;

		case GTK_DIRECTION_RIGHT:
			if (tx + allocation.width + parent_padding.right + horizontal_offset + twidth <= monitor.x + monitor.width ||
			        available_right >= available_left)
				tx += allocation.width + parent_padding.right + horizontal_offset;
			else
			{
				priv->submenu_direction = GTK_DIRECTION_LEFT;
				tx -= twidth + parent_padding.left + horizontal_offset;
			}
			break;
		}

		ty += vertical_offset;

		/* If the height of the menu doesn't fit we move it upward. */
		ty = CLAMP (ty, monitor.y, MAX (monitor.y, monitor.y + monitor.height - theight));
		break;
	}

	/* If we have negative, tx, here it is because we can't get
	 * the menu all the way on screen. Favor the left portion.
	 */
	*x = CLAMP (tx, monitor.x, MAX (monitor.x, monitor.x + monitor.width - twidth));
	*y = ty;

	gtk_menu_set_monitor (menu, monitor_num);

	if (!gtk_widget_get_visible (menu->priv->toplevel))
	{
		gtk_window_set_type_hint (GTK_WINDOW (menu->priv->toplevel), priv->from_menubar?
		                              GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU : GDK_WINDOW_TYPE_HINT_POPUP_MENU);
	}
#else
	GtkMenuItem *menu_item;
	GtkWidget *widget;
	GtkMenuItem *parent_menu_item;
	GdkScreen *screen;
	gint twidth, theight;
	gint tx, ty;
	GtkTextDirection direction;
	GdkRectangle monitor;
	gint monitor_num;
	gint horizontal_offset;
	gint vertical_offset;
	gint parent_xthickness;
	gint available_left, available_right;

	g_return_if_fail (menu != NULL);
	g_return_if_fail (x != NULL);
	g_return_if_fail (y != NULL);

	menu_item = GTK_MENU_ITEM (user_data);
	widget = GTK_WIDGET (user_data);

	if (push_in)
		*push_in = FALSE;

	direction = gtk_widget_get_direction (widget);

	twidth = GTK_WIDGET (menu)->requisition.width;
	theight = GTK_WIDGET (menu)->requisition.height;

	screen = gtk_widget_get_screen (GTK_WIDGET (menu));
	monitor_num = gdk_screen_get_monitor_at_window (screen, menu_item->event_window);
	if (monitor_num < 0)
		monitor_num = 0;
	gdk_screen_get_monitor_geometry (screen, monitor_num, &monitor);

	if (!gdk_window_get_origin (widget->window, &tx, &ty))
	{
		g_warning ("Menu not on screen");
		return;
	}

	tx += widget->allocation.x;
	ty += widget->allocation.y;

	get_offsets (menu, &horizontal_offset, &vertical_offset);

	available_left = tx - monitor.x;
	available_right = monitor.x + monitor.width - (tx + widget->allocation.width);

	if (GTK_IS_MENU_BAR (widget->parent))
	{
		menu_item->from_menubar = TRUE;
	}
	else if (GTK_IS_MENU (widget->parent))
	{
		if (GTK_MENU (widget->parent)->parent_menu_item)
			menu_item->from_menubar = GTK_MENU_ITEM (GTK_MENU (widget->parent)->parent_menu_item)->from_menubar;
		else
			menu_item->from_menubar = FALSE;
	}
	else
	{
		menu_item->from_menubar = FALSE;
	}

	switch (menu_item->submenu_placement)
	{
	case GTK_TOP_BOTTOM:
		if (direction == GTK_TEXT_DIR_LTR)
			menu_item->submenu_direction = GTK_DIRECTION_RIGHT;
		else
		{
			menu_item->submenu_direction = GTK_DIRECTION_LEFT;
			tx += widget->allocation.width - twidth;
		}
		if ((ty + widget->allocation.height + theight) <= monitor.y + monitor.height)
			ty += widget->allocation.height;
		else if ((ty - theight) >= monitor.y)
			ty -= theight;
		else if (monitor.y + monitor.height - (ty + widget->allocation.height) > ty)
			ty += widget->allocation.height;
		else
			ty -= theight;
		break;

	case GTK_LEFT_RIGHT:
		if (GTK_IS_MENU (widget->parent))
			parent_menu_item = GTK_MENU_ITEM (GTK_MENU (widget->parent)->parent_menu_item);
		else
			parent_menu_item = NULL;

		parent_xthickness = widget->parent->style->xthickness;

		if (parent_menu_item && !GTK_MENU (widget->parent)->torn_off)
		{
			menu_item->submenu_direction = parent_menu_item->submenu_direction;
		}
		else
		{
			if (direction == GTK_TEXT_DIR_LTR)
				menu_item->submenu_direction = GTK_DIRECTION_RIGHT;
			else
				menu_item->submenu_direction = GTK_DIRECTION_LEFT;
		}

		switch (menu_item->submenu_direction)
		{
		case GTK_DIRECTION_LEFT:
			if (tx - twidth - parent_xthickness - horizontal_offset >= monitor.x ||
			        available_left >= available_right)
				tx -= twidth + parent_xthickness + horizontal_offset;
			else
			{
				menu_item->submenu_direction = GTK_DIRECTION_RIGHT;
				tx += widget->allocation.width + parent_xthickness + horizontal_offset;
			}
			break;

		case GTK_DIRECTION_RIGHT:
			if (tx + widget->allocation.width + parent_xthickness + horizontal_offset + twidth <= monitor.x + monitor.width ||
			        available_right >= available_left)
				tx += widget->allocation.width + parent_xthickness + horizontal_offset;
			else
			{
				menu_item->submenu_direction = GTK_DIRECTION_LEFT;
				tx -= twidth + parent_xthickness + horizontal_offset;
			}
			break;
		}

		ty += vertical_offset;

		/* If the height of the menu doesn't fit we move it upward. */
		ty = CLAMP (ty, monitor.y, MAX (monitor.y, monitor.y + monitor.height - theight));
		break;
	}

	/* If we have negative, tx, here it is because we can't get
   * the menu all the way on screen. Favor the left portion.
   */
	*x = CLAMP (tx, monitor.x, MAX (monitor.x, monitor.x + monitor.width - twidth));
	*y = ty;

	gtk_menu_set_monitor (menu, monitor_num);

	if (!gtk_widget_get_visible (menu->toplevel))
	{
		gtk_window_set_type_hint (GTK_WINDOW (menu->toplevel), menu_item->from_menubar?
		                              GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU : GDK_WINDOW_TYPE_HINT_POPUP_MENU);
	}
#endif
}

static void handle_menuitem_notify(GtkMenuItem *item, GParamSpec *pspec, GtkMenuItem *proxy)
{
	// Note that it is OK to compare strings by pointer as they are all interned
	if (pspec->name == pname_submenu) {
		// Nothing to do!
	} else if (pspec->name == pname_label) {
		const gchar *label = gtk_menu_item_get_label(item);
		gtk_menu_item_set_label(proxy, label);
	} else if (pspec->name == pname_sensitive) {
		gtk_widget_set_sensitive(GTK_WIDGET(proxy),
		                         gtk_widget_get_sensitive(GTK_WIDGET(item)));
	} else if (pspec->name == pname_visible) {
		if (gtk_widget_get_visible(GTK_WIDGET(item))) {
			gtk_widget_show(GTK_WIDGET(proxy));
		} else {
			gtk_widget_hide(GTK_WIDGET(proxy));
		}
	}
}

static gboolean handle_menuitem_mnemonic_activate(GtkMenuItem *item, gboolean cycling, GtkMenuItem *proxy)
{
	TopMenuMonitor *monitor = topmenu_monitor_get_instance();
	GtkWidget *parent = gtk_widget_get_parent(GTK_WIDGET(proxy));

	if (parent && monitor->available) {
		GtkMenuShell *parent_shell = GTK_MENU_SHELL(parent);
		if (GTK_IS_MENU_BAR(parent_shell) || parent_shell->priv->active) {
			gtk_widget_mnemonic_activate(GTK_WIDGET(proxy), cycling);
			return TRUE;
		}
	}

	return FALSE;
}

static gboolean handle_menu_leave_notify(GtkMenu *menu, GdkEvent *event, GtkMenuItem *item)
{
	return TRUE;
}

static void handle_parent_move_current(GtkMenuShell *shell, GtkMenuDirectionType dir, GtkMenuItem *item)
{
	if (dir == GTK_MENU_DIR_CHILD) {
		GtkWidget *submenu = gtk_menu_item_get_submenu(item);
		if (submenu) {
			gtk_menu_shell_select_first(GTK_MENU_SHELL(submenu), TRUE);
		}
	}
}

static void handle_proxy_select(GtkMenuItem *proxy, GtkMenuItem *item)
{
	GtkWidget *submenu = gtk_menu_item_get_submenu(item);
	GtkWidget *parent = gtk_widget_get_parent(GTK_WIDGET(proxy));

	if (submenu && parent) {
		if (!gtk_widget_is_sensitive(GTK_WIDGET(submenu)))
			return;

		GtkMenuShell *parent_shell = GTK_MENU_SHELL(parent);
		GTimeVal *popup_time = g_slice_new0(GTimeVal);

        g_get_current_time(popup_time);
		g_object_set_data_full(G_OBJECT(submenu),
		                                "gtk-menu-exact-popup-time", popup_time,
		                                (GDestroyNotify) free_timeval);

		g_signal_connect_object(submenu, "leave-notify-event",
		                        G_CALLBACK(handle_menu_leave_notify), item, 0);

		g_signal_connect_object(gtk_widget_get_parent(GTK_WIDGET(proxy)), "move-current",
		                        G_CALLBACK(handle_parent_move_current), item, 0);

		gtk_menu_popup(GTK_MENU(submenu),
		               GTK_WIDGET(parent_shell),
		               GTK_WIDGET(proxy),
		               menu_item_position_menu,
		               proxy,
		               parent_shell->priv->button,
		               0);
	}
}

static void handle_proxy_deselect(GtkMenuItem *proxy, GtkMenuItem *item)
{
	GtkWidget *submenu = gtk_menu_item_get_submenu(item);

	if (submenu) {
		g_signal_handlers_disconnect_by_func(submenu, handle_menu_leave_notify, item);
		g_signal_handlers_disconnect_by_func(gtk_widget_get_parent(GTK_WIDGET(proxy)),
		                                     handle_parent_move_current, item);
		gtk_menu_popdown(GTK_MENU(submenu));
	}
}

static void handle_proxy_activate(GtkMenuItem *proxy, GtkMenuItem *item)
{
	GtkWidget *submenu = gtk_menu_item_get_submenu(item);

	if (submenu) {
		// Do nothing!
	} else {
		gtk_menu_item_activate(item);
	}
}

static void handle_proxy_activate_item(GtkMenuItem *proxy, GtkMenuItem *item)
{
	GtkWidget *submenu = gtk_menu_item_get_submenu(item);

	if (submenu) {
		GtkMenuShell *parent = GTK_MENU_SHELL(gtk_widget_get_parent(GTK_WIDGET(proxy)));
		if (parent) {
			parent->priv->active = TRUE;
			// We do not add grabs here, like Gtk+ does, because they are already done.
			gtk_menu_shell_select_item(parent, GTK_WIDGET(proxy));
			gtk_menu_shell_select_first(GTK_MENU_SHELL(submenu), TRUE);
		}
	}
}

static GtkWidget *construct_image_widget_proxy(GtkImage *widget)
{
	GtkImageType itype = gtk_image_get_storage_type(widget);
	gchar *icon_name;
	GtkIconSize icon_size;
	switch (itype) {
	case GTK_IMAGE_STOCK:
		gtk_image_get_stock(widget, &icon_name, &icon_size);
		return gtk_image_new_from_stock(icon_name, icon_size);
	case GTK_IMAGE_EMPTY:
		return NULL;
	default:
		return NULL;
	}
}

GtkMenuItem *topmenu_create_proxy_menu_item(GtkMenuItem *item)
{
	init_static_data();

	GtkMenuItem *proxy = NULL;

	const gchar *label = gtk_menu_item_get_label(item);

	if (GTK_IS_IMAGE_MENU_ITEM(item)) {
		GtkImageMenuItem *iitem = GTK_IMAGE_MENU_ITEM(item);
		if (gtk_image_menu_item_get_use_stock(iitem)) {
			proxy = GTK_MENU_ITEM(gtk_image_menu_item_new_from_stock(label, NULL));
		} else {
			GtkWidget *iwidget = gtk_image_menu_item_get_image(iitem);
			proxy = GTK_MENU_ITEM(gtk_image_menu_item_new_with_mnemonic(label));
			if (iwidget) {
				// Let's suppport some common widget types
				if (GTK_IS_IMAGE(iwidget)) {
					GtkImage *iwidgetimg = GTK_IMAGE(iwidget);
					gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(proxy),
					                              construct_image_widget_proxy(iwidgetimg));
				}
			}
		}
		gtk_image_menu_item_set_always_show_image(GTK_IMAGE_MENU_ITEM(proxy),
		                                          gtk_image_menu_item_get_always_show_image(iitem));
	} else if (GTK_IS_SEPARATOR_MENU_ITEM(item)) {
		proxy = GTK_MENU_ITEM(gtk_separator_menu_item_new());
	} else {
		proxy = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic(label));
	}

	gtk_widget_set_sensitive(GTK_WIDGET(proxy),
	                         gtk_widget_get_sensitive(GTK_WIDGET(item)));

	if (gtk_widget_get_visible(GTK_WIDGET(item))) {
		gtk_widget_show(GTK_WIDGET(proxy));
	}

	g_signal_connect_object(item, "notify",
	                        G_CALLBACK(handle_menuitem_notify), proxy, 0);
	g_signal_connect_object(item, "mnemonic-activate",
	                        G_CALLBACK(handle_menuitem_mnemonic_activate), proxy, 0);

	g_signal_connect_object(proxy, "select",
	                        G_CALLBACK(handle_proxy_select), item, 0);
	g_signal_connect_object(proxy, "deselect",
	                        G_CALLBACK(handle_proxy_deselect), item, 0);
	g_signal_connect_object(proxy, "activate",
	                        G_CALLBACK(handle_proxy_activate), item, 0);
	g_signal_connect_object(proxy, "activate-item",
	                        G_CALLBACK(handle_proxy_activate_item), item, 0);

	return proxy;
}
