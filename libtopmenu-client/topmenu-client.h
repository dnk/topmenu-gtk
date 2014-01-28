#ifndef _TOPMENU_CLIENT_H_
#define _TOPMENU_CLIENT_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

void topmenu_client_connect_window_widget(GdkWindow *window, GtkWidget *widget);
void topmenu_client_disconnect_window(GdkWindow *window);

G_END_DECLS

#endif
