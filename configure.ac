AC_INIT(lumiera, 0.1pre)
AC_CONFIG_SRCDIR(src/lib/plugin.c)
AC_CONFIG_AUX_DIR(scripts)
AM_INIT_AUTOMAKE
AC_PREREQ(2.59)

AC_COPYRIGHT([
  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
])

#
# Required programs
#
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

#
# test for headers
#
AC_STDC_HEADERS
AC_CHECK_HEADER([nobug.h], AC_DEFINE(HAVE_NOBUG_H))
AC_CHECK_HEADER([execinfo.h], AC_DEFINE(HAVE_EXECINFO_H))
# there is a warning in nobug, disabled til fixed AC_CHECK_HEADER([valgrind/valgrind.h], AC_DEFINE(HAVE_VALGRIND_VALGRIND_H))

ACX_PTHREAD
##AM_CONDITIONAL([HAVE_THREADING], [test x"$acx_pthread_ok" = xyes])



#
# user options
#


############## NoBug build levels
AH_TEMPLATE(EBUG_ALPHA, [Define to 1 for selecting NoBug ALPHA build level])
AH_TEMPLATE(EBUG_BETA, [Define to 1 for selecting NoBug BETA build level])
AH_TEMPLATE(NDEBUG, [Define to 1 for selecting NoBug RELEASE build level])
AC_ARG_ENABLE(alpha, AC_HELP_STRING([--enable-alpha], [select NoBug ALPHA build level]),
              nobug_level=alpha
              AC_DEFINE(EBUG_ALPHA),
[
AC_ARG_ENABLE(beta, AC_HELP_STRING([--enable-beta], [select NoBug BETA build level]),
              nobug_level=beta
              AC_DEFINE(EBUG_BETA),
[
AC_ARG_ENABLE(release, AC_HELP_STRING([--enable-release], [select NoBug RELEASE build level]),
              nobug_level=release
              AC_DEFINE(NDEBUG),

# default to ALPHA
              nobug_level=alpha
              AC_DEFINE(EBUG_ALPHA)
)])])
AC_MSG_RESULT([NoBug build level: $nobug_level])
# END NoBug


############## Internatinalization
#GETTEXT_PACKAGE=gtk-lumiera
#AC_SUBST(GETTEXT_PACKAGE)
#AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
#AM_GLIB_GNU_GETTEXT
#IT_PROG_INTLTOOL([0.35.0])

# END Internatinalization


############## X11 Dependancies
AC_PATH_X
AC_PATH_XTRA
# CFLAGS="$CFLAGS $X_CFLAGS"
LIBS="$LIBS $X_PRE_LIBS $X_LIBS $X_EXTRA_LIBS"

AC_CHECK_HEADERS([X11/Xlib.h X11/Xutil.h],[],
	[AC_MSG_ERROR([Xlib.h or Xutil.h not found install xdevel])])

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

# END X11 Dependancies


############## Gtk Dependancies
PKG_CHECK_MODULES(GTK_LUMIERA, [gtkmm-2.4 >= 2.8 gdl-1.0 >= 0.6.1 cairomm-1.0 >= 0.6.0])

AC_SUBST(GTK_LUMIERA_CFLAGS)
AC_SUBST(GTK_LUMIERA_LIBS)

# END Gtk Dependancies


AC_CONFIG_FILES(Makefile)
AC_OUTPUT
