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
 ** up the GUI. The loading and shutdown process is carried out by stage::GuiFacade and
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
 ** (currently as of 1/16 this is the interface stage::GuiNotification.)
 ** 
 ** @see lumiera::AppState
 ** @see stage::GuiFacade
 ** @see guifacade.cpp
 ** @see ui-manager.hpp
 */

#include "stage/gtk-base.hpp"

#include "lib/error.hpp"
#include "lib/thread.hpp"
#include "stage/ui-bus.hpp"
#include "stage/guifacade.hpp"
#include "stage/ctrl/ui-manager.hpp"
#include "common/subsys.hpp"
#include "lib/nocopy.hpp"

extern "C" {
#include "common/interface.h"
#include "common/interface-descriptor.h"
}

#include <string>



using lumiera::Subsys;
using lib::launchDetached;
using lumiera::error::LUMIERA_ERROR_STATE;
using stage::LUMIERA_INTERFACE_INAME(lumieraorg_Gui, 1);

using std::string;


namespace lumiera {
namespace error {
  /* == definition common error marks for the UI, declared in gtk-base.hpp == */
  LUMIERA_ERROR_DEFINE (UIWIRING, "GUI state contradicts assumptions in signal wiring");
}}


namespace stage {
  
  namespace { // implementation details
    
    using ctrl::UiManager;
    
    
    /**************************************************************************//**
     * Implement the necessary steps for actually making the Lumiera UI available.
     * Establish the UI backbone services and start up the GTK GUI main event loop.
     * @warning to ensure reliable invocation of the termination signal,
     *          any members should be failsafe on initialisation
     */
    class GtkLumiera
      : util::NonCopyable
      {
        UiBus     uiBus_;
        UiManager uiManager_;
        
      public:
        GtkLumiera ()
          : uiBus_{}
          , uiManager_{uiBus_}
          { }
        
        
        string
        run()
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
            
            return errorMsgBuff;
          }
      };
    //(End) GUI-Thread.
  }//(End) impl details
  
  
  
  
  bool
  launchUI (Subsys::SigTerm& reportOnTermination)
  {
    try
      {
        launchDetached ("GUI-Main"
                       , [reportOnTermination]
                            {
                              string shutdownLog = GtkLumiera{}.run();
                               // inform main thread that the GUI has been shut down...
                              reportOnTermination (&shutdownLog);
                            });
        return true; // if we reach this line...
      }
    catch(...)
      {
        const char* errID = lumiera_error(); // clear C-style error flag
        WARN (stage, "Unexpected error while starting the GUI thread.");
        if (errID)
          TRACE (stage, "Error flag was: %s", errID);
        return false;
      }
  }

} // namespace stage






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
                                                                          return stage::launchUI (*reinterpret_cast<Subsys::SigTerm *> (termSig));
                                                                        }
                                                                     )
                                          )
                );

} // extern "C"
