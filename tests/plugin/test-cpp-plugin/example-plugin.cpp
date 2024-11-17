/*
  ExamplePlugin  -  example plugin (C++) for testing the interface/plugin system

   Copyright (C)
    2008,               Christian Thaeter <ct@pipapo.org>,
                        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file example-plugin.cpp
 ** Dummy C++ plug-in to cover the plugin loader for unit test
 ** This file defines a Lumiera plug-in written in C++,
 ** implementing the `lumieraorg_testhello` interface
 ** @see test-interfaces.c
 */



#include "common/interface-descriptor.h"
#include "common/config-interface.h"
#include "lib/format-string.hpp"

extern "C" {
#include "common/interface.h"
#include "common/interface-descriptor.h"

#include "interface/say-hello.h"
}

#include <iostream>

using std::cout;
using std::endl;



class ExamplePlugin
  {
  public:
    static LumieraInterface
    myopen (LumieraInterface self, LumieraInterface interfaces)
      {
        static util::_Fmt fmt("opened %x global interfaces %x");
        cout << fmt % self % interfaces << endl;
        return self;
      }
    
    static void
    myclose (LumieraInterface)
      {
        cout << "dying" << endl;
      }
  };


class ExamplePlugin_de
  : public ExamplePlugin
  {
  public:
    static void
    griazi ()
      {
        cout << "Hallo Welt!" << endl;
      }
    
    static void
    servus (const char* m)
      {
        cout << "Tschüss " << m << endl;
      }
  };


class ExamplePlugin_en
  : public ExamplePlugin
  {
  public:
    static void
    hello ()
      {
        cout << "Hello World!" << endl;
      }
    
    static void
    bye (const char* m)
      {
        cout << "Bye " << m << endl;
      }
  };




extern "C" { /* ================== define two lumieraorg_testhello instance ======================= */
  
  
  
  LUMIERA_EXPORT( /* ===================== PLUGIN EXPORTS ================================== */
                 
                 LUMIERA_INTERFACE_DEFINE (lumieraorg_testhello, 0
                                          ,lumieraorg_hello_german_cpp
                                          , NULL  /* no descriptor given */
                                          , ExamplePlugin::myopen
                                          , ExamplePlugin::myclose
                                          , LUMIERA_INTERFACE_MAP (hello,
                                                                   ExamplePlugin_de::griazi)
                                          , LUMIERA_INTERFACE_MAP (goodbye,
                                                                   ExamplePlugin_de::servus)
                                          ),
                 LUMIERA_INTERFACE_DEFINE (lumieraorg_testhello, 0
                                          ,lumieraorg_hello_english_cpp
                                          , NULL  /* no descriptor given */
                                          , ExamplePlugin::myopen
                                          , ExamplePlugin::myclose
                                          , LUMIERA_INTERFACE_MAP (hello,
                                                                   ExamplePlugin_en::hello)
                                          , LUMIERA_INTERFACE_MAP (goodbye,
                                                                   ExamplePlugin_en::bye)
                                          )
                );

} // extern "C"
