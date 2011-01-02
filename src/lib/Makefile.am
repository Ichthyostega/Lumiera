#  Copyright (C)         Lumiera.org
#    2007, 2008, 2009    Christian Thaeter <ct@pipapo.org>
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

liblumiera_la_srcdir = $(top_srcdir)/src/lib
lib_LTLIBRARIES += liblumiera.la

liblumiera_la_CFLAGS = 						\
	$(AM_CFLAGS)						\
	$(LUMIERA_LIBRARY_CFLAGS)				\
	-std=gnu99 -Wall -Wextra -Werror

liblumiera_la_CXXFLAGS =					\
	$(AM_CXXFLAGS)						\
	$(LUMIERA_LIBRARY_CFLAGS)				\
	-Wall -Wextra

liblumiera_la_LIBADD =	 					\
	$(LUMIERA_LIBRARY_LIBS)


liblumiera_la_SOURCES =					\
	$(liblumiera_la_srcdir)/advice/advice.cpp	\
	$(liblumiera_la_srcdir)/advice/binding.cpp	\
	$(liblumiera_la_srcdir)/allocation-cluster.cpp	\
	$(liblumiera_la_srcdir)/cmdline.cpp		\
	$(liblumiera_la_srcdir)/condition.c		\
	$(liblumiera_la_srcdir)/error.c			\
	$(liblumiera_la_srcdir)/exception.cpp		\
	$(liblumiera_la_srcdir)/lifecycle.cpp		\
	$(liblumiera_la_srcdir)/lockerror.c		\
	$(liblumiera_la_srcdir)/logging.cpp		\
	$(liblumiera_la_srcdir)/luid.c			\
	$(liblumiera_la_srcdir)/mpool.c			\
	$(liblumiera_la_srcdir)/mrucache.c		\
	$(liblumiera_la_srcdir)/mutex.c			\
	$(liblumiera_la_srcdir)/nobug-init.cpp		\
	$(liblumiera_la_srcdir)/psplay.c		\
	$(liblumiera_la_srcdir)/query.cpp		\
	$(liblumiera_la_srcdir)/reccondition.c		\
	$(liblumiera_la_srcdir)/recmutex.c		\
	$(liblumiera_la_srcdir)/rwlock.c		\
	$(liblumiera_la_srcdir)/safeclib.c		\
	$(liblumiera_la_srcdir)/streamtype.cpp		\
	$(liblumiera_la_srcdir)/symbol-impl.cpp		\
	$(liblumiera_la_srcdir)/test/suite.cpp		\
	$(liblumiera_la_srcdir)/test/test-helper.cpp	\
	$(liblumiera_la_srcdir)/test/testoption.cpp	\
	$(liblumiera_la_srcdir)/time.c			\
	$(liblumiera_la_srcdir)/time/lumitime.cpp	\
	$(liblumiera_la_srcdir)/time/quantiser.cpp	\
	$(liblumiera_la_srcdir)/time/timecode.cpp	\
	$(liblumiera_la_srcdir)/tmpbuf.c		\
	$(liblumiera_la_srcdir)/util.cpp


noinst_HEADERS +=						\
	$(liblumiera_la_srcdir)/advice.hpp			\
	$(liblumiera_la_srcdir)/advice/binding.hpp		\
	$(liblumiera_la_srcdir)/advice/index.hpp		\
	$(liblumiera_la_srcdir)/allocation-cluster.hpp		\
	$(liblumiera_la_srcdir)/allocation-cluster.hpp		\
	$(liblumiera_la_srcdir)/condition.h			\
	$(liblumiera_la_srcdir)/error.h				\
	$(liblumiera_la_srcdir)/error.hpp			\
	$(liblumiera_la_srcdir)/factory.hpp			\
	$(liblumiera_la_srcdir)/frameid.hpp			\
	$(liblumiera_la_srcdir)/lifecycleregistry.hpp		\
	$(liblumiera_la_srcdir)/lockerror.h			\
	$(liblumiera_la_srcdir)/luid.h				\
	$(liblumiera_la_srcdir)/lumitime.hpp			\
	$(liblumiera_la_srcdir)/meta/configflags.hpp		\
	$(liblumiera_la_srcdir)/meta/generator.hpp		\
	$(liblumiera_la_srcdir)/meta/typelist-util.hpp		\
	$(liblumiera_la_srcdir)/meta/typelist.hpp		\
	$(liblumiera_la_srcdir)/mpool.h				\
	$(liblumiera_la_srcdir)/mrucache.h			\
	$(liblumiera_la_srcdir)/mrucache.h			\
	$(liblumiera_la_srcdir)/mutex.h				\
	$(liblumiera_la_srcdir)/nobug-init.hpp			\
	$(liblumiera_la_srcdir)/observable-list.hpp		\
	$(liblumiera_la_srcdir)/p.hpp				\
	$(liblumiera_la_srcdir)/ppmpl.h				\
	$(liblumiera_la_srcdir)/ppmpl.h				\
	$(liblumiera_la_srcdir)/psplay.h			\
	$(liblumiera_la_srcdir)/psplay.h			\
	$(liblumiera_la_srcdir)/query.hpp			\
	$(liblumiera_la_srcdir)/reccondition.h			\
	$(liblumiera_la_srcdir)/recmutex.h			\
	$(liblumiera_la_srcdir)/rwlock.h			\
	$(liblumiera_la_srcdir)/safeclib.h			\
	$(liblumiera_la_srcdir)/scoped-holder.hpp		\
	$(liblumiera_la_srcdir)/singleton-subclass.hpp		\
	$(liblumiera_la_srcdir)/singleton.hpp			\
	$(liblumiera_la_srcdir)/singleton-factory.hpp		\
	$(liblumiera_la_srcdir)/singleton-policies.hpp		\
	$(liblumiera_la_srcdir)/singleton-preconfigure.hpp	\
	$(liblumiera_la_srcdir)/sync-classlock.hpp		\
	$(liblumiera_la_srcdir)/sync.hpp			\
	$(liblumiera_la_srcdir)/sync.hpp			\
	$(liblumiera_la_srcdir)/test/mockinjector.hpp		\
	$(liblumiera_la_srcdir)/test/run.hpp			\
	$(liblumiera_la_srcdir)/test/suite.hpp			\
	$(liblumiera_la_srcdir)/test/test-helper.hpp		\
	$(liblumiera_la_srcdir)/test/testoption.hpp		\
	$(liblumiera_la_srcdir)/time.h				\
	$(liblumiera_la_srcdir)/time.h				\
	$(liblumiera_la_srcdir)/tmpbuf.h			\
	$(liblumiera_la_srcdir)/tree.hpp			\
	$(liblumiera_la_srcdir)/util.hpp			\
	$(liblumiera_la_srcdir)/visitor-dispatcher.hpp		\
	$(liblumiera_la_srcdir)/visitor-policies.hpp		\
	$(liblumiera_la_srcdir)/visitor.hpp

