#  Copyright (C)         Lumiera.org
#    2007-2008,          Christian Thaeter <ct@pipapo.org>
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

tests_srcdir = $(top_srcdir)/tests

check_PROGRAMS += test-error
test_error_SOURCES = $(tests_srcdir)/error/errortest.c
test_error_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_error_LDADD = liblumi.a $(NOBUGMT_LUMIERA_LIBS) -ldl

check_PROGRAMS += test-locking
test_locking_SOURCES = \
	$(tests_srcdir)/locking/test-locking.c \
	$(tests_srcdir)/locking/mutex.c \
	$(tests_srcdir)/locking/condition.c
test_locking_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_locking_LDADD = liblumi.a $(NOBUGMT_LUMIERA_LIBS) -ldl -lm

check_PROGRAMS += test-llist
test_llist_SOURCES = $(tests_srcdir)/library/test-llist.c
test_llist_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_llist_LDADD = liblumi.a $(NOBUGMT_LUMIERA_LIBS) -ldl -lm

check_PROGRAMS += test-safeclib
test_safeclib_SOURCES = $(tests_srcdir)/library/test-safeclib.c
test_safeclib_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_safeclib_LDADD = $(builddir)/liblumi.a $(NOBUGMT_LUMIERA_LIBS) -ldl -lm

check_PROGRAMS += test-uuid
test_uuid_SOURCES = $(tests_srcdir)/library/test-uuid.c
test_uuid_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_uuid_LDADD = $(builddir)/liblumi.a $(NOBUGMT_LUMIERA_LIBS) -ldl -lm

check_PROGRAMS += test-references
test_references_SOURCES = $(tests_srcdir)/library/test-references.c
test_references_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_references_LDADD = liblumi.a $(NOBUGMT_LUMIERA_LIBS) -ldl -lm -lrt

check_PROGRAMS += test-filedescriptors
test_filedescriptors_SOURCES = $(tests_srcdir)/backend/test-filedescriptors.c
test_filedescriptors_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_filedescriptors_LDADD = liblumibackend.a liblumi.a $(NOBUGMT_LUMIERA_LIBS) -ldl -lm

check_PROGRAMS += test-filehandles
test_filehandles_SOURCES = $(tests_srcdir)/backend/test-filehandles.c
test_filehandles_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_filehandles_LDADD = liblumibackend.a liblumi.a $(NOBUGMT_LUMIERA_LIBS) -ldl -lm

TESTS = $(tests_srcdir)/test.sh
