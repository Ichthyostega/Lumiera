/*
  config_typed.c  -  Lumiera configuration highlevel interface

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

/**
 * @file
 * Here are the high level typed configuration interfaces defined
 */

/**
 * Number
 * signed integer numbers, in different formats (decimal, hex, oct, binary(for masks))
 */
int
lumiera_config_number_get (const char* key, long long* value, const char* def)
{
  UNIMPLEMENTED();
  return -1;
}

int
lumiera_config_number_set (const char* key, long long* value, const char* fmt)
{
  UNIMPLEMENTED();
  return -1;
}


/**
 * Real
 * floating point number in standard formats (see printf/scanf)
 */
int
lumiera_config_real_get (const char* key, long double* value, const char* def)
{
  UNIMPLEMENTED();
  return -1;
}

int
lumiera_config_real_set (const char* key, long double* value, const char* fmt)
{
  UNIMPLEMENTED();
  return -1;
}


/**
 * String
 * either a string which covers the whole line
 * or a quoted string until the ending quote
 * suggestion:
 *   "doublequotes" allow normal C backslash escapes
 *   'singlequotes' dont allow escapes except a double '' is the ' itself
 */
int
lumiera_config_string_get (const char* key, const char** value, const char* def)
{
  UNIMPLEMENTED();
  return -1;
}

int
lumiera_config_string_set (const char* key, const char** value, const char* fmt)
{
  UNIMPLEMENTED();
  return -1;
}



/**
 * Word
 * A single word, no quotes, nothing
 */
int
lumiera_config_word_get (const char* key, const char** value, const char* def)
{
  UNIMPLEMENTED();
  return -1;
}

int
lumiera_config_word_set (const char* key, const char** value, const char* fmt)
{
  UNIMPLEMENTED();
  return -1;
}


/**
 * Bool
 * Bool in various formats, (0,1(!1), yes/no, true/false, on/off, set/clear, localized)
 */
int
lumiera_config_bool_get (const char* key, int* value, const char* def)
{
  UNIMPLEMENTED();
  return -1;
}

int
lumiera_config_bool_set (const char* key, int* value, const char* fmt)
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
