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
