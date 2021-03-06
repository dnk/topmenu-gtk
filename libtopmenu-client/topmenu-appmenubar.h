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

#ifndef _TOPMENU_APPMENUBAR_H_
#define _TOPMENU_APPMENUBAR_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define TOPMENU_TYPE_APP_MENU_BAR           topmenu_app_menu_bar_get_type()
#define TOPMENU_APP_MENU_BAR(obj)           (G_TYPE_CHECK_INSTANCE_CAST((obj), TOPMENU_TYPE_APP_MENU_BAR, TopMenuAppMenuBar))
#define TOPMENU_IS_APP_MENU_BAR(obj)        (G_TYPE_CHECK_INSTANCE_TYPE((obj), TOPMENU_TYPE_APP_MENU_BAR))
#define TOPMENU_APP_MENU_BAR_CLASS(c)       (G_TYPE_CHECK_CLASS_CAST((c), TOPMENU_TYPE_APP_MENU_BAR, TopMenuAppMenuBarClass))
#define TOPMENU_IS_APP_MENU_BAR_CLASS(c)    (G_TYPE_CHECK_CLASS_TYPE((c), TOPMENU_TYPE_APP_MENU_BAR))
#define TOPMENU_APP_MENU_BAR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TOPMENU_TYPE_APP_MENU_BAR, TopMenuAppMenuBarClass))

typedef struct _TopMenuAppMenuBar        TopMenuAppMenuBar;
typedef struct _TopMenuAppMenuBarClass   TopMenuAppMenuBarClass;

struct _TopMenuAppMenuBar
{
	GtkMenuBar parent_instance;
	GtkMenuItem *app_menu_item;
};

struct _TopMenuAppMenuBarClass
{
	GtkMenuBarClass parent_class;
};

GType topmenu_app_menu_bar_get_type(void);

TopMenuAppMenuBar *topmenu_app_menu_bar_new(void);

void topmenu_app_menu_bar_set_app_menu(TopMenuAppMenuBar *self, GtkWidget *menu);
GtkWidget *topmenu_app_menu_bar_get_app_menu(TopMenuAppMenuBar *self);

G_END_DECLS

#endif /* _TOPMENU_APP_MENU_BAR_H_ */
