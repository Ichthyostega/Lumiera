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

liblumibackend_a_srcdir = $(top_srcdir)/src/backend
noinst_LIBRARIES += liblumibackend.a

liblumibackend_a_CPPFLAGS = $(AM_CPPFLAGS)
liblumibackend_a_CFLAGS = $(AM_CFLAGS) -std=gnu99 -Wextra -Wall -Werror

liblumibackend_a_SOURCES =					\
	$(liblumibackend_a_srcdir)/mediaaccessfacade.cpp	\
	$(liblumibackend_a_srcdir)/backend.c			\
	$(liblumibackend_a_srcdir)/file.c			\
	$(liblumibackend_a_srcdir)/filehandle.c			\
	$(liblumibackend_a_srcdir)/filedescriptor.c		\
	$(liblumibackend_a_srcdir)/resourcecollector.c		\
	$(liblumibackend_a_srcdir)/mmap.c			\
	$(liblumibackend_a_srcdir)/mmapings.c			\
	$(liblumibackend_a_srcdir)/mmapcache.c			\
	$(liblumibackend_a_srcdir)/filehandlecache.c

noinst_HEADERS +=						\
	$(liblumibackend_a_srcdir)/mediaaccessfacade.cpp	\
	$(liblumibackend_a_srcdir)/backend.h			\
	$(liblumibackend_a_srcdir)/file.h			\
	$(liblumibackend_a_srcdir)/filehandle.h			\
	$(liblumibackend_a_srcdir)/filedescriptor.h		\
	$(liblumibackend_a_srcdir)/filehandlecache.h		\
	$(liblumibackend_a_srcdir)/resourcecollector.h		\
	$(liblumibackend_a_srcdir)/mmap.h			\
	$(liblumibackend_a_srcdir)/mmapings.h			\
	$(liblumibackend_a_srcdir)/mmapcache.h
