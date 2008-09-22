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


TEST ("configitem_simple")
{
  REQUIRE (argv[2]);
  lumiera_config_init ("./");

  LumieraConfigitem item;

  item = lumiera_configitem_new (argv[2]);
  ENSURE (item);

  printf ("line = '%s'\n", item->line);
  if (item->key)
    printf ("key = '%.*s'\n", (int)item->key_size, item->key);
  if (item->delim)
    {
      printf ("delim = '%c'\n", *item->delim);
      printf ("value = '%s'\n", item->delim+1);
    }

  lumiera_configitem_delete (item, NULL);

  lumiera_config_destroy ();
}


TEST ("lookup")
{
  lumiera_config_init ("./");

  lumiera_config_lookup lookup;
  lumiera_config_lookup_init (&lookup);

  LumieraConfigitem item = lumiera_configitem_new ("foo.bar = test");
  lumiera_config_lookup_insert (&lookup, item);

  LumieraConfigitem found = lumiera_config_lookup_item_find (&lookup, "foo.bar");
  ENSURE (found == item);

  lumiera_config_lookup_remove (&lookup, found);
  ENSURE (!lumiera_config_lookup_item_find (&lookup, "foo.bar"));

  lumiera_config_lookup_destroy (&lookup);
  lumiera_config_destroy ();
}


TEST ("basic_set_get")
{
  REQUIRE (argv[2]);
  REQUIRE (argv[3]);

  lumiera_config_init ("./");

  if (!lumiera_config_set (argv[2], argv[3]))
    printf ("failure setting first time '%s%s': %s\n", argv[2], argv[3], lumiera_error ());

  if (!lumiera_config_set (argv[2], argv[3]))
    printf ("failure setting second time '%s%s': %s\n", argv[2], argv[3], lumiera_error ());

  const char* value;
  if (lumiera_config_get (argv[2], &value))
    printf ("%s\n", value);
  else
    printf ("failure retrieving '%s': %s\n", argv[2], lumiera_error ());

  lumiera_config_destroy ();
}




TEST ("number_get")
{
  REQUIRE (argv[2]);
  REQUIRE (argv[3]);

  lumiera_config_init ("./");

  long long number = 0;

  lumiera_config_setdefault (lumiera_tmpbuf_snprintf (SIZE_MAX, "%s = %s", argv[2], argv[3]));

  if (lumiera_config_number_get (argv[2], &number))
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

  if (lumiera_config_number_get (argv[2], &number))
    printf ("%lld\n", number);
  else
    printf ("%s\n", lumiera_error ());

  lumiera_config_destroy ();
}


TEST ("number_set")
{
  REQUIRE (argv[2]);
  REQUIRE (argv[3]);

  lumiera_config_init ("./");

  signed long long number = atoll (argv[3]);

  if (!lumiera_config_number_set (argv[2], &number))
    printf ("failed setting number '%s=%lld': %s\n", argv[2], number, lumiera_error ());

  if (lumiera_config_number_get (argv[2], &number))
    printf ("'%lld'\n", number);
  else
    printf ("%s\n", lumiera_error ());

  lumiera_config_destroy ();
}


TEST ("string_get")
{
  REQUIRE (argv[2]);
  REQUIRE (argv[3]);

  lumiera_config_init ("./");

  const char* string;

  lumiera_config_setdefault (lumiera_tmpbuf_snprintf (SIZE_MAX, "%s = %s", argv[2], argv[3]));

  if (lumiera_config_string_get (argv[2], &string))
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

  if (!lumiera_config_string_set (argv[2], &argv[3]))
    printf ("failed setting string '%s=%s': %s\n", argv[2], argv[3], lumiera_error ());

  const char* string;
  if (lumiera_config_string_get (argv[2], &string))
    printf ("'%s'\n", string);
  else
    printf ("%s\n", lumiera_error ());

  lumiera_config_destroy ();
}


TEST ("word_get")
{
  REQUIRE (argv[2]);
  REQUIRE (argv[3]);

  lumiera_config_init ("./");

  const char* word;

  lumiera_config_setdefault (lumiera_tmpbuf_snprintf (SIZE_MAX, "%s = %s", argv[2], argv[3]));

  if (lumiera_config_word_get (argv[2], &word))
    printf ("'%s'\n", word);
  else
    printf ("%s, '%s'\n", lumiera_error (), word);

  lumiera_config_destroy ();
}


TEST ("word_set")
{
  REQUIRE (argv[2]);
  REQUIRE (argv[3]);

  lumiera_config_init ("./");

  if (!lumiera_config_word_set (argv[2], &argv[3]))
    printf ("failed setting word '%s=%s': %s\n", argv[2], argv[3], lumiera_error ());

  const char* word;
  if (lumiera_config_word_get (argv[2], &word))
    printf ("'%s'\n", word);
  else
    printf ("%s\n", lumiera_error ());

  lumiera_config_destroy ();
}


TEST ("configitem_simple_ctor_dtor")
{
  REQUIRE (argv[2]);
  lumiera_config_init ("./");

  LumieraConfigitem item;

  item = lumiera_configitem_new (argv[2]);

  lumiera_config_destroy ();
}

TEST ("configitem_simple_content_check")
{
  REQUIRE (argv[2]);
  lumiera_config_init ("./");

  LumieraConfigitem item;

  item = lumiera_configitem_new (argv[2]);

  if ( item->line )
    {
      printf("item->line = '%s'\n", item->line);
    }
  if ( item->key_size )
    {
      printf("item->key_size = '%zi'\n", item->key_size);
    }
  if ( item->key )
    {
      printf("item->key = '%s'\n", item->key);
    }
  if ( item->delim )
    {
      printf("item->delim = '%s'\n", item->delim);
    }

  lumiera_config_destroy ();
}


TEST ("wordlist_get_nth")
{
  REQUIRE (argv[2]);
  REQUIRE (argv[3]);
  REQUIRE (argv[4]);

  lumiera_config_init ("./");

  if (!lumiera_config_wordlist_set (argv[2], &argv[3]))
    printf ("failed setting word '%s=%s': %s\n", argv[2], argv[3], lumiera_error ());

  const char* word = lumiera_config_wordlist_get_nth (argv[2], atoi (argv[4]));

  printf ("'%s'\n", word?word:"NULL");

  lumiera_config_destroy ();
}



TESTS_END
