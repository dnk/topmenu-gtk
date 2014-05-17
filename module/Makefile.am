
gtk_moduledir = $(GTK_MODULE_DIR)
gtk_module_LTLIBRARIES = libtopmenu-gtk-module.la

libtopmenu_gtk_module_la_SOURCES = main.c data.c data.h \
	menuitem-proxy.c menuitem-proxy.h appmenu.c appmenu.h
libtopmenu_gtk_module_la_CPPFLAGS = $(GTK_CFLAGS) -DG_LOG_DOMAIN=\"topmenu-module\"
libtopmenu_gtk_module_la_LIBADD = $(GTK_LIBS) ../libtopmenu-client/libtopmenu-client-gtk$(GTK_VERSION).la
libtopmenu_gtk_module_la_LDFLAGS = -avoid-version -module -shared
