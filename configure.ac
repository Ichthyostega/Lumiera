AC_INIT(cinelerra, 3.0pre)
AC_CONFIG_SRCDIR(src/lib/plugin.c)
AC_CONFIG_AUX_DIR(scripts)
AM_INIT_AUTOMAKE
AC_PREREQ(2.59)

AC_COPYRIGHT([
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>

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
AC_PROG_LIBTOOL

#
# test for headers
#
AC_STDC_HEADERS
AC_CHECK_HEADER([pthread.h], AC_DEFINE(HAVE_PTHREAD_H))
AC_CHECK_HEADER([nobug.h], AC_DEFINE(HAVE_NOBUG_H))
AC_CHECK_HEADER([execinfo.h], AC_DEFINE(HAVE_EXECINFO_H))
# there is a warning in nobug, disabled til fixed AC_CHECK_HEADER([valgrind/valgrind.h], AC_DEFINE(HAVE_VALGRIND_VALGRIND_H))

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


AC_CONFIG_FILES(Makefile)
AC_OUTPUT
