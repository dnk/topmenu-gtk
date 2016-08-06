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

#include "topmenu-mate-panel-applet.h"

#include <glib/gi18n.h>
#include <gdk/gdkx.h>

G_DEFINE_TYPE(TopMenuMatePanelApplet, topmenu_mate_panel_applet, PANEL_TYPE_APPLET)

static void display_preferences_dialog(GtkAction *action, TopMenuMatePanelApplet *self)
{
	// TODO
}

static void display_about_dialog(GtkAction *action, TopMenuMatePanelApplet *self)
{
	GtkWindow *parent = NULL;
	GtkWidget *parent_widget = gtk_widget_get_toplevel(GTK_WIDGET(self));
	if (GTK_IS_WINDOW(parent_widget)) {
		parent = GTK_WINDOW(parent_widget);
	}

	gtk_show_about_dialog(parent,
	                      "program-name", "TopMenu Mate Panel Applet",
	                      NULL);
}

static const GtkActionEntry menu_verbs[] = {
	{ "TopMenuPreferences", GTK_STOCK_PROPERTIES, N_("_Preferences"),
	  NULL, NULL,
	  G_CALLBACK (display_preferences_dialog) },
	{ "TopMenuAbout", GTK_STOCK_ABOUT, N_("_About"),
	  NULL, NULL,
	  G_CALLBACK (display_about_dialog) }
};

static void topmenu_mate_panel_applet_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
	TopMenuMatePanelApplet *self = TOPMENU_MATE_PANEL_APPLET(widget);
	if (self->menu_widget) {
		gtk_widget_size_allocate(GTK_WIDGET(self->menu_widget), allocation);
	}
	GTK_WIDGET_CLASS(topmenu_mate_panel_applet_parent_class)->size_allocate(widget, allocation);
}

#if GTK_MAJOR_VERSION == 3
static void topmenu_mate_panel_get_preferred_width(GtkWidget *widget, gint *minimal_width, gint *natural_width)
{
	TopMenuMatePanelApplet *self = TOPMENU_MATE_PANEL_APPLET(widget);
	if (self->menu_widget) {
		gtk_widget_get_preferred_width(GTK_WIDGET(self->menu_widget), minimal_width, natural_width);
	}
}

static void topmenu_mate_panel_get_preferred_height(GtkWidget *widget, gint *minimal_height, gint *natural_height)
{
	TopMenuMatePanelApplet *self = TOPMENU_MATE_PANEL_APPLET(widget);
	if (self->menu_widget) {
		gtk_widget_get_preferred_height(GTK_WIDGET(self->menu_widget), minimal_height, natural_height);
	}
}
#elif GTK_MAJOR_VERSION == 2
static void topmenu_mate_panel_applet_size_request(GtkWidget *widget, GtkRequisition *requisition)
{
	TopMenuMatePanelApplet *self = TOPMENU_MATE_PANEL_APPLET(widget);
	if (self->menu_widget) {
		gtk_widget_size_request(GTK_WIDGET(self->menu_widget), requisition);
	}
}
#endif

static void topmenu_mate_panel_applet_class_init(TopMenuMatePanelAppletClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
	widget_class->size_allocate = topmenu_mate_panel_applet_size_allocate;
#if GTK_MAJOR_VERSION == 3
	widget_class->get_preferred_width = topmenu_mate_panel_get_preferred_width;
	widget_class->get_preferred_height = topmenu_mate_panel_get_preferred_height;
#elif GTK_MAJOR_VERSION == 2
	widget_class->size_request = topmenu_mate_panel_applet_size_request;
#endif
}

static void topmenu_mate_panel_applet_init(TopMenuMatePanelApplet *self)
{
	self->menu_widget = TOPMENU_WIDGET(topmenu_widget_new());
	gtk_widget_set_can_focus(GTK_WIDGET(self->menu_widget), TRUE);
	gtk_container_add(GTK_CONTAINER(self), GTK_WIDGET(self->menu_widget));

	GtkActionGroup *action_group = gtk_action_group_new("TopMenu Mate Panel Applet Actions");
	gtk_action_group_add_actions(action_group,
	                             menu_verbs, G_N_ELEMENTS(menu_verbs), self);

	mate_panel_applet_set_flags(MATE_PANEL_APPLET(self),
	                            MATE_PANEL_APPLET_EXPAND_MINOR);
	mate_panel_applet_setup_menu(MATE_PANEL_APPLET(self),
	                             "<menuitem name=\"TopMenu About Item\" action=\"TopMenuAbout\"/>",
	                             action_group);

	g_object_unref(action_group);
}

MatePanelApplet *topmenu_mate_panel_applet_new(void)
{
	return MATE_PANEL_APPLET(g_object_new(TOPMENU_TYPE_MATE_PANEL_APPLET, NULL));
}
