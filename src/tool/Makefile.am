#  Copyright (C)         Lumiera.org
#    2008                Christian Thaeter <ct@pipapo.org>
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

lumitool_srcdir = $(top_srcdir)/src/tool

noinst_PROGRAMS += luidgen

luidgen_CFLAGS = $(CFLAGS) -std=gnu99 -Wall -Werror
luidgen_CPPFLAGS = -I$(top_srcdir)/src/
luidgen_SOURCES =                                               \
	  $(lumitool_srcdir)/luidgen.c

luidgen_LDADD = liblumiera.la $(NOBUGMT_LUMIERA_LIBS)
