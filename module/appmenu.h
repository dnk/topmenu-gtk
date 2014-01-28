#ifndef _APPMENU_H_
#define _APPMENU_H_

#include <gtk/gtk.h>

typedef struct _AppMenu
{
	GtkMenu *menu;
	GtkMenuItem *about_item;
	GtkMenuItem *prefs_item;
	GtkMenuItem *sep1_item;
	GtkMenuItem *quit_item;
} AppMenu;

GtkWidget * topmenu_appmenu_build(AppMenu *appmenu);

void topmenu_appmenu_scan_for_items(AppMenu *self, GtkMenuShell *menu_shell);

void topmenu_appmenu_destroy(AppMenu *self);

#endif
