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

#ifndef _TOPMENU_MONITOR_H_
#define _TOPMENU_MONITOR_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define TOPMENU_TYPE_MONITOR           topmenu_monitor_get_type()
#define TOPMENU_MONITOR(obj)           (G_TYPE_CHECK_INSTANCE_CAST((obj), TOPMENU_TYPE_MONITOR, TopMenuMonitor))
#define TOPMENU_IS_MONITOR(obj)        (G_TYPE_CHECK_INSTANCE_TYPE((obj), TOPMENU_TYPE_MONITOR))
#define TOPMENU_MONITOR_CLASS(c)       (G_TYPE_CHECK_CLASS_CAST((c), TOPMENU_TYPE_MONITOR, TopMenuMonitorClass))
#define TOPMENU_IS_MONITOR_CLASS(c)    (G_TYPE_CHECK_CLASS_TYPE((c), TOPMENU_TYPE_MONITOR))
#define TOPMENU_MONITOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TOPMENU_TYPE_MONITOR, TopMenuMonitorClass))

typedef struct _TopMenuMonitor        TopMenuMonitor;
typedef struct _TopMenuMonitorClass   TopMenuMonitorClass;
typedef struct _TopMenuMonitorPrivate TopMenuMonitorPrivate;

struct _TopMenuMonitor
{
	GObject parent_instance;
	TopMenuMonitorPrivate *priv;
	gboolean available;
};

struct _TopMenuMonitorClass
{
	GObjectClass parent_class;
};

GType topmenu_monitor_get_type(void);

TopMenuMonitor * topmenu_monitor_get_instance(void);

gboolean topmenu_monitor_is_topmenu_available(TopMenuMonitor * self);

G_END_DECLS

#endif /* _TOPMENU_MONITOR_H_ */
