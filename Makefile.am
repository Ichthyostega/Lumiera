#  Copyright (C)         CinelerraCV
#    2007,               Christian Thaeter <ct@pipapo.org>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

bin_PROGRAMS =
lib_LTLIBRARIES =
noinst_PROGRAMS =
noinst_LIBRARIES =
noinst_LTLIBRARIES =
noinst_HEADERS =
BUILT_SOURCES =
EXTRA_DIST =
SUBDIRS =

# Only use subdirs if really needed, prefer the include scheme below
#SUBDIRS +=

# core
include $(top_srcdir)/src/lib/Makefile.am

# plugins
#include $(top_srcdir)/src...

# tests
include $(top_srcdir)/tests/Makefile.am
include $(top_srcdir)/tests/examples/Makefile.am

#EXTRA_DIST += admin debian doc depcomp README.BUILD LICENSE \
#	cinelerra-cvs-current.spec
AUTOMAKE_OPTIONS=foreign
#ACLOCAL_AMFLAGS = -I m4

