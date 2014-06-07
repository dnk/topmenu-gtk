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

#include <string.h>

#include "topmenu-mate-panel-applet.h"

static gboolean topmenu_mate_panel_applet_factory(MatePanelApplet *applet,
                                                  const gchar *iid,
                                                  gpointer data)
{
	if (strcmp(iid, "TopMenuMatePanelApplet") == 0) {
		gtk_widget_show_all(GTK_WIDGET(applet));
		return TRUE;
	}

	return FALSE;
}

MATE_PANEL_APPLET_OUT_PROCESS_FACTORY("TopMenuMatePanelAppletFactory",
                                      TOPMENU_TYPE_MATE_PANEL_APPLET,
                                      "TopMenuMatePanelApplet",
                                      topmenu_mate_panel_applet_factory,
                                      NULL)
