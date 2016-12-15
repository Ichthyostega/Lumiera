/*
  SessionCommandService  -  public service to invoke commands on the session

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file session-command-service.cpp
 ** Implementation of command invocation on the Session interface.
 ** This is the actual service implementation and runs within Session subsystem.
 ** 
 ** @todo implement a minimal version of a "Session subsystem" and instantiate SessionCommandService there ///////////TICKET #318
 ** 
 */

#include "proc/control/session-command-service.hpp"
#include "include/logging.h"
#include "lib/depend.hpp"
#include "lib/util.hpp"

extern "C" {
#include "common/interface-descriptor.h"
}

#include <string>




namespace proc {
namespace control {
  
  using std::string;
  using util::cStr;

  
  void 
  SessionCommandService::bla_TODO (string const& text)
  {
    INFO (gui, "@Session: display '%s' as notification message.", cStr(text));
    UNIMPLEMENTED ("do bla");  ////////////////////////TODO actually do something
  }
  
  
  void
  SessionCommandService::blubb_TODO (string const& cause)
  {
    NOTICE (gui, "@Session: shutdown triggered with explanation '%s'....", cStr(cause));
    UNIMPLEMENTED ("do blubb");  ////////////////////////TODO actually do something
  }
  
  
  namespace { // facade implementation details
    
    
    /* ================== define an lumieraorg_GuiNotification instance ======================= */
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0
                               ,lumieraorg_SessionCommandFacade_descriptor
                               , NULL, NULL, NULL
                               , LUMIERA_INTERFACE_INLINE (name,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "SessionCommand"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (brief,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "Session Interface: invoke pre-defined commands to operate on the session"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (homepage,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "http://www.lumiera.org/develompent.html" ;}
                                                          )
                               , LUMIERA_INTERFACE_INLINE (version,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "0.3~pre"; }
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
                                                                 "  2016               Hermann Vosseler <Ichthyostega@web.de>";
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
                                                             {return 0;}  ////////////////////////////////////////////TODO define version ordering
                                                          )
                               );
    
    
    
    
    
    using lumiera::facade::LUMIERA_ERROR_FACADE_LIFECYCLE;
    typedef lib::SingletonRef<SessionCommand>::Accessor InstanceRef;
    
    InstanceRef _instance; ///< a backdoor for the C Language impl to access the actual SessionCommand implementation...
    
    
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_SessionCommand, 0
                               ,lumieraorg_SessionCommandService
                               , LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_SessionCommandFacade_descriptor)
                               , NULL /* on  open  */
                               , NULL /* on  close */
                               , LUMIERA_INTERFACE_INLINE (bla_TODO,
                                                           void, (const char* text),
                                                             { 
                                                               if (!_instance) lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, text);
                                                               else
                                                                 _instance->bla_TODO(text);
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (blubb_TODO,
                                                           void, (const char* cause),
                                                             { 
                                                               if (!_instance) lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, cause);
                                                               else
                                                                 _instance->blubb_TODO(cause);
                                                             }
                                                          )
                               );
    
    
    
  } // (END) facade implementation details
  
  
  
  
  SessionCommandService::SessionCommandService  (CommandDispatch& dispatcherLoopInterface)
    : dispatcher_{dispatcherLoopInterface}
    , implInstance_{this,_instance}
    , serviceInstance_{ LUMIERA_INTERFACE_REF (lumieraorg_SessionCommand, 0, lumieraorg_SessionCommandService)}
  {
    INFO (gui, "SessionCommand Facade opened.");
  }
  
  
  
  
}} // namespace proc::control
