/*
  luidgen.c  - generate a lumiera uuid

  Copyright (C)         Lumiera.org
    2008                Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "lib/tmpbuf.h"
#include "lib/luid.h"

#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <nobug.h>

/**
 * @file
 * Generate amd print a Lumiera uid as octal escaped string
 * or process a file replaceing 'LUIDGEN' with a octal escaped string
 */


int
main (int argc, char** argv)
{
  NOBUG_INIT;
  lumiera_uid luid;

  if (argc == 1)
    {
      lumiera_uid_gen (&luid);
      printf ("\"");
      for (int i = 0; i < 16; ++i)
        printf ("\\%.3hho", *(((char*)&luid)+i));
      printf ("\"\n");
    }
  else
    {
      for (int i = 1; i < argc; ++i)
        {
          FILE* in = fopen (argv[i], "r");
          if (!in)
            {
              fprintf (stderr, "Failed to open file %s for reading: %s\n", argv[i], strerror (errno));
              continue;
            }

          char* outname = lumiera_tmpbuf_snprintf (SIZE_MAX, "%s.luidgen", argv[i]);
          FILE* out = fopen (outname, "wx");
          if (!out)
            {
              fprintf (stderr, "Failed to open file %s for writing: %s\n", outname, strerror (errno));
              fclose (in);
              continue;
            }

          char buf[4096];
          char luidbuf[67];

          printf ("Luidgen %s ", argv[i]); fflush (stdout);

          while (fgets (buf, 4096, in))
            {
              char* pos;
              while ((pos = strstr(buf, "LUIDGEN")))
                {
                  memmove (pos+66, pos+7, strlen (pos+7)+1);
                  lumiera_uid_gen (&luid);
                  sprintf (luidbuf, "\""LUMIERA_UID_FMT"\"", LUMIERA_UID_ELEMENTS(luid));
                  memcpy (pos, luidbuf, 66);
                  putchar ('.'); fflush (stdout);
                }
              fputs (buf, out);
            }

          fclose (out);
          fclose (in);

          char* backup = lumiera_tmpbuf_snprintf (SIZE_MAX, "%s~", argv[i]);
          unlink (backup);
 
          if (!!rename (argv[i], backup))
            {
              fprintf (stderr, "Failed to create backupfile %s: %s\n", backup, strerror (errno));
              continue;
            }

          if (!!rename (outname, argv[i]))
            {
              fprintf (stderr, "Renaming %s to %s failed: %s\n", outname, argv[i], strerror (errno));
              rename (backup, argv[i]);
              continue;
            }

          printf (" done\n");
        }
    }

  return 0;
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
