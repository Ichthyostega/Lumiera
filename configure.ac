AC_INIT(lumiera, 0.1pre)
AC_CONFIG_SRCDIR([src/lib/luid.c])
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_PREREQ(2.59)

AC_COPYRIGHT([
  Copyright (C)         Lumiera.org
    2008, 2009, 2010    Christian Thaeter <ct@pipapo.org>
                        Joel Holdsworth <joel@airwebreathe.org.uk>
                        et. al.

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

#################################################################################
#
# Global configuration
#
AC_GNU_SOURCE
AC_DISABLE_STATIC

#################################################################################
#
# Required programs
#
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG([0.22])

#################################################################################
#
# user options
#

# NoBug build levels
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
# END NoBug




#################################################################################
#
# global test for headers and functions
#
AC_STDC_HEADERS


ACX_PTHREAD
LIBS="$PTHREAD_LIBS $LIBS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CC="$PTHREAD_CC"

# Nobug Dependencies
AC_CHECK_HEADER([execinfo.h], AC_DEFINE(HAVE_EXECINFO_H))
PKG_CHECK_MODULES(VALGRIND, [valgrind], AC_DEFINE(HAVE_VALGRIND_H),
                             AC_MSG_NOTICE([valgrind not found                                 (optional)]))

PKG_CHECK_MODULES(NOBUGMT, [nobugmt >= 201001.1],
			   AC_DEFINE(HAVE_NOBUGMT_H),
			   AC_MSG_ERROR([Missing required nobug version                    (http://www.lumiera.org/nobug_manual.html)])
)

LUMIERA_CFLAGS="$NOBUGMT_CFLAGS $VALGRIND_CFLAGS"
LUMIERA_LIBS="$NOBUGMT_LIBS"

AC_SUBST(LUMIERA_CFLAGS)
AC_SUBST(LUMIERA_LIBS)


#boost
# C++ headers and libraries
AC_LANG_PUSH([C++])
# check for the minimum Boost version
# (http://www.randspringer.de/boost/upt.html)
AX_BOOST_BASE([1.34.1])

AC_CHECK_HEADER([tr1/memory],
        ,
        AC_MSG_ERROR([std::tr1 proposed standard extension for shared_ptr missing])
        )

AC_CHECK_HEADER([boost/shared_ptr.hpp],
        ,
        AC_MSG_ERROR([boost::shared_ptr missing                 (http://www.boost.org/)])
        )

AC_CHECK_HEADER([boost/program_options.hpp],
        [LUMIERA_BOOST_LIBS="$LUMIERA_BOOST_LIBS -lboost_program_options-mt"],
        AC_MSG_ERROR([boost::program_options missing            (http://www.boost.org/)])
        )

AC_CHECK_HEADER([boost/regex.hpp],
        [LUMIERA_BOOST_LIBS="$LUMIERA_BOOST_LIBS -lboost_regex-mt"],
        AC_MSG_ERROR([boost::regex missing                      (http://www.boost.org/)])
        )

AC_LANG_POP([C++])
AC_SUBST(LUMIERA_BOOST_CFLAGS)
AC_SUBST(LUMIERA_BOOST_LIBS)



#################################################################################
#
# Configs for subsystems
# ----------------------
#

#################################################################################
#
# library
#

PKG_CHECK_MODULES(LUMIERA_LIBRARY, [
    gavl >= 1.0.1
    ])

LUMIERA_LIBRARY_CFLAGS="$LUMIERA_LIBRARY_CFLAGS $LUMIERA_CFLAGS"
LUMIERA_LIBRARY_LIBS="$LUMIERA_LIBRARY_CFLAGS $LUMIERA_LIBS $LUMIERA_BOOST_LIBS"

AC_SUBST(LUMIERA_LIBRARY_CFLAGS)
AC_SUBST(LUMIERA_LIBRARY_LIBS)

#################################################################################
#
# common
#
PKG_CHECK_MODULES(LUMIERA_COMMON, [sigc++-2.0 >= 2.0.17])

AC_LANG_PUSH([C])
AC_CHECK_LIB(dl, dlopen, [LUMIERA_COMMON_LIBS="$LUMIERA_COMMON_LIBS -ldl"],
                   [AC_MSG_ERROR([Dynamic linking not supported, report a bug])]
)
AC_LANG_POP([C])

LUMIERA_COMMON_CFLAGS="$LUMIERA_COMMON_CFLAGS $LUMIERA_CFLAGS"
LUMIERA_COMMON_LIBS="$LUMIERA_COMMON_LIBS $LUMIERA_LIBS"

AC_SUBST(LUMIERA_COMMON_CFLAGS)
AC_SUBST(LUMIERA_COMMON_LIBS)

#################################################################################
#
# backend
#

LUMIERA_BACKEND_CFLAGS="$LUMIERA_CFLAGS"
LUMIERA_BACKEND_LIBS="$LUMIERA_LIBS"

AC_SUBST(LUMIERA_BACKEND_CFLAGS)
AC_SUBST(LUMIERA_BACKEND_LIBS)

#################################################################################
#
# proc
#


LUMIERA_PROC_CFLAGS="$LUMIERA_PROC_CFLAGS $LUMIERA_CFLAGS"
LUMIERA_PROC_LIBS="$LUMIERA_PROC_CFLAGS $LUMIERA_LIBS"

AC_SUBST(LUMIERA_PROC_CFLAGS)
AC_SUBST(LUMIERA_PROC_LIBS)

#################################################################################
#
# gui
#

############## Internatinalization
#GETTEXT_PACKAGE=gtk-lumiera
#AC_SUBST(GETTEXT_PACKAGE)
#AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
#AM_GLIB_GNU_GETTEXT
#IT_PROG_INTLTOOL([0.35.0])

# END Internatinalization

############## Gtk Dependancies

# gtk+-2.0 >= 2.8 gtkmm-2.4 >= 2.8 for Debian Etch compatibility
# gtk+-2.0 >= 2.12 gtkmm-2.4 >= 2.12 for Debian Lenny compatibility

PKG_CHECK_MODULES(LUMIERA_GUI, [
    gtk+-2.0 >= 2.8
    gtkmm-2.4 >= 2.12
    cairomm-1.0 >= 0.6.0
    librsvg-2.0 >= 2.18.1
    gavl >= 1.0.1
    gthread-2.0 >= 2.12.4])

# Check for gdl-1.0 with a gdl-lum fallback
PKG_CHECK_MODULES(GDL, gdl-1.0 >= 2.27.1, ,
    [PKG_CHECK_MODULES(GDL, gdl-lum >= 2.27.1) ])

# END Gtk Dependancies

############## X11 Dependancies
AC_PATH_X
AC_PATH_XTRA

AC_CHECK_HEADERS([X11/Xlib.h X11/Xutil.h],[],
                             [AC_MSG_ERROR([Xlib.h or Xutil.h not found install xdevel])])

AC_CHECK_HEADERS([sys/ipc.h sys/shm.h],,
	[AC_MSG_ERROR([Required header not found. Please check that it is installed])]
)
AC_CHECK_HEADERS([X11/extensions/Xvlib.h X11/extensions/XShm.h],,
	[AC_MSG_ERROR([Required xvideo (Xv) extension to X not found. Please check that it is installed.])],
        [#include <X11/Xlib.h>]
)

AC_CHECK_LIB(Xext, XInitExtension, [LUMIERA_GUI_LIBS="$LUMIERA_GUI_LIBS -lXext"],
                   [AC_MSG_ERROR([Could not link with libXext. Check that you have libXext installed])], -lX11
)
AC_CHECK_LIB(Xv, XvQueryAdaptors, [LUMIERA_GUI_LIBS="$LUMIERA_GUI_LIBS -lXv"],
     [AC_MSG_ERROR([Could not link with libXv. Check that you have libXv installed])]
)

# END X11 Dependancies

LUMIERA_GUI_CFLAGS="$LUMIERA_GUI_CFLAGS $LUMIERA_CFLAGS $GDL_CFLAGS"
LUMIERA_GUI_LIBS="$LUMIERA_GUI_LIBS $LUMIERA_LIBS $GDL_LIBS"

AC_SUBST(LUMIERA_GUI_CFLAGS)
AC_SUBST(LUMIERA_GUI_LIBS)


#################################################################################
#
# tools
#

LUMIERA_TOOL_CFLAGS="$LUMIERA_TOOL_CFLAGS $LUMIERA_CFLAGS $LUMIERA_GUI_CFLAGS"
LUMIERA_TOOL_LIBS="$LUMIERA_TOOL_CFLAGS $LUMIERA_LIBS $LUMIERA_BOOST_LIBS $LUMIERA_GUI_LIBS"

AC_SUBST(LUMIERA_TOOL_CFLAGS)
AC_SUBST(LUMIERA_TOOL_LIBS)

#################################################################################
#
# Generate output
#

# Print a summary
AC_MSG_RESULT([
Configuration Summary:])
AC_MSG_RESULT([        NoBug build level:               $nobug_level])
# Add more summary results here

AC_MSG_RESULT([
Configuration complete, you can now build Lumiera with 'make'
])

AC_CONFIG_FILES(Makefile)
AC_OUTPUT
