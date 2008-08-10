#  Copyright (C)         Lumiera.org
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

liblumi_a_srcdir = $(top_srcdir)/src/lib
noinst_LIBRARIES += liblumi.a

liblumi_a_CFLAGS = $(CFLAGS) -std=gnu99 -Wall -Werror
liblumi_a_CPPFLAGS = -I$(top_srcdir)/src/

liblumi_a_SOURCES =				\
	$(liblumi_a_srcdir)/plugin.c		\
	$(liblumi_a_srcdir)/error.c		\
	$(liblumi_a_srcdir)/mutex.c		\
	$(liblumi_a_srcdir)/rwlock.c		\
	$(liblumi_a_srcdir)/condition.c		\
	$(liblumi_a_srcdir)/luid.c		\
	$(liblumi_a_srcdir)/safeclib.c		\
	$(liblumi_a_srcdir)/cuckoo.c		\
	$(liblumi_a_srcdir)/mrucache.c		\
	$(liblumi_a_srcdir)/time.c		\
	$(liblumi_a_srcdir)/appconfig.cpp

noinst_HEADERS +=						\
	$(liblumi_a_srcdir)/plugin.h				\
	$(liblumi_a_srcdir)/error.h				\
	$(liblumi_a_srcdir)/mutex.h				\
	$(liblumi_a_srcdir)/rwlock.h				\
	$(liblumi_a_srcdir)/condition.h				\
	$(liblumi_a_srcdir)/luid.h				\
	$(liblumi_a_srcdir)/safeclib.h				\
	$(liblumi_a_srcdir)/cuckoo.h				\
	$(liblumi_a_srcdir)/mrucache.h				\
	$(liblumi_a_srcdir)/time.h				\
	$(liblumi_a_srcdir)/appconfig.hpp			\
	$(liblumi_a_srcdir)/appconfig.hpp

