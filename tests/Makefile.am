#  Copyright (C)         CinelerraCV
#    2007,               Christian Thaeter <ct@pipapo.org>
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
test_error_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src/
test_error_LDADD = $(builddir)/libcin3.a -lnobugmt -lpthread -ldl

TESTS = $(tests_srcdir)/test.sh
