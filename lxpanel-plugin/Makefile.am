if WANT_LXPANEL_PLUGIN

plugindir = $(LXPANEL_PLUGIN_DIR)
plugin_LTLIBRARIES = libtopmenu.la

libtopmenu_la_SOURCES = topmenu-lxpanel-plugin.c
libtopmenu_la_CPPFLAGS = $(GTK_CFLAGS) $(LXPANELPLUGIN_CFLAGS) -DG_LOG_DOMAIN=\"topmenu-xfce-panel-applet\"
libtopmenu_la_LIBADD = $(GTK_LIBS) $(LXPANELPLUGIN_LIBS) ../libtopmenu-server/libtopmenu-server-gtk$(GTK_VERSION).la
libtopmenu_la_LDFLAGS = -avoid-version -shared -no-undefined

endif
