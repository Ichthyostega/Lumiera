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
    
    
    struct GuiNotificationFacade
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
    
    lumiera::Singleton<GuiNotificationFacade> _facade;
    
    
    
    
    /* ================== define an lumieraorg_guinotification instance ======================= */
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0
                               ,lumieraorg_GuiNotificationFacade_descriptor
                               , NULL, NULL, NULL
                               , LUMIERA_INTERFACE_INLINE (name, "\044\117\156\365\344\056\362\220\166\350\320\214\115\221\302\177",
                                                           const char*, (LumieraInterface iface),
                                                             { return "GuiNotification"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (brief, "\160\240\102\325\175\145\270\140\350\241\163\303\331\343\253\142",
                                                           const char*, (LumieraInterface iface),
                                                             { return "GUI Interface: push state update and notification of events into the GUI"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (homepage, "\217\232\066\101\042\116\054\217\070\233\253\241\166\145\234\133",
                                                           const char*, (LumieraInterface iface),
                                                             { return "http://www.lumiera.org/develompent.html" ;}
                                                          )
                               , LUMIERA_INTERFACE_INLINE (version, "\350\365\121\052\037\022\300\021\171\357\017\367\270\071\266\376",
                                                           const char*, (LumieraInterface iface),
                                                             { return "0.1~pre"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (author, "\117\007\006\234\153\206\000\344\303\043\027\261\045\320\166\133",
                                                           const char*, (LumieraInterface iface),
                                                             { return "Hermann Vosseler"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (email, "\356\243\022\137\345\275\016\034\337\144\031\260\303\050\140\327",
                                                           const char*, (LumieraInterface iface),
                                                             { return "Ichthyostega@web.de"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (copyright, "\365\220\260\051\267\345\330\046\145\134\331\320\115\157\332\356",
                                                           const char*, (LumieraInterface iface),
                                                             {
                                                               return
                                                                 "Copyright (C)        Lumiera.org\n"
                                                                 "  2008               Hermann Vosseler <Ichthyostega@web.de>";
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (license, "\155\335\361\364\221\012\115\325\306\046\153\152\002\117\075\077",
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
                               , LUMIERA_INTERFACE_INLINE (state, "\227\170\230\144\310\330\131\175\367\152\327\324\113\275\223\245",
                                                           int, (LumieraInterface iface),
                                                             {return LUMIERA_INTERFACE_EXPERIMENTAL; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (versioncmp, "\253\066\335\233\025\161\135\347\377\156\121\135\347\313\130\014",
                                                           int, (const char* a, const char* b),
                                                             {return 0;}  ////////////////////////////////////////////TODO
                                                          )
                               );
    
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_GuiNotification, 1
                               ,lumieraorg_GuiNotificationFacade
                               , LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_GuiNotificationFacade_descriptor)
                               , NULL /* on  open  */
                               , NULL /* on  close */
                               , LUMIERA_INTERFACE_INLINE (displayInfo, "\366\075\213\163\207\040\221\233\010\366\174\374\317\126\331\205",
                                                           void, (const char* text),
                                                             { return _facade().displayInfo(text); }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (triggerGuiShutdown, "\267\043\244\065\107\314\370\175\063\330\264\257\302\146\326\303",
                                                           void, (const char* cause),
                                                             { return _facade().triggerGuiShutdown(cause); }
                                                          )
                               );
    
    
  } // (END) facade implementation details

} // namespace gui
