#  Copyright (C)         Lumiera.org
#    2008,               Christian Thaeter <ct@pipapo.org>
#                        Hermann Vosseler <Ichthyostega@web.de>
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

testcommon_srcdir = $(top_srcdir)/tests/common

check_PROGRAMS += test-common
test_common_CPPFLAGS = $(AM_CPPFLAGS) -Wall -I$(testcommon_srcdir)
test_common_LDADD =								\
	liblumiproc.a								\
	liblumiprocengine.a							\
	liblumiprocmobjectbuilder.a						\
	liblumiprocmobjectsession.a						\
	liblumiprocasset.a							\
	liblumiprocmobject.a							\
	liblumi.a								\
	liblumicommon.a								\
	liblumibackend.a							\
	$(NOBUGMT_LUMIERA_LIBS) -ldl -lboost_program_options-mt -lboost_regex-mt

test_common_SOURCES =						\
	$(testcommon_srcdir)/appconfigtest.cpp			\
	$(testcommon_srcdir)/customsharedptrtest.cpp		\
	$(testcommon_srcdir)/exceptionerrortest.cpp		\
	$(testcommon_srcdir)/factoryspecialtest.cpp		\
	$(testcommon_srcdir)/factorytest.cpp			\
	$(testcommon_srcdir)/helloworldtest.cpp			\
	$(testcommon_srcdir)/lifecycletest.cpp			\
	$(testcommon_srcdir)/mainsuite.cpp				\
	$(testcommon_srcdir)/query/queryutilstest.cpp		\
	$(testcommon_srcdir)/removefromsettest.cpp		\
	$(testcommon_srcdir)/sanitizedidentifiertest.cpp	\
	$(testcommon_srcdir)/singletonsubclasstest.cpp		\
	$(testcommon_srcdir)/singletontest.cpp			\
	$(testcommon_srcdir)/singletontestmocktest.cpp		\
	$(testcommon_srcdir)/test/cmdlinewrappertest.cpp	\
	$(testcommon_srcdir)/test/testoptiontest.cpp		\
	$(testcommon_srcdir)/meta/typelisttest.cpp         	\
	$(testcommon_srcdir)/meta/typelistmaniptest.cpp     	\
	$(testcommon_srcdir)/meta/generatortest.cpp        	\
	$(testcommon_srcdir)/meta/configflagstest.cpp      	\
	$(testcommon_srcdir)/visitingtoolconcept.cpp		\
	$(testcommon_srcdir)/visitingtoolextendedtest.cpp	\
	$(testcommon_srcdir)/visitingtooltest.cpp

noinst_HEADERS +=					\
	$(testcommon_srcdir)/query/querydiagnostics.hpp	\
	$(testcommon_srcdir)/testtargetobj.hpp
