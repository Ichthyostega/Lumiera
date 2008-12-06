/*
  GuiNotificationFacade  -  access point for pushing informations into the GUI
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "include/guinotificationfacade.h"
#include "common/singleton.hpp"
#include "include/nobugcfg.h"
#include "common/util.hpp"

extern "C" {
#include "lumiera/interfacedescriptor.h"
}

#include <string>


namespace gui {
  
  using std::string;
  using util::cStr;
  
  namespace { // facade implementation details
    
    
    class GuiNotificationFacade
      : public GuiNotification
      {
        void
        displayInfo (string const& text)
          {
            INFO (operate, "@GUI: display '%s' as notification message.", cStr(text));
          }
        
        void
        triggerGuiShutdown (string const& cause)
          {
            NOTICE (operate, "@GUI: shutdown triggered with explanation '%s'....", cStr(cause));
          }
      };
    
    
    /*=================== define an lumieraorg_guinotification instance ====================== */
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0
                               ,lumieraorg_GuiNotificationFacade_descriptor
                               , NULL, NULL, NULL
                               , LUMIERA_INTERFACE_INLINE (name, LUIDGEN,
                                                           const char*, (LumieraInterface iface),
                                                             { return "GuiNotification"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (brief, LUIDGEN,
                                                           const char*, (LumieraInterface iface),
                                                             { return "GUI Interface: push state update and notification of events into the GUI"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (homepage, LUIDGEN,
                                                           const char*, (LumieraInterface iface),
                                                             { return "http://www.lumiera.org/develompent.html" ;}
                                                          )
                               , LUMIERA_INTERFACE_INLINE (version, LUIDGEN,
                                                           const char*, (LumieraInterface iface),
                                                             { return "0.1~pre"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (author, LUIDGEN,
                                                           const char*, (LumieraInterface iface),
                                                             { return "Hermann Vosseler"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (email, LUIDGEN,
                                                           const char*, (LumieraInterface iface),
                                                             { return "Ichthyostega@web.de"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (copyright, LUIDGEN,
                                                           const char*, (LumieraInterface iface),
                                                             {
                                                               return
                                                                 "Copyright (C)        Lumiera.org\n"
                                                                 "  2008               Hermann Vosseler <Ichthyostega@web.de>";
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (license, LUIDGEN,
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
                               , LUMIERA_INTERFACE_INLINE (state, LUIDGEN,
                                                           int, (LumieraInterface iface),
                                                             {return LUMIERA_INTERFACE_EXPERIMENTAL; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (versioncmp, LUIDGEN,
                                                           int, (const char* a, const char* b),
                                                             {return 0;}  ////////////////////////////////////////////TODO
                                                          )
                               );
    
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_GuiNotification, 1
                               ,lumieraorg_GuiNotificationFacade
                               , LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_GuiNotificationFacade_descriptor)
                               , open_facade
                               , close_facade
                               , LUMIERA_INTERFACE_INLINE (displayInfo, LUIDGEN,
                                                           void, (const char* text),
                                                             { return _facade->displayInfo(text); }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (triggerGuiShutdown, LUIDGEN,
                                                           void, (const char* cause),
                                                             { return _facade->triggerGuiShutdown(cause); }
                                                          )
                               );
    
    
  } // (END) facade implementation details

} // namespace gui
