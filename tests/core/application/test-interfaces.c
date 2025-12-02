/*
  TEST-INTERFACES  -  test interfaces declaration and implementation

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file test-interfaces.c
 ** C unit test to cover the basics of our interface and plugin-loading system
 ** @see interface.h
 ** @see interfaceregistry.h
 */


#include "common/interface.h"
#include "common/interfaceregistry.h"
#include "common/interface-descriptor.h"
#include "common/config.h"
#include "common/config-interface.h"

#include "lib/test/test.h"
#include "interface/say-hello.h"

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
  ECHO ("Called as '%s'", message);
}



LumieraInterface
testacquire (LumieraInterface self, LumieraInterface interface)
{
  (void) interface;
  ECHO ("Acquire %s_%d_%s", self->interface, self->version, self->name);
  return self;
}


void
testrelease (LumieraInterface self)
{
  ECHO ("Release %s_%d_%s", self->interface, self->version, self->name);
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
                            LUMIERA_INTERFACE_INLINE (name,
                                                      const char*, (LumieraInterface iface),
                                                      {(void)iface; return "LumieraTest";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (brief,
                                                      const char*, (LumieraInterface iface),
                                                      {(void)iface; return "Lumiera Test suite examples";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (homepage,
                                                      const char*, (LumieraInterface iface),
                                                      {(void)iface; return "http://www.lumiera.org/develompent.html";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (version,
                                                      const char*, (LumieraInterface iface),
                                                      {(void)iface; return "No Version";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (author,
                                                      const char*, (LumieraInterface iface),
                                                      {(void)iface; return "Christian Thaeter";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (email,
                                                      const char*, (LumieraInterface iface),
                                                      {(void)iface; return "ct@pipapo.org";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (copyright,
                                                      const char*, (LumieraInterface iface),
                                                      {
                                                        (void)iface; 
                                                        return
                                                                 "Copyright (C)\n"
                                                                 "  2008,            Christian Thaeter <ct@pipapo.org>";
                                                      }
                                                      ),
                            LUMIERA_INTERFACE_INLINE (license,
                                                      const char*, (LumieraInterface iface),
                                                      {
                                                        (void)iface;   
                                                        return
                                                                 "**Lumiera** is free software; you can redistribute it and/or modify it\n"
                                                                 "under the terms of the GNU General Public License as published by the\n"
                                                                 "Free Software Foundation; either version 2 of the License, or (at your\n"
                                                                 "option) any later version. See the file COPYING for further details."
                                                                 ;
                                                      }
                                                      ),

                            LUMIERA_INTERFACE_INLINE (state,
                                                      int, (LumieraInterface iface),
                                                      {(void)iface; return LUMIERA_INTERFACE_EXPERIMENTAL;}
                                                      ),

                            LUMIERA_INTERFACE_INLINE (versioncmp,
                                                      int, (const char* a, const char* b),
                                                      {(void)a;(void)b;  return 0;}
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
testacquire_one (LumieraInterface self, LumieraInterface interface)
{
  (void) interface;
  ECHO ("Acquire one %s_%d_%s", self->interface, self->version, self->name);
  one_keeps_two = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_two);
  one_keeps_three = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_three);
  return self;
}

void
testrelease_one (LumieraInterface self)
{
  ECHO ("Release one %s_%d_%s", self->interface, self->version, self->name);
  lumiera_interface_close ((LumieraInterface)one_keeps_two);
  lumiera_interface_close ((LumieraInterface)one_keeps_three);
}

static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) two_keeps_one;
static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) two_keeps_four;

LumieraInterface
testacquire_two (LumieraInterface self, LumieraInterface interface)
{
  (void) interface;
  ECHO ("Acquire two %s_%d_%s", self->interface, self->version, self->name);
  two_keeps_one = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_one);
  two_keeps_four = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_four);
  return self;
}

void
testrelease_two (LumieraInterface self)
{
  ECHO ("Release two %s_%d_%s", self->interface, self->version, self->name);
  lumiera_interface_close ((LumieraInterface)two_keeps_one);
  lumiera_interface_close ((LumieraInterface)two_keeps_four);
}

static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) three_keeps_two;
static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) three_keeps_four;

LumieraInterface
testacquire_three (LumieraInterface self, LumieraInterface interface)
{
  (void) interface;
  ECHO ("Acquire three %s_%d_%s", self->interface, self->version, self->name);
  three_keeps_two = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_two);
  three_keeps_four = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_four);
  return self;
}

void
testrelease_three (LumieraInterface self)
{
  ECHO ("Release three %s_%d_%s", self->interface, self->version, self->name);
  lumiera_interface_close ((LumieraInterface)three_keeps_two);
  lumiera_interface_close ((LumieraInterface)three_keeps_four);
}

static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) four_keeps_one;
static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) four_keeps_two;
static LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) four_keeps_three;

LumieraInterface
testacquire_four (LumieraInterface self, LumieraInterface interface)
{
  (void) interface;
  ECHO ("Acquire four %s_%d_%s", self->interface, self->version, self->name);
  four_keeps_one = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_one);
  four_keeps_two = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_two);
  four_keeps_three = LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_three);
  return self;
}

void
testrelease_four (LumieraInterface self)
{
  ECHO ("Release four %s_%d_%s", self->interface, self->version, self->name);
  lumiera_interface_close ((LumieraInterface)four_keeps_one);
  lumiera_interface_close ((LumieraInterface)four_keeps_two);
  lumiera_interface_close ((LumieraInterface)four_keeps_three);
}


LUMIERA_EXPORT (
                LUMIERA_INTERFACE_DEFINE (lumieraorg_testexample_one, 0,
                                          lumieraorg_first_test,
                                          LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_tests_descriptor),
                                          testacquire,
                                          testrelease,
                                          LUMIERA_INTERFACE_MAP (foo1,
                                                                 testfunc),
                                          LUMIERA_INTERFACE_MAP (bar1,
                                                                 testfunc)
                                          ),
                LUMIERA_INTERFACE_DEFINE (lumieraorg_testexample_two, 0,
                                          lumieraorg_second_test,
                                          LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_tests_descriptor),
                                          testacquire,
                                          testrelease,
                                          LUMIERA_INTERFACE_MAP (foo2,
                                                                 testfunc),
                                          LUMIERA_INTERFACE_MAP (bar2,
                                                                 testfunc)
                                          ),
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

TEST (basic)
{
  lumiera_interfaceregistry_init ();

  lumiera_interfaceregistry_bulkregister_interfaces (lumiera_plugin_interfaces(), NULL);


  /* some ugly lowlevel handling tests */

  LumieraInterface handle1 =
    lumiera_interfaceregistry_interface_find ("lumieraorg_testexample_one", 0, "lumieraorg_first_test");

  (LUMIERA_INTERFACE_CAST(lumieraorg_testexample_one, 0)handle1)->bar1 ("this is bar1");


  LUMIERA_INTERFACE_TYPE(lumieraorg_testexample_two, 0)* handle2 = LUMIERA_INTERFACE_CAST(lumieraorg_testexample_two, 0)
    lumiera_interfaceregistry_interface_find ("lumieraorg_testexample_two", 0, "lumieraorg_second_test");

  handle2->foo2 ("this is foo2");

  LUMIERA_INTERFACE_UNREGISTEREXPORTED;
  lumiera_interfaceregistry_destroy ();
}

TEST (open_close)
{
  lumiera_interfaceregistry_init ();
  lumiera_interfaceregistry_bulkregister_interfaces (lumiera_plugin_interfaces(), NULL);

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_one, 0) handle =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_one, 0, 0, lumieraorg_first_test);
  CHECK (handle);

  handle->bar1 ("this is bar1");

  lumiera_interface_close ((LumieraInterface)handle);

  LUMIERA_INTERFACE_UNREGISTEREXPORTED;
  lumiera_interfaceregistry_destroy ();
}

TEST (dependencies_one)
{
  lumiera_interfaceregistry_init ();
  LUMIERA_INTERFACE_REGISTEREXPORTED;

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_one);
  CHECK (handle);

  ECHO ("Sucessfully opened");

  lumiera_interface_close ((LumieraInterface)handle);

  LUMIERA_INTERFACE_UNREGISTEREXPORTED;
  lumiera_interfaceregistry_destroy ();
}


TEST (dependencies_two)
{
  lumiera_interfaceregistry_init ();
  LUMIERA_INTERFACE_REGISTEREXPORTED;

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_two);
  CHECK (handle);

  ECHO ("Sucessfully opened");

  lumiera_interface_close ((LumieraInterface)handle);

  LUMIERA_INTERFACE_UNREGISTEREXPORTED;
  lumiera_interfaceregistry_destroy ();
}

TEST (dependencies_three)
{
  lumiera_interfaceregistry_init ();
  LUMIERA_INTERFACE_REGISTEREXPORTED;

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_three);
  CHECK (handle);

  ECHO ("Sucessfully opened");

  lumiera_interface_close ((LumieraInterface)handle);

  LUMIERA_INTERFACE_UNREGISTEREXPORTED;
  lumiera_interfaceregistry_destroy ();
}


TEST (dependencies_four)
{
  lumiera_interfaceregistry_init ();
  LUMIERA_INTERFACE_REGISTEREXPORTED;

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_four);
  CHECK (handle);

  ECHO ("Sucessfully opened");

  lumiera_interface_close ((LumieraInterface)handle);

  LUMIERA_INTERFACE_UNREGISTEREXPORTED;
  lumiera_interfaceregistry_destroy ();
}



TEST (dependencies_all)
{
  lumiera_interfaceregistry_init ();
  LUMIERA_INTERFACE_REGISTEREXPORTED;

  ECHO ("OPEN one");
  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle_one =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_one);
  CHECK (handle_one);

  ECHO ("OPEN three");
  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle_three =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_three);
  CHECK (handle_three);

  ECHO ("OPEN two");
  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle_two =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_two);
  CHECK (handle_two);

  ECHO ("OPEN four");
  LUMIERA_INTERFACE_HANDLE(lumieraorg_testexample_void, 0) handle_four =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testexample_void, 0, 0, lumieraorg_dependencytest_four);
  CHECK (handle_four);

  ECHO ("Sucessfully OPENED");

  ECHO ("CLOSE four");
  lumiera_interface_close ((LumieraInterface)handle_four);

  ECHO ("CLOSE two");
  lumiera_interface_close ((LumieraInterface)handle_two);

  ECHO ("CLOSE three");
  lumiera_interface_close ((LumieraInterface)handle_three);

  ECHO ("CLOSE one");
  lumiera_interface_close ((LumieraInterface)handle_one);


  LUMIERA_INTERFACE_UNREGISTEREXPORTED;
  lumiera_interfaceregistry_destroy ();
}


TEST (plugin_discover)
{
  lumiera_interfaceregistry_init ();

  if (lumiera_plugin_discover (lumiera_plugin_load, lumiera_plugin_register))
    {
      LumieraPlugin p = lumiera_plugin_lookup ("modules/test-c-plugin.lum");
      printf ("found plugin: %s\n", lumiera_plugin_name (p));
      p = lumiera_plugin_lookup ("modules/test-cpp-plugin.lum");
      printf ("found plugin: %s\n", lumiera_plugin_name (p));
      lumiera_plugin_discover (lumiera_plugin_load, lumiera_plugin_register);
    }
  else
    printf ("error: %s\n", lumiera_error ());

  lumiera_interfaceregistry_destroy ();
}


TEST (plugin_unload)
{
  lumiera_interfaceregistry_init ();

  lumiera_plugin_discover (lumiera_plugin_load, lumiera_plugin_register);
  LumieraPlugin p = lumiera_plugin_lookup ("modules/test-c-plugin.lum");
  printf ("plugin discovered before unload: %p\n", p);
  CHECK (p, "prerequisite: need to load test-c-plugin.lum");

  lumiera_plugin_unload (lumiera_plugin_lookup ("modules/test-c-plugin.lum"));
  p = lumiera_plugin_lookup ("test-c-plugin.lum");
  printf ("plugin discovered after unload: %p\n", p);
  CHECK (!p, "failed to unload plugin.");

  lumiera_interfaceregistry_destroy ();
}


TEST (plugin_exampleplugin)
{
  lumiera_interfaceregistry_init ();
  lumiera_plugin_discover (lumiera_plugin_load, lumiera_plugin_register);

  TODO ("macro to derive minminor version from a slot");

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testhello, 0) german =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testhello, 0, 0, lumieraorg_hello_german);

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testhello, 0) english =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testhello, 0, 0, lumieraorg_hello_english);

  german->hello ();
  german->goodbye ("Welt!");

  english->hello ();
  english->goodbye ("World!");

  LUMIERA_INTERFACE_CLOSE (german);
  LUMIERA_INTERFACE_CLOSE (english);

  lumiera_interfaceregistry_destroy ();
}

TEST (plugin_exampleplugin_nested)
{
  lumiera_interfaceregistry_init ();
  lumiera_plugin_discover (lumiera_plugin_load, lumiera_plugin_register);
  lumiera_config_interface_init ();

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testtest, 0) test =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testtest, 0, 0, lumieraorg_test_both);

  test->testit ();

  LUMIERA_INTERFACE_CLOSE (test);

  lumiera_config_interface_destroy ();
  lumiera_interfaceregistry_destroy ();
}


TEST (plugin_exampleplugin_cpp)
{
  lumiera_interfaceregistry_init ();
  lumiera_plugin_discover (lumiera_plugin_load, lumiera_plugin_register);


  LUMIERA_INTERFACE_HANDLE(lumieraorg_testhello, 0) german =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testhello, 0, 0, lumieraorg_hello_german_cpp);

  LUMIERA_INTERFACE_HANDLE(lumieraorg_testhello, 0) english =
    LUMIERA_INTERFACE_OPEN (lumieraorg_testhello, 0, 0, lumieraorg_hello_english_cpp);

  german->hello ();
  german->goodbye ("schnöde Welt!");

  english->hello ();
  english->goodbye ("Vale of Tears!");

  LUMIERA_INTERFACE_CLOSE (german);
  LUMIERA_INTERFACE_CLOSE (english);

  lumiera_interfaceregistry_destroy ();
}

TESTS_END





/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
