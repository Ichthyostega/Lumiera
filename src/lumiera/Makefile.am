#  Copyright (C)         Lumiera.org
#    2008,               Christian Thaeter <ct@pipapo.org>
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

lumiera_srcdir = $(top_srcdir)/src/lumiera

bin_PROGRAMS += lumiera
lumiera_CPPFLAGS = $(AM_CPPFLAGS)
lumiera_CFLAGS = $(AM_CFLAGS) $(LUMIERA_CFLAGS) -std=gnu99 -Wall -Wextra -Werror
lumiera_CXXFLAGS = $(AM_CXXFLAGS) $(LUMIERA_CFLAGS) -Wall -Wextra

lumiera_LDADD =					\
	$(LUMIERA_LIBS)				\
	liblumieracommon.la			\
	liblumierabackend.la			\
	liblumieraproc.la			\
	liblumiera.la

lumiera_SOURCES =				\
	$(lumiera_srcdir)/main.cpp


