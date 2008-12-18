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

testlib_srcdir = $(top_srcdir)/tests/lib

check_PROGRAMS += test-lib
test_lib_CPPFLAGS = $(AM_CPPFLAGS) -Wall -I$(testlib_srcdir)
test_lib_LDADD =									\
	liblumiprocmobjectbuilder.a							\
	liblumiprocmobjectsession.a							\
	liblumiprocmobject.a								\
	liblumiprocengine.a								\
	liblumiproccontrol.a								\
	liblumiproc.a									\
	liblumiprocasset.a								\
	liblumibackend.a								\
	liblumicommon.a									\
	liblumieracore.a								\
	liblumiera.a									\
	$(NOBUGMT_LUMIERA_LIBS) -ldl -lboost_program_options-mt -lboost_regex-mt

test_lib_SOURCES =						\
	$(testlib_srcdir)/allocationclustertest.cpp		\
	$(testlib_srcdir)/appconfigtest.cpp			\
	$(testlib_srcdir)/customsharedptrtest.cpp		\
	$(testlib_srcdir)/exceptionerrortest.cpp		\
	$(testlib_srcdir)/factoryspecialtest.cpp		\
	$(testlib_srcdir)/factorytest.cpp			\
	$(testlib_srcdir)/helloworldtest.cpp			\
	$(testlib_srcdir)/lifecycletest.cpp			\
	$(testlib_srcdir)/mainsuite.cpp				\
	$(testlib_srcdir)/meta/typelisttest.cpp         	\
	$(testlib_srcdir)/meta/typelistmaniptest.cpp    	\
	$(testlib_srcdir)/meta/generatortest.cpp        	\
	$(testlib_srcdir)/meta/configflagstest.cpp      	\
	$(testlib_srcdir)/query/queryutilstest.cpp		\
	$(testlib_srcdir)/removefromsettest.cpp			\
	$(testlib_srcdir)/sanitizedidentifiertest.cpp		\
	$(testlib_srcdir)/scopedholdertest.cpp			\
	$(testlib_srcdir)/scopedholdertransfertest.cpp		\
	$(testlib_srcdir)/singletonsubclasstest.cpp		\
	$(testlib_srcdir)/singletontest.cpp			\
	$(testlib_srcdir)/singletontestmocktest.cpp		\
	$(testlib_srcdir)/streamtypebasicstest.cpp		\
	$(testlib_srcdir)/streamtypelifecycletest.cpp		\
	$(testlib_srcdir)/test/cmdlinewrappertest.cpp		\
	$(testlib_srcdir)/test/testoptiontest.cpp		\
	$(testlib_srcdir)/vectortransfertest.cpp		\
	$(testlib_srcdir)/visitingtoolconcept.cpp		\
	$(testlib_srcdir)/visitingtoolextendedtest.cpp		\
	$(testlib_srcdir)/visitingtooltest.cpp

noinst_HEADERS +=					\
	$(testlib_srcdir)/query/querydiagnostics.hpp	\
	$(testlib_srcdir)/testdummy.hpp 		\
	$(testlib_srcdir)/testtargetobj.hpp
