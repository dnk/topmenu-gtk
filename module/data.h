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

#ifndef _DATA_H_
#define _DATA_H_

#include <gtk/gtk.h>

#include "../libtopmenu-client/topmenu-appmenubar.h"
#include "appmenu.h"

typedef struct _WindowData WindowData;
typedef struct _MenuShellData MenuShellData;
typedef struct _MenuItemData MenuItemData;

struct _WindowData
{
	GSList              *menus;
	TopMenuAppMenuBar   *appmenubar;
	AppMenu              appmenu;
	gulong               monitor_connection_id;
};

struct _MenuShellData
{
	GtkWindow           *window;
};

struct _MenuItemData
{
	GtkMenuItem         *proxy;
};
gboolean
topmenu_is_blacklisted (void);

gboolean
topmenu_is_window_blacklisted (GtkWindow *window);

WindowData *
topmenu_get_window_data (GtkWindow *window);

void
topmenu_remove_window_data (GtkWindow *window);

MenuShellData *
topmenu_get_menu_shell_data (GtkMenuShell *menu_shell);

MenuItemData *
topmenu_get_menu_item_data (GtkMenuItem *menu_item);

#endif
