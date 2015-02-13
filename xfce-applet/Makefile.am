if WANT_XFCE_APPLET

plugindir = $(libdir)/xfce4/panel-plugins
plugin_LTLIBRARIES = libtopmenu.la

libtopmenu_la_SOURCES = topmenu-xfce-panel-applet.c topmenu-xfce-panel-applet.h
libtopmenu_la_CPPFLAGS = $(GTK_CFLAGS) $(XFCEPANELAPPLET_CFLAGS) -DG_LOG_DOMAIN=\"topmenu-xfce-panel-applet\"
libtopmenu_la_LIBADD = $(GTK_LIBS) $(XFCEPANELAPPLET_LIBS) ../libtopmenu-server/libtopmenu-server-gtk$(GTK_VERSION).la
libtopmenu_la_LDFLAGS = -avoid-version -shared -no-undefined -export-symbols-regex '^xfce_panel_module_(preinit|init|construct)'

desktopdir = $(datadir)/xfce4/panel-plugins
desktop_DATA = topmenu.desktop

$(desktop_DATA): %: %.in Makefile
	$(AM_V_GEN)sed \
		-e "s|\@LIBEXECDIR\@|$(libexecdir)|" \
		$< > $@

EXTRA_DIST = topmenu.desktop.in
CLEANFILES = $(desktop_DATA)

endif
