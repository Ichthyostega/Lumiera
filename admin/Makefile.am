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

admin_srcdir = $(top_srcdir)/admin

noinst_PROGRAMS += vgsuppression
vgsuppression_SOURCES = $(admin_srcdir)/vgsuppression.c
vgsuppression_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
vgsuppression_LDADD = liblumi.a $(NOBUGMT_LUMIERA_LIBS) -ldl

noinst_PROGRAMS += rsvg-convert
rsvg_convert_SOURCES = $(admin_srcdir)/rsvg-convert.c
rsvg_convert_CPPFLAGS = $(GTK_LUMIERA_CFLAGS) $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
rsvg_convert_LDADD = -lcairo -lglib-2.0 -lgthread-2.0 -lrsvg-2
