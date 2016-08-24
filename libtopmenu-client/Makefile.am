if GTK3

lib_LTLIBRARIES = libtopmenu-client-gtk3.la
libtopmenu_client_gtk3_la_SOURCES = topmenu-client.c topmenu-client.h \
	topmenu-monitor.c topmenu-monitor.h \
	topmenu-appmenubar.c topmenu-appmenubar.h
libtopmenu_client_gtk3_la_CPPFLAGS = $(X11_CFLAGS) $(GTK_CFLAGS) -DG_LOG_DOMAIN=\"topmenu-client\"
libtopmenu_client_gtk3_la_LIBADD = $(X11_LIBS) $(GTK_LIBS)

else

lib_LTLIBRARIES = libtopmenu-client-gtk2.la
libtopmenu_client_gtk2_la_SOURCES = topmenu-client.c topmenu-client.h \
	topmenu-monitor.c topmenu-monitor.h \
	topmenu-appmenubar.c topmenu-appmenubar.h
libtopmenu_client_gtk2_la_CPPFLAGS = $(X11_CFLAGS) $(GTK_CFLAGS) -DG_LOG_DOMAIN=\"topmenu-client\"
libtopmenu_client_gtk2_la_LIBADD = $(X11_LIBS) $(GTK_LIBS)

endif

include_HEADERS = topmenu-client.h topmenu-monitor.h topmenu-appmenubar.h
