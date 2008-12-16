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

liblumierabackend_la_srcdir = $(top_srcdir)/src/backend
lib_LTLIBRARIES += liblumierabackend.la

liblumierabackend_la_CPPFLAGS = $(AM_CPPFLAGS)
liblumierabackend_la_CFLAGS = $(AM_CFLAGS) -std=gnu99 -Wextra -Wall -Werror
liblumierabackend_la_LIBADD = liblumiera.la

liblumierabackend_la_SOURCES =					\
	$(liblumierabackend_la_srcdir)/mediaaccessfacade.cpp	\
	$(liblumierabackend_la_srcdir)/backend.c		\
	$(liblumierabackend_la_srcdir)/file.c			\
	$(liblumierabackend_la_srcdir)/filehandle.c		\
	$(liblumierabackend_la_srcdir)/filedescriptor.c		\
	$(liblumierabackend_la_srcdir)/resourcecollector.c	\
	$(liblumierabackend_la_srcdir)/mmap.c			\
	$(liblumierabackend_la_srcdir)/mmapings.c		\
	$(liblumierabackend_la_srcdir)/mmapcache.c		\
	$(liblumierabackend_la_srcdir)/filehandlecache.c	\
	$(liblumierabackend_la_srcdir)/enginefacade.cpp		\
	$(liblumierabackend_la_srcdir)/scriptrunnerfacade.cpp	\
	$(liblumierabackend_la_srcdir)/netnodefacade.cpp


noinst_HEADERS +=						\
	$(liblumierabackend_la_srcdir)/backend.h		\
	$(liblumierabackend_la_srcdir)/file.h			\
	$(liblumierabackend_la_srcdir)/filehandle.h		\
	$(liblumierabackend_la_srcdir)/filedescriptor.h		\
	$(liblumierabackend_la_srcdir)/filehandlecache.h	\
	$(liblumierabackend_la_srcdir)/resourcecollector.h	\
	$(liblumierabackend_la_srcdir)/mmap.h			\
	$(liblumierabackend_la_srcdir)/mmapings.h		\
	$(liblumierabackend_la_srcdir)/mmapcache.h
