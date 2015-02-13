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

#include "topmenu-xfce-panel-applet.h"

XFCE_PANEL_DEFINE_PLUGIN_RESIDENT(TopMenuXfcePanelApplet, topmenu_xfce_panel_applet)

static void topmenu_xfce_panel_applet_construct(XfcePanelPlugin *plugin)
{
	TopMenuXfcePanelApplet *self = TOPMENU_XFCE_PANEL_APPLET(plugin);
	xfce_panel_plugin_set_expand(plugin, TRUE);
	xfce_panel_plugin_add_action_widget(plugin, GTK_WIDGET(self->menu_widget));
}

static gboolean topmenu_xfce_panel_applet_size_changed(XfcePanelPlugin *plugin, gint size)
{
	return TRUE;
}

static void topmenu_xfce_panel_applet_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
	TopMenuXfcePanelApplet *self = TOPMENU_XFCE_PANEL_APPLET(widget);
	if (self->menu_widget) {
		gtk_widget_size_allocate(GTK_WIDGET(self->menu_widget), allocation);
	}
	GTK_WIDGET_CLASS(topmenu_xfce_panel_applet_parent_class)->size_allocate(widget, allocation);
}

static void topmenu_xfce_panel_applet_size_request(GtkWidget *widget, GtkRequisition *requisition)
{
	TopMenuXfcePanelApplet *self = TOPMENU_XFCE_PANEL_APPLET(widget);
	if (self->menu_widget) {
		gtk_widget_size_request(GTK_WIDGET(self->menu_widget), requisition);
	}
}

static void topmenu_xfce_panel_applet_class_init(TopMenuXfcePanelAppletClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
	widget_class->size_allocate = topmenu_xfce_panel_applet_size_allocate;
	widget_class->size_request = topmenu_xfce_panel_applet_size_request;

	XfcePanelPluginClass *plugin_class = XFCE_PANEL_PLUGIN_CLASS(klass);
	plugin_class->construct = topmenu_xfce_panel_applet_construct;
	plugin_class->size_changed = topmenu_xfce_panel_applet_size_changed;
}

static void topmenu_xfce_panel_applet_init(TopMenuXfcePanelApplet *self)
{
	self->menu_widget = TOPMENU_WIDGET(topmenu_widget_new());
	gtk_widget_set_can_focus(GTK_WIDGET(self->menu_widget), TRUE);
	gtk_container_add(GTK_CONTAINER(self), GTK_WIDGET(self->menu_widget));
	gtk_widget_show_all(GTK_WIDGET(self->menu_widget));
}

XfcePanelPlugin *topmenu_xfce_panel_applet_new(void)
{
	return XFCE_PANEL_PLUGIN(g_object_new(TOPMENU_TYPE_XFCE_PANEL_APPLET, NULL));
}
