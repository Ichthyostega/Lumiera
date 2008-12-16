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

liblumiera_la_srcdir = $(top_srcdir)/src/lib
lib_LTLIBRARIES += liblumiera.la

liblumiera_la_CFLAGS = $(CFLAGS) -std=gnu99 -Wall -Werror -DLUMIERA_PLUGIN_PATH="\"$(pkglibdir)\""

liblumiera_la_SOURCES =					\
	$(liblumiera_la_srcdir)/error.c			\
	$(liblumiera_la_srcdir)/mutex.c			\
	$(liblumiera_la_srcdir)/rwlock.c		\
	$(liblumiera_la_srcdir)/condition.c		\
	$(liblumiera_la_srcdir)/luid.c			\
	$(liblumiera_la_srcdir)/safeclib.c		\
	$(liblumiera_la_srcdir)/cuckoo.c		\
	$(liblumiera_la_srcdir)/psplay.c		\
	$(liblumiera_la_srcdir)/mrucache.c		\
	$(liblumiera_la_srcdir)/time.c			\
	$(liblumiera_la_srcdir)/config.c		\
	$(liblumiera_la_srcdir)/config_typed.c		\
	$(liblumiera_la_srcdir)/config_wordlist.c	\
	$(liblumiera_la_srcdir)/configentry.c		\
	$(liblumiera_la_srcdir)/configitem.c		\
	$(liblumiera_la_srcdir)/config_lookup.c		\
	$(liblumiera_la_srcdir)/config_interface.c	\
	$(liblumiera_la_srcdir)/interface.c		\
	$(liblumiera_la_srcdir)/interfaceregistry.c	\
	$(liblumiera_la_srcdir)/plugin.c		\
	$(liblumiera_la_srcdir)/plugin_dynlib.c		\
	$(liblumiera_la_srcdir)/allocationcluster.cpp	\
	$(liblumiera_la_srcdir)/external/libgavl.cpp	\
	$(liblumiera_la_srcdir)/cmdline.cpp

noinst_HEADERS +=						\
	$(liblumiera_la_srcdir)/error.h				\
	$(liblumiera_la_srcdir)/mutex.h				\
	$(liblumiera_la_srcdir)/rwlock.h			\
	$(liblumiera_la_srcdir)/condition.h			\
	$(liblumiera_la_srcdir)/luid.h				\
	$(liblumiera_la_srcdir)/safeclib.h			\
	$(liblumiera_la_srcdir)/cuckoo.h			\
	$(liblumiera_la_srcdir)/psplay.h			\
	$(liblumiera_la_srcdir)/mrucache.h			\
	$(liblumiera_la_srcdir)/time.h				\
	$(liblumiera_la_srcdir)/ppmpl.h				\
	$(liblumiera_la_srcdir)/interface.h			\
	$(liblumiera_la_srcdir)/interfaceregistry.h		\
	$(liblumiera_la_srcdir)/interfacedescriptor.h		\
	$(liblumiera_la_srcdir)/plugin.h			\
	$(liblumiera_la_srcdir)/config.h			\
	$(liblumiera_la_srcdir)/configentry.h			\
	$(liblumiera_la_srcdir)/configitem.h			\
	$(liblumiera_la_srcdir)/config_lookup.h			\
	$(liblumiera_la_srcdir)/config_interface.h		\
	$(liblumiera_la_srcdir)/allocationcluster.hpp		\
	$(liblumiera_la_srcdir)/scopedholdertransfer.hpp	\
	$(liblumiera_la_srcdir)/scopedholder.hpp		\
	$(liblumiera_la_srcdir)/external/libgavl.hpp		\
	$(liblumiera_la_srcdir)/lifecycleregistry.hpp
