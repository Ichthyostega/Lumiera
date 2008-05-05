dnl Process this file with autoconf to produce a configure script.

AC_INIT(gtk-lumiera, 0.1)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC




dnl ***************************************************************************
dnl Internatinalization
dnl ***************************************************************************
GETTEXT_PACKAGE=gtk-lumiera
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])



AM_PROG_LIBTOOL


# ######################################################################
# Checks for X11 and XVideo
# ######################################################################
AC_PATH_X
AC_PATH_XTRA
# CFLAGS="$CFLAGS $X_CFLAGS"
LIBS="$LIBS $X_PRE_LIBS $X_LIBS $X_EXTRA_LIBS"

AC_CHECK_HEADERS([X11/Xlib.h X11/Xutil.h],[],
	[AC_MSG_ERROR([Xlib.h or Xutil.h not found install xdevel])])

# OK, this is confusing. The old tests test for the existence of a number 
# of header files, and add -lXv to the linker flags if they are there.
# However, if they are _not_ there, only a warning is issued, but
# src/displayer.h includes these unconditionally.
# I conclude that -lXv _is_ required. If not, src/displayer.h should be edited, 
# I think.
# This test requires libXv and, therefore, libXext

AC_CHECK_HEADERS([sys/ipc.h sys/shm.h],,
	[AC_MSG_ERROR([Required header not found. Please check that it is installed])]
)
AC_CHECK_HEADERS([X11/extensions/Xvlib.h X11/extensions/XShm.h],,
	[AC_MSG_ERROR([Required xvideo (Xv) extension to X not found. Please check that it is installed.])],
        [#include <X11/Xlib.h>]
)

AC_CHECK_LIB(Xext, XInitExtension, ,
    [AC_MSG_ERROR([Could not link with libXext. Check that you have libXext installed])], -lX11
)
AC_CHECK_LIB(Xv, XvQueryAdaptors, ,
    [AC_MSG_ERROR([Could not link with libXv. Check that you have libXv installed])]
)

# ######################################################################
# Checks for GTK stuff
# ######################################################################
PKG_CHECK_MODULES(GTK_LUMIERA, [gtkmm-2.4 >= 2.8 gdl-1.0 >= 0.6.1 cairomm-1.0 >= 0.6.0])

AC_SUBST(GTK_LUMIERA_CFLAGS)
AC_SUBST(GTK_LUMIERA_LIBS)

AC_OUTPUT([
Makefile
src/Makefile
po/Makefile.in
])
