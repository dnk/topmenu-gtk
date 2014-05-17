#include "topmenu-appmenubar.h"

G_DEFINE_TYPE(TopMenuAppMenuBar, topmenu_app_menu_bar, GTK_TYPE_MENU_BAR)

enum {
	PROP_0,
	PROP_APP_MENU,
	N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES] = { NULL };

static void topmenu_app_menu_bar_get_property(GObject *obj, guint property_id, GValue *value, GParamSpec *pspec)
{
	TopMenuAppMenuBar *self = TOPMENU_APP_MENU_BAR(obj);
	switch (property_id) {
	case PROP_APP_MENU:
		g_value_set_object(value, topmenu_app_menu_bar_get_app_menu(self));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
	}
}

static void topmenu_app_menu_bar_set_property(GObject *obj, guint property_id, const GValue *value, GParamSpec *pspec)
{
	TopMenuAppMenuBar *self = TOPMENU_APP_MENU_BAR(obj);
	switch (property_id) {
	case PROP_APP_MENU:
		topmenu_app_menu_bar_set_app_menu(self, g_value_get_object(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
	}
}

static void topmenu_app_menu_bar_class_init(TopMenuAppMenuBarClass *klass)
{
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	obj_class->get_property = topmenu_app_menu_bar_get_property;
	obj_class->set_property = topmenu_app_menu_bar_set_property;

	properties[PROP_APP_MENU] = g_param_spec_object("app-menu",
	                                                "Application menu",
	                                                "The application menu (shown under the application name)",
	                                                GTK_TYPE_MENU,
	                                                G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE);

	g_object_class_install_properties(obj_class, N_PROPERTIES, properties);

#if GTK_MAJOR_VERSION == 2
	gtk_rc_parse_string (
		"style \"app-menubar-style\"\n"
		"{\n"
		"  GtkMenuBar::shadow-type = none\n"
		"  GtkMenuBar::internal-padding = 0\n"
		"}\n"
		"class \"TopMenuAppMenuBar\" style \"app-menubar-style\"");
#endif
}

static void topmenu_app_menu_bar_init(TopMenuAppMenuBar *self)
{
#if GTK_MAJOR_VERSION == 3
	GError *error = NULL;
	GtkCssProvider *provider = gtk_css_provider_new();
	GtkStyleContext *style_context = gtk_widget_get_style_context(GTK_WIDGET(self));
	static const char *css =
	        "TopMenuAppMenuBar {\n"
	        "   box-shadow: none;\n"
	        "	padding: 0;\n"
	        "	background-color: @os_chrome_bg_color;\n"
			"	background-image: none;\n"
			"	color: @os_chrome_fg_color;\n"
	        "}\n"
	        "\n"
			"TopMenuAppMenuBar .menu .menuitem *:active {\n"
	        "	color: @theme_text_color;\n"
			"}\n"
			"\n"
			"TopMenuAppMenuBar .menu .menuitem *:selected {\n"
	        "	color: @theme_selected_fg_color;\n"
			"}\n";
	if (gtk_css_provider_load_from_data(provider, css, -1, &error)) {
		gtk_style_context_add_provider(style_context,
		                               GTK_STYLE_PROVIDER(provider),
		                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	} else {
		g_warning("Error while loading CSS: %s", error->message);
		g_error_free(error);
	}
#endif

	self->app_menu_item = GTK_MENU_ITEM(gtk_menu_item_new_with_label(g_get_application_name()));
	GtkLabel *app_label = GTK_LABEL(gtk_bin_get_child(GTK_BIN(self->app_menu_item)));
	PangoAttrList *app_label_attr = pango_attr_list_new();
	pango_attr_list_insert(app_label_attr, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
	gtk_label_set_attributes(app_label, app_label_attr);
	pango_attr_list_unref(app_label_attr);
	gtk_widget_show(GTK_WIDGET(self->app_menu_item));

	gtk_menu_shell_prepend(GTK_MENU_SHELL(self), GTK_WIDGET(self->app_menu_item));
}

TopMenuAppMenuBar *topmenu_app_menu_bar_new(void)
{
	return TOPMENU_APP_MENU_BAR(g_object_new(TOPMENU_TYPE_APP_MENU_BAR, NULL));
}

void topmenu_app_menu_bar_set_app_menu(TopMenuAppMenuBar *self, GtkWidget *menu)
{
	gtk_menu_item_set_submenu(self->app_menu_item, GTK_WIDGET(menu));
}

GtkWidget *topmenu_app_menu_bar_get_app_menu(TopMenuAppMenuBar *self)
{
	return gtk_menu_item_get_submenu(self->app_menu_item);
}
