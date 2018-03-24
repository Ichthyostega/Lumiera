/*
  GtkLumiera  -  entry point for the lumiera GUI loaded as shared module

  Copyright (C)         Lumiera.org
    2007-2008,          Joel Holdsworth <joel@airwebreathe.org.uk>
    2009,               Hermann Vosseler <Ichthyostega@web.de>
                        Christian Thaeter <ct@pipapo.org>

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


/** @file gtk-lumiera.cpp
 ** Start up the Lumiera GTK GUI when loading it as dynamic module.
 ** This plugin is linked together with the Lumiera GUI code; when loaded as
 ** Lumiera plugin, it allows to kick off the GTK main event loop and thus to bring
 ** up the GUI. The loading and shutdown process is carried out by gui::GuiFacade and
 ** controlled by lumiera::AppState, which in turn is activated by Lumiera main().
 ** 
 ** After successfully loading this module, a call to GuiFacade::launchUI is expected to
 ** happen, passing a termination signal (callback) to be executed when the GUI terminates.
 ** The implementation of GuiFacade in the GuiRunner in fact issues this call from the ctor
 ** body, while the interface is opened via an InstanceHandle member. The `launchUI()` call
 ** starts a new thread, which then becomes the UI event thread and remains blocked within
 ** the main GTK event loop. Before entering this loop, the CoreService of the GUI and
 ** especially the [UI-Bus](\ref ui-bus.hpp) is started see \ref GtkLumiera::run().
 ** This entails also to open the primary "business" interface(s) of the GUI
 ** (currently as of 1/16 this is the interface gui::GuiNotification.)
 ** 
 ** @see lumiera::AppState
 ** @see gui::GuiFacade
 ** @see guifacade.cpp
 ** @see ui-manager.hpp
 */

#include "gui/gtk-base.hpp"

#include "lib/error.hpp"
#include "gui/ui-bus.hpp"
#include "gui/guifacade.hpp"
#include "gui/ctrl/ui-manager.hpp"
#include "gui/display-service.hpp"
#include "backend/thread-wrapper.hpp"
#include "common/subsys.hpp"
#include "lib/nocopy.hpp"

extern "C" {
#include "common/interface.h"
#include "common/interface-descriptor.h"
}

#include <string>



using backend::Thread;
using lumiera::Subsys;
using lumiera::error::LUMIERA_ERROR_STATE;
using gui::LUMIERA_INTERFACE_INAME(lumieraorg_Gui, 1);

using std::string;


namespace gui {
  
  namespace { // implementation details
    
    using ctrl::UiManager;
    
    
    /**************************************************************************//**
     * Implement the necessary steps for actually making the Lumiera Gui available.
     * Establish the UI backbone services and start up the GTK GUI main event loop.
     * @todo to ensure invocation of the termination signal, any members
     *       should be failsafe on initialisation (that means, we must not
     *       open other interfaces here...)            ///////////////////////////TICKET #82
     */
    class GtkLumiera
      : util::NonCopyable
      {
        UiBus uiBus_;
        UiManager uiManager_;
        DisplayService activateDisplayService_;        ///////////////////////////TICKET #82 will go away once we have a real OutputSlot offered by the UI
        
      public:
        GtkLumiera ()
          : uiBus_{}
          , uiManager_{uiBus_}
          , activateDisplayService_()                  ///////////////////////////TICKET #82 obsolete (and incurs a race)
          { }
        
        
        void
        run (Subsys::SigTerm reportOnTermination)
          {
            string errorMsgBuff;
            try
              {
                // execute the GTK Event Loop____________
                uiManager_.createApplicationWindow();
                uiManager_.performMainLoop();
              }                                        // all went well, regular shutdown
            
            catch (lumiera::Error& problem)
              {
                errorMsgBuff = problem.what();
                lumiera_error();                       // clear error flag
              }
            catch (...)
              {
                errorMsgBuff = "unexpected error terminated the GUI.";
              }
            if (lumiera_error_peek())
              errorMsgBuff = string{lumiera_error()};
            
            reportOnTermination(&errorMsgBuff);        // inform main thread that the GUI has been shut down. 
          }
      };
    
    
    void
    runGUI (Subsys::SigTerm& reportOnTermination)
    {
      GtkLumiera{}.run (reportOnTermination);
    }
    
  } // (End) impl details
  
  
  
  
  bool
  launchUI (Subsys::SigTerm& terminationHandle)
  {
    try
      {
        Thread {"GUI-Main", bind (&runGUI, terminationHandle)};
        return true; // if we reach this line...
      }
    catch(...)
      {
        if (!lumiera_error_peek())
          LUMIERA_ERROR_SET (gui, STATE, "unexpected error when starting the GUI thread");
        return false;
      }           // note: lumiera_error state remains set
  }

} // namespace gui






extern "C" { /* ================== define a lumieraorg_Gui instance ======================= */
  
  
  LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0
                             ,lumieraorg_GuiStarterPlugin_descriptor
                             , NULL, NULL, NULL
                             , LUMIERA_INTERFACE_INLINE (name,
                                                         const char*, (LumieraInterface ifa),
                                                           { (void)ifa;  return "GuiStarterPlugin"; }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (brief,
                                                         const char*, (LumieraInterface ifa),
                                                           { (void)ifa;  return "entry point to start up the Lumiera GTK GUI contained in this dynamic module"; }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (homepage,
                                                         const char*, (LumieraInterface ifa),
                                                           { (void)ifa;  return "http://www.lumiera.org/develompent.html" ;}
                                                        )
                             , LUMIERA_INTERFACE_INLINE (version,
                                                         const char*, (LumieraInterface ifa),
                                                           { (void)ifa;  return "0.1~pre"; }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (author,
                                                         const char*, (LumieraInterface ifa),
                                                           { (void)ifa;  return "Joel Holdsworth, Christian Thaeter, Hermann Vosseler"; }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (email,
                                                         const char*, (LumieraInterface ifa),
                                                           { (void)ifa;  return "Lumiera@lists.lumiera.org"; }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (copyright,
                                                         const char*, (LumieraInterface ifa),
                                                           {
                                                             (void)ifa;
                                                             return
                                                               "Copyright (C)       Lumiera.org\n"
                                                               "2007-2008,          Joel Holdsworth <joel@airwebreathe.org.uk>\n"
                                                               "2009,               Christian Thaeter <ct@pipapo.org>\n"
                                                               "                    Hermann Vosseler <Ichthyostega@web.de>";
                                                           }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (license,
                                                         const char*, (LumieraInterface ifa),
                                                           {
                                                             (void)ifa;
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
                             , LUMIERA_INTERFACE_INLINE (state,
                                                         int, (LumieraInterface ifa),
                                                           {(void)ifa;  return LUMIERA_INTERFACE_EXPERIMENTAL; }
                                                        )
                             , LUMIERA_INTERFACE_INLINE (versioncmp,
                                                         int, (const char* a, const char* b),
                                                           {(void)a;(void)b;  return 0;}  ////////////////////////////////////////////TODO define version ordering
                                                        )
                             );
  
  
  LUMIERA_EXPORT( /* ===================== PLUGIN EXPORTS ================================== */
                 
                 LUMIERA_INTERFACE_DEFINE (lumieraorg_Gui, 1
                                          ,lumieraorg_GuiStarterPlugin
                                          , LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_GuiStarterPlugin_descriptor)
                                          , NULL  /* on open  */
                                          , NULL  /* on close */
                                          , LUMIERA_INTERFACE_INLINE (launchUI,
                                                                      bool, (void* termSig),
                                                                        {
                                                                          return gui::launchUI (*reinterpret_cast<Subsys::SigTerm *> (termSig));
                                                                        }
                                                                     )
                                          )
                );

} // extern "C"
