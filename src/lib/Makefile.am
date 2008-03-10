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

libcin3_a_srcdir = $(top_srcdir)/src/lib
noinst_LIBRARIES += libcin3.a

libcin3_a_CFLAGS = $(CFLAGS) -std=gnu99 -Wall -Werror
libcin3_a_CPPFLAGS = -I$(top_srcdir)/src/

libcin3_a_SOURCES = \
	$(libcin3_a_srcdir)/plugin.c \
	$(libcin3_a_srcdir)/error.c \
	$(libcin3_a_srcdir)/time.c \
	$(libcin3_a_srcdir)/framerate.c \
	$(libcin3_a_srcdir)/mutex.c \
	$(libcin3_a_srcdir)/rwlock.c \
	$(libcin3_a_srcdir)/condition.c \
	$(libcin3_a_srcdir)/references.c


noinst_HEADERS += \
	$(libcin3_a_srcdir)/plugin.h \
	$(libcin3_a_srcdir)/error.h \
	$(libcin3_a_srcdir)/time.h \
	$(libcin3_a_srcdir)/framerate.h \
	$(libcin3_a_srcdir)/locking.h \
	$(libcin3_a_srcdir)/mutex.h \
	$(libcin3_a_srcdir)/rwlock.h \
	$(libcin3_a_srcdir)/condition.h \
	$(libcin3_a_srcdir)/references.h

