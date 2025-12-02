/*
  Luidgen  -  generate and replace Lumiera UIDs for source files

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file luidgen.c
 ** Generate a fixed LUID to be hard-wired into source code.
 ** This Helper generates and prints a Lumiera UID as octal escaped string
 ** or processes a file, replacing the token \c LUIDGEN with the octal representation
 ** of a newly generated LUID. This can be used to fill in some LUID values into
 ** new source code prior to compiling it for the first time.
 ** 
 ** The Lumiera build system generates stand-alone executable from this source file.
 ** 
 ** @see luid.h
 ** @see interface.h
 **
 */


#include "lib/tmpbuf.h"
#include "lib/luid.h"

#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <nobug.h>



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
