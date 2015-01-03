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

#ifndef _TOPMENU_MATE_PANEL_APPLET_H_
#define _TOPMENU_MATE_PANEL_APPLET_H_

#include <mate-panel-applet.h>
#include "../libtopmenu-server/topmenu-widget.h"

G_BEGIN_DECLS

#define TOPMENU_TYPE_MATE_PANEL_APPLET           topmenu_mate_panel_applet_get_type()
#define TOPMENU_MATE_PANEL_APPLET(obj)           (G_TYPE_CHECK_INSTANCE_CAST((obj), TOPMENU_TYPE_MATE_PANEL_APPLET, TopMenuMatePanelApplet))
#define TOPMENU_IS_MATE_PANEL_APPLET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE((obj), TOPMENU_TYPE_MATE_PANEL_APPLET))
#define TOPMENU_MATE_PANEL_APPLET_CLASS(c)       (G_TYPE_CHECK_CLASS_CAST((c), TOPMENU_TYPE_MATE_PANEL_APPLET, TopMenuMatePanelAppletClass))
#define TOPMENU_IS_MATE_PANEL_APPLET_CLASS(c)    (G_TYPE_CHECK_CLASS_TYPE((c), TOPMENU_TYPE_MATE_PANEL_APPLET))
#define TOPMENU_MATE_PANEL_APPLET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TOPMENU_TYPE_MATE_PANEL_APPLET, TopMenuMatePanelAppletClass))

typedef struct _TopMenuMatePanelApplet        TopMenuMatePanelApplet;
typedef struct _TopMenuMatePanelAppletClass   TopMenuMatePanelAppletClass;

struct _TopMenuMatePanelApplet
{
	MatePanelApplet parent_instance;
	TopMenuWidget *menu_widget;
	GtkActionGroup *actions;
};

struct _TopMenuMatePanelAppletClass
{
	MatePanelAppletClass parent_class;
};

GType topmenu_mate_panel_applet_get_type(void);

MatePanelApplet *topmenu_mate_panel_applet_new(void);

G_END_DECLS

#endif /* _TOPMENU_MATE_PANEL_APPLET_H_ */
