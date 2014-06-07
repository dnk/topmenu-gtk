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

#ifndef _TOPMENU_WIDGET_H_
#define _TOPMENU_WIDGET_H_

#include <gtk/gtk.h>

#if GTK_MAJOR_VERSION == 3
#include <gtk/gtkx.h>
#endif

G_BEGIN_DECLS

#define TOPMENU_TYPE_WIDGET           topmenu_widget_get_type()
#define TOPMENU_WIDGET(obj)           (G_TYPE_CHECK_INSTANCE_CAST((obj), TOPMENU_TYPE_WIDGET, TopMenuWidget))
#define TOPMENU_IS_WIDGET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE((obj), TOPMENU_TYPE_WIDGET))
#define TOPMENU_WIDGET_CLASS(c)       (G_TYPE_CHECK_CLASS_CAST((c), TOPMENU_TYPE_WIDGET, TopMenuWidgetClass))
#define TOPMENU_IS_WIDGET_CLASS(c)    (G_TYPE_CHECK_CLASS_TYPE((c), TOPMENU_TYPE_WIDGET))
#define TOPMENU_WIDGET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TOPMENU_TYPE_WIDGET, TopMenuWidgetClass))

typedef struct _TopMenuWidget        TopMenuWidget;
typedef struct _TopMenuWidgetClass   TopMenuWidgetClass;
typedef struct _TopMenuWidgetPrivate TopMenuWidgetPrivate;

struct _TopMenuWidget
{
	GtkBin parent_instance;

	TopMenuWidgetPrivate *priv;

	GtkSocket *socket;
};

struct _TopMenuWidgetClass
{
	GtkBinClass parent_class;
};

GType topmenu_widget_get_type(void);

GtkWidget *topmenu_widget_new(void);

G_END_DECLS

#endif /* _TOPMENU_WIDGET_H_ */
