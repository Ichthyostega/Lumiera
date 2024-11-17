/*
  ExamplePlugin  -  example plugin for testing the interface/plugin system

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file example-plugin.c
 ** Dummy C plug-in to cover the plugin loader for unit test
 ** This file defines a Lumiera plug-in written in plain C,
 ** implementing the `lumieraorg_testhello` interface
 ** @see test-interfaces.c
 */


#include <stdio.h>

#include "common/interface-descriptor.h"
#include "common/config-interface.h"

#include "interface/say-hello.h"


LUMIERA_PLUGIN_INTERFACEHANDLE;

LumieraInterface
myopen (LumieraInterface self, LumieraInterface interfaces)
{
  LUMIERA_PLUGIN_STORE_INTERFACEHANDLE (interfaces);

  fprintf (stderr, "opened %p global interfaces %p\n", self, interfaces);
  return self;
}

void
myclose (LumieraInterface self)
{
  fprintf (stderr, "closed %p\n", self);
}

void hallo (void)
{
  printf ("Hallo Welt!\n");
}

void tschuess (const char* m)
{
  printf ("Tschuess %s\n", m);
}

void hello (void)
{
  printf ("Hello World!\n");
}

void bye (const char* m)
{
  printf ("Bye %s\n", m);
}


void yeahbabe (void)
{
  LUMIERA_INTERFACE_HANDLE (lumieraorg_testhello, 0) german =
    LUMIERA_INTERFACE_OPEN  (lumieraorg_testhello, 0, 0, lumieraorg_hello_german);

  LUMIERA_INTERFACE_HANDLE (lumieraorg_testhello, 0) english =
    LUMIERA_INTERFACE_OPEN  (lumieraorg_testhello, 0, 0, lumieraorg_hello_english);

  LUMIERA_INTERFACE_HANDLE (lumieraorg_configuration, 0) config =
    LUMIERA_INTERFACE_OPEN  (lumieraorg_configuration, 0, 0, lumieraorg_configuration);

  const char* path;
  if (config->wordlist_get ("config.path", &path))
    printf ("config path is: %s\n", path);
  if (config->wordlist_get ("plugin.path", &path))
    printf ("plugin path is: %s\n", path);

  LUMIERA_INTERFACE_CLOSE (config);

  german->hello ();
  english->hello ();
  english->goodbye ("World!");
  german->goodbye ("Welt!");

  LUMIERA_INTERFACE_CLOSE (german);
  LUMIERA_INTERFACE_CLOSE (english);
}



LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0,
                            lumieraorg_exampleplugin_descriptor,
                            NULL, NULL, NULL,
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


LUMIERA_EXPORT(
               LUMIERA_INTERFACE_DEFINE (lumieraorg_testhello, 0,
                                         lumieraorg_hello_german,
                                         LUMIERA_INTERFACE_REF (lumieraorg_interfacedescriptor, 0, lumieraorg_exampleplugin_descriptor),
                                         myopen,
                                         myclose,
                                         LUMIERA_INTERFACE_MAP (hello,
                                                                hallo),
                                         LUMIERA_INTERFACE_MAP (goodbye,
                                                                tschuess)
                                         ),
               LUMIERA_INTERFACE_DEFINE (lumieraorg_testhello, 0,
                                         lumieraorg_hello_english,
                                         LUMIERA_INTERFACE_REF (lumieraorg_interfacedescriptor, 0, lumieraorg_exampleplugin_descriptor),
                                         myopen,
                                         myclose,
                                         LUMIERA_INTERFACE_MAP (hello,
                                                                hello),
                                         LUMIERA_INTERFACE_MAP (goodbye,
                                                                bye)
                                         ),
               LUMIERA_INTERFACE_DEFINE (lumieraorg_testtest, 0,
                                         lumieraorg_test_both,
                                         LUMIERA_INTERFACE_REF (lumieraorg_interfacedescriptor, 0, lumieraorg_exampleplugin_descriptor),
                                         myopen,
                                         myclose,
                                         LUMIERA_INTERFACE_MAP (testit,
                                                                yeahbabe)
                                         )
               )
