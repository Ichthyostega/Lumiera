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

lumitesttool_srcdir = $(top_srcdir)/tests/tool

noinst_PROGRAMS += vgsuppression
vgsuppression_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
vgsuppression_LDADD = $(LUMIERA_LIBS) liblumieraproc.la liblumierabackend.la liblumieracommon.la liblumiera.la -lboost_regex-mt -lboost_program_options-mt -ldl
vgsuppression_SOURCES = $(lumitesttool_srcdir)/vgsuppression.c

