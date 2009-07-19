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
test_lib_CPPFLAGS = $(AM_CPPFLAGS) -Wall -I$(testlib_srcdir)                                   	$(LUMIERA_GUI_CFLAGS)

test_lib_LDADD =			\
	liblumiprocmobjectbuilder.la	\
	liblumiprocmobjectsession.la	\
	liblumiprocmobject.la		\
	liblumiprocengine.la		\
	liblumiproccontrol.la		\
	liblumieraproc.la		\
	liblumiprocasset.la		\
	liblumierabackend.la		\
	liblumieracommon.la		\
	liblumiera.la			\
	$(LUMIERA_GUI_LIBS)			\
	$(NOBUGMT_LUMIERA_LIBS) -ldl -lboost_program_options-mt -lboost_regex-mt

test_lib_SOURCES =						\
	$(testlib_srcdir)/allocationclustertest.cpp		\
	$(testlib_srcdir)/appconfigtest.cpp			\
	$(testlib_srcdir)/customsharedptrtest.cpp		\
	$(testlib_srcdir)/bool-checkable-test.cpp		\
	$(testlib_srcdir)/exceptionerrortest.cpp		\
	$(testlib_srcdir)/factory-special-test.cpp		\
	$(testlib_srcdir)/factorytest.cpp			\
	$(testlib_srcdir)/hash-indexed-test.cpp			\
	$(testlib_srcdir)/helloworldtest.cpp			\
	$(testlib_srcdir)/lifecycletest.cpp			\
	$(testlib_srcdir)/iter-adapter-test.cpp			\
	$(testlib_srcdir)/mainsuite.cpp				\
	$(testlib_srcdir)/meta/typelist-test.cpp         	\
	$(testlib_srcdir)/meta/typelist-manip-test.cpp    	\
	$(testlib_srcdir)/meta/typeseq-manip-test.cpp    	\
	$(testlib_srcdir)/meta/type-tuple-test.cpp    		\
	$(testlib_srcdir)/meta/function-composition-test.cpp   	\
	$(testlib_srcdir)/meta/function-closure-test.cpp   	\
	$(testlib_srcdir)/meta/function-erasure-test.cpp   	\
	$(testlib_srcdir)/meta/generator-test.cpp        	\
	$(testlib_srcdir)/meta/config-flags-test.cpp      	\
	$(testlib_srcdir)/query/queryutilstest.cpp		\
	$(testlib_srcdir)/removefromsettest.cpp			\
	$(testlib_srcdir)/sanitised-identifier-test.cpp		\
	$(testlib_srcdir)/opaque-unchecked-buffer-test.cpp	\
	$(testlib_srcdir)/opaque-holder-test.cpp		\
	$(testlib_srcdir)/scoped-holder-test.cpp		\
	$(testlib_srcdir)/scopedholdertransfertest.cpp		\
	$(testlib_srcdir)/scoped-ptrvect-test.cpp		\
	$(testlib_srcdir)/singleton-subclass-test.cpp		\
	$(testlib_srcdir)/singleton-test.cpp			\
	$(testlib_srcdir)/singletontestmocktest.cpp		\
	$(testlib_srcdir)/streamtypebasicstest.cpp		\
	$(testlib_srcdir)/streamtypelifecycletest.cpp		\
	$(testlib_srcdir)/subsystem-runner-test.cpp		\
	$(testlib_srcdir)/sync-classlock-test.cpp		\
	$(testlib_srcdir)/sync-locking-test.cpp			\
	$(testlib_srcdir)/sync-waiting-test.cpp			\
	$(testlib_srcdir)/thread-wrapper-test.cpp		\
	$(testlib_srcdir)/thread-wrapper-join-test.cpp		\
	$(testlib_srcdir)/test/cmdlinewrappertest.cpp		\
	$(testlib_srcdir)/test/testoptiontest.cpp		\
	$(testlib_srcdir)/test/test-helper-test.cpp		\
	$(testlib_srcdir)/vectortransfertest.cpp		\
	$(testlib_srcdir)/visitingtoolconcept.cpp		\
	$(testlib_srcdir)/visitingtoolextendedtest.cpp		\
	$(testlib_srcdir)/visitingtooltest.cpp

noinst_HEADERS +=					\
	$(testlib_srcdir)/query/querydiagnostics.hpp	\
	$(testlib_srcdir)/testdummy.hpp 		\
	$(testlib_srcdir)/testtargetobj.hpp
