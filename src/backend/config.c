/*
  config.c  -  Lumiera configuration system

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

//TODO: Support library includes//
#include "lib/safeclib.h"


//TODO: Lumiera header includes//
#include "backend/config.h"

//TODO: internal/static forward declarations//


//TODO: System includes//
#include <stdint.h>

/**
 * @file
 *
 */

NOBUG_DEFINE_FLAG_PARENT (config_all, backend);
NOBUG_DEFINE_FLAG_PARENT (config, config_all);

LUMIERA_ERROR_DEFINE (CONFIG_SYNTAX, "Syntax error in configfile");
LUMIERA_ERROR_DEFINE (CONFIG_TYPE, "Config value has wrong type");


/* singleton config */
static LumieraConfig the_config = NULL;


int
lumiera_config_init (const char* path)
{
  REQUIRE (!the_config, "Configuration subsystem already initialized");

  the_config = malloc (sizeof (*the_config));
  the_config->path = lumiera_strndup (path, SIZE_MAX);
  return 0;
}


void
lumiera_config_destroy ()
{
  if (the_config)
    {
      lumiera_free (the_config->path);
      lumiera_free (the_config);
      the_config = NULL;
    }
  else
    WARN (config, "Tried to destroy non initialized config subsystem");
}


int
lumiera_config_load (const char* file)
{
  UNIMPLEMENTED();
  return -1;
}


int
lumiera_config_save ()
{
  UNIMPLEMENTED();
  return -1;
}


int
lumiera_config_purge (const char* filename)
{
  UNIMPLEMENTED();
  return -1;
}


int
lumiera_config_get (const char* key, const char** value)
{
  UNIMPLEMENTED();
  // env var override

  return -1;
}


int
lumiera_config_set (const char* key, const char* value)
{
  UNIMPLEMENTED();
  return -1;
}


int
lumiera_config_reset(const char* key)
{
  UNIMPLEMENTED();
  return -1;
}


int
lumiera_config_info (const char* key, const char** filename, unsigned* line)
{
  UNIMPLEMENTED();
  return -1;
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
