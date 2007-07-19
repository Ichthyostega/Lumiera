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

examples_srcdir = $(top_srcdir)/tests/examples
noinst_PROGRAMS += plugin_example

plugin_example_CFLAGS = $(CFLAGS) -std=gnu99 -Wall -Werror
plugin_example_CPPFLAGS = $(CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src/lib/
plugin_example_LDADD = -lnobugmt -lpthread -ldl $(builddir)/libcin3.a

plugin_example_SOURCES = $(examples_srcdir)/plugin_main.c
noinst_HEADERS += $(examples_srcdir)/hello_interface.h
