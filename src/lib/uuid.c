/*
  uuid  -  Universal unique identifiers

  Copyright (C)         CinelerraCV
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

#include "lib/uuid.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void
lumiera_uuid_set_ptr (lumiera_uuid* uuid, void* ptr)
{
  memset (uuid, 0, 16);
  *(void**)uuid = ptr;
}


void*
lumiera_uuid_ptr_get (lumiera_uuid* uuid)
{
  return *(void**)uuid;
}


void
lumiera_uuid_gen (lumiera_uuid* uuid)
{
  static int fd = -2;
  if (!uuid)
    return;

  if (fd == -2)
    {
      fd = open ("/dev/urandom", O_RDONLY);
      if (fd == -1)
        fd = open ("/dev/random", O_RDONLY);
      if (fd >= 0)
        fcntl (fd, F_SETFD, FD_CLOEXEC);
      else
        srand (getpid () + time (NULL));
    }
  if (fd < 0)
    {
      for (int i = 0; i < 16; ++i)
        ((unsigned char*)uuid)[i] = (unsigned char)(rand()>>7);
    }
  else
    {
      if (read (fd, uuid, 16) < 0)
        abort ();
    }
}


void
lumiera_uuid_copy (lumiera_uuid* dest, lumiera_uuid* src)
{
  memcpy (dest, src, 16);
}


int
lumiera_uuid_eq (lumiera_uuid* uuida, lumiera_uuid* uuidb)
{
  return !memcmp (uuida, uuidb, 16);
}

size_t
lumiera_uuid_hash (lumiera_uuid* uuid)
{
  return *(size_t*)uuid;
}


