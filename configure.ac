#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(libtopmenu-gtk, 1.0, javier@javispedro.com)
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
AC_ARG_WITH([wnck],
            [AS_HELP_STRING([--with-wnck], [support window management using [wnck1|matewnck] @<:@default=check@:>@])],
            [],
            [with_wnck=check])

AC_ARG_ENABLE([mate-applet],
              [AS_HELP_STRING([--enable-mate-applet], [build the Mate panel applet @<:@default=check@:>@])],
			  [],
			  [enable_mate_applet=check])

AC_SUBST([GTK_VERSION], [$with_gtk])
AC_DEFINE_UNQUOTED([GTK_VERSION], [$with_gtk], [Define to GTK major version used])
AC_SUBST([GTK_MODULE_DIR], [$with_gtk_module_dir])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

# Checks for libraries.
PKG_CHECK_MODULES([GTK], [gtk+-x11-$with_gtk.0])

AS_IF([test "x$with_wnck" = xwnck1 -o "x$with_wnck" = xcheck],
	[PKG_CHECK_MODULES([WNCK], [libwnck-1.0],
		[
			AC_DEFINE([HAVE_WNCK1], [1], [Define if you have libwnck-1.0])
			with_wnck=libwnck1
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

AM_CONDITIONAL([WANT_MATE_APPLET], [test x$enable_mate_applet = xyes])

# Output files
AC_CONFIG_FILES([
	Makefile
	libtopmenu-client/Makefile
	libtopmenu-server/Makefile
	module/Makefile
	mate-applet/Makefile
	test/Makefile
])

AC_OUTPUT
