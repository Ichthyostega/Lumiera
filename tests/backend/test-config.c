/*
  test-config.c  -  test the config system

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Simeon Voelkel <simeon_voelkel@arcor.de>

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
#include "lib/safeclib.h"

#include "backend/config.h"
#include "backend/configitem.h"

#include "tests/test.h"

TESTS_BEGIN

TEST ("init")
{
  lumiera_config_init ("./");
  printf ("initialized\n");
  lumiera_config_destroy ();
  printf ("destroyed\n");
}


TEST ("number_get")
{
  REQUIRE (argv[2]);
  REQUIRE (argv[3]);

  lumiera_config_init ("./");

  long long number = 0;

  if (!lumiera_config_number_get (argv[2], &number, argv[3]))
    printf ("%lld\n", number);
  else
    printf ("%s, %lld\n", lumiera_error (), number);

  lumiera_config_destroy ();
}


TEST ("number_get_nodefault")
{
  REQUIRE (argv[2]);

  lumiera_config_init ("./");

  long long number = 0;

  if (!lumiera_config_number_get (argv[2], &number, NULL))
    printf ("%lld\n", number);
  else
    printf ("%s\n", lumiera_error ());

  lumiera_config_destroy ();
}


TEST ("string_get")
{
  REQUIRE (argv[2]);
  REQUIRE (argv[3]);

  lumiera_config_init ("./");

  char* string;

  if (!lumiera_config_string_get (argv[2], &string, argv[3]))
    printf ("'%s'\n", string);
  else
    printf ("%s, '%s'\n", lumiera_error (), string);

  lumiera_config_destroy ();
}


TEST ("string_set")
{
  REQUIRE (argv[2]);
  REQUIRE (argv[3]);

  lumiera_config_init ("./");

  lumiera_config_string_set (argv[2], &argv[3], NULL);
  FIXME ("handle error");

  const char* string;
  if (!lumiera_config_get (argv[2], &string))
    printf ("'%s'\n", string);
  else
    printf ("%s, '%s'\n", lumiera_error (), string);

  lumiera_config_destroy ();
}


TEST ("word_get")
{
  REQUIRE (argv[2]);
  REQUIRE (argv[3]);

  lumiera_config_init ("./");

  char* word;

  if (!lumiera_config_word_get (argv[2], &word, argv[3]))
    printf ("'%s'\n", word);
  else
    printf ("%s, '%s'\n", lumiera_error (), word);

  lumiera_config_destroy ();
}

TEST ("empty_line_configitem")
{
  LumieraConfigitem item;

  item = lumiera_configitem_new ( "" );

  lumiera_configitem_delete(item);
  item = NULL;
}

TEST ("blank_line_configitem")
{
  LumieraConfigitem item;

  item = lumiera_configitem_new ( "	 	" );

  lumiera_configitem_delete(item);
  item = NULL;
}


TESTS_END
