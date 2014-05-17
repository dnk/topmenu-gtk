#include <X11/Xatom.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include "../global.h"

#include "topmenu-monitor.h"

struct _TopMenuMonitorPrivate
{
	GdkAtom atom_selection;
	GtkClipboard *selection;
	GdkWindow *cur_server;
};

enum {
	PROP_0,
	PROP_AVAILABLE,
	N_PROPERTIES
};

G_DEFINE_TYPE(TopMenuMonitor, topmenu_monitor, G_TYPE_OBJECT)

#define TOPMENU_MONITOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), TOPMENU_TYPE_MONITOR, TopMenuMonitorPrivate))

static GParamSpec *properties[N_PROPERTIES] = { NULL };

static void topmenu_monitor_update(TopMenuMonitor *self);

static void handle_clipboard_owner_change(GtkClipboard *clipboard, GdkEvent *event, TopMenuMonitor *self)
{
	topmenu_monitor_update(self);
}

static GdkFilterReturn handle_cur_server_event(GdkXEvent *xevent, GdkEvent *event, gpointer data)
{
	XEvent *e = (XEvent*)xevent;
	if (e->type == DestroyNotify) {
		g_debug("Current server has been destroyed");
		TopMenuMonitor *self = TOPMENU_MONITOR(data);
		if (self->priv->cur_server &&
		        gdk_x11_window_get_xid(self->priv->cur_server) == e->xdestroywindow.window) {
			topmenu_monitor_update(self);
		}
	}
	return GDK_FILTER_CONTINUE;
}

static void topmenu_monitor_set_cur_server(TopMenuMonitor *self, GdkWindow *window)
{
	if (self->priv->cur_server == window) {
		// Nothing to do
		return;
	}
	g_debug("Setting current server to 0x%lx", gdk_x11_window_get_xid(window));
	if (self->priv->cur_server) {
		gdk_window_remove_filter(window, handle_cur_server_event, self);
		g_object_unref(self->priv->cur_server);
		self->priv->cur_server = 0;
	}
	if (window) {
		gdk_window_set_events(window, gdk_window_get_events(window) | GDK_STRUCTURE_MASK);
		gdk_window_add_filter(window, handle_cur_server_event, self);
		self->priv->cur_server = window;
	}
	if (self->priv->cur_server && !self->available) {
		// Signal availability
		self->available = TRUE;
		g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_AVAILABLE]);
	} else if (!self->priv->cur_server && self->available) {
		// Signal no availability
		self->available = FALSE;
		g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_AVAILABLE]);
	}
}

static void topmenu_monitor_update(TopMenuMonitor *self)
{
	GdkScreen *screen = gdk_screen_get_default();
	GdkDisplay *display = gdk_screen_get_display(screen);

	Display *xdpy = GDK_DISPLAY_XDISPLAY(display);
	Atom atom = gdk_x11_atom_to_xatom_for_display(display, self->priv->atom_selection);

	Window xwin = XGetSelectionOwner(xdpy, atom);

	if (xwin) {
		GdkWindow *window = gdk_x11_window_foreign_new_for_display(display, xwin);
		topmenu_monitor_set_cur_server(self, window);
	} else {
		topmenu_monitor_set_cur_server(self, NULL);
	}
}

static void topmenu_monitor_get_property(GObject *obj, guint property_id, GValue *value, GParamSpec *pspec)
{
	TopMenuMonitor *self = TOPMENU_MONITOR(obj);
	switch (property_id) {
	case PROP_AVAILABLE:
		g_value_set_boolean(value, self->available);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
	}
}

static void topmenu_monitor_dispose(GObject *obj)
{
	TopMenuMonitor *self = TOPMENU_MONITOR(obj);
	if (self->priv->cur_server) {
		gdk_window_remove_filter(self->priv->cur_server,
		                         handle_cur_server_event, self);
		g_object_unref(self->priv->cur_server);
		self->priv->cur_server = 0;
	}
	self->priv->selection = NULL;
	G_OBJECT_CLASS(topmenu_monitor_parent_class)->dispose(obj);
}

static void topmenu_monitor_class_init(TopMenuMonitorClass *klass)
{
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	obj_class->get_property = topmenu_monitor_get_property;
	obj_class->dispose = topmenu_monitor_dispose;

	properties[PROP_AVAILABLE] = g_param_spec_boolean("available",
	                                                  "TopMenu's availability",
	                                                  "Set to TRUE whether a TopMenu server is currently available",
	                                                  FALSE,
	                                                  G_PARAM_STATIC_STRINGS | G_PARAM_READABLE);
	g_object_class_install_properties(obj_class, N_PROPERTIES, properties);

	g_type_class_add_private(klass, sizeof(TopMenuMonitorPrivate));
}

static void topmenu_monitor_init(TopMenuMonitor *self)
{
	self->priv = TOPMENU_MONITOR_GET_PRIVATE(self);
	self->available = FALSE;

	self->priv->atom_selection = gdk_atom_intern_static_string(ATOM_TOPMENU_SERVER_SELECTION);
	self->priv->selection = gtk_clipboard_get(self->priv->atom_selection);
	self->priv->cur_server = NULL;

	g_signal_connect_object(self->priv->selection, "owner-change",
	                        G_CALLBACK(handle_clipboard_owner_change), self, 0);

	topmenu_monitor_update(self);
}

TopMenuMonitor * topmenu_monitor_get_instance()
{
	static TopMenuMonitor *instance = NULL;
	if (!instance) {
		instance = TOPMENU_MONITOR(g_object_new(TOPMENU_TYPE_MONITOR, NULL));
	}
	return instance;
}

gboolean topmenu_monitor_is_topmenu_available(TopMenuMonitor * self)
{
	return self->available;
}
