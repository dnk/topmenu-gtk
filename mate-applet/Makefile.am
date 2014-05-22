if WANT_MATE_APPLET

libexec_PROGRAMS = topmenu-mate-panel-applet

topmenu_mate_panel_applet_SOURCES = main.c topmenu-mate-panel-applet.c topmenu-mate-panel-applet.h
topmenu_mate_panel_applet_CPPFLAGS = $(GTK_CFLAGS) $(MATEPANELAPPLET_CFLAGS) -DG_LOG_DOMAIN=\"topmenu-mate-panel-applet\"
topmenu_mate_panel_applet_LDADD = $(GTK_LIBS) $(MATEPANELAPPLET_LIBS) ../libtopmenu-server/libtopmenu-server-gtk$(GTK_VERSION).la

appletdir = $(datadir)/mate-panel/applets
applet_DATA = com.javispedro.topmenu.MatePanelApplet.mate-panel-applet

$(applet_DATA): %: %.in Makefile
	$(AM_V_GEN)sed \
		-e "s|\@LIBEXECDIR\@|$(libexecdir)|" \
		$< > $@

servicedir = $(datadir)/dbus-1/services
service_DATA = org.mate.panel.applet.TopMenuMatePanelAppletFactory.service

$(service_DATA): %: %.in Makefile
	$(AM_V_GEN)sed \
		-e "s|\@LIBEXECDIR\@|$(libexecdir)|" \
		$< > $@

EXTRA_DIST = com.javispedro.topmenu.MatePanelApplet.mate-panel-applet.in \
	org.mate.panel.applet.TopMenuMatePanelAppletFactory.service.in
CLEANFILES = $(applet_DATA) $(service_DATA)

endif
