/*
  Fileheader  -  Definitions of generic lumiera file headers and identification

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


/** @file fileheader.c
 ** Implementation of a common header format for working data files created by the Lumiera Vault.
 ** @todo development in this area is stalled since 2010
 */


#include "lib/tmpbuf.h"

#include "vault/fileheader.h"
#include "vault/file.h"
#include "include/logging.h"

#include <errno.h>




LUMIERA_ERROR_DEFINE (FILEHEADER_NOWRITE, "File is not writable");
LUMIERA_ERROR_DEFINE (FILEHEADER_HEADER, "Error in header");
LUMIERA_ERROR_DEFINE (FILEHEADER_FLAGS, "Inconsistent Flags");
LUMIERA_ERROR_DEFINE (FILEHEADER_FLAGSPACE, "No more space for flags left");
LUMIERA_ERROR_DEFINE (FILEHEADER_ENDIANESS, "Unsupported Endianess");



lumiera_fileheader
lumiera_fileheader_create (LumieraFile file, char* fourcc, int version, size_t size, const char* flags)
{
  lumiera_fileheader self = {NULL, NULL};

  REQUIRE (fourcc && strlen (fourcc) == 4, "fourcc must be of size 4 and nothing else: %s", fourcc);
  REQUIRE_IF (flags, strlen (flags) <= sizeof(self.header->flags), "to much flags given");

  if (file)
    {
      if (lumiera_file_checkflags (file, O_RDWR))
        {
          WARN_IF (version == 0, fileheader, "Experimental version 0 given for file %s, %s", file->name, fourcc);

          if ((self.map = lumiera_mmap_new_exact (file, 0, size)))
            {
              self.header = lumiera_mmap_address (self.map, 0);

              memcpy (self.header->fourcc, fourcc, 4);
              snprintf (self.header->version, 4, "%3u", version);
              self.header->newline1 = '\n';
              for (size_t i = 0; i < sizeof(self.header->meta); ++i)
                self.header->meta[i] = ' ';
              self.header->newline2 = '\n';
              for (size_t i = 0; i < sizeof(self.header->flags); ++i)
                self.header->flags[i] = ' ';
              self.header->newline3 = '\n';
              self.header->null = '\0';
              self.header->endianess_mark = LUMIERA_FILEHEADER_ENDIANMAGIC;

              lumiera_fileheader_flags_set (&self, flags);
            }
        }
      else
        LUMIERA_ERROR_SET_CRITICAL(fileheader, FILEHEADER_NOWRITE, file->name);
    }

  return self;
}


lumiera_fileheader
lumiera_fileheader_open (LumieraFile file, char* fourcc, size_t size, const char* flags_expected, const char* flags_remove)
{
  lumiera_fileheader self = {NULL, NULL};

  REQUIRE (fourcc && strlen (fourcc) == 4, "fourcc must be of size 4 and nothing else: %s", fourcc);

  if (file)
    {
      if ((self.map = lumiera_mmap_new_exact (file, 0, size)))
        {
          self.header = lumiera_mmap_address (self.map, 0);

          if (memcmp (self.header->fourcc, fourcc, 4))
            {
              LUMIERA_ERROR_SET_WARNING (fileheader, FILEHEADER_HEADER, file->name);
              goto err;
            }

          if (!lumiera_fileheader_flags_validate (&self, flags_expected, NULL))
            {
              LUMIERA_ERROR_SET_WARNING (fileheader, FILEHEADER_FLAGS, self.header->flags);
              goto err;
            }

          ////////////////////TODO only clear flags when file is writable!
          lumiera_fileheader_flags_clear (&self, flags_remove);

          static uint64_t endianess_mark = LUMIERA_FILEHEADER_ENDIANMAGIC;

          if (lumiera_fileheader_flags_validate (&self, LUMIERA_FILEHEADER_FLAG_ENDIANESS, NULL) &&
              memcmp (&self.header->endianess_mark, (char*)&endianess_mark, sizeof (endianess_mark)))
            {
              LUMIERA_ERROR_SET_CRITICAL (fileheader, FILEHEADER_ENDIANESS, NULL);
              goto err;
            }
        }
    }

  return self;

 err:
  lumiera_mmap_delete (self.map);
  self.map = NULL;
  self.header = NULL;
  return self;
}


void
lumiera_fileheader_close (LumieraFileheader self, const char* flags_add)
{
  if (self && self->header)
    {
      lumiera_fileheader_flags_set (self, flags_add);
      self->header = NULL;
      lumiera_mmap_delete (self->map);
    }
}


int
lumiera_fileheader_version (LumieraFileheader self)
{
  int ret = -1;
  if (self && self->header)
    {
      errno = 0;
      ret = (int) strtol(self->header->version, NULL, 10);
      if (errno)
        {
          ret = -1;
          LUMIERA_ERROR_SET_WARNING (fileheader, FILEHEADER_HEADER, lumiera_tmpbuf_snprintf (32, "%s", self->header->fourcc));
        }
    }
  return ret;
}




/**
 * check if all flags given from some sets are either set or not.
 */
int
lumiera_fileheader_flags_validate (LumieraFileheader self, const char* expected, const char* unexpected)
{
  for (; expected && *expected; ++expected)
    if (!strchr (self->header->flags, *expected))
      return 0;

  for (; unexpected && *unexpected; ++unexpected)
    if (strchr (self->header->flags, *unexpected))
      return 0;

  return 1;
}


/**
 * Sets flags if not already set
 */
LumieraFileheader
lumiera_fileheader_flags_set (LumieraFileheader self, const char* flags)
{
  for (; flags && *flags; ++flags)
    if (!strchr(self->header->flags, *flags))
      {
        char* pos = strchr (self->header->flags, ' ');
        if (!pos)
          {
            LUMIERA_ERROR_SET_CRITICAL (fileheader, FILEHEADER_FLAGSPACE, flags);
            return NULL;
          }
        *pos = *flags;
      }

  return self;
}


/**
 * Clear flags if present
 */
LumieraFileheader
lumiera_fileheader_flags_clear (LumieraFileheader self, const char* flags)
{
  char* pos;

  for (; flags && *flags; ++flags)
    if ((pos = strchr (self->header->flags, *flags)))
      *pos = ' ';

  return self;
}



/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
