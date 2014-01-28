#include <string.h>

#include "appmenu.h"
#include "data.h"
#include "menuitem-proxy.h"

typedef enum _AppMenuRole {
	APP_MENU_ROLE_NONE = 0,
	APP_MENU_ROLE_ABOUT,
	APP_MENU_ROLE_PREFERENCES,
	APP_MENU_ROLE_QUIT,
	APP_MENU_ROLE_MAX
} AppMenuRole;

static AppMenuRole detect_role_by_stock(const gchar *stock_id)
{
	if (g_strcmp0(stock_id, GTK_STOCK_PREFERENCES) == 0) {
		return APP_MENU_ROLE_PREFERENCES;
	} else if (g_strcmp0(stock_id, GTK_STOCK_QUIT) == 0) {
		return APP_MENU_ROLE_QUIT;
	} else if (g_strcmp0(stock_id, GTK_STOCK_ABOUT) == 0) {
		return APP_MENU_ROLE_ABOUT;
	} else {
		return APP_MENU_ROLE_NONE;
	}
}

static AppMenuRole detect_item_role(GtkMenuItem *item)
{
	if (GTK_IS_IMAGE_MENU_ITEM(item)) {
		GtkImageMenuItem *iitem = GTK_IMAGE_MENU_ITEM(item);
		if (gtk_image_menu_item_get_use_stock(iitem)) {
			return detect_role_by_stock(gtk_menu_item_get_label(item));
		} else {
			GtkWidget *iwidget = gtk_image_menu_item_get_image(iitem);
			if (GTK_IS_IMAGE(iwidget)) {
				GtkImage *image = GTK_IMAGE(iwidget);
				if (gtk_image_get_storage_type(image) == GTK_IMAGE_STOCK) {
					gchar *stock_id;
					GtkIconSize icon_size;
					gtk_image_get_stock(image, &stock_id, &icon_size);
					return detect_role_by_stock(stock_id);
				}
			}
		}
	}

	return APP_MENU_ROLE_NONE;
}

static void handle_default_quit(GtkMenuItem *item, gpointer user_data)
{
	gtk_main_quit();
}

static GtkMenuItem *create_separator()
{
	GtkWidget *sep = gtk_separator_menu_item_new();
	gtk_widget_show(sep);
	return GTK_MENU_ITEM(sep);
}

static GtkMenuItem *create_default_exit()
{
	GtkMenuItem *item = GTK_MENU_ITEM(gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL));
	g_signal_connect(item, "activate", G_CALLBACK(handle_default_quit), NULL);
	gtk_widget_show_all(GTK_WIDGET(item));
	return item;
}

static gint get_role_position(AppMenu *self, AppMenuRole role)
{
	gint position = 0;
	switch (role) {
	case APP_MENU_ROLE_MAX:
		if (self->quit_item) position++;
		if (self->sep1_item) position++;
	case APP_MENU_ROLE_QUIT:
		if (self->prefs_item) position++;
	case APP_MENU_ROLE_PREFERENCES:
		if (self->about_item) position++;
	case APP_MENU_ROLE_ABOUT:
		break;
	default:
		g_warn_if_reached();
		break;
	}
	return position;
}

static void set_item_for_role(AppMenu *self, GtkMenuItem *item, AppMenuRole role)
{
	g_return_if_fail(role != APP_MENU_ROLE_NONE);

	switch (role) {
	case APP_MENU_ROLE_ABOUT:
		if (self->about_item) {
			gtk_widget_destroy(GTK_WIDGET(self->about_item));
			self->about_item = NULL;
		}
		if (item) {
			if (self->quit_item && !self->sep1_item) {
				self->sep1_item = create_separator();
				gtk_menu_shell_insert(GTK_MENU_SHELL(self->menu),
				                      GTK_WIDGET(self->sep1_item),
				                      get_role_position(self, APP_MENU_ROLE_QUIT));
			}
			self->about_item = item;
			gtk_menu_shell_insert(GTK_MENU_SHELL(self->menu),
			                      GTK_WIDGET(self->about_item),
			                      get_role_position(self, APP_MENU_ROLE_ABOUT));
		}
		break;
	case APP_MENU_ROLE_PREFERENCES:
		if (self->prefs_item) {
			gtk_widget_destroy(GTK_WIDGET(self->prefs_item));
			self->prefs_item = NULL;
		}
		if (item) {
			if (self->quit_item && !self->sep1_item) {
				self->sep1_item = create_separator();
				gtk_menu_shell_insert(GTK_MENU_SHELL(self->menu),
				                      GTK_WIDGET(self->sep1_item),
				                      get_role_position(self, APP_MENU_ROLE_QUIT));
			}
			self->prefs_item = item;
			gtk_menu_shell_insert(GTK_MENU_SHELL(self->menu),
			                      GTK_WIDGET(self->prefs_item),
			                      get_role_position(self, APP_MENU_ROLE_PREFERENCES));
		}
		break;
	case APP_MENU_ROLE_QUIT:
		if (self->quit_item) {
			gtk_widget_destroy(GTK_WIDGET(self->quit_item));
			self->quit_item = NULL;
		}
		if (item) {
			if ((self->about_item || self->prefs_item) && !self->sep1_item) {
				self->sep1_item = create_separator();
				gtk_menu_shell_insert(GTK_MENU_SHELL(self->menu),
				                      GTK_WIDGET(self->sep1_item),
				                      get_role_position(self, APP_MENU_ROLE_QUIT));
			}
			self->quit_item = item;
			gtk_menu_shell_insert(GTK_MENU_SHELL(self->menu),
			                      GTK_WIDGET(self->quit_item),
			                      get_role_position(self, APP_MENU_ROLE_QUIT) + 1);
		}
		break;
	default:
		g_warn_if_reached();
	}
}

GtkWidget * topmenu_appmenu_build(AppMenu *self)
{
	self->menu = GTK_MENU(gtk_menu_new());

	return GTK_WIDGET(self->menu);
}

typedef struct _MenuScanData
{
	AppMenu *appmenu;
	gint depth;
} MenuScanData;

static void appmenu_scan_cb(GtkWidget *widget, gpointer user_data)
{
	MenuScanData *data = user_data;
	g_return_if_fail(GTK_IS_MENU_ITEM(widget));

	GtkMenuItem *item = GTK_MENU_ITEM(widget);
	GtkWidget *submenu = gtk_menu_item_get_submenu(item);

	if (!submenu) {
		MenuItemData *item_data = topmenu_get_menu_item_data(item);
		if (item_data && item_data->proxy) {
			submenu = gtk_menu_item_get_submenu(item_data->proxy);
		}
	}

	if (submenu) {
		data->depth--;
		if (data->depth >= 0) {
			g_warn_if_fail(GTK_IS_CONTAINER(submenu));
			gtk_container_foreach(GTK_CONTAINER(submenu), appmenu_scan_cb, data);
		}
		data->depth++;
	} else {
		AppMenuRole role = detect_item_role(item);
		if (role != APP_MENU_ROLE_NONE) {
			GtkMenuItem *proxy = topmenu_create_proxy_menu_item(item);
			set_item_for_role(data->appmenu, proxy, role);
		}
	}
}

void topmenu_appmenu_scan_for_items(AppMenu *self, GtkMenuShell *menu_shell)
{
	g_return_if_fail(GTK_IS_MENU(self->menu));

	MenuScanData data;
	data.appmenu = self;
	data.depth = 1;
	gtk_container_foreach(GTK_CONTAINER(menu_shell), appmenu_scan_cb, &data);

	// Create default items
	if (!self->quit_item) {
		GtkMenuItem *item = create_default_exit();
		set_item_for_role(self, item, APP_MENU_ROLE_QUIT);
	}
}

void topmenu_appmenu_destroy(AppMenu *self)
{
	if (self->menu) {
		gtk_widget_destroy(GTK_WIDGET(self->menu));
		self->menu = NULL;
	}
	memset(self, 0, sizeof(AppMenu));
}
