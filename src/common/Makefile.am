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

liblumieracommon_la_CPPFLAGS = $(AM_CPPFLAGS)
liblumieracommon_la_CFLAGS = $(AM_CFLAGS) -std=gnu99 -Wextra -Wall -Werror
liblumieracommon_la_LIBADD = liblumiera.la

liblumieracommon_la_SOURCES =                  \
	$(liblumieracommon_la_srcdir)/guifacade.cpp   	\
	$(liblumieracommon_la_srcdir)/configfacade.cpp	\
	$(liblumieracommon_la_srcdir)/appstate.cpp     	\
	$(liblumieracommon_la_srcdir)/option.cpp 	\
	$(liblumieracommon_la_srcdir)/subsys.cpp 	\
	$(liblumieracommon_la_srcdir)/interfaceproxy.cpp\
	$(liblumieracommon_la_srcdir)/nobugcfg.cpp          


noinst_HEADERS +=               \
    $(lumiera_srcdir)/lumiera/subsys.hpp  		\
    $(lumiera_srcdir)/lumiera/appstate.hpp      	\
    $(lumiera_srcdir)/lumiera/option.hpp   		\
    $(lumiera_srcdir)/lumiera/subsystemrunner.hpp	\
    $(lumiera_srcdir)/lumiera/instancehandle.hpp

