/*
  test-config.c  -  test the config system

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Simeon Voelkel <simeon_voelkel@arcor.de>

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
*/
#include "lib/tmpbuf.h"

#include "common/config.h"
#include "common/configitem.h"

#include "tests/test.h"

TESTS_BEGIN

TEST (init)
{
/* Note:  lumiera_config_init and lumiera_config_destroy are
 *        invoked automatically from ConfigFacade
 */
  printf ("initialized\n");
  lumiera_config_destroy ();
  printf ("destroyed\n");
  /* there will be a warning about destroying the already 
   * destroyed or uninitialised config system....
   */
}


TEST (configitem_simple)
{
  CHECK (argv[2]);

  LumieraConfigitem item;

  item = lumiera_configitem_new (argv[2]);
  CHECK (item);

  printf ("line = '%s'\n", item->line);
  if (item->key)
    printf ("key = '%.*s'\n", (int)item->key_size, item->key);
  if (item->delim)
    {
      printf ("delim = '%c'\n", *item->delim);
      printf ("value = '%s'\n", item->delim+1);
    }

  lumiera_configitem_delete (item, NULL);
}


TEST (lookup)
{

  lumiera_config_lookup lookup;
  lumiera_config_lookup_init (&lookup);

  LumieraConfigitem item = lumiera_configitem_new ("foo.bar = test");
  lumiera_config_lookup_insert (&lookup, item);

  LumieraConfigitem found = lumiera_config_lookup_item_find (&lookup, "foo.bar");
  CHECK (found == item);

  lumiera_config_lookup_remove (&lookup, found);
  CHECK (!lumiera_config_lookup_item_find (&lookup, "foo.bar"));

  lumiera_config_lookup_destroy (&lookup);
}


TEST (change_value)
{
  CHECK (argv[2]);
  CHECK (argv[3]);
  CHECK (argv[4]);

  const char* value;

  if (!lumiera_config_set (argv[2], argv[3]))
    printf ("failure setting first time '%s%s': %s\n", argv[2], argv[3], lumiera_error ());

  if (lumiera_config_get (argv[2], &value))
    printf ("%s\n", value);
  else
    printf ("failure retrieving '%s': %s\n", argv[2], lumiera_error ());

  if (!lumiera_config_set (argv[2], argv[4]))
    printf ("failure setting second time '%s%s': %s\n", argv[2], argv[4], lumiera_error ());

  if (lumiera_config_get (argv[2], &value))
    printf ("%s\n", value);
  else
    printf ("failure retrieving '%s': %s\n", argv[2], lumiera_error ());
}



TEST (basic_set_get)
{
  CHECK (argv[2]);
  CHECK (argv[3]);

  if (!lumiera_config_set (argv[2], argv[3]))
    printf ("failure setting first time '%s%s': %s\n", argv[2], argv[3], lumiera_error ());

  if (!lumiera_config_set (argv[2], argv[3]))
    printf ("failure setting second time '%s%s': %s\n", argv[2], argv[3], lumiera_error ());

  const char* value;
  if (lumiera_config_get (argv[2], &value))
    printf ("%s\n", value);
  else
    printf ("failure retrieving '%s': %s\n", argv[2], lumiera_error ());
}




TEST (number_get)
{
  CHECK (argv[2]);
  CHECK (argv[3]);

  long long number = 0;

  lumiera_config_setdefault (lumiera_tmpbuf_snprintf (SIZE_MAX, "%s = %s", argv[2], argv[3]));

  if (lumiera_config_number_get (argv[2], &number))
    printf ("%lld\n", number);
  else
    printf ("%s, %lld\n", lumiera_error (), number);
}


TEST (number_get_nodefault)
{
  CHECK (argv[2]);

  long long number = 0;

  if (lumiera_config_number_get (argv[2], &number))
    printf ("%lld\n", number);
  else
    printf ("%s\n", lumiera_error ());
}


TEST (number_set)
{
  CHECK (argv[2]);
  CHECK (argv[3]);

  signed long long number = atoll (argv[3]);

  if (!lumiera_config_number_set (argv[2], &number))
    printf ("failed setting number '%s=%lld': %s\n", argv[2], number, lumiera_error ());

  if (lumiera_config_number_get (argv[2], &number))
    printf ("'%lld'\n", number);
  else
    printf ("%s\n", lumiera_error ());
}


TEST (string_get)
{
  CHECK (argv[2]);
  CHECK (argv[3]);

  const char* string;

  lumiera_config_setdefault (lumiera_tmpbuf_snprintf (SIZE_MAX, "%s = %s", argv[2], argv[3]));

  if (lumiera_config_string_get (argv[2], &string))
    printf ("'%s'\n", string);
  else
    printf ("%s, '%s'\n", lumiera_error (), string);
}


TEST (string_set)
{
  CHECK (argv[2]);
  CHECK (argv[3]);

  if (!lumiera_config_string_set (argv[2], &argv[3]))
    printf ("failed setting string '%s=%s': %s\n", argv[2], argv[3], lumiera_error ());

  const char* string;
  if (lumiera_config_string_get (argv[2], &string))
    printf ("'%s'\n", string);
  else
    printf ("%s\n", lumiera_error ());
}


TEST (word_get)
{
  CHECK (argv[2]);
  CHECK (argv[3]);

  const char* word;

  lumiera_config_setdefault (lumiera_tmpbuf_snprintf (SIZE_MAX, "%s = %s", argv[2], argv[3]));

  if (lumiera_config_word_get (argv[2], &word))
    printf ("'%s'\n", word);
  else
    printf ("%s, '%s'\n", lumiera_error (), word);

  lumiera_config_destroy ();
}


TEST (word_set)
{
  CHECK (argv[2]);
  CHECK (argv[3]);

  if (!lumiera_config_word_set (argv[2], &argv[3]))
    printf ("failed setting word '%s=%s': %s\n", argv[2], argv[3], lumiera_error ());

  const char* word;
  if (lumiera_config_word_get (argv[2], &word))
    printf ("'%s'\n", word);
  else
    printf ("%s\n", lumiera_error ());
}


TEST (configitem_simple_ctor_dtor)
{
  CHECK (argv[2]);

  LumieraConfigitem item;

  item = lumiera_configitem_new (argv[2]);
}


TEST (configitem_simple_content_check)
{
  CHECK (argv[2]);

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
}


TEST (wordlist_get_nth)
{
  CHECK (argv[2]);
  CHECK (argv[3]);
  CHECK (argv[4]);

  if (!lumiera_config_wordlist_set (argv[2], &argv[3]))
    printf ("failed setting word '%s=%s': %s\n", argv[2], argv[3], lumiera_error ());

  const char* word = lumiera_config_wordlist_get_nth (argv[2], atoi (argv[4]), " \t,;");

  printf ("'%s'\n", word?word:"NULL");
}


TEST (wordlist_find)
{
  CHECK (argv[2]);
  CHECK (argv[3]);
  CHECK (argv[4]);

  if (!lumiera_config_wordlist_set (argv[2], &argv[3]))
    printf ("failed setting word '%s=%s': %s\n", argv[2], argv[3], lumiera_error ());

  int n = lumiera_config_wordlist_find (argv[2], argv[4], " \t,;");

  printf ("'%d'\n", n);
}


TEST (wordlist_replace)
{
  CHECK (argv[2]);
  CHECK (argv[3]);
  CHECK (argv[4]);
  CHECK (argv[5]);
  CHECK (argv[6]);

  if (!lumiera_config_wordlist_set (argv[2], &argv[3]))
    printf ("failed setting word '%s=%s': %s\n", argv[2], argv[3], lumiera_error ());

  const char* wordlist = lumiera_config_wordlist_replace (argv[2], argv[4], *argv[5]?argv[5]:NULL, *argv[6]?argv[6]:NULL, " \t,;");

  if (wordlist)
    printf ("'%s'\n", wordlist);
  else
    printf ("%s\n", lumiera_error ());
}


TEST (wordlist_add)
{
  CHECK (argv[2]);
  CHECK (argv[3]);
  CHECK (argv[4]);
  CHECK (argv[5]);

  if (!lumiera_config_wordlist_set (argv[2], &argv[3]))
    printf ("failed setting word '%s=%s': %s\n", argv[2], argv[3], lumiera_error ());

  const char* wordlist = lumiera_config_wordlist_add (argv[2], argv[4], " \t,;");
  if (wordlist)
    printf ("'%s'\n", wordlist);
  else
    printf ("%s\n", lumiera_error ());

  wordlist = lumiera_config_wordlist_add (argv[2], argv[5], " \t,;");
  if (wordlist)
    printf ("'%s'\n", wordlist);
  else
    printf ("%s\n", lumiera_error ());
}


TESTS_END
