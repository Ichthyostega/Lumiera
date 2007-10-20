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

examples_srcdir = $(top_srcdir)/tests/plugin
noinst_PROGRAMS += test-plugin

test_plugin_CFLAGS = $(AM_CFLAGS) -std=gnu99 -Wall -Werror
test_plugin_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src
test_plugin_LDADD = libcin3.a -lnobugmt -lpthread -ldl
test_plugin_SOURCES = $(examples_srcdir)/plugin_main.c

noinst_HEADERS += $(examples_srcdir)/hello_interface.h

check_LTLIBRARIES += example_plugin.la example_plugin_cpp.la
example_plugin_la_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src
example_plugin_la_SOURCES = $(examples_srcdir)/example_plugin.c
# the -rpath option is required, prolly a automake bug?
example_plugin_la_LDFLAGS = -avoid-version -module -rpath $(shell pwd)

example_plugin_cpp_la_CPPFLAGS = $(AM_CPPFLAGS) -Wall -Werror -I$(top_srcdir)/src
example_plugin_cpp_la_SOURCES = $(examples_srcdir)/example_plugin.cpp
# the -rpath option is required, prolly a automake bug?
example_plugin_cpp_la_LDFLAGS = -avoid-version -module -rpath $(shell pwd)
