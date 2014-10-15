/*
  Config-interface  -  Lumiera configuration interface implementation

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

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


#include "common/config-interface.h"
#include "common/config.h"



LUMIERA_EXPORT(
               LUMIERA_INTERFACE_DEFINE (lumieraorg_configuration, 0,
                                         lumieraorg_configuration,
                                         NULL,
                                         NULL,
                                         NULL,
                                         LUMIERA_INTERFACE_MAP (load,
                                                                lumiera_config_load),
                                         LUMIERA_INTERFACE_MAP (save,
                                                                lumiera_config_save),
                                         LUMIERA_INTERFACE_MAP (purge,
                                                                lumiera_config_purge),
                                         LUMIERA_INTERFACE_MAP (dump,
                                                                lumiera_config_dump),
                                         LUMIERA_INTERFACE_INLINE (setdefault,
                                                                   int, (const char* line),
                                                                   {return !!lumiera_config_setdefault (line);}
                                                                   ),
                                         LUMIERA_INTERFACE_MAP (reset,
                                                                lumiera_config_reset),
                                         LUMIERA_INTERFACE_MAP (info,
                                                                lumiera_config_info),

                                         LUMIERA_INTERFACE_MAP (wordlist_get_nth,
                                                                lumiera_config_wordlist_get_nth),
                                         LUMIERA_INTERFACE_MAP (wordlist_find,
                                                                lumiera_config_wordlist_find),
                                         LUMIERA_INTERFACE_INLINE (wordlist_replace,
                                                                   int, (const char* key,
                                                                         const char* value,
                                                                         const char* subst1,
                                                                         const char* subst2,
                                                                         const char* delims),
                                                                   {return !!lumiera_config_wordlist_replace (key, value, subst1, subst2, delims);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (wordlist_add,
                                                                   int, (const char* key, const char* value, const char* delims),
                                                                   {return !!lumiera_config_wordlist_add (key, value, delims);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (link_get,
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_link_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (link_set,
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_link_set (key, value);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (number_get,
                                                                   int, (const char* key, long long* value),
                                                                   {return !!lumiera_config_number_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (number_set,
                                                                   int, (const char* key, long long* value),
                                                                   {return !!lumiera_config_number_set (key, value);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (real_get,
                                                                   int, (const char* key, long double* value),
                                                                   {return !!lumiera_config_real_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (real_set,
                                                                   int, (const char* key, long double* value),
                                                                   {return !!lumiera_config_real_set (key, value);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (string_get,
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_string_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (string_set,
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_string_set (key, value);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (wordlist_get,
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_wordlist_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (wordlist_set,
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_wordlist_set (key, value);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (word_get,
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_word_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (word_set,
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_word_set (key, value);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (bool_get,
                                                                   int, (const char* key, int* value),
                                                                   {return !!lumiera_config_bool_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (bool_set,
                                                                   int, (const char* key, int* value),
                                                                   {return !!lumiera_config_bool_set (key, value);}
                                                                   ),
                                         )
                           );


#ifndef LUMIERA_PLUGIN

#include "common/interfaceregistry.h"
void
lumiera_config_interface_init (void)
{
  LUMIERA_INTERFACE_REGISTEREXPORTED;
}

void
lumiera_config_interface_destroy (void)
{
  LUMIERA_INTERFACE_UNREGISTEREXPORTED;
}

#endif

