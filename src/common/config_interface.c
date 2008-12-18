/*
  config_interface.c  -  Lumiera configuration interface implementation

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
#include "common/config_interface.h"
#include "common/config.h"

//TODO: internal/static forward declarations//


//TODO: System includes//


LUMIERA_EXPORT(
               LUMIERA_INTERFACE_DEFINE (lumieraorg_configuration, 0,
                                         lumieraorg_configuration,
                                         NULL,
                                         NULL,
                                         NULL,
                                         LUMIERA_INTERFACE_MAP (load, "\177\002\314\014\245\001\056\246\317\203\322\346\114\061\262\366",
                                                                lumiera_config_load),
                                         LUMIERA_INTERFACE_MAP (save, "\271\141\152\017\060\312\103\343\342\063\007\142\374\201\163\334",
                                                                lumiera_config_save),
                                         LUMIERA_INTERFACE_MAP (purge, "\321\275\170\140\010\075\323\340\371\050\345\131\267\070\076\130",
                                                                lumiera_config_purge),
                                         LUMIERA_INTERFACE_MAP (dump, "\317\357\304\076\137\035\100\023\130\021\327\253\314\237\311\045",
                                                                lumiera_config_dump),
                                         LUMIERA_INTERFACE_INLINE (setdefault, "\344\337\116\131\101\037\175\227\310\151\216\240\125\316\241\115",
                                                                   int, (const char* line),
                                                                   {return !!lumiera_config_setdefault (line);}
                                                                   ),
                                         LUMIERA_INTERFACE_MAP (reset, "\337\143\274\052\364\362\256\353\165\161\352\267\026\204\154\231",
                                                                lumiera_config_reset),
                                         LUMIERA_INTERFACE_MAP (info, "\355\305\326\372\236\302\122\253\114\024\376\010\326\055\132\130",
                                                                lumiera_config_info),

                                         LUMIERA_INTERFACE_MAP (wordlist_get_nth, "\151\224\043\353\244\134\360\125\361\352\236\267\225\371\020\173",
                                                                lumiera_config_wordlist_get_nth),
                                         LUMIERA_INTERFACE_MAP (wordlist_find, "\276\103\153\017\257\171\300\277\311\352\027\054\174\017\321\213",
                                                                lumiera_config_wordlist_find),
                                         LUMIERA_INTERFACE_INLINE (wordlist_replace, "\365\115\171\203\374\371\236\246\212\021\121\003\161\364\060\011",
                                                                   int, (const char* key,
                                                                         const char* value,
                                                                         const char* subst1,
                                                                         const char* subst2,
                                                                         const char* delims),
                                                                   {return !!lumiera_config_wordlist_replace (key, value, subst1, subst2, delims);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (wordlist_add, "\133\255\034\074\300\073\374\067\241\145\302\067\256\301\206\034",
                                                                   int, (const char* key, const char* value, const char* delims),
                                                                   {return !!lumiera_config_wordlist_add (key, value, delims);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (link_get, "\062\177\332\266\004\256\323\102\252\064\153\054\313\151\124\361",
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_link_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (link_set, "\303\273\325\021\071\344\270\041\342\341\233\072\262\216\333\100",
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_link_set (key, value);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (number_get, "\300\070\066\026\154\053\052\176\212\021\376\240\314\224\235\300",
                                                                   int, (const char* key, long long* value),
                                                                   {return !!lumiera_config_number_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (number_set, "\231\142\147\006\243\203\002\354\152\237\147\074\115\363\345\127",
                                                                   int, (const char* key, long long* value),
                                                                   {return !!lumiera_config_number_set (key, value);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (real_get, "\356\335\127\112\365\220\056\000\051\077\247\013\046\210\164\120",
                                                                   int, (const char* key, long double* value),
                                                                   {return !!lumiera_config_real_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (real_set, "\205\011\020\140\014\112\134\076\232\352\246\025\274\227\347\047",
                                                                   int, (const char* key, long double* value),
                                                                   {return !!lumiera_config_real_set (key, value);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (string_get, "\215\104\350\103\163\245\065\235\373\237\364\155\312\225\027\304",
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_string_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (string_set, "\014\003\357\020\117\035\073\353\302\026\231\351\037\051\163\346",
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_string_set (key, value);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (wordlist_get, "\000\276\355\321\257\366\166\247\246\141\224\304\142\306\047\131",
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_wordlist_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (wordlist_set, "\340\352\216\303\161\056\354\264\121\162\003\061\345\001\306\265",
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_wordlist_set (key, value);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (word_get, "\057\337\302\110\276\332\307\172\035\012\246\315\370\203\226\135",
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_word_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (word_set, "\151\276\334\165\173\165\333\224\076\264\352\371\205\174\105\013",
                                                                   int, (const char* key, const char** value),
                                                                   {return !!lumiera_config_word_set (key, value);}
                                                                   ),

                                         LUMIERA_INTERFACE_INLINE (bool_get, "\204\254\234\117\147\323\210\236\102\234\123\323\124\221\305\234",
                                                                   int, (const char* key, int* value),
                                                                   {return !!lumiera_config_bool_get (key, value);}
                                                                   ),
                                         LUMIERA_INTERFACE_INLINE (bool_set, "\215\151\207\006\032\117\354\053\311\035\261\141\120\117\310\007",
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

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
