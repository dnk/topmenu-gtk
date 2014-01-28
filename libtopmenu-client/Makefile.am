lib_LTLIBRARIES = libtopmenu-client.la
libtopmenu_client_la_SOURCES = topmenu-client.c topmenu-client.h \
	topmenu-monitor.c topmenu-monitor.h \
	topmenu-appmenubar.c topmenu-appmenubar.h
libtopmenu_client_la_CPPFLAGS = $(GTK_CFLAGS) -DG_LOG_DOMAIN=\"topmenu-client\"
libtopmenu_client_la_LIBADD = $(GTK_LIBS)

include_HEADERS = topmenu-client.h topmenu-monitor.h topmenu-appmenubar.h
