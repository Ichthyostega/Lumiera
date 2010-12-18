/*
  example_plugin.cpp  -  example plugin (C++) for testing the interface/plugin system

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>,
                        Hermann Vosseler <Ichthyostega@web.de>

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


#include <iostream>
#include <boost/format.hpp>

#include "common/interfacedescriptor.h"
#include "common/config_interface.h"

extern "C" {
#include "common/interface.h"
#include "common/interfacedescriptor.h"

#include "tests/common/hello_interface.h"
}

using boost::format;
using std::cout;
using std::endl;



class example_plugin
  {
  public:
    static LumieraInterface
    myopen (LumieraInterface self, LumieraInterface interfaces)
      {
        static format fmt("opened %x global interfaces %x");
        cout << fmt % self % interfaces << endl;
        return self;
      }
    
    static void
    myclose (LumieraInterface)
      {
        std::cout << "closed" << endl;
      }
  };


class example_plugin_de
  : public example_plugin
  {
  public:
    static void
    griazi ()
      {
        std::cout << "Hallo Welt!" << endl;
      }
    
    static void
    servus (const char* m)
      {
        std::cout << "Tschuess " << m << endl;
      }
  };


class example_plugin_en
  : public example_plugin
  {
  public:
    static void
    hello ()
      {
        std::cout << "Hello World!" << endl;
      }
    
    static void
    bye (const char* m)
      {
        std::cout << "Bye " << m << endl;
      }
  };




extern "C" { /* ================== define two lumieraorg_testhello instance ======================= */
  
  
  
  LUMIERA_EXPORT( /* ===================== PLUGIN EXPORTS ================================== */
                 
                 LUMIERA_INTERFACE_DEFINE (lumieraorg_testhello, 0
                                          ,lumieraorg_hello_german_cpp
                                          , NULL  /* no descriptor given */
                                          , example_plugin::myopen
                                          , example_plugin::myclose
                                          , LUMIERA_INTERFACE_MAP (hello, "\300\244\125\265\235\312\175\263\335\044\371\047\247\263\015\322",
                                                                   example_plugin_de::griazi)
                                          , LUMIERA_INTERFACE_MAP (goodbye, "\115\365\126\102\201\104\012\257\153\232\006\210\010\346\076\070",
                                                                   example_plugin_de::servus)
                                          ),
                 LUMIERA_INTERFACE_DEFINE (lumieraorg_testhello, 0
                                          ,lumieraorg_hello_english_cpp
                                          , NULL  /* no descriptor given */
                                          , example_plugin::myopen
                                          , example_plugin::myclose
                                          , LUMIERA_INTERFACE_MAP (hello, "\303\367\107\154\077\063\237\066\034\034\050\136\170\220\260\226",
                                                                   example_plugin_en::hello)
                                          , LUMIERA_INTERFACE_MAP (goodbye, "\107\207\072\105\101\102\150\201\322\043\104\110\232\023\205\161",
                                                                   example_plugin_en::bye)
                                          )
                );

} // extern "C"
