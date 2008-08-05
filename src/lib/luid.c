/*
  luid  -  Lumiera unique identifiers

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

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

#include "lib/luid.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

void
lumiera_uid_set_ptr (lumiera_uid* luid, void* ptr)
{
  memset (luid, 0, 16);
  *(void**)luid = ptr;
}


void*
lumiera_uid_ptr_get (lumiera_uid* luid)
{
  return *(void**)luid;
}


void
lumiera_uid_gen (lumiera_uid* luid)
{
  static int fd = -2;
  if (!luid)
    return;

  if (fd == -2)
    {
      fd = open ("/dev/urandom", O_RDONLY);
      /* on linux /dev/random would be way to slow for our purpose, so we comment that out for now.
         other unixiods offer a /dev/random which has the same semantics as linux /dev/urandom has,
         configuration should do this right some day.
      if (fd == -1)
        fd = open ("/dev/random", O_RDONLY);
      */
      if (fd >= 0)
        fcntl (fd, F_SETFD, FD_CLOEXEC);
      else
        srand (getpid () + time (NULL));
    }

  do
    {
      if (fd < 0)
        {
          for (int i = 0; i < 16; ++i)
            ((unsigned char*)luid)[i] = (unsigned char)(rand()>>7);
        }
      else
        {
          if (read (fd, luid, 16) < 16)
            abort ();
        }
    }
  /* we identify generic pointers by having some zeros in the luid,
   * this happens very unlikely to be in a random luid, just regenerate it then */
  while (!*(((intptr_t*)luid)+1));
}


void
lumiera_uid_copy (lumiera_uid* dest, lumiera_uid* src)
{
  memcpy (dest, src, 16);
}


int
lumiera_uid_eq (lumiera_uid* luida, lumiera_uid* luidb)
{
  return !memcmp (luida, luidb, 16);
}

size_t
lumiera_uid_hash (lumiera_uid* luid)
{
  return *(size_t*)luid;
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
