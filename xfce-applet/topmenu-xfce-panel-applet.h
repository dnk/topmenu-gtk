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

#ifndef _TOPMENU_XFCE_PANEL_APPLET_H_
#define _TOPMENU_XFCE_PANEL_APPLET_H_

#include <libxfce4panel/xfce-panel-plugin.h>
#include "../libtopmenu-server/topmenu-widget.h"

G_BEGIN_DECLS

#define TOPMENU_TYPE_XFCE_PANEL_APPLET           topmenu_xfce_panel_applet_get_type()
#define TOPMENU_XFCE_PANEL_APPLET(obj)           (G_TYPE_CHECK_INSTANCE_CAST((obj), TOPMENU_TYPE_XFCE_PANEL_APPLET, TopMenuXfcePanelApplet))
#define TOPMENU_IS_XFCE_PANEL_APPLET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE((obj), TOPMENU_TYPE_XFCE_PANEL_APPLET))
#define TOPMENU_XFCE_PANEL_APPLET_CLASS(c)       (G_TYPE_CHECK_CLASS_CAST((c), TOPMENU_TYPE_XFCE_PANEL_APPLET, TopMenuXfcePanelAppletClass))
#define TOPMENU_IS_XFCE_PANEL_APPLET_CLASS(c)    (G_TYPE_CHECK_CLASS_TYPE((c), TOPMENU_TYPE_XFCE_PANEL_APPLET))
#define TOPMENU_XFCE_PANEL_APPLET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TOPMENU_TYPE_XFCE_PANEL_APPLET, TopMenuXfcePanelAppletClass))

typedef struct _TopMenuXfcePanelApplet        TopMenuXfcePanelApplet;
typedef struct _TopMenuXfcePanelAppletClass   TopMenuXfcePanelAppletClass;

struct _TopMenuXfcePanelApplet
{
	XfcePanelPlugin parent_instance;
	TopMenuWidget *menu_widget;
	GtkActionGroup *actions;
};

struct _TopMenuXfcePanelAppletClass
{
	XfcePanelPluginClass parent_class;
};

GType topmenu_xfce_panel_applet_get_type(void);

XfcePanelPlugin *topmenu_xfce_panel_applet_new(void);

G_END_DECLS

#endif /* _TOPMENU_XFCE_PANEL_APPLET_H_ */
