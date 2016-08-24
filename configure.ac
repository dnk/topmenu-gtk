#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(libtopmenu-gtk, 0.3, dev.bugs@javispedro.com)
AC_CONFIG_SRCDIR([libtopmenu-server/topmenu-server.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_MACRO_DIR([m4])

AC_ARG_WITH([gtk],
            [AS_HELP_STRING([--with-gtk=2|3], [GTK+ version [default=2]])],
			[],
			[with_gtk=2])
AC_ARG_WITH([gtk-libdir],
            [AS_HELP_STRING([--with-gtk-libdir=DIR], [GTK+ library directory [default=`pkg-config --variable=libdir gtk+-3.0`]])],
			[],
			[with_gtk_libdir=`pkg-config --variable=libdir gtk+-\$with_gtk.0`])
AC_ARG_WITH([gtk-module-dir],
            [AS_HELP_STRING([--with-gtk-module-dir=DIR], [GTK+ module directory [default=`pkg-config --variable=libdir gtk+-3.0`/gtk-3.0/modules]])],
			[],
			[with_gtk_module_dir=$with_gtk_libdir/gtk-$with_gtk.0/modules])
AC_ARG_WITH([lxpanel-plugin-dir],
            [AS_HELP_STRING([--with-lxpanel-plugin-dir=DIR], [LXPanel plugin directory [default=`pkg-config --variable=pluginsdir lxpanel`]])],
			[],
			[with_lxpanel_plugin_dir=$with_gtk_libdir/lxpanel/plugins])
AC_ARG_WITH([wnck],
            [AS_HELP_STRING([--with-wnck], [support window management using [wnck1|wnck3|matewnck] @<:@default=check@:>@])],
            [],
            [with_wnck=check])

AC_ARG_ENABLE([mate-applet],
              [AS_HELP_STRING([--enable-mate-applet], [build the Mate panel applet @<:@default=check@:>@])],
			  [],
			  [enable_mate_applet=check])

AC_ARG_ENABLE([xfce-applet],
              [AS_HELP_STRING([--enable-xfce-applet], [build the Xfce panel applet @<:@default=check@:>@])],
              [],
              [enable_xfce_applet=check])

AC_ARG_ENABLE([lxpanel-plugin],
              [AS_HELP_STRING([--enable-lxpanel-plugin], [build the LXPanel plugin @<:@default=check@:>@])],
              [],
              [enable_lxpanel_plugin=check])

AM_CONDITIONAL([GTK3], [test GTK$with_gtk = GTK3])
AC_SUBST([GTK_VERSION], [$with_gtk])
AC_DEFINE_UNQUOTED([GTK_VERSION], [$with_gtk], [Define to GTK major version used])
AC_SUBST([GTK_MODULE_DIR], [$with_gtk_module_dir])
AC_SUBST([LXPANEL_PLUGIN_DIR], [$with_lxpanel_plugin_dir])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

# Checks for libraries.
PKG_CHECK_MODULES([GTK], [gtk+-x11-$with_gtk.0])
PKG_CHECK_MODULES([X11], [x11])

AS_IF([test "x$with_gtk" = x3],
	[AS_IF([test "x$with_wnck" = xwnck3 -o "x$with_wnck" = xcheck],
		[PKG_CHECK_MODULES([WNCK3], [libwnck-3.0],
			[
				AC_DEFINE([HAVE_WNCK3], [1], [Define if you have libwnck-3.0])
				with_wnck=wnck3
			],
			[if test "x$with_wnck" = xwnck3; then
				AC_MSG_FAILURE([--with-wnck=wnck3 was given, but test for libwnck-3.0 failed])
			fi]
		)])
	AS_IF([test "x$enable_mate_applet" != xno],
		[PKG_CHECK_MODULES([MATEPANELAPPLET], [libmatepanelapplet-4.0 >= 1.12],
			[
				AC_DEFINE([HAVE_MATEPANELAPPLET], [1], [Define if you have libmatepanelapplet])
				enable_mate_applet=yes
			],
			[if test "x$enable_mate_applet" = xyes; then
				AC_MSG_FAILURE([--enable-mate-applet was given, but test for libmatepanelapplet failed])
			fi]
		)])],

	dnl # ![test "x$with_gtk" = x3]
	[AS_IF([test "x$with_wnck" = xwnck1 -o "x$with_wnck" = xcheck],
		[PKG_CHECK_MODULES([WNCK1], [libwnck-1.0],
			[
				AC_DEFINE([HAVE_WNCK1], [1], [Define if you have libwnck-1.0])
				with_wnck=wnck1
			],
			[if test "x$with_wnck" = xwnck1; then
				AC_MSG_FAILURE([--with-wnck=wnck1 was given, but test for libwnck-1.0 failed])
			fi]
		)])
	AS_IF([test "x$with_wnck" = xmatewnck -o "x$with_wnck" = xcheck],
		[PKG_CHECK_MODULES([MATEWNCK], [libmatewnck],
			[
				AC_DEFINE([HAVE_MATEWNCK], [1], [Define if you have libmatewnck])
				with_wnck=libmatewnck
			],
			[if test "x$with_wnck" = xmatewnck; then
				AC_MSG_FAILURE([--with-wnck=matewnck was given, but test for libmatewnck failed])
			fi]
		)])
	AS_IF([test "x$enable_mate_applet" != xno],
		[PKG_CHECK_MODULES([MATEPANELAPPLET], [libmatepanelapplet-4.0],
			[
				AC_DEFINE([HAVE_MATEPANELAPPLET], [1], [Define if you have libmatepanelapplet])
				enable_mate_applet=yes
			],
			[if test "x$enable_mate_applet" = xyes; then
				AC_MSG_FAILURE([--enable-mate-applet was given, but test for libmatepanelapplet failed])
			fi]
		)])
	AS_IF([test "x$enable_xfce_applet" != xno],
		[PKG_CHECK_MODULES([XFCEPANELAPPLET], [libxfce4panel-1.0 >= 4.8],
			[
				AC_DEFINE([HAVE_XFCEPANELAPPLET], [1], [Define if you have libxfce4panel])
				enable_xfce_applet=yes
			],
			[if test "x$enable_xfce_applet" = xyes; then
				AC_MSG_FAILURE([--enable-xfce-applet was given, but test for libxfce4panel failed])
			fi]
		)])
	AS_IF([test "x$enable_lxpanel_plugin" != xno],
		[PKG_CHECK_MODULES([LXPANELPLUGIN], [lxpanel >= 0.8.0 libfm >= 1.2.3],
			[
				AC_DEFINE([HAVE_LXPANEL], [1], [Define if you have lxpanel])
				enable_lxpanel_plugin=yes
			],
			[if test "x$enable_lxpanel_plugin" = xyes; then
				AC_MSG_FAILURE([--enable-lxpanel-plugin was given, but test for lxpanel failed])
			fi]
		)])])

AM_CONDITIONAL([WANT_MATE_APPLET], [test x$enable_mate_applet = xyes])
AM_CONDITIONAL([WANT_XFCE_APPLET], [test x$enable_xfce_applet = xyes])
AM_CONDITIONAL([WANT_LXPANEL_PLUGIN], [test x$enable_lxpanel_plugin = xyes])

# Output files
AC_CONFIG_FILES([
	Makefile
	libtopmenu-client/Makefile
	libtopmenu-server/Makefile
	module/Makefile
	mate-applet/Makefile
	xfce-applet/Makefile
	lxpanel-plugin/Makefile
	icons/Makefile
	test/Makefile
])

AC_OUTPUT
