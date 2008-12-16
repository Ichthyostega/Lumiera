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

liblumiera_a_srcdir = $(top_srcdir)/src/lib
noinst_LIBRARIES += liblumiera.a

liblumiera_a_CFLAGS = $(CFLAGS) -std=gnu99 -Wall -Werror -DLUMIERA_PLUGIN_PATH="\"$(pkglibdir)\""

liblumiera_a_SOURCES =					\
	$(liblumiera_a_srcdir)/error.c			\
	$(liblumiera_a_srcdir)/mutex.c			\
	$(liblumiera_a_srcdir)/rwlock.c			\
	$(liblumiera_a_srcdir)/condition.c		\
	$(liblumiera_a_srcdir)/luid.c			\
	$(liblumiera_a_srcdir)/safeclib.c		\
	$(liblumiera_a_srcdir)/cuckoo.c			\
	$(liblumiera_a_srcdir)/psplay.c			\
	$(liblumiera_a_srcdir)/mrucache.c		\
	$(liblumiera_a_srcdir)/time.c			\
	$(liblumiera_a_srcdir)/config.c			\
	$(liblumiera_a_srcdir)/config_typed.c		\
	$(liblumiera_a_srcdir)/config_wordlist.c	\
	$(liblumiera_a_srcdir)/configentry.c		\
	$(liblumiera_a_srcdir)/configitem.c		\
	$(liblumiera_a_srcdir)/config_lookup.c		\
	$(liblumiera_a_srcdir)/config_interface.c	\
	$(liblumiera_a_srcdir)/interface.c		\
	$(liblumiera_a_srcdir)/interfaceregistry.c	\
	$(liblumiera_a_srcdir)/plugin.c			\
	$(liblumiera_a_srcdir)/plugin_dynlib.c		\
	$(liblumiera_a_srcdir)/allocationcluster.cpp	\
	$(liblumiera_a_srcdir)/external/libgavl.cpp	\
	$(liblumiera_a_srcdir)/cmdline.cpp

noinst_HEADERS +=					\
	$(liblumiera_a_srcdir)/error.h			\
	$(liblumiera_a_srcdir)/mutex.h			\
	$(liblumiera_a_srcdir)/rwlock.h			\
	$(liblumiera_a_srcdir)/condition.h		\
	$(liblumiera_a_srcdir)/luid.h			\
	$(liblumiera_a_srcdir)/safeclib.h		\
	$(liblumiera_a_srcdir)/cuckoo.h			\
	$(liblumiera_a_srcdir)/psplay.h			\
	$(liblumiera_a_srcdir)/mrucache.h		\
	$(liblumiera_a_srcdir)/time.h			\
	$(liblumiera_a_srcdir)/ppmpl.h			\
	$(liblumiera_a_srcdir)/interface.h		\
	$(liblumiera_a_srcdir)/interfaceregistry.h	\
	$(liblumiera_a_srcdir)/interfacedescriptor.h	\
	$(liblumiera_a_srcdir)/plugin.h			\
	$(liblumiera_a_srcdir)/config.h			\
	$(liblumiera_a_srcdir)/configentry.h		\
	$(liblumiera_a_srcdir)/configitem.h		\
	$(liblumiera_a_srcdir)/config_lookup.h		\
	$(liblumiera_a_srcdir)/config_interface.h	\
	$(liblumiera_a_srcdir)/allocationcluster.hpp	\
	$(liblumiera_a_srcdir)/scopedholdertransfer.hpp	\
	$(liblumiera_a_srcdir)/scopedholder.hpp		\
	$(liblumiera_a_srcdir)/external/libgavl.hpp	\
	$(liblumiera_a_srcdir)/lifecycleregistry.hpp
