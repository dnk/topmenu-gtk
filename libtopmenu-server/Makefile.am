if GTK3

lib_LTLIBRARIES = libtopmenu-server-gtk3.la
libtopmenu_server_gtk3_la_SOURCES = topmenu-server.c topmenu-server.h topmenu-widget.c topmenu-widget.h
libtopmenu_server_gtk3_la_CPPFLAGS = $(X11_CFLAGS) $(GTK_CFLAGS) $(WNCK3_CFLAGS) -DG_LOG_DOMAIN=\"topmenu-server\"
libtopmenu_server_gtk3_la_LIBADD = $(X11_LIBS) $(GTK_LIBS) $(WNCK3_LIBS)

else

lib_LTLIBRARIES = libtopmenu-server-gtk2.la
libtopmenu_server_gtk2_la_SOURCES = topmenu-server.c topmenu-server.h topmenu-widget.c topmenu-widget.h
libtopmenu_server_gtk2_la_CPPFLAGS = $(X11_CFLAGS) $(GTK_CFLAGS) $(WNCK1_CFLAGS) $(MATEWNCK_CFLAGS) -DG_LOG_DOMAIN=\"topmenu-server\"
libtopmenu_server_gtk2_la_LIBADD = $(X11_LIBS) $(GTK_LIBS) $(WNCK1_LIBS) $(MATEWNCK_LIBS)

endif

include_HEADERS = topmenu-server.h topmenu-widget.h

