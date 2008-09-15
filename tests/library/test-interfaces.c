/*
  test-interfaces.c  -  test interfaces declaration and implementation

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

#include "lib/interface.h"
#include "tests/test.h"

/*
  define 2 example interfaces
 */

LUMIERA_INTERFACE_DECLARE (example1, 0,
                           LUMIERA_INTERFACE_SLOT (void, foo1, (const char*)),
                           LUMIERA_INTERFACE_SLOT (void, bar1, (const char*)),
);

LUMIERA_INTERFACE_DECLARE (example2, 0,
                           LUMIERA_INTERFACE_SLOT (void, foo2, (const char*)),
                           LUMIERA_INTERFACE_SLOT (void, bar2, (const char*)),
);


/*
 now the functions we want to bind to them
 */

void
testfunc (const char* message)
{
  printf ("Called as '%s'\n", message);
}



/*
  implementation of some example interfaces
 */

LUMIERA_INTERFACE_INSTANCE (example1, 0,
                            example1_standalone_implementation,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            LUMIERA_INTERFACE_MAP (foo1, testfunc,
                                                   "\066\177\042\305\165\243\236\352\164\250\357\307\211\374\123\066"),
                            LUMIERA_INTERFACE_MAP (bar1, testfunc,
                                                   "\162\335\262\306\101\113\106\055\342\205\300\151\262\073\257\343")
                            );


LUMIERA_EXPORT (interfaces_defined_here,
                LUMIERA_INTERFACE_DEFINE (example1, 0,
                                          example1_implementation,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          LUMIERA_INTERFACE_MAP (foo1, testfunc,
                                                                 "\214\310\136\372\003\344\163\377\075\100\070\200\375\221\227\324"),
                                          LUMIERA_INTERFACE_MAP (bar1, testfunc,
                                                                 "\262\253\067\211\157\052\212\140\114\334\231\250\340\075\214\030")
                                          ),
                LUMIERA_INTERFACE_DEFINE (example2, 0,
                                          example2_implementation,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          LUMIERA_INTERFACE_MAP (foo2, testfunc,
                                                                 "\110\152\002\271\363\052\324\272\373\045\132\270\277\000\271\217"),
                                          LUMIERA_INTERFACE_MAP (bar2, testfunc,
                                                                 "\376\042\027\336\355\113\132\233\350\312\170\077\377\370\356\167")
                                          )
                );


TESTS_BEGIN

TEST ("basic")
{
  interfaces_defined_here();
}

TEST ("basic")
{
}

TEST ("basic")
{
}

TEST ("basic")
{
}

TESTS_END





/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
