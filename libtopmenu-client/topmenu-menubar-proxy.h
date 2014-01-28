#ifndef _TOPMENU_MENUBAR_PROXY_H_
#define _TOPMENU_MENUBAR_PROXY_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define TOPMENU_TYPE_MENUBAR_PROXY           topmenu_menubar_proxy_get_type()
#define TOPMENU_MENUBAR_PROXY(obj)           (G_TYPE_CHECK_INSTANCE_CAST((obj), TOPMENU_TYPE_MENUBAR_PROXY, TopMenuMenuBarProxy))
#define TOPMENU_IS_MENUBAR_PROXY(obj)        (G_TYPE_CHECK_INSTANCE_TYPE((obj), TOPMENU_TYPE_MENUBAR_PROXY))
#define TOPMENU_MENUBAR_PROXY_CLASS(c)       (G_TYPE_CHECK_CLASS_CAST((c), TOPMENU_TYPE_MENUBAR_PROXY, TopMenuMenuBarProxyClass))
#define TOPMENU_IS_MENUBAR_PROXY_CLASS(c)    (G_TYPE_CHECK_CLASS_TYPE((c), TOPMENU_TYPE_MENUBAR_PROXY))
#define TOPMENU_MENUBAR_PROXY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TOPMENU_TYPE_MENUBAR_PROXY, TopMenuMenuBarProxyClass))

typedef struct _TopMenuMenuBarProxy        TopMenuMenuBarProxy;
typedef struct _TopMenuMenuBarProxyClass   TopMenuMenuBarProxyClass;
typedef struct _TopMenuMenuBarProxyPrivate TopMenuMenuBarProxyPrivate;

struct _TopMenuMenuBarProxy
{
	GtkMenuBar parent_instance;
	TopMenuMenuBarProxyPrivate *priv;
	GtkMenuItem *app_menu_item;
	GtkMenu *app_menu;
};

struct _TopMenuMenuBarProxyClass
{
	GtkMenuBarClass parent_class;
};

GType topmenu_menubar_proxy_get_type(void);

TopMenuMenuBarProxy *topmenu_menubar_proxy_new(void);

void topmenu_menubar_proxy_add_menu(TopMenuMenuBarProxy *self, GtkMenuShell *shell);
void topmenu_menubar_proxy_remove_menu(TopMenuMenuBarProxy *self, GtkMenuShell *shell);

typedef enum _MenuItemRole MenuItemRole;
void topmenu_menubar_proxy_add_app_menu_item(TopMenuMenuBarProxy *self, GtkMenuItem *item, MenuItemRole role);

G_END_DECLS

#endif /* _TOPMENU_MENUBAR_PROXY_H_ */
