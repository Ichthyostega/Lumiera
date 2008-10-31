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

#include "backend/interface.h"
#include "backend/interfaceregistry.h"
#include "backend/interfacedescriptor.h"
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

LUMIERA_INTERFACE_DECLARE (lumieraorg_testexample_void, 0
);


/*
 now the functions we want to bind to them
 */

void
testfunc (const char* message)
{
  printf ("Called as '%s'\n", message);
}



LumieraInterface
testacquire (LumieraInterface self)
{
  printf ("Acquire %s_%d_%s\n", self->interface, self->version, self->name);
  return self;
}


void
testrelease (LumieraInterface self)
{
  printf ("Release %s_%d_%s\n", self->interface, self->version, self->name);
}

/*
  implementation of some example interfaces
 */

LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0,
                            lumieraorg_tests_descriptor,
                            /*self reference, yay*/
                            LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_tests_descriptor),
                            testacquire,
                            testrelease,
                            LUMIERA_INTERFACE_INLINE (name, "\073\003\054\127\344\046\324\321\221\262\232\026\376\123\125\243",
                                                      const char*, (LumieraInterface iface),
                                                      {return "LumieraTest";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (brief, "\241\337\035\172\323\377\355\036\171\326\323\163\177\242\364\172",
                                                      const char*, (LumieraInterface iface),
                                                      {return "Lumiera Test suite examples";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (homepage, "\030\374\031\152\024\167\154\346\303\372\177\353\304\306\275\247",
                                                      const char*, (LumieraInterface iface),
                                                      {return "http://www.lumiera.org/develompent.html";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (version, "\271\330\345\066\304\217\211\065\157\120\031\365\304\363\364\074",
                                                      const char*, (LumieraInterface iface),
                                                      {return "No Version";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (author, "\367\160\342\065\147\007\237\371\141\335\371\131\025\030\257\232",
                                                      const char*, (LumieraInterface iface),
                                                      {return "Christian Thaeter";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (email, "\334\272\125\312\140\347\121\020\155\047\226\352\244\141\107\363",
                                                      const char*, (LumieraInterface iface),
                                                      {return "ct@pipapo.org";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (copyright, "\163\106\344\014\251\125\111\252\236\322\174\120\335\225\333\245",
                                                      const char*, (LumieraInterface iface),
                                                      {
                                                        return
                                                          "Copyright (C)        Lumiera.org\n"
                                                          "  2008               Christian Thaeter <ct@pipapo.org>";
                                                      }
                                                      ),
                            LUMIERA_INTERFACE_INLINE (license, "\343\031\207\122\225\217\014\163\015\023\243\101\165\377\222\350",
                                                      const char*, (LumieraInterface iface),
                                                      {
                                                        return
                                                          "This program is free software; you can redistribute it and/or modify\n"
                                                          "it under the terms of the GNU General Public License as published by\n"
                                                          "the Free Software Foundation; either version 2 of the License, or\n"
                                                          "(at your option) any later version.\n"
                                                          "\n"
                                                          "This program is distributed in the hope that it will be useful,\n"
                                                          "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                                                          "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                                                          "GNU General Public License for more details.\n"
                                                          "\n"
                                                          "You should have received a copy of the GNU General Public License\n"
                                                          "along with this program; if not, write to the Free Software\n"
                                                          "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA";
                                                      }
                                                      ),

                            LUMIERA_INTERFACE_INLINE (state, "\364\002\115\170\361\321\301\244\350\322\270\362\010\231\207\334",
                                                      int, (LumieraInterface iface),
                                                      {return LUMIERA_INTERFACE_EXPERIMENTAL;}
                                                      ),

                            LUMIERA_INTERFACE_INLINE (versioncmp, "\136\037\234\304\100\247\244\342\324\353\072\060\161\053\077\263",
                                                      int, (const char* a, const char* b),
                                                      {return 0;}
                                                      )
                            );



LUMIERA_EXPORT (interfaces_defined_here,
                LUMIERA_INTERFACE_DEFINE (lumieraorg_testexample_one, 0,
                                          lumieraorg_first_test,
                                          LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_tests_descriptor),
                                          testacquire,
                                          testrelease,
                                          LUMIERA_INTERFACE_MAP (foo1, "\214\310\136\372\003\344\163\377\075\100\070\200\375\221\227\324",
                                                                 testfunc),
                                          LUMIERA_INTERFACE_MAP (bar1, "\262\253\067\211\157\052\212\140\114\334\231\250\340\075\214\030",
                                                                 testfunc)
                                          ),
                LUMIERA_INTERFACE_DEFINE (lumieraorg_testexample_two, 0,
                                          lumieraorg_second_test,
                                          LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_tests_descriptor),
                                          testacquire,
                                          testrelease,
                                          LUMIERA_INTERFACE_MAP (foo2, "\110\152\002\271\363\052\324\272\373\045\132\270\277\000\271\217",
                                                                 testfunc),
                                          LUMIERA_INTERFACE_MAP (bar2, "\376\042\027\336\355\113\132\233\350\312\170\077\377\370\356\167",
                                                                 testfunc)
                                          )
                );


/*
  Now we rig a cross dependency test

  we have 4 instances, the respective acquire/release operations set following up:

  one   depends on two and three
  two   depends on one and four
  three depends on two and four
  four  depends on one, two three

  These all are empty interfaces with no slots
*/

static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) one_keeps_two;
static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) one_keeps_three;

LumieraInterface
testacquire_one (LumieraInterface self)
{
  TRACE (tests, "Acquire one %s_%d_%s", self->interface, self->version, self->name);
  one_keeps_two = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_two);
  one_keeps_three = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_three);
  return self;
}

void
testrelease_one (LumieraInterface self)
{
  TRACE (tests, "Release one %s_%d_%s", self->interface, self->version, self->name);
  lumiera_interface_close ((LumieraInterface)one_keeps_two);
  lumiera_interface_close ((LumieraInterface)one_keeps_three);
}

static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) two_keeps_one;
static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) two_keeps_four;

LumieraInterface
testacquire_two (LumieraInterface self)
{
  TRACE (tests, "Acquire two %s_%d_%s", self->interface, self->version, self->name);
  two_keeps_one = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_one);
  two_keeps_four = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_four);
  return self;
}

void
testrelease_two (LumieraInterface self)
{
  TRACE (tests, "Release two %s_%d_%s", self->interface, self->version, self->name);
  lumiera_interface_close ((LumieraInterface)two_keeps_one);
  lumiera_interface_close ((LumieraInterface)two_keeps_four);
}

static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) three_keeps_two;
static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) three_keeps_four;

LumieraInterface
testacquire_three (LumieraInterface self)
{
  TRACE (tests, "Acquire three %s_%d_%s", self->interface, self->version, self->name);
  three_keeps_two = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_two);
  three_keeps_four = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_four);
  return self;
}

void
testrelease_three (LumieraInterface self)
{
  TRACE (tests, "Release three %s_%d_%s", self->interface, self->version, self->name);
  lumiera_interface_close ((LumieraInterface)three_keeps_two);
  lumiera_interface_close ((LumieraInterface)three_keeps_four);
}

static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) four_keeps_one;
static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) four_keeps_two;
static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) four_keeps_three;

LumieraInterface
testacquire_four (LumieraInterface self)
{
  TRACE (tests, "Acquire four %s_%d_%s", self->interface, self->version, self->name);
  four_keeps_one = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_one);
  four_keeps_two = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_two);
  four_keeps_three = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_three);
  return self;
}

void
testrelease_four (LumieraInterface self)
{
  TRACE (tests, "Release four %s_%d_%s", self->interface, self->version, self->name);
  lumiera_interface_close ((LumieraInterface)four_keeps_one);
  lumiera_interface_close ((LumieraInterface)four_keeps_two);
  lumiera_interface_close ((LumieraInterface)four_keeps_three);
}


LUMIERA_EXPORT (dependencytests,
                LUMIERA_INTERFACE_DEFINE (lumieraorg_testexample_void, 0,
                                          lumieraorg_dependencytest_one,
                                          LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_tests_descriptor),
                                          testacquire_one,
                                          testrelease_one
                                          ),
                LUMIERA_INTERFACE_DEFINE (lumieraorg_testexample_void, 0,
                                          lumieraorg_dependencytest_two,
                                          LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_tests_descriptor),
                                          testacquire_two,
                                          testrelease_two
                                          ),
                LUMIERA_INTERFACE_DEFINE (lumieraorg_testexample_void, 0,
                                          lumieraorg_dependencytest_three,
                                          LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_tests_descriptor),
                                          testacquire_three,
                                          testrelease_three
                                          ),
                LUMIERA_INTERFACE_DEFINE (lumieraorg_testexample_void, 0,
                                          lumieraorg_dependencytest_four,
                                          LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_tests_descriptor),
                                          testacquire_four,
                                          testrelease_four
                                          )
                );


TESTS_BEGIN

TEST ("basic")
{
  lumiera_interfaceregistry_init ();

  lumiera_interfaceregistry_bulkregister_interfaces (interfaces_defined_here(), NULL);


  /* some ugly lowlevel handling tests */

  LumieraInterface handle1 =
    lumiera_interfaceregistry_interface_find ("lumieraorg_testexample_one", 0, "lumieraorg_first_test");

  (LUMIERA_INTERFACE_CAST(lumieraorg_testexample_one, 0)handle1)->bar1 ("this is bar1");


  LUMIERA_INTERFACE_TYPE(lumieraorg_testexample_two, 0)* handle2 = LUMIERA_INTERFACE_CAST(lumieraorg_testexample_two, 0)
    lumiera_interfaceregistry_interface_find ("lumieraorg_testexample_two", 0, "lumieraorg_second_test");

  handle2->foo2 ("this is foo2");

  lumiera_interfaceregistry_bulkremove_interfaces (interfaces_defined_here());
  lumiera_interfaceregistry_destroy ();
}

TEST ("open_close")
{
  lumiera_interfaceregistry_init ();
  lumiera_interfaceregistry_bulkregister_interfaces (interfaces_defined_here(), NULL);

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_one, 0) handle =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_one, 0, 0, lumieraorg_first_test);
  ENSURE (handle);

  handle->bar1 ("this is bar1");

  lumiera_interface_close ((LumieraInterface)handle);

  lumiera_interfaceregistry_bulkremove_interfaces (interfaces_defined_here());
  lumiera_interfaceregistry_destroy ();
}

TEST ("dependencies_one")
{
  lumiera_interfaceregistry_init ();
  lumiera_interfaceregistry_bulkregister_interfaces (dependencytests(), NULL);

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_one);
  ENSURE (handle);

  TRACE (tests, "Sucessfully opened");

  lumiera_interface_close ((LumieraInterface)handle);

  lumiera_interfaceregistry_bulkremove_interfaces (dependencytests());
  lumiera_interfaceregistry_destroy ();
}


TEST ("dependencies_two")
{
  lumiera_interfaceregistry_init ();
  lumiera_interfaceregistry_bulkregister_interfaces (dependencytests(), NULL);

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_two);
  ENSURE (handle);

  TRACE (tests, "Sucessfully opened");

  lumiera_interface_close ((LumieraInterface)handle);

  lumiera_interfaceregistry_bulkremove_interfaces (dependencytests());
  lumiera_interfaceregistry_destroy ();
}

TEST ("dependencies_three")
{
  lumiera_interfaceregistry_init ();
  lumiera_interfaceregistry_bulkregister_interfaces (dependencytests(), NULL);

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_three);
  ENSURE (handle);

  TRACE (tests, "Sucessfully opened");

  lumiera_interface_close ((LumieraInterface)handle);

  lumiera_interfaceregistry_bulkremove_interfaces (dependencytests());
  lumiera_interfaceregistry_destroy ();
}


TEST ("dependencies_four")
{
  lumiera_interfaceregistry_init ();
  lumiera_interfaceregistry_bulkregister_interfaces (dependencytests(), NULL);

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_four);
  ENSURE (handle);

  TRACE (tests, "Sucessfully opened");

  lumiera_interface_close ((LumieraInterface)handle);

  lumiera_interfaceregistry_bulkremove_interfaces (dependencytests());
  lumiera_interfaceregistry_destroy ();
}



TEST ("dependencies_all")
{
  lumiera_interfaceregistry_init ();
  lumiera_interfaceregistry_bulkregister_interfaces (dependencytests(), NULL);

  TRACE (tests, "OPEN one");
  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle_one =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_one);
  ENSURE (handle_one);

  TRACE (tests, "OPEN three");
  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle_three =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_three);
  ENSURE (handle_three);

  TRACE (tests, "OPEN two");
  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle_two =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_two);
  ENSURE (handle_two);

  TRACE (tests, "OPEN four");
  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle_four =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_four);
  ENSURE (handle_four);

  TRACE (tests, "Sucessfully OPENED");

  TRACE (tests, "CLOSE four");
  lumiera_interface_close ((LumieraInterface)handle_four);

  TRACE (tests, "CLOSE two");
  lumiera_interface_close ((LumieraInterface)handle_two);

  TRACE (tests, "CLOSE three");
  lumiera_interface_close ((LumieraInterface)handle_three);

  TRACE (tests, "CLOSE one");
  lumiera_interface_close ((LumieraInterface)handle_one);


  lumiera_interfaceregistry_bulkremove_interfaces (dependencytests());
  lumiera_interfaceregistry_destroy ();
}



TEST ("highlevel, plugin")
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
