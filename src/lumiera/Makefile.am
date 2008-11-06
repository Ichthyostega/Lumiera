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


noinst_LIBRARIES += liblumieraconfig.a
liblumieraconfig_a_CPPFLAGS = $(AM_CPPFLAGS)
liblumieraconfig_a_CFLAGS = $(AM_CFLAGS) -std=gnu99 -Wextra -Wall -Werror

liblumieraconfig_a_SOURCES =			\
	$(lumiera_srcdir)/config.c		\
	$(lumiera_srcdir)/config_typed.c	\
	$(lumiera_srcdir)/config_wordlist.c	\
	$(lumiera_srcdir)/configentry.c		\
	$(lumiera_srcdir)/configitem.c		\
	$(lumiera_srcdir)/config_lookup.c



noinst_LIBRARIES += liblumieraplugin.a
liblumieraplugin_a_CPPFLAGS = $(AM_CPPFLAGS) -DLUMIERA_PLUGIN_PATH="\"$(pkglibdir)\""
liblumieraplugin_a_CFLAGS = $(AM_CFLAGS) -std=gnu99 -Wextra -Wall -Werror

liblumieraplugin_a_SOURCES =			\
	$(lumiera_srcdir)/interface.c		\
	$(lumiera_srcdir)/interfaceregistry.c	\
	$(lumiera_srcdir)/plugin.c		\
	$(lumiera_srcdir)/plugin_dynlib.c



noinst_LIBRARIES += liblumierainit.a
liblumierainit_a_CPPFLAGS = $(AM_CPPFLAGS)
liblumierainit_a_CFLAGS = $(AM_CFLAGS) -std=gnu99 -Wextra -Wall -Werror

liblumierainit_a_SOURCES =			\
	$(lumiera_srcdir)/lumiera_init.c


bin_PROGRAMS += lumiera
lumiera_CPPFLAGS = $(AM_CPPFLAGS)
lumiera_CFLAGS = $(AM_CFLAGS) -std=gnu99 -Wextra -Wall -Werror
lumiera_LDADD = liblumierainit.a liblumieraconfig.a liblumieraplugin.a liblumiera.a $(LUMIERA_PLUGIN_LIBS) $(NOBUGMT_LUMIERA_LIBS)

lumiera_SOURCES =				\
	$(lumiera_srcdir)/lumiera.c


noinst_HEADERS +=				\
	$(lumiera_srcdir)/interface.h		\
	$(lumiera_srcdir)/interfaceregistry.h	\
	$(lumiera_srcdir)/interfacedescriptor.h	\
	$(lumiera_srcdir)/plugin.h		\
	$(lumiera_srcdir)/config.h		\
	$(lumiera_srcdir)/configentry.h		\
	$(lumiera_srcdir)/configitem.h		\
	$(lumiera_srcdir)/config_lookup.h	\
	$(lumiera_srcdir)/lumiera.h
