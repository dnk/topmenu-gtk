#include "topmenu-mate-panel-applet.h"

#include <glib/gi18n.h>
#include <gdk/gdkx.h>

G_DEFINE_TYPE(TopMenuMatePanelApplet, topmenu_mate_panel_applet, PANEL_TYPE_APPLET)

static void display_preferences_dialog(GtkAction *action, TopMenuMatePanelApplet *self)
{
	// TODO
}

static void display_about_dialog(GtkAction *action, TopMenuMatePanelApplet *self)
{
	GtkWindow *parent = NULL;
	GtkWidget *parent_widget = gtk_widget_get_toplevel(GTK_WIDGET(self));
	if (GTK_IS_WINDOW(parent_widget)) {
		parent = GTK_WINDOW(parent_widget);
	}

	gtk_show_about_dialog(parent,
	                      "program-name", "TopMenu Mate Panel Applet",
	                      NULL);
}

static const GtkActionEntry menu_verbs[] = {
	{ "TopMenuPreferences", GTK_STOCK_PROPERTIES, N_("_Preferences"),
	  NULL, NULL,
	  G_CALLBACK (display_preferences_dialog) },
	{ "TopMenuAbout", GTK_STOCK_ABOUT, N_("_About"),
	  NULL, NULL,
	  G_CALLBACK (display_about_dialog) }
};

static void topmenu_mate_panel_applet_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
	TopMenuMatePanelApplet *self = TOPMENU_MATE_PANEL_APPLET(widget);
	if (self->menu_widget) {
		gtk_widget_size_allocate(GTK_WIDGET(self->menu_widget), allocation);
	}
	GTK_WIDGET_CLASS(topmenu_mate_panel_applet_parent_class)->size_allocate(widget, allocation);
}

static void topmenu_mate_panel_applet_size_request(GtkWidget *widget, GtkRequisition *requisition)
{
	TopMenuMatePanelApplet *self = TOPMENU_MATE_PANEL_APPLET(widget);
	if (self->menu_widget) {
		gtk_widget_size_request(GTK_WIDGET(self->menu_widget), requisition);
	}
}

static void topmenu_mate_panel_applet_class_init(TopMenuMatePanelAppletClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
	widget_class->size_allocate = topmenu_mate_panel_applet_size_allocate;
	widget_class->size_request = topmenu_mate_panel_applet_size_request;
}

static void topmenu_mate_panel_applet_init(TopMenuMatePanelApplet *self)
{
	self->menu_widget = TOPMENU_WIDGET(topmenu_widget_new());
	gtk_widget_set_can_focus(GTK_WIDGET(self->menu_widget), TRUE);
	gtk_container_add(GTK_CONTAINER(self), GTK_WIDGET(self->menu_widget));

	GtkActionGroup *action_group = gtk_action_group_new("TopMenu Mate Panel Applet Actions");
	gtk_action_group_add_actions(action_group,
	                             menu_verbs, G_N_ELEMENTS(menu_verbs), self);

	mate_panel_applet_set_flags(MATE_PANEL_APPLET(self),
	                            MATE_PANEL_APPLET_EXPAND_MINOR);
	mate_panel_applet_setup_menu(MATE_PANEL_APPLET(self),
	                             "<menuitem name=\"TopMenu Preferences Item\" action=\"TopMenuPreferences\"/>"
	                             "<menuitem name=\"TopMenu About Item\" action=\"TopMenuAbout\"/>",
	                             action_group);

	g_object_unref(action_group);
}

MatePanelApplet *topmenu_mate_panel_applet_new(void)
{
	return MATE_PANEL_APPLET(g_object_new(TOPMENU_TYPE_MATE_PANEL_APPLET, NULL));
}
