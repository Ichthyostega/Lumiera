#  Copyright (C)         Lumiera.org
#    2007,               Christian Thaeter <ct@pipapo.org>
#    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
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

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS =
lib_LTLIBRARIES =
noinst_PROGRAMS =
check_PROGRAMS =
noinst_LIBRARIES =
check_LTLIBRARIES =
noinst_HEADERS =
BUILT_SOURCES =
EXTRA_DIST =
SUBDIRS =

# global pre-processor flags. Per-target variables should include
# these at the front (if possible) for consistency
AM_CPPFLAGS = -I$(top_srcdir)/src/ \
	$(NOBUGMT_LUMIERA_CFLAGS)

# Only use subdirs if really needed, prefer the include scheme below
#SUBDIRS +=

# administrative tools
include $(top_srcdir)/admin/Makefile.am

# core
include $(top_srcdir)/src/proc/Makefile.am
include $(top_srcdir)/src/common/Makefile.am
include $(top_srcdir)/src/lib/Makefile.am
include $(top_srcdir)/src/backend/Makefile.am

# tools
include $(top_srcdir)/src/tool/Makefile.am

# gui
include $(top_srcdir)/src/gui/Makefile.am

# plugins
#include $(top_srcdir)/src...

# resources
include $(top_srcdir)/icons/Makefile.am

# tests
include $(top_srcdir)/tests/common/Makefile.am
include $(top_srcdir)/tests/components/Makefile.am
include $(top_srcdir)/tests/Makefile.am

#EXTRA_DIST += admin debian doc depcomp README.BUILD LICENSE \
#	cinelerra-cvs-current.spec
AUTOMAKE_OPTIONS=foreign
#ACLOCAL_AMFLAGS = -I m4

