/*
  GuiStart  -  entry point for the lumiera GUI loaded as shared module
 
  Copyright (C)         Lumiera.org
    2007-2008,          Joel Holdsworth <joel@airwebreathe.org.uk>
                        Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file guistart.cpp
 ** Start up the Lumiera GTK GUI when loading it as dynamic module.
 ** This plugin is linked together with the Lumiera GUI code; when loaded as
 ** Lumiera plugin, it allows to kick off the main GUI thread and thus to bring up
 ** the GUI. The loading and shutdown process is carried out by gui::GuiFacade and
 ** controlled by lumiera::AppState, which in turn is activated by Lumiera main().
 ** 
 ** After successfully loading this module, a call to #kickOff is expected to be
 ** issued, passing a termination signal (callback) to be executed when the GUI
 ** terminates. This call returns immediately, after spawning off the main thread
 ** and setting up the termination callback accordingly. Additionally, it cares
 ** for opening the primary "business" interface of the GUI, i.e. the interface
 ** gui::GuiNotification.
 **
 ** @see lumiera::AppState
 ** @see gui::GuiFacade
 ** @see guifacade.cpp
 ** @see ///////////////////////////////////TODO: add link to the gui main routine here!
 */


#include "gui/guifacade.hpp"
#include "lumiera/subsys.hpp"
#include "common/singleton.hpp"

extern "C" {
#include "lumiera/interface.h"
#include "lumiera/interfacedescriptor.h"
}

#include <string>

using std::string;

#include <iostream> /////////////TODO
using std::cout;   //////////////TODO


using lumiera::Subsys;
using gui::LUMIERA_INTERFACE_INAME(lumieraorg_Gui, 1);


namespace gui {
  
  namespace { // implementation details
    
    /** 
     * Implement the necessary steps for starting up the GUI main thread
     */
    struct GuiFacadeImpl
      : public GuiFacade
      {
        
        bool kickOff (Subsys::SigTerm& terminationHandle) 
          {
            cout << " *** Ha Ha Ha\n"
                 << "     this is the GuiStarterPlugin speaking!\n"
                 << "     now, the Lumiera GUI should be spawned....\n"
                 << "     but actually nothing happens!!!!!!!!!!!!!!\n\n";
            
            terminationHandle(0); // signal immediate shutdown without error
            return true;
          }
      };
    
    
    lumiera::Singleton<GuiFacadeImpl> facade_;
    
  } // (End) impl details

} // namespace gui






extern "C" { /* ================== define an lumieraorg_Gui instance ======================= */
  
  
  LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0
                             ,lumieraorg_GuiStarterPlugin_descriptor
                             , NULL, NULL, NULL
                             , LUMIERA_INTERFACE_INLINE (name, "\126\247\365\337\126\254\173\037\130\310\337\345\200\347\323\136",
                                                         const char*, (LumieraInterface iface),
                                                           { return "GuiStarterPlugin"; }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (brief, "\056\346\322\365\344\104\232\232\355\213\367\056\301\144\051\021",
                                                         const char*, (LumieraInterface iface),
                                                           { return "entry point to start up the Lumiera GTK GUI contained in this dynamic module"; }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (homepage, "\357\056\117\165\320\066\273\130\113\100\367\022\221\350\236\256",
                                                         const char*, (LumieraInterface iface),
                                                           { return "http://www.lumiera.org/develompent.html" ;}
                                                        )
                             , LUMIERA_INTERFACE_INLINE (version, "\013\117\366\210\070\320\274\076\253\230\032\116\271\161\027\354",
                                                         const char*, (LumieraInterface iface),
                                                           { return "0.1~pre"; }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (author, "\371\262\024\273\170\105\163\261\351\240\051\003\153\040\256\155",
                                                         const char*, (LumieraInterface iface),
                                                           { return "Joel Holdsworth, Christian Thaeter, Hermann Vosseler"; }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (email, "\353\242\247\130\056\242\314\145\053\162\003\060\200\357\303\214",
                                                         const char*, (LumieraInterface iface),
                                                           { return "Lumiera@lists.lumiera.org"; }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (copyright, "\172\325\335\304\015\222\377\372\343\151\255\020\030\103\320\101",
                                                         const char*, (LumieraInterface iface),
                                                           {
                                                             return
                                                               "Copyright (C)       Lumiera.org\n"
                                                               "2007-2008,          Joel Holdsworth <joel@airwebreathe.org.uk>\n"
                                                               "                    Christian Thaeter <ct@pipapo.org>\n"
                                                               "                    Hermann Vosseler <Ichthyostega@web.de>";
                                                           }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (license, "\016\264\202\005\160\305\033\227\037\077\143\363\263\011\167\257",
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
                                                        )
                             , LUMIERA_INTERFACE_INLINE (state, "\006\070\035\065\267\073\016\107\376\027\355\035\135\176\107\064",
                                                         int, (LumieraInterface iface),
                                                           {return LUMIERA_INTERFACE_EXPERIMENTAL; }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (versioncmp, "\224\077\275\040\357\244\311\244\112\030\042\163\061\166\245\325",
                                                         int, (const char* a, const char* b),
                                                           {return 0;}  ////////////////////////////////////////////TODO define version ordering
                                                        )
                             );
  
  
  LUMIERA_EXPORT( /* ===================== PLUGIN EXPORTS ================================== */
                 
                 LUMIERA_INTERFACE_DEFINE (lumieraorg_Gui, 1
                                          ,lumieraorg_GuiStarterPlugin
                                          , LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_GuiStarterPlugin_descriptor)
                                          , NULL  /* on open  */
                                          , NULL  /* on close */
                                          , LUMIERA_INTERFACE_INLINE (kickOff, "\255\142\006\244\057\170\152\312\301\372\220\323\230\026\200\065",
                                                                      bool, (void* termSig),
                                                                        { 
                                                                          return gui::facade_().kickOff (
                                                                                     *reinterpret_cast<Subsys::SigTerm *> (termSig));
                                                                        }
                                                                     )
                                          )
                );

} // extern "C"
