#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>

#include "../global.h"

#include "topmenu-widget.h"
#include "topmenu-server.h"

#ifdef HAVE_MATEWNCK
#include <libmatewnck/libmatewnck.h>
#endif

struct _TopMenuWidgetPrivate
{
	Atom atom_window;
	Atom atom_transient_for;
	GQueue followed_windows;
#ifdef HAVE_MATEWNCK
	MatewnckScreen *screen;
#endif
};

G_DEFINE_TYPE(TopMenuWidget, topmenu_widget, GTK_TYPE_BIN)

#define TOPMENU_WIDGET_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), TOPMENU_TYPE_WIDGET, TopMenuWidgetPrivate))

static Window read_window_property(Display *dpy, Window window, Atom property)
{
	Atom actual_type;
	int actual_format;
	unsigned long nitems, bytes_after;
	unsigned char *prop_return;

	if (XGetWindowProperty(dpy, window, property,
	                       0, sizeof(Window), False,
	                       XA_WINDOW, &actual_type, &actual_format, &nitems,
	                       &bytes_after, &prop_return) == Success) {
		if (prop_return && actual_type == XA_WINDOW) {
			return *(Window*)prop_return;
		}
	}

	return None;
}

static Display * topmenu_widget_get_display(TopMenuWidget *self)
{
	GdkWindow *gdk_win = gtk_widget_get_window(GTK_WIDGET(self));
	if (gdk_win) {
		return GDK_WINDOW_XDISPLAY(gdk_win);
	}
	return NULL;
}

static Window topmenu_widget_get_toplevel_xwindow(TopMenuWidget *self)
{
	GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(self));
	GdkWindow *window = gtk_widget_get_window(toplevel);
	if (window) {
		return GDK_WINDOW_XID(window);
	} else {
		return None;
	}
}

static Window topmenu_widget_get_current_active_window(TopMenuWidget *self)
{
#ifdef HAVE_MATEWNCK
	MatewnckWindow *window = matewnck_screen_get_active_window(self->priv->screen);
	if (window) {
		return matewnck_window_get_xid(window);
	} else {
		return None;
	}
#else
	return None;
#endif
}

static Window topmenu_widget_get_session_leader(TopMenuWidget *self, Window window)
{
#ifdef HAVE_MATEWNCK
	MatewnckWindow *w = matewnck_window_get(window);
	if (w) {
		return matewnck_window_get_group_leader(w);
	} else {
		return None;
	}
#else
	return None;
#endif
}

static Window topmenu_widget_get_any_app_window_with_menu(TopMenuWidget *self, Window window)
{
#ifdef HAVE_MATEWNCK
	Display *dpy = topmenu_widget_get_display(self);

	MatewnckWindow *w = matewnck_window_get(window);
	if (!w) return None;

	MatewnckApplication *app = matewnck_window_get_application(w);
	if (!app) return None;

	GList *i, *windows = matewnck_screen_get_windows_stacked(self->priv->screen);
	if (!windows) return None;

	for (i = g_list_last(windows); i; i = g_list_previous(i)) {
		if (i->data != w && matewnck_window_get_application(i->data) == app) {
			Window candidate = matewnck_window_get_xid(i->data);
			Window menu_window = read_window_property(dpy, candidate, self->priv->atom_window);
			if (menu_window) {
				return candidate;
			}
		}
	}
	return None;
#else
	return None;
#endif
}

static void topmenu_widget_embed_topmenu_window(TopMenuWidget *self, Window window)
{
	g_return_if_fail(self->socket);
	GdkWindow *cur = gtk_socket_get_plug_window(self->socket);

	if (cur) {
		if (GDK_WINDOW_XID(cur) == window) {
			// Trying to embed the same client again
			return; // Nothing to do
		}

		// Otherwise, disembed the current client
		g_debug("Disembedding window 0x%lx", GDK_WINDOW_XID(cur));
		gdk_window_hide(cur);

		// Reparent back to root window to end embedding
		GdkScreen *screen = gdk_window_get_screen(cur);
		gdk_window_reparent(cur, gdk_screen_get_root_window(screen), 0, 0);
	}

	if (window) {
		g_debug("Embedding window 0x%lx", window);
		gtk_socket_add_id(self->socket, window);
	}
}

static gboolean topmenu_widget_try_window(TopMenuWidget *self, Window window)
{
	Display *dpy = topmenu_widget_get_display(self);
	g_return_val_if_fail(dpy, FALSE);
	g_return_val_if_fail(window, FALSE);

	Window menu_window = read_window_property(dpy, window, self->priv->atom_window);
	if (menu_window) {
		topmenu_widget_embed_topmenu_window(self, menu_window);
		return TRUE;
	}

	return FALSE;
}

static gboolean topmenu_widget_follow_window(TopMenuWidget *self, Window window)
{
	Display *dpy = topmenu_widget_get_display(self);
	g_return_val_if_fail(dpy, FALSE);
	g_return_val_if_fail(window, FALSE);

	if (window == topmenu_widget_get_toplevel_xwindow(self)) {
		return FALSE; // Ignore the window this widget is on as a candidate
	}

	// Add this window to the list of windows we are following
	g_queue_push_head(&self->priv->followed_windows, GSIZE_TO_POINTER(window));

	XWindowAttributes win_attrs;
	if (XGetWindowAttributes(dpy, window, &win_attrs)) {
		XSelectInput(dpy, window, win_attrs.your_event_mask | PropertyChangeMask);
	}

	if (topmenu_widget_try_window(self, window)) {
		// Found a menu bar on this window
		return TRUE;
	} else {
		// This window had no menu bar, so let's check its transient_for windows.
		Window transient_for;
		if (XGetTransientForHint(dpy, window, &transient_for)) {
			if (topmenu_widget_follow_window(self, transient_for)) {
				return TRUE;
			}
		}

		// Also see if its client leader has a global menu bar....
		Window leader = topmenu_widget_get_session_leader(self, window);
		if (leader && leader != window) {
			if (topmenu_widget_follow_window(self, leader)) {
				return TRUE;
			}
		}

		// Otherwise, if this program has more than one window, then let's search
		// for any other window with a menu bar
		Window other = topmenu_widget_get_any_app_window_with_menu(self, window);
		if (other && other != window) {
			if (topmenu_widget_follow_window(self, other)) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

static void topmenu_widget_set_followed_window(TopMenuWidget *self, Window window)
{
	Display *dpy = topmenu_widget_get_display(self);
	g_return_if_fail(dpy);

	g_debug("Setting active window to 0x%lx", window);

	// Clear the list of currently followed windows.
	g_queue_clear(&self->priv->followed_windows);

	if (window) {
		// Initialize atoms now
		if (self->priv->atom_window == None) {
			self->priv->atom_window = XInternAtom(dpy, ATOM_TOPMENU_WINDOW, False);
		}
		if (self->priv->atom_transient_for) {
			self->priv->atom_transient_for = XInternAtom(dpy, "WM_TRANSIENT_FOR", False);
		}

		// Start by checking the active window
		// This will recursively check its transient_for windows.
		if (topmenu_widget_follow_window(self, window)) {
			g_debug("Also following %d windows",
			        g_queue_get_length(&self->priv->followed_windows));
			return;
		}

		// Otherwise fallback to "no menu bar".
		g_debug("Active window has no menu bar; following %d windows",
		        g_queue_get_length(&self->priv->followed_windows));
	}

	topmenu_widget_embed_topmenu_window(self, None);
}

static void handle_socket_realize(GtkSocket *socket, TopMenuWidget *self)
{
	// Workaround a "bug workaround" where GtkSocket will not select ButtonPress
	// events
	g_warn_if_fail(gtk_widget_get_realized(GTK_WIDGET(socket)));
	gtk_widget_add_events(GTK_WIDGET(socket),
	                      GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
}

static gboolean handle_socket_plug_removed(GtkSocket *socket, TopMenuWidget *self)
{
	g_debug("Plug has been removed");
	// No need to do anything
	return TRUE; // Do not destroy the socket
}

#ifdef HAVE_MATEWNCK
static void handle_active_window_changed(MatewnckScreen *screen, MatewnckWindow *prev_window, TopMenuWidget *self)
{
	if (!gtk_widget_get_visible(GTK_WIDGET(self))) {
		return;
	}
	MatewnckWindow *window = matewnck_screen_get_active_window(screen);
	if (window) {
		topmenu_widget_set_followed_window(self, matewnck_window_get_xid(window));
	} else {
		// No active window?
	}
}
#endif

static GdkFilterReturn handle_gdk_event(GdkXEvent *xevent, GdkEvent *event, gpointer data)
{
	TopMenuWidget *self = TOPMENU_WIDGET(data);
	XEvent *e = (XEvent*) xevent;

	if (e->type == PropertyNotify &&
	        (e->xproperty.atom == self->priv->atom_transient_for ||
	         e->xproperty.atom == self->priv->atom_window)) {
		// One of the properties we are interested in changed.
		// See if it's one of the windows we're following.
		if (g_queue_find(&self->priv->followed_windows,
		                 GSIZE_TO_POINTER(e->xproperty.window))) {
			// If so, try refollowing the currently followed window in order
			// to see if any window has suddenly grown a menu bar.
			g_debug("One of our followed windows changed");
			Window window = GPOINTER_TO_SIZE(g_queue_peek_tail(&self->priv->followed_windows));
			topmenu_widget_set_followed_window(self, window);
		}
	}

	return GDK_FILTER_CONTINUE;
}

static void topmenu_widget_map(GtkWidget *widget)
{
	TopMenuWidget *self = TOPMENU_WIDGET(widget);
	topmenu_server_register_server_widget(widget);
	topmenu_widget_set_followed_window(self,
	                                   topmenu_widget_get_current_active_window(self));
	GTK_WIDGET_CLASS(topmenu_widget_parent_class)->map(widget);
}

static void topmenu_widget_unmap(GtkWidget *widget)
{
	TopMenuWidget *self = TOPMENU_WIDGET(widget);
	topmenu_widget_set_followed_window(self, None);
	topmenu_server_unregister_server_widget(widget);
	GTK_WIDGET_CLASS(topmenu_widget_parent_class)->unmap(widget);
}

static void topmenu_widget_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
	TopMenuWidget *self = TOPMENU_WIDGET(widget);
	if (self->socket) {
		gtk_widget_size_allocate(GTK_WIDGET(self->socket), allocation);
	}
	GTK_WIDGET_CLASS(topmenu_widget_parent_class)->size_allocate(widget, allocation);
}

#if GTK_MAJOR_VERSION == 3
static void topmenu_widget_get_preferred_width(GtkWidget *widget, gint *minimal_width, gint *natural_width)
{
	TopMenuWidget *self = TOPMENU_WIDGET(widget);
	if (self->socket) {
		gtk_widget_get_preferred_width(GTK_WIDGET(self->socket), minimal_width, natural_width);
	}
}

static void topmenu_widget_get_preferred_height(GtkWidget *widget, gint *minimal_height, gint *natural_height)
{
	TopMenuWidget *self = TOPMENU_WIDGET(widget);
	if (self->socket) {
		gtk_widget_get_preferred_height(GTK_WIDGET(self->socket), minimal_height, natural_height);
	}
}
#elif GTK_MAJOR_VERSION == 2
static void topmenu_widget_size_request(GtkWidget *widget, GtkRequisition *requisition)
{
	TopMenuWidget *self = TOPMENU_WIDGET(widget);
	if (self->socket) {
		gtk_widget_size_request(GTK_WIDGET(self->socket), requisition);
	}
}
#endif

static void topmenu_widget_dispose(GObject *obj)
{
	TopMenuWidget *self = TOPMENU_WIDGET(obj);
	gdk_window_remove_filter(NULL, handle_gdk_event, self);
	if (self->socket) {
		g_signal_handlers_disconnect_by_data(self->socket, self);
		self->socket = NULL;
	}
	g_queue_clear(&self->priv->followed_windows);
#ifdef HAVE_MATEWNCK
	if (self->priv->screen) {
		g_signal_handlers_disconnect_by_data(self->priv->screen, self);
		self->priv->screen = NULL;
	}
#endif
	G_OBJECT_CLASS(topmenu_widget_parent_class)->dispose(obj);
}

static void topmenu_widget_class_init(TopMenuWidgetClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
	widget_class->map = topmenu_widget_map;
	widget_class->unmap = topmenu_widget_unmap;
	widget_class->size_allocate = topmenu_widget_size_allocate;
#if GTK_MAJOR_VERSION == 3
	widget_class->get_preferred_width = topmenu_widget_get_preferred_width;
	widget_class->get_preferred_height = topmenu_widget_get_preferred_height;
#elif GTK_MAJOR_VERSION == 2
	widget_class->size_request = topmenu_widget_size_request;
#endif

	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	obj_class->dispose = topmenu_widget_dispose;

	g_type_class_add_private(klass, sizeof(TopMenuWidgetPrivate));
}

static void topmenu_widget_init(TopMenuWidget *self)
{
	self->priv = TOPMENU_WIDGET_GET_PRIVATE(self);
	self->socket = GTK_SOCKET(gtk_socket_new());
	g_signal_connect_after(self->socket, "realize",
	                       G_CALLBACK(handle_socket_realize), self);
	g_signal_connect(self->socket, "plug-removed",
	                 G_CALLBACK(handle_socket_plug_removed), self);
	self->priv->atom_window = None;
	self->priv->atom_transient_for = None;
	g_queue_init(&self->priv->followed_windows);
#ifdef HAVE_MATEWNCK
	self->priv->screen = matewnck_screen_get_default();
	g_signal_connect(self->priv->screen, "active-window-changed",
	                 G_CALLBACK(handle_active_window_changed), self);
#endif
	gdk_window_add_filter(NULL, handle_gdk_event, self);
	gtk_container_add(GTK_CONTAINER(self), GTK_WIDGET(self->socket));
}

GtkWidget *topmenu_widget_new(void)
{
	return GTK_WIDGET(g_object_new(TOPMENU_TYPE_WIDGET, NULL));
}
