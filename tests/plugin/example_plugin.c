/*
  example_plugin  -  example plugin for testing the interface/plugin system
 
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
 
* *****************************************************/


#include <stdio.h>

#include "common/interfacedescriptor.h"
#include "common/config_interface.h"

#include "tests/common/hello_interface.h"


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
                            LUMIERA_INTERFACE_INLINE (name, "\003\307\005\305\201\304\175\377\120\105\332\016\136\354\251\022",
                                                      const char*, (LumieraInterface iface),
                                                      {return "LumieraTest";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (brief, "\303\047\265\010\242\210\365\340\024\030\350\310\067\171\170\260",
                                                      const char*, (LumieraInterface iface),
                                                      {return "Lumiera Test suite examples";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (homepage, "\363\125\352\312\056\255\274\322\351\245\051\350\120\024\115\263",
                                                      const char*, (LumieraInterface iface),
                                                      {return "http://www.lumiera.org/develompent.html";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (version, "\114\043\133\175\354\011\232\002\117\240\107\141\234\157\217\176",
                                                      const char*, (LumieraInterface iface),
                                                      {return "No Version";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (author, "\313\300\055\156\126\320\144\247\140\023\261\002\270\367\017\267",
                                                      const char*, (LumieraInterface iface),
                                                      {return "Christian Thaeter";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (email, "\163\051\312\276\137\317\267\305\237\274\133\012\276\006\255\160",
                                                      const char*, (LumieraInterface iface),
                                                      {return "ct@pipapo.org";}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (copyright, "\160\246\161\204\123\262\375\351\157\276\333\073\355\036\062\341",
                                                      const char*, (LumieraInterface iface),
                                                      {
                                                        return
                                                          "Copyright (C)        Lumiera.org\n"
                                                          "  2008               Christian Thaeter <ct@pipapo.org>";
                                                      }
                                                      ),
                            LUMIERA_INTERFACE_INLINE (license, "\007\311\044\214\064\223\201\326\331\111\233\356\055\264\211\201",
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

                            LUMIERA_INTERFACE_INLINE (state, "\331\353\126\162\067\376\340\242\232\175\167\105\122\177\306\354",
                                                      int, (LumieraInterface iface),
                                                      {return LUMIERA_INTERFACE_EXPERIMENTAL;}
                                                      ),

                            LUMIERA_INTERFACE_INLINE (versioncmp, "\363\145\363\224\325\104\177\057\344\023\367\111\376\221\152\135",
                                                      int, (const char* a, const char* b),
                                                      {return 0;}
                                                      )
                            );


LUMIERA_EXPORT(
               LUMIERA_INTERFACE_DEFINE (lumieraorg_testhello, 0,
                                         lumieraorg_hello_german,
                                         LUMIERA_INTERFACE_REF (lumieraorg_interfacedescriptor, 0, lumieraorg_exampleplugin_descriptor),
                                         myopen,
                                         myclose,
                                         LUMIERA_INTERFACE_MAP (hello, "\167\012\306\023\031\151\006\362\026\003\125\017\170\022\100\333",
                                                                hallo),
                                         LUMIERA_INTERFACE_MAP (goodbye, "\324\267\214\166\340\213\155\053\157\125\064\264\167\235\020\223",
                                                                tschuess)
                                         ),
               LUMIERA_INTERFACE_DEFINE (lumieraorg_testhello, 0,
                                         lumieraorg_hello_english,
                                         LUMIERA_INTERFACE_REF (lumieraorg_interfacedescriptor, 0, lumieraorg_exampleplugin_descriptor),
                                         myopen,
                                         myclose,
                                         LUMIERA_INTERFACE_MAP (hello, "\326\247\370\247\032\103\223\357\262\007\356\042\051\330\073\116",
                                                                hello),
                                         LUMIERA_INTERFACE_MAP (goodbye, "\365\141\371\047\101\230\050\106\071\231\022\235\325\112\354\241",
                                                                bye)
                                         ),
               LUMIERA_INTERFACE_DEFINE (lumieraorg_testtest, 0,
                                         lumieraorg_test_both,
                                         LUMIERA_INTERFACE_REF (lumieraorg_interfacedescriptor, 0, lumieraorg_exampleplugin_descriptor),
                                         myopen,
                                         myclose,
                                         LUMIERA_INTERFACE_MAP (testit, "\101\060\122\277\370\023\164\257\347\247\164\325\157\266\323\370",
                                                                yeahbabe)
                                         )
               )
