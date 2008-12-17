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

bin_PROGRAMS += lumiera
lumiera_CPPFLAGS = $(AM_CPPFLAGS) -DLUMIERA_CONFIG_PATH="\"$(pkgdatadir)/config\""
lumiera_CFLAGS = $(AM_CFLAGS) -std=gnu99 -Wextra -Wall -Werror
lumiera_LDADD = liblumierabackend.la liblumieraproc.la liblumiera.la \
		$(LUMIERA_PLUGIN_LIBS) $(NOBUGMT_LUMIERA_LIBS) -lboost_program_options-mt -lboost_regex-mt

lumiera_SOURCES =				\
	$(lumiera_srcdir)/guifacade.cpp		\
	$(lumiera_srcdir)/configfacade.cpp	\
	$(lumiera_srcdir)/appstate.cpp		\
	$(lumiera_srcdir)/option.cpp		\
	$(lumiera_srcdir)/subsys.cpp		\
	$(lumiera_srcdir)/interfaceproxy.cpp	\
	$(lumiera_srcdir)/nobugcfg.cpp		\
	$(lumiera_srcdir)/guistart.cpp		\
	$(lumiera_srcdir)/main.cpp


noinst_HEADERS +=				\
	$(lumiera_srcdir)/subsys.hpp		\
	$(lumiera_srcdir)/appstate.hpp		\
	$(lumiera_srcdir)/option.hpp		\
	$(lumiera_srcdir)/subsystemrunner.hpp	\
	$(lumiera_srcdir)/instancehandle.hpp
