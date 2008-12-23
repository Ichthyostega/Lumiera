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
test_error_LDADD = liblumiera.la $(NOBUGMT_LUMIERA_LIBS)     liblumieracommon.la liblumieraproc.la -ldl -lboost_program_options-mt -lboost_regex-mt

check_PROGRAMS += test-locking
test_locking_SOURCES = $(tests_srcdir)/library/test-locking.c
test_locking_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_locking_LDADD = liblumiera.la  $(NOBUGMT_LUMIERA_LIBS)     liblumieracommon.la liblumieraproc.la -ldl -lboost_program_options-mt -lboost_regex-mt

check_PROGRAMS += test-llist
test_llist_SOURCES = $(tests_srcdir)/library/test-llist.c
test_llist_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_llist_LDADD = liblumiera.la $(NOBUGMT_LUMIERA_LIBS)     liblumieracommon.la liblumieraproc.la -ldl -lboost_program_options-mt -lboost_regex-mt

check_PROGRAMS += test-psplay
test_psplay_SOURCES = $(tests_srcdir)/library/test-psplay.c
test_psplay_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src/
test_psplay_LDADD = liblumiera.la $(NOBUGMT_LUMIERA_LIBS)     liblumieracommon.la liblumieraproc.la -ldl -lboost_program_options-mt -lboost_regex-mt

check_PROGRAMS += test-safeclib
test_safeclib_SOURCES = $(tests_srcdir)/library/test-safeclib.c
test_safeclib_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_safeclib_LDADD = liblumiera.la $(NOBUGMT_LUMIERA_LIBS)     liblumieracommon.la liblumieraproc.la -ldl -lboost_program_options-mt -lboost_regex-mt

check_PROGRAMS += test-luid
test_luid_SOURCES = $(tests_srcdir)/library/test-luid.c
test_luid_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_luid_LDADD = liblumiera.la $(NOBUGMT_LUMIERA_LIBS)     liblumieracommon.la liblumieraproc.la -ldl -lboost_program_options-mt -lboost_regex-mt

check_PROGRAMS += test-filedescriptors
test_filedescriptors_SOURCES = $(tests_srcdir)/backend/test-filedescriptors.c
test_filedescriptors_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_filedescriptors_LDADD = liblumierabackend.la liblumiera.la $(NOBUGMT_LUMIERA_LIBS)     liblumieracommon.la liblumieraproc.la -ldl -lboost_program_options-mt -lboost_regex-mt

check_PROGRAMS += test-filehandles
test_filehandles_SOURCES = $(tests_srcdir)/backend/test-filehandles.c
test_filehandles_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_filehandles_LDADD = liblumierabackend.la liblumiera.la $(NOBUGMT_LUMIERA_LIBS)     liblumieracommon.la liblumieraproc.la -ldl -lboost_program_options-mt -lboost_regex-mt


check_PROGRAMS += test-config
test_config_SOURCES = $(tests_srcdir)/common/test-config.c
test_config_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src/
test_config_LDADD =				\
	liblumieraproc.la			\
	liblumieracommon.la			\
	liblumiera.la				\
	$(LUMIERA_PLUGIN_LIBS)			\
	$(NOBUGMT_LUMIERA_LIBS)			\
	-ldl					\
	-lboost_program_options-mt		\
	-lboost_regex-mt



check_LTLIBRARIES += examplepluginc.la
examplepluginc_la_SOURCES = $(tests_srcdir)/plugin/example_plugin.c
examplepluginc_la_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -DLUMIERA_PLUGIN -I$(top_srcdir)/src/
examplepluginc_la_LDFLAGS = -module -avoid-version -no-undefined -rpath /dev/null

check_PROGRAMS += test-interfaces
test_interfaces_SOURCES = $(tests_srcdir)/common/test-interfaces.c
test_interfaces_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_interfaces_LDADD = liblumiera.la $(LUMIERA_PLUGIN_LIBS) $(NOBUGMT_LUMIERA_LIBS)      liblumieracommon.la liblumieraproc.la -ldl -lboost_program_options-mt -lboost_regex-mt
test_interfaces_DEPENDENCIES = examplepluginc.la liblumiera.la 

check_PROGRAMS += test-filemmap
test_filemmap_SOURCES = $(tests_srcdir)/backend/test-filemmap.c
test_filemmap_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src/
test_filemmap_LDADD = liblumierabackend.la liblumiera.la -lnobugmt -lpthread -ldl -lm     liblumieracommon.la liblumieraproc.la -ldl -lboost_program_options-mt -lboost_regex-mt

check_PROGRAMS += test-resourcecollector
test_resourcecollector_SOURCES = $(tests_srcdir)/library/test-resourcecollector.c
test_resourcecollector_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src/
test_resourcecollector_LDADD = liblumierabackend.la liblumiera.la -lnobugmt -lpthread -ldl -lm     liblumieracommon.la liblumieraproc.la -ldl -lboost_program_options-mt -lboost_regex-mt

TESTS = $(tests_srcdir)/test.sh
