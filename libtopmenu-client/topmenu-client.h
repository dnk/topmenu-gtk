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

#ifndef _TOPMENU_CLIENT_H_
#define _TOPMENU_CLIENT_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

void topmenu_client_connect_window_widget(GdkWindow *window, GtkWidget *widget);
void topmenu_client_disconnect_window(GdkWindow *window);

G_END_DECLS

#endif
