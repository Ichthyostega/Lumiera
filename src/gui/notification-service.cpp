/*
  NotificationService  -  public service allowing to push information into the GUI

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file notification-service.cpp
 ** Implementation of notifications and updates within the GUI.
 ** This is the actual service implementation and runs within the GUI plugin.
 ** 
 ** @todo 1/2017 actually forward invocations on the UI-Bus
 ** @todo 1/2017 find a solution for passing diff messages    /////////////////////////////////////////////////TICKET #1066
 ** 
 */



#include "gui/ctrl/ui-manager.hpp"
#include "gui/ctrl/ui-dispatcher.hpp"
#include "gui/notification-service.hpp"
#include "include/logging.h"
#include "lib/util.hpp"

extern "C" {
#include "common/interface-descriptor.h"
}

#include <string>


using lib::diff::TreeMutator;
using gui::ctrl::UiDispatcher;
using gui::ctrl::BusTerm;
using std::string;
using util::cStr;


namespace gui {
  
  
  void 
  NotificationService::displayInfo (string const& text)
  {
    INFO (gui, "@GUI: display '%s' as notification message.", cStr(text));
    ////////////////////////TODO actually push the information to the GUI   ///////////////////////////////////TICKET #1098 : use a suitable Dispatcher
  }
  
  
  void
  NotificationService::markError (ID uiElement, string const& text)
  {
    UNIMPLEMENTED ("send a error mark message via UI-Bus");
    ////////////////////////TODO actually push the information to the GUI   ///////////////////////////////////TICKET #1098 : use a suitable Dispatcher
  }
  
  
  void
  NotificationService::markNote  (ID uiElement, string const& text)
  {
    UNIMPLEMENTED ("send a info/warning message via UI-Bus");
    ////////////////////////TODO actually push the information to the GUI   ///////////////////////////////////TICKET #1098 : use a suitable Dispatcher
  }
  
  
  void
  NotificationService::mutate (ID uiElement, DiffMessage&)    /////////////////////////////////////////////////TICKET #1066 : how to pass a diff message
  {
    UNIMPLEMENTED ("actually produce a MutationMessage on the UI-Bus");     ///////////////////////////////////TICKET #1066 : how to build and pass a MutationMessage
    ////////////////////////TODO actually push the information to the GUI   ///////////////////////////////////TICKET #1098 : use a suitable Dispatcher
  }
  
  
  void
  NotificationService::triggerGuiShutdown (string const& cause)
  {
    NOTICE (gui, "@GUI: shutdown triggered with explanation '%s'....", cStr(cause));
    TODO ("actually request a shutdown from the GUI");                      ///////////////////////////////////TICKET #1098 : use a suitable Dispatcher
    // NOTE basically this means to issue the call...
    // uiManager_.terminateUI();
  }
  
  
  
  namespace { // facade implementation details
    
    
    /* ================== define an lumieraorg_GuiNotification instance ======================= */
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0
                               ,lumieraorg_GuiNotificationFacade_descriptor
                               , NULL, NULL, NULL
                               , LUMIERA_INTERFACE_INLINE (name,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "GuiNotification"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (brief,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "GUI Interface: push state update and notification of events into the GUI"; }
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
                                                             { (void)ifa;  return "Hermann Vosseler"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (email,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "Ichthyostega@web.de"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (copyright,
                                                           const char*, (LumieraInterface ifa),
                                                             {
                                                               (void)ifa;
                                                               return
                                                                 "Copyright (C)        Lumiera.org\n"
                                                                 "  2008               Hermann Vosseler <Ichthyostega@web.de>";
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
    
    
    
    
    
    using lumiera::facade::LUMIERA_ERROR_FACADE_LIFECYCLE;
    typedef lib::SingletonRef<GuiNotification>::Accessor InstanceRef;
    
    InstanceRef _instance; ///< a backdoor for the C Language impl to access the actual GuiNotification implementation...
    
    
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_GuiNotification, 0
                               ,lumieraorg_GuiNotificationService
                               , LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_GuiNotificationFacade_descriptor)
                               , NULL /* on  open  */
                               , NULL /* on  close */
                               , LUMIERA_INTERFACE_INLINE (displayInfo,
                                                           void, (const char* text),
                                                             {
                                                               if (!_instance) lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, text);
                                                               else
                                                                 _instance->displayInfo (text);
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (markError,
                                                           void, (LumieraUid element, const char* text),
                                                             {
                                                               if (!_instance) lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, text);
                                                               else
                                                                 _instance->markError (reinterpret_cast<ID> (*element), text);
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (markNote,
                                                           void, (LumieraUid element, const char* text),
                                                             {
                                                               if (!_instance) lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, text);
                                                               else
                                                                 _instance->markNote (reinterpret_cast<ID> (*element), text);
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (mutate,
                                                           void, (LumieraUid element, void* diff),
                                                             {
                                                               if (!_instance) lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, "passing diff message");
                                                               else
                                                                 _instance->mutate (reinterpret_cast<ID> (*element), *reinterpret_cast<DiffMessage*> (diff));
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (triggerGuiShutdown,
                                                           void, (const char* cause),
                                                             {
                                                               if (!_instance) lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, cause);
                                                               else
                                                                 _instance->triggerGuiShutdown (cause);
                                                             }
                                                          )
                               );
    
    
    
  } // (END) facade implementation details
  
  
  
  
  /**
   * When started, NotificationService connects to the [UI-Bus](ui-bus.hpp) via
   * the provided connection. This is a simple, unidirectional up-link connection,
   * without actively adding NotificationService into the routing tables in [Nexus].
   * Yet this simple connection is sufficient to implement this service by talking
   * to other facilities within the UI layer.
   */
  NotificationService::NotificationService (ctrl::BusTerm& upLink, ctrl::UiManager& uiManager)
    : BusTerm{lib::idi::EntryID<NotificationService>{}, upLink}
    , dispatch_{new UiDispatcher{}}
    , uiManager_{uiManager}
    , implInstance_(this,_instance)
    , serviceInstance_( LUMIERA_INTERFACE_REF (lumieraorg_GuiNotification, 0,lumieraorg_GuiNotificationService))
  {
    INFO (gui, "GuiNotification Facade opened.");
  }
  
  
  NotificationService::~NotificationService() { } // emit dtors of embedded objects here...
  
  
} // namespace gui
