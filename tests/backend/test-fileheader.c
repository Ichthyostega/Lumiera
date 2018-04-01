/*
  TEST-FILEHEADER  -  File identification

  Copyright (C)         Lumiera.org
    2010,               Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/

/** @file test-fileheader.c
 ** C unit test to verify file identification functions
 ** @see fileheader.h
 */


#include "backend/backend.h"
#include "backend/fileheader.h"

#include "lib/test/test.h"

#include "backend/filehandlecache.h"
extern LumieraFilehandlecache lumiera_fhcache;

TESTS_BEGIN


TEST (create_basic)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-fileheader", LUMIERA_FILE_RECREATE);

  LUMIERA_FILE_WRLOCK_SECTION (NOBUG_ON, file)
    {
      lumiera_fileheader header = lumiera_fileheader_create (file, "TEST", 0, sizeof (lumiera_fileheader_raw), LUMIERA_FILEHEADER_FLAG_ENDIANESS);

      CHECK (lumiera_error_peek() == NULL);

      ECHO ("fileheader: %s:", (char*)header.header);

      lumiera_fileheader_close (&header, LUMIERA_FILEHEADER_FLAG_CLEAN);
    }

  lumiera_file_delete (file);
  lumiera_backend_destroy ();
}



TEST (create_nowrite)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-fileheader", LUMIERA_FILE_READONLY);
  CHECK(file);

  LUMIERA_FILE_RDLOCK_SECTION (NOBUG_ON, file)
    {
      lumiera_fileheader header = lumiera_fileheader_create (file, "TEST", 0, sizeof (lumiera_fileheader), NULL);

      CHECK (lumiera_error() == LUMIERA_ERROR_FILEHEADER_NOWRITE);

      lumiera_fileheader_close (&header, LUMIERA_FILEHEADER_FLAG_CLEAN);
    }

  lumiera_file_delete (file);
  lumiera_backend_destroy ();
}



TEST (acquire_wrongheader)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-fileheader", LUMIERA_FILE_READONLY);

  LUMIERA_FILE_RDLOCK_SECTION (NOBUG_ON, file)
    {
      lumiera_fileheader header = lumiera_fileheader_open (file, "BADH",
                                                           sizeof (lumiera_fileheader),
                                                           LUMIERA_FILEHEADER_FLAG_CLEAN, LUMIERA_FILEHEADER_FLAG_CLEAN);

      CHECK (!header.header);
      CHECK (lumiera_error() == LUMIERA_ERROR_FILEHEADER_HEADER);

      lumiera_fileheader_close (&header, LUMIERA_FILEHEADER_FLAG_CLEAN);
    }

  lumiera_file_delete (file);
  lumiera_backend_destroy ();
}



TEST (acquire_basic)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-fileheader", LUMIERA_FILE_READWRITE);

  LUMIERA_FILE_RDLOCK_SECTION (NOBUG_ON, file)
    {
      lumiera_fileheader header = lumiera_fileheader_open (file, "TEST",
                                                           sizeof (lumiera_fileheader),
                                                           LUMIERA_FILEHEADER_FLAG_CLEAN, LUMIERA_FILEHEADER_FLAG_CLEAN);

      CHECK (header.header);
      CHECK (!lumiera_error());

      CHECK (lumiera_fileheader_version (&header) == 0);

      ECHO ("fileheader: %s:", (char*)header.header);

      lumiera_fileheader_close (&header, LUMIERA_FILEHEADER_FLAG_CLEAN);
    }

  lumiera_file_delete (file);
  lumiera_backend_destroy ();

}


TEST (acquire_basic_readonly)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-fileheader", LUMIERA_FILE_READONLY);

  LUMIERA_FILE_RDLOCK_SECTION (NOBUG_ON, file)
    {
      lumiera_fileheader header = lumiera_fileheader_open (file, "TEST",
                                                           sizeof (lumiera_fileheader),
                                                           LUMIERA_FILEHEADER_FLAG_CLEAN, NULL);

      CHECK (header.header);
      CHECK (!lumiera_error());

      CHECK (lumiera_fileheader_version (&header) == 0);

      ECHO ("fileheader: %s:", (char*)header.header);

      lumiera_fileheader_close (&header, LUMIERA_FILEHEADER_FLAG_CLEAN);
    }

  lumiera_file_delete (file);
  lumiera_backend_destroy ();

}



TESTS_END
