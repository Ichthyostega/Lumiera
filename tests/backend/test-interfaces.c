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
#include "backend/interfaceregistry.h"
//#include "lib/interfacedescriptor.h"
#include "tests/test.h"

/*
  define 2 example interfaces
 */

LUMIERA_INTERFACE_DECLARE (lumieraorg_testexample_one, 0,
                           LUMIERA_INTERFACE_SLOT (void, foo1, (const char*)),
                           LUMIERA_INTERFACE_SLOT (void, bar1, (const char*)),
);

LUMIERA_INTERFACE_DECLARE (lumieraorg_testexample_two, 0,
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

LUMIERA_INTERFACE_INSTANCE (lumieraorg_testexample_one, 0,
                            lumieraorg_standalone_test,
                            NULL,
                            NULL,
                            NULL,
                            LUMIERA_INTERFACE_INLINE (foo1, "\066\177\042\305\165\243\236\352\164\250\357\307\211\374\123\066",
                                                      void, (const char* msg),
                                                      {
                                                        printf ("inline\n");
                                                      }
                                                      ),
                            LUMIERA_INTERFACE_MAP (bar1, "\162\335\262\306\101\113\106\055\342\205\300\151\262\073\257\343",
                                                   testfunc)
                            );


LUMIERA_EXPORT (interfaces_defined_here,
                LUMIERA_INTERFACE_DEFINE (lumieraorg_testexample_one, 0,
                                          lumieraorg_first_test,
                                          NULL,
                                          NULL,
                                          NULL,
                                          LUMIERA_INTERFACE_MAP (foo1, "\214\310\136\372\003\344\163\377\075\100\070\200\375\221\227\324",
                                                                 testfunc),
                                          LUMIERA_INTERFACE_MAP (bar1, "\262\253\067\211\157\052\212\140\114\334\231\250\340\075\214\030",
                                                                 testfunc)
                                          ),
                LUMIERA_INTERFACE_DEFINE (lumieraorg_testexample_two, 0,
                                          lumieraorg_second_test,
                                          NULL,
                                          NULL,
                                          NULL,
                                          LUMIERA_INTERFACE_MAP (foo2, "\110\152\002\271\363\052\324\272\373\045\132\270\277\000\271\217",
                                                                 testfunc),
                                          LUMIERA_INTERFACE_MAP (bar2, "\376\042\027\336\355\113\132\233\350\312\170\077\377\370\356\167",
                                                                 testfunc)
                                          )
                );


TESTS_BEGIN

TEST ("basic")
{
  lumiera_interfaceregistry_init ();

  lumiera_interfaceregistry_bulkregister_interfaces (interfaces_defined_here());


  /* some ugly lowlevel handling tests */

  LumieraInterface handle1 =
    lumiera_interfaceregistry_interface_find ("lumieraorg_testexample_one", 0, "lumieraorg_first_test");

  (LUMIERA_INTERFACE_CAST(lumieraorg_testexample_one, 0)handle1)->bar1 ("this is bar1");


  LUMIERA_INTERFACE_TYPE(lumieraorg_testexample_two, 0)* handle2 = LUMIERA_INTERFACE_CAST(lumieraorg_testexample_two, 0)
    lumiera_interfaceregistry_interface_find ("lumieraorg_testexample_two", 0, "lumieraorg_second_test");

  handle2->foo2 ("this is foo2");

  /* higher level, WIP */

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_two, 0, 0, lumieraorg_second_test, handle3);
  handle3->bar2 ("this is bar2");

  lumiera_interfaceregistry_bulkremove_interfaces (interfaces_defined_here());
  lumiera_interfaceregistry_destroy ();
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
