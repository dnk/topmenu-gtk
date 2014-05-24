#include <stdlib.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "../libtopmenu-client/topmenu-client.h"
#include "../libtopmenu-client/topmenu-monitor.h"

static GtkWindow *mainwin;

static GtkWidget * create_menu_bar(void)
{
	GtkMenuBar *bar = GTK_MENU_BAR(gtk_menu_bar_new());
	GtkMenuItem *app = GTK_MENU_ITEM(gtk_menu_item_new_with_label("Client"));
	GtkMenuItem *file = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic("_File"));
	GtkMenuItem *edit = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic("_Edit"));
	GtkMenuItem *help = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic("_Help"));

	GtkLabel *app_label = GTK_LABEL(gtk_bin_get_child(GTK_BIN(app)));
	gtk_label_set_markup(app_label, "<b>Client</b>");

	gtk_menu_shell_append(GTK_MENU_SHELL(bar), GTK_WIDGET(app));
	gtk_menu_shell_append(GTK_MENU_SHELL(bar), GTK_WIDGET(file));
	gtk_menu_shell_append(GTK_MENU_SHELL(bar), GTK_WIDGET(edit));
	gtk_menu_shell_append(GTK_MENU_SHELL(bar), GTK_WIDGET(help));

	GtkMenu *app_menu = GTK_MENU(gtk_menu_new());
	GtkMenuItem *quit = GTK_MENU_ITEM(gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL));
	gtk_menu_shell_append(GTK_MENU_SHELL(app_menu), GTK_WIDGET(quit));
	gtk_menu_item_set_submenu(app, GTK_WIDGET(app_menu));

	GtkMenu *file_menu = GTK_MENU(gtk_menu_new());
	GtkMenuItem *new = GTK_MENU_ITEM(gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, NULL));
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), GTK_WIDGET(new));
	GtkMenuItem *open = GTK_MENU_ITEM(gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL));
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), GTK_WIDGET(open));
	GtkMenuItem *close = GTK_MENU_ITEM(gtk_image_menu_item_new_from_stock(GTK_STOCK_CLOSE, NULL));
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), GTK_WIDGET(close));
	gtk_menu_item_set_submenu(file, GTK_WIDGET(file_menu));

	gtk_widget_show_all(GTK_WIDGET(bar));

	return GTK_WIDGET(bar);
}

GtkWindow * create_main_window()
{
	GtkWindow *win = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	GtkVBox *box = GTK_VBOX(gtk_vbox_new(FALSE, 0));
	GtkLabel *label = GTK_LABEL(gtk_label_new("Hello World"));

#if 0
	GtkWidget *bar = create_menu_bar();
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(bar), FALSE, FALSE, 0);
#endif

	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(label), TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(win), GTK_WIDGET(box));

	return win;
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);

	mainwin = create_main_window();

	g_signal_connect(mainwin, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_realize(GTK_WIDGET(mainwin));

#if 1
	topmenu_client_connect_window_widget(gtk_widget_get_window(GTK_WIDGET(mainwin)),
	                                     create_menu_bar());
#endif

	gtk_widget_show_all(GTK_WIDGET(mainwin));

	gtk_main();

	return EXIT_SUCCESS;
}
