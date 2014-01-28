#ifndef _TOPMENU_MATE_PANEL_APPLET_H_
#define _TOPMENU_MATE_PANEL_APPLET_H_

#include <mate-panel-applet.h>
#include "../libtopmenu-server/topmenu-widget.h"

G_BEGIN_DECLS

#define TOPMENU_TYPE_MATE_PANEL_APPLET           topmenu_mate_panel_applet_get_type()
#define TOPMENU_MATE_PANEL_APPLET(obj)           (G_TYPE_CHECK_INSTANCE_CAST((obj), TOPMENU_TYPE_MATE_PANEL_APPLET, TopMenuMatePanelApplet))
#define TOPMENU_IS_MATE_PANEL_APPLET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE((obj), TOPMENU_TYPE_MATE_PANEL_APPLET))
#define TOPMENU_MATE_PANEL_APPLET_CLASS(c)       (G_TYPE_CHECK_CLASS_CAST((c), TOPMENU_TYPE_MATE_PANEL_APPLET, TopMenuMatePanelAppletClass))
#define TOPMENU_IS_MATE_PANEL_APPLET_CLASS(c)    (G_TYPE_CHECK_CLASS_TYPE((c), TOPMENU_TYPE_MATE_PANEL_APPLET))
#define TOPMENU_MATE_PANEL_APPLET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TOPMENU_TYPE_MATE_PANEL_APPLET, TopMenuMatePanelAppletClass))

typedef struct _TopMenuMatePanelApplet        TopMenuMatePanelApplet;
typedef struct _TopMenuMatePanelAppletClass   TopMenuMatePanelAppletClass;

struct _TopMenuMatePanelApplet
{
	MatePanelApplet parent_instance;
	TopMenuWidget *menu_widget;
	GdkWindow *cur_plug;
	GtkActionGroup *actions;
};

struct _TopMenuMatePanelAppletClass
{
	MatePanelAppletClass parent_class;
};

GType topmenu_mate_panel_applet_get_type(void);

MatePanelApplet *topmenu_mate_panel_applet_new(void);

G_END_DECLS

#endif /* _TOPMENU_MATE_PANEL_APPLET_H_ */
