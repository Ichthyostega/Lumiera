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
test_error_SOURCES = $(tests_srcdir)/library/test-error.c
test_error_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_error_LDADD = $(LUMIERA_LIBS) liblumiera.la

check_PROGRAMS += test-locking
test_locking_SOURCES = $(tests_srcdir)/library/test-locking.c
test_locking_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_locking_LDADD = $(LUMIERA_LIBS) liblumiera.la

check_PROGRAMS += test-llist
test_llist_SOURCES = $(tests_srcdir)/library/test-llist.c
test_llist_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_llist_LDADD = $(LUMIERA_LIBS) liblumiera.la

check_PROGRAMS += test-mpool
test_mpool_SOURCES = $(tests_srcdir)/library/test-mpool.c
test_mpool_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_mpool_LDADD = $(LUMIERA_LIBS) liblumiera.la

check_PROGRAMS += test-psplay
test_psplay_SOURCES = $(tests_srcdir)/library/test-psplay.c
test_psplay_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src/
test_psplay_LDADD = $(LUMIERA_LIBS) liblumiera.la

check_PROGRAMS += test-safeclib
test_safeclib_SOURCES = $(tests_srcdir)/library/test-safeclib.c
test_safeclib_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_safeclib_LDADD = $(LUMIERA_LIBS) liblumiera.la

check_PROGRAMS += test-time
test_time_SOURCES = $(tests_srcdir)/library/test-time.c
test_time_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_time_LDADD = $(LUMIERA_LIBS) liblumiera.la

check_PROGRAMS += test-luid
test_luid_SOURCES = $(tests_srcdir)/library/test-luid.c
test_luid_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_luid_LDADD = $(LUMIERA_LIBS) liblumiera.la

check_PROGRAMS += test-filedescriptors
test_filedescriptors_SOURCES = $(tests_srcdir)/backend/test-filedescriptors.c
test_filedescriptors_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_filedescriptors_LDADD = $(LUMIERA_LIBS) liblumierabackend.la liblumieracommon.la liblumieraproc.la
# FIXME stray dependencies on proc

check_PROGRAMS += test-filehandles
test_filehandles_SOURCES = $(tests_srcdir)/backend/test-filehandles.c
test_filehandles_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_filehandles_LDADD = $(LUMIERA_LIBS) liblumierabackend.la liblumieracommon.la liblumieraproc.la
# FIXME stray dependencies on proc

check_PROGRAMS += test-filemmap
test_filemmap_SOURCES = $(tests_srcdir)/backend/test-filemmap.c
test_filemmap_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src/
test_filemmap_LDADD = $(LUMIERA_LIBS) liblumierabackend.la liblumieracommon.la liblumieraproc.la
# FIXME stray dependencies on proc

#check_PROGRAMS += test-resourcecollector
#test_resourcecollector_SOURCES = $(tests_srcdir)/backend/test-resourcecollector.c
#test_resourcecollector_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src/
#test_resourcecollector_LDADD = $(LUMIERA_LIBS) liblumiera.la liblumierabackend.la liblumieraproc.la liblumieracommon.la

check_PROGRAMS += test-fileheader
test_fileheader_SOURCES = $(tests_srcdir)/backend/test-fileheader.c
test_fileheader_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src/
test_fileheader_LDADD = $(LUMIERA_LIBS) liblumierabackend.la liblumieracommon.la liblumieraproc.la
# FIXME stray dependencies on proc

# FIXME stray dependencies on proc

check_PROGRAMS += test-slist
test_slist_SOURCES = $(tests_srcdir)/library/test-slist.c
test_slist_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_slist_LDADD = $(LUMIERA_LIBS) liblumiera.la

check_PROGRAMS += test-config
test_config_SOURCES = $(tests_srcdir)/common/test-config.c
test_config_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src/
test_config_LDADD = $(LUMIERA_LIBS) liblumieracommon.la	liblumieraproc.la liblumierabackend.la
# FIXME stray dependencies on proc

check_LTLIBRARIES += examplepluginc.la
examplepluginc_la_SOURCES = $(tests_srcdir)/plugin/examplepluginc/example_plugin.c
examplepluginc_la_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -DLUMIERA_PLUGIN -I$(top_srcdir)/src/
examplepluginc_la_LDFLAGS = -module -avoid-version -no-undefined -rpath /dev/null -shrext .lum

check_PROGRAMS += test-interfaces
test_interfaces_SOURCES = $(tests_srcdir)/common/test-interfaces.c
test_interfaces_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror
test_interfaces_LDADD =	$(LUMIERA_LIBS) liblumieracommon.la liblumierabackend.la liblumieraproc.la
# FIXME stray dependency on backend, proc

check_PROGRAMS += test-threads
test_threads_SOURCES = $(tests_srcdir)/backend/test-threads.c
test_threads_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src/
test_threads_CFLAGS = $(AM_CFLAGS) $(PTHREAD_CFLAGS)
test_threads_LDADD = liblumierabackend.la liblumieracommon.la liblumieraproc.la

check_PROGRAMS += test-threadpool
test_threadpool_SOURCES = $(tests_srcdir)/backend/test-threadpool.c
test_threadpool_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu99 -Wall -Werror -I$(top_srcdir)/src/
test_threadpool_CFLAGS = $(AM_CFLAGS) $(LUMIERA_BACKEND_CFLAGS)
test_threadpool_LDADD = $(LUMIERA_LIBS) liblumierabackend.la liblumieracommon.la liblumieraproc.la

TESTS = $(tests_srcdir)/test.sh
