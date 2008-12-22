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

liblumieracommon_la_srcdir = $(top_srcdir)/src/common
lib_LTLIBRARIES += liblumieracommon.la

liblumieracommon_la_CPPFLAGS = $(AM_CPPFLAGS)  -DLUMIERA_CONFIG_PATH="\"$(pkgdatadir)/config\"" -DLUMIERA_PLUGIN_PATH="\"$(pkglibdir)/\""
liblumieracommon_la_CFLAGS = $(AM_CFLAGS) -std=gnu99 -Wextra -Wall -Werror
liblumieracommon_la_CXXFLAGS = $(AM_CXXFLAGS) -Wextra -Wall
liblumieracommon_la_LIBADD = liblumiera.la

liblumieracommon_la_SOURCES =						\
	$(liblumieracommon_la_srcdir)/config.c				\
	$(liblumieracommon_la_srcdir)/config_typed.c			\
	$(liblumieracommon_la_srcdir)/config_wordlist.c			\
	$(liblumieracommon_la_srcdir)/configentry.c			\
	$(liblumieracommon_la_srcdir)/configitem.c			\
	$(liblumieracommon_la_srcdir)/config_lookup.c			\
	$(liblumieracommon_la_srcdir)/config_interface.c		\
	$(liblumieracommon_la_srcdir)/configrules.cpp			\
	$(liblumieracommon_la_srcdir)/query/fake-configrules.cpp	\
	$(liblumieracommon_la_srcdir)/interface.c			\
	$(liblumieracommon_la_srcdir)/interfaceregistry.c		\
	$(liblumieracommon_la_srcdir)/plugin.c				\
	$(liblumieracommon_la_srcdir)/plugin_dynlib.c			\
	$(liblumieracommon_la_srcdir)/guifacade.cpp			\
	$(liblumieracommon_la_srcdir)/configfacade.cpp			\
	$(liblumieracommon_la_srcdir)/appstate.cpp			\
	$(liblumieracommon_la_srcdir)/option.cpp			\
	$(liblumieracommon_la_srcdir)/subsys.cpp			\
	$(liblumieracommon_la_srcdir)/interfaceproxy.cpp		\
	$(liblumieracommon_la_srcdir)/nobugcfg.cpp


noinst_HEADERS +=							\
	$(liblumieracommon_la_srcdir)/interface.h			\
	$(liblumieracommon_la_srcdir)/interfaceregistry.h		\
	$(liblumieracommon_la_srcdir)/interfacedescriptor.h		\
	$(liblumieracommon_la_srcdir)/plugin.h				\
	$(liblumieracommon_la_srcdir)/config.h				\
	$(liblumieracommon_la_srcdir)/configentry.h			\
	$(liblumieracommon_la_srcdir)/configitem.h			\
	$(liblumieracommon_la_srcdir)/config_lookup.h			\
	$(liblumieracommon_la_srcdir)/config_interface.h		\
	$(liblumieracommon_la_srcdir)/configrules.hpp			\
	$(liblumieracommon_la_srcdir)/query/fake-configrules.hpp	\
	$(liblumieracommon_la_srcdir)/subsys.hpp			\
	$(liblumieracommon_la_srcdir)/appstate.hpp			\
	$(liblumieracommon_la_srcdir)/option.hpp			\
	$(liblumieracommon_la_srcdir)/subsystemrunner.hpp		\
	$(liblumieracommon_la_srcdir)/instancehandle.hpp

