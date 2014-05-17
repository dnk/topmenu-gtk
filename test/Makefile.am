noinst_PROGRAMS = client server

AM_CPPFLAGS = $(GTK_CFLAGS)
AM_LDFLAGS = $(GTK_LIBS)

client_SOURCES = client.c
client_CPPFLAGS = $(GTK_CFLAGS)
client_LDADD = $(GTK_LIBS) ../libtopmenu-client/libtopmenu-client-gtk$(GTK_VERSION).la

server_SOURCES = server.c
server_CPPFLAGS = $(GTK_CFLAGS)
server_LDADD = $(GTK_LIBS) ../libtopmenu-server/libtopmenu-server-gtk$(GTK_VERSION).la
