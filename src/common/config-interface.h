/*
  CONFIG-INTERFACE.h  -  Lumiera configuration interface

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file config-interface.h
 ** External interface to the lumiera configuration system.
 ** This file provides an interface descriptor for external entities
 ** (e.g. plug-ins) to gain access to the configuration system (as
 ** planned in 2008).
 ** 
 ** @note this is unfinished work, development in this area stalled in 2008
 ** @warning since 2012 it is not clear if we retain this kind of configuration system.
 ** @todo as of 2016, the code is still there in same unfinished shape and basically unused
 ** 
 ** @see config.h
 */

#ifndef LUMIERA_CONFIG_INTERFACE_H
#define LUMIERA_CONFIG_INTERFACE_H


#include "common/interface.h"



void
lumiera_config_interface_init (void);

void
lumiera_config_interface_destroy (void);

/*
  Note that some interfaces return ints rather than underlying opaque pointers, this is then the truth value of the pointer
 */

LUMIERA_INTERFACE_DECLARE (lumieraorg_configuration, 0,
                           LUMIERA_INTERFACE_SLOT (int, load, (const char* file)),
                           LUMIERA_INTERFACE_SLOT (int, save, (void)),
                           LUMIERA_INTERFACE_SLOT (int, purge, (const char* filename)),
                           LUMIERA_INTERFACE_SLOT (void, dump, (FILE* out)),
                           LUMIERA_INTERFACE_SLOT (int, setdefault, (const char* line)),
                           LUMIERA_INTERFACE_SLOT (int, reset, (const char* key)),
                           LUMIERA_INTERFACE_SLOT (int, info, (const char* key, const char** filename, unsigned* line)),

                           LUMIERA_INTERFACE_SLOT (const char*, wordlist_get_nth, (const char* key, unsigned nth, const char* delims)),
                           LUMIERA_INTERFACE_SLOT (int, wordlist_find, (const char* key, const char* value, const char* delims)),
                           LUMIERA_INTERFACE_SLOT (int, wordlist_replace,
                                                   (const char* key, const char* value, const char* subst1, const char* subst2, const char* delims)),
                           LUMIERA_INTERFACE_SLOT (int, wordlist_add, (const char* key, const char* value, const char* delims)),

                           LUMIERA_INTERFACE_SLOT (int, link_get, (const char* key, const char** value)),
                           LUMIERA_INTERFACE_SLOT (int, link_set, (const char* key, const char** value)),
                           LUMIERA_INTERFACE_SLOT (int, number_get, (const char* key, long long* value)),
                           LUMIERA_INTERFACE_SLOT (int, number_set, (const char* key, long long* value)),
                           LUMIERA_INTERFACE_SLOT (int, real_get, (const char* key, long double* value)),
                           LUMIERA_INTERFACE_SLOT (int, real_set, (const char* key, long double* value)),
                           LUMIERA_INTERFACE_SLOT (int, string_get, (const char* key, const char** value)),
                           LUMIERA_INTERFACE_SLOT (int, string_set, (const char* key, const char** value)),
                           LUMIERA_INTERFACE_SLOT (int, wordlist_get, (const char* key, const char** value)),
                           LUMIERA_INTERFACE_SLOT (int, wordlist_set, (const char* key, const char** value)),
                           LUMIERA_INTERFACE_SLOT (int, word_get, (const char* key, const char** value)),
                           LUMIERA_INTERFACE_SLOT (int, word_set, (const char* key, const char** value)),
                           LUMIERA_INTERFACE_SLOT (int, bool_get, (const char* key, int* value)),
                           LUMIERA_INTERFACE_SLOT (int, bool_set, (const char* key, int* value)),
                           );


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
