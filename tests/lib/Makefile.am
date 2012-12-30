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
test_lib_CPPFLAGS = $(AM_CPPFLAGS) -Wall -I$(testlib_srcdir) $(LUMIERA_GUI_CFLAGS)
#why gui cflags/libs?

test_lib_LDADD =				\
	$(LUMIERA_LIBRARY_LIBS)			\
	$(LUMIERA_GUI_LIBS)			\
	liblumiera.la				\
	liblumierabackend.la			\
	liblumieracommon.la			\
	liblumieraproc.la			\
	liblumiprocasset.la			\
	liblumiproccontrol.la			\
	liblumiprocengine.la			\
	liblumiprocmobject.la			\
	liblumiprocmobjectbuilder.la		\
	liblumiprocmobjectsession.la

test_lib_SOURCES =							\
	$(testlib_srcdir)/advice/advice-basics-test.cpp			\
	$(testlib_srcdir)/advice/advice-binding-pattern-test.cpp	\
	$(testlib_srcdir)/advice/advice-configuration-test.cpp		\
	$(testlib_srcdir)/advice/advice-index-test.cpp			\
	$(testlib_srcdir)/advice/advice-multiplicity-test.cpp		\
	$(testlib_srcdir)/advice/advice-situations-test.cpp		\
	$(testlib_srcdir)/allocation-cluster-test.cpp			\
	$(testlib_srcdir)/app-config-test.cpp				\
	$(testlib_srcdir)/bool-checkable-test.cpp			\
	$(testlib_srcdir)/cmdline-wrapper-test.cpp			\
	$(testlib_srcdir)/custom-shared-ptr-test.cpp			\
	$(testlib_srcdir)/del-stash-test.cpp				\
	$(testlib_srcdir)/exceptionerrortest.cpp			\
	$(testlib_srcdir)/factory-special-test.cpp			\
	$(testlib_srcdir)/factorytest.cpp				\
	$(testlib_srcdir)/format-helper-test.cpp			\
	$(testlib_srcdir)/functor-util-test.cpp				\
	$(testlib_srcdir)/hash-indexed-test.cpp				\
	$(testlib_srcdir)/helloworldtest.cpp				\
	$(testlib_srcdir)/item-wrapper-test.cpp				\
	$(testlib_srcdir)/iter-adapter-stl-test.cpp			\
	$(testlib_srcdir)/iter-adapter-test.cpp				\
	$(testlib_srcdir)/iter-source-test.cpp				\
	$(testlib_srcdir)/itertools-test.cpp				\
	$(testlib_srcdir)/lifecycletest.cpp				\
	$(testlib_srcdir)/lumitime-test.cpp				\
	$(testlib_srcdir)/mainsuite.cpp					\
	$(testlib_srcdir)/meta/config-flags-test.cpp			\
	$(testlib_srcdir)/meta/duck-detector-test.cpp			\
	$(testlib_srcdir)/meta/function-closure-test.cpp		\
	$(testlib_srcdir)/meta/function-composition-test.cpp		\
	$(testlib_srcdir)/meta/function-erasure-test.cpp		\
	$(testlib_srcdir)/meta/generator-test.cpp			\
	$(testlib_srcdir)/meta/iterable-classification-test.cpp		\
	$(testlib_srcdir)/meta/type-tuple-test.cpp			\
	$(testlib_srcdir)/meta/typelist-manip-test.cpp			\
	$(testlib_srcdir)/meta/typelist-test.cpp			\
	$(testlib_srcdir)/meta/typeseq-manip-test.cpp			\
	$(testlib_srcdir)/multifact-argument-test.cpp			\
	$(testlib_srcdir)/multifact-test.cpp				\
	$(testlib_srcdir)/null-value-test.cpp				\
	$(testlib_srcdir)/opaque-holder-test.cpp			\
	$(testlib_srcdir)/opaque-unchecked-buffer-test.cpp		\
	$(testlib_srcdir)/optional-ref-test.cpp				\
	$(testlib_srcdir)/query/query-utils-test.cpp			\
	$(testlib_srcdir)/removefromsettest.cpp				\
	$(testlib_srcdir)/sanitised-identifier-test.cpp			\
	$(testlib_srcdir)/scoped-holder-test.cpp			\
	$(testlib_srcdir)/scoped-holder-transfer-test.cpp		\
	$(testlib_srcdir)/scoped-ptrvect-test.cpp			\
	$(testlib_srcdir)/singleton-subclass-test.cpp			\
	$(testlib_srcdir)/singleton-test.cpp				\
	$(testlib_srcdir)/singleton-testmock-test.cpp			\
	$(testlib_srcdir)/streamtypebasicstest.cpp			\
	$(testlib_srcdir)/streamtypelifecycletest.cpp			\
	$(testlib_srcdir)/sub-id-test.cpp				\
	$(testlib_srcdir)/subsystem-runner-test.cpp			\
	$(testlib_srcdir)/symbol-hashtable-test.cpp			\
	$(testlib_srcdir)/symbol-test.cpp				\
	$(testlib_srcdir)/sync-classlock-test.cpp			\
	$(testlib_srcdir)/sync-locking-test.cpp				\
	$(testlib_srcdir)/sync-waiting-test.cpp				\
	$(testlib_srcdir)/test/test-helper-test.cpp			\
	$(testlib_srcdir)/test/testoptiontest.cpp			\
	$(testlib_srcdir)/thread-wrapper-join-test.cpp			\
	$(testlib_srcdir)/thread-wrapper-test.cpp			\
	$(testlib_srcdir)/time/time-formats-test.cpp			\
	$(testlib_srcdir)/time/time-quantisation-test.cpp		\
	$(testlib_srcdir)/time/time-value-test.cpp			\
	$(testlib_srcdir)/typed-allocation-manager-test.cpp		\
	$(testlib_srcdir)/typed-counter-test.cpp			\
	$(testlib_srcdir)/util-foreach-test.cpp				\
	$(testlib_srcdir)/vector-transfer-test.cpp			\
	$(testlib_srcdir)/visitingtoolconcept.cpp			\
	$(testlib_srcdir)/visitingtoolextendedtest.cpp			\
	$(testlib_srcdir)/visitingtooltest.cpp

noinst_HEADERS +=					\
	$(testlib_srcdir)/query/querydiagnostics.hpp	\
	$(testlib_srcdir)/testdummy.hpp 		\
	$(testlib_srcdir)/testtargetobj.hpp
