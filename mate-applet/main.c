#include <string.h>

#include "topmenu-mate-panel-applet.h"

static gboolean topmenu_mate_panel_applet_factory(MatePanelApplet *applet,
                                                  const gchar *iid,
                                                  gpointer data)
{
	if (strcmp(iid, "TopMenuMatePanelApplet") == 0) {
		gtk_widget_show_all(GTK_WIDGET(applet));
		return TRUE;
	}

	return FALSE;
}

MATE_PANEL_APPLET_OUT_PROCESS_FACTORY("TopMenuMatePanelAppletFactory",
                                      TOPMENU_TYPE_MATE_PANEL_APPLET,
                                      "TopMenuMatePanelApplet",
                                      topmenu_mate_panel_applet_factory,
                                      NULL)
