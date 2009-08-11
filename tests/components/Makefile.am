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

testcomponents_srcdir = $(top_srcdir)/tests/components

check_PROGRAMS += test-components
test_components_CPPFLAGS = $(AM_CPPFLAGS) -Wall -I$(testcomponents_srcdir)

test_components_LDADD =								\
	liblumieraproc.la							\
	liblumiprocengine.la							\
	liblumiprocmobjectbuilder.la						\
	liblumiprocmobjectsession.la						\
	liblumiprocasset.la							\
	liblumiprocmobject.la							\
	liblumierabackend.la							\
	liblumieracommon.la  							\
	liblumiera.la								\
	$(NOBUGMT_LUMIERA_LIBS) -ldl -lboost_program_options-mt -lboost_regex-mt

test_components_SOURCES =							\
	$(testcomponents_srcdir)/backend/mediaaccessmock.cpp			\
	$(testcomponents_srcdir)/backend/mediaaccessmocktest.cpp		\
	$(testcomponents_srcdir)/proc/asset/assetcategorytest.cpp		\
	$(testcomponents_srcdir)/proc/asset/compoundmediatest.cpp		\
	$(testcomponents_srcdir)/proc/asset/deleteassettest.cpp			\
	$(testcomponents_srcdir)/proc/asset/identityofassetstest.cpp		\
	$(testcomponents_srcdir)/proc/asset/mediastructurequerytest.cpp		\
	$(testcomponents_srcdir)/proc/asset/basicpipetest.cpp			\
	$(testcomponents_srcdir)/proc/asset/createassettest.cpp			\
	$(testcomponents_srcdir)/proc/asset/dependent-assets-test.cpp		\
	$(testcomponents_srcdir)/proc/asset/makecliptest.cpp			\
	$(testcomponents_srcdir)/proc/asset/orderingofassetstest.cpp		\
	$(testcomponents_srcdir)/proc/asset/testasset.cpp			\
	$(testcomponents_srcdir)/proc/control/command-basic-test.cpp		\
	$(testcomponents_srcdir)/proc/control/command-argument-test.cpp		\
	$(testcomponents_srcdir)/proc/control/command-mutation-test.cpp		\
	$(testcomponents_srcdir)/proc/control/command-use1-test.cpp		\
	$(testcomponents_srcdir)/proc/control/command-use2-test.cpp		\
	$(testcomponents_srcdir)/proc/control/command-use3-test.cpp		\
	$(testcomponents_srcdir)/proc/control/command-registry-test.cpp		\
	$(testcomponents_srcdir)/proc/control/argument-tuple-accept-test.cpp	\
	$(testcomponents_srcdir)/proc/control/memento-tie-test.cpp		\
	$(testcomponents_srcdir)/proc/engine/bufftabletest.cpp			\
	$(testcomponents_srcdir)/proc/engine/sourcenodetest.cpp         	\
	$(testcomponents_srcdir)/proc/mobject/builder/buildertooltest.cpp	\
	$(testcomponents_srcdir)/proc/mobject/builder/buildsegmenttest.cpp	\
	$(testcomponents_srcdir)/proc/mobject/controller/rendersegmenttest.cpp	\
	$(testcomponents_srcdir)/proc/mobject/mobject-ref-test.cpp		\
	$(testcomponents_srcdir)/proc/mobject/placement-basic-test.cpp		\
	$(testcomponents_srcdir)/proc/mobject/placement-hierarchy-test.cpp	\
	$(testcomponents_srcdir)/proc/mobject/placement-index-test.cpp		\
	$(testcomponents_srcdir)/proc/mobject/placement-ref-test.cpp		\
	$(testcomponents_srcdir)/proc/mobject/session/addcliptest.cpp		\
	$(testcomponents_srcdir)/proc/mobject/session/deletecliptest.cpp	\
	$(testcomponents_srcdir)/proc/mobject/session/rebuildfixturetest.cpp	\
	$(testcomponents_srcdir)/proc/mobject/session/sessionmanagertest.cpp	\
	$(testcomponents_srcdir)/proc/mobject/session/testclip.cpp		\
	$(testcomponents_srcdir)/proc/mobject/session/defsmanagerimpltest.cpp	\
	$(testcomponents_srcdir)/proc/mobject/session/defsmanagertest.cpp	\
	$(testcomponents_srcdir)/proc/mobject/session/defsregistryimpltest.cpp	\
	$(testcomponents_srcdir)/proc/mobject/session/sessionstructuretest.cpp	\
	$(testcomponents_srcdir)/mainsuite.cpp

noinst_HEADERS +=							\
	$(testcomponents_srcdir)/backend/mediaaccessmock.hpp		\
	$(testcomponents_srcdir)/proc/asset/testclipasset.hpp		\
	$(testcomponents_srcdir)/proc/asset/assetdiagnostics.hpp	\
	$(testcomponents_srcdir)/proc/asset/testasset.hpp		\
	$(testcomponents_srcdir)/proc/mobject/session/testclip.hpp	\
	$(testcomponents_srcdir)/proc/mobject/session/testsession1.hpp
