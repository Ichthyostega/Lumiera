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


//TODO: Lumiera header includes//
#include "backend/config.h"

//TODO: internal/static forward declarations//


//TODO: System includes//


/**
 * @file
 *
 */

NOBUG_DEFINE_FLAG_PARENT (config_all, backend);
NOBUG_DEFINE_FLAG_PARENT (config, config_all);

LUMIERA_ERROR_DEFINE (CONFIG_SYNTAX, "Syntax error in configfile");



lumiera_err
lumiera_config_init (const char* path)
{
  UNIMPLEMENTED();
  return NULL;
}


lumiera_err
lumiera_config_destroy()
{
  UNIMPLEMENTED();
  return NULL;
}


lumiera_err
lumiera_config_load (const char* file)
{
  UNIMPLEMENTED();
  return NULL;
}


lumiera_err
lumiera_config_save ()
{
  UNIMPLEMENTED();
  return NULL;
}


lumiera_err
lumiera_config_purge (const char* filename)
{
  UNIMPLEMENTED();
  return NULL;
}


lumiera_err
lumiera_config_get (const char* key, const char** value)
{
  UNIMPLEMENTED();
  return NULL;
}


lumiera_err
lumiera_config_set (const char* key, const char* value)
{
  UNIMPLEMENTED();
  return NULL;
}


lumiera_err
lumiera_config_reset(const char* key)
{
  UNIMPLEMENTED();
  return NULL;
}


lumiera_err
lumiera_config_info (const char* key, const char** filename, unsigned* line)
{
  UNIMPLEMENTED();
  return NULL;
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
