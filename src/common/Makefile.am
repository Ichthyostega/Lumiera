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


# TODO use -Wextra -Werror

liblumicommon_a_srcdir = $(top_srcdir)/src/common
noinst_LIBRARIES += liblumicommon.a

liblumicommon_a_CXXFLAGS = $(CXXFLAGS) -Wall

liblumicommon_a_SOURCES =					\
	$(liblumicommon_a_srcdir)/lumitime.cpp		\
	$(liblumicommon_a_srcdir)/util.cpp			\
	$(liblumicommon_a_srcdir)/visitor.cpp			\
	$(liblumicommon_a_srcdir)/cmdline.cpp			\
	$(liblumicommon_a_srcdir)/configrules.cpp		\
	$(liblumicommon_a_srcdir)/error.cpp			\
	$(liblumicommon_a_srcdir)/query.cpp			\
	$(liblumicommon_a_srcdir)/query/mockconfigrules.cpp	\
	$(liblumicommon_a_srcdir)/test/suite.cpp		\
	$(liblumicommon_a_srcdir)/test/testoption.cpp


noinst_HEADERS +=						\
	$(liblumicommon_a_srcdir)/cmdline.hpp			\
	$(liblumicommon_a_srcdir)/factory.hpp			\
	$(liblumicommon_a_srcdir)/frameid.hpp			\
	$(liblumicommon_a_srcdir)/singleton.hpp			\
	$(liblumicommon_a_srcdir)/singletonpolicies.hpp		\
	$(liblumicommon_a_srcdir)/singletonpreconfigure.hpp	\
	$(liblumicommon_a_srcdir)/lumitime.hpp			\
	$(liblumicommon_a_srcdir)/visitor.hpp			\
	$(liblumicommon_a_srcdir)/visitordispatcher.hpp		\
	$(liblumicommon_a_srcdir)/visitorpolicies.hpp		\
	$(liblumicommon_a_srcdir)/configrules.hpp		\
	$(liblumicommon_a_srcdir)/error.hpp			\
	$(liblumicommon_a_srcdir)/multithread.hpp		\
	$(liblumicommon_a_srcdir)/p.hpp				\
	$(liblumicommon_a_srcdir)/query.hpp			\
	$(liblumicommon_a_srcdir)/query/mockconfigrules.hpp	\
	$(liblumicommon_a_srcdir)/singletonfactory.hpp		\
	$(liblumicommon_a_srcdir)/singletonsubclass.hpp		\
	$(liblumicommon_a_srcdir)/meta/typelist.hpp		\
	$(liblumicommon_a_srcdir)/meta/configflags.hpp		\
	$(liblumicommon_a_srcdir)/meta/generator.hpp		\
	$(liblumicommon_a_srcdir)/meta/typelistutil.hpp		\
	$(liblumicommon_a_srcdir)/util.hpp			\
	$(liblumicommon_a_srcdir)/test/mockinjector.hpp		\
	$(liblumicommon_a_srcdir)/test/suite.hpp		\
	$(liblumicommon_a_srcdir)/test/testoption.hpp		\
	$(liblumicommon_a_srcdir)/test/run.hpp

