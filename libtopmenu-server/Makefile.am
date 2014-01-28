lib_LTLIBRARIES = libtopmenu-server.la
libtopmenu_server_la_SOURCES = topmenu-server.c topmenu-server.h topmenu-widget.c topmenu-widget.h
libtopmenu_server_la_CPPFLAGS = $(GTK_CFLAGS) $(WNCK_CFLAGS) $(MATEWNCK_CFLAGS) -DG_LOG_DOMAIN=\"topmenu-server\"
libtopmenu_server_la_LIBADD = $(GTK_LIBS) $(WNCK_LIBS) $(MATEWNCK_LIBS)

include_HEADERS = topmenu-server.h topmenu-widget.h

