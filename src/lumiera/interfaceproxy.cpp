/*
  InterfaceProxy  -  definition of forwarding proxies for the facade interfaces
 
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


#include "common/singletonsubclass.hpp"
#include "include/guinotificationfacade.h"
#include "common/util.hpp"
#include "include/error.hpp"

extern "C" {
#include "lib/interface.h"
}

using util::cStr;


namespace singleton = lumiera::singleton;

namespace gui {

  class GuiNotificationInterfaceProxy
    : public GuiNotification
    {
      LUMIERA_INTERFACE_INAME(lumieraorg_GuiNotification, 1) * interface_;
      
      GuiNotificationInterfaceProxy ()
        {
          interface_ = LUMIERA_INTERFACE_OPEN (lumieraorg_GuiNotification, 1, 2, lumieraorg_GuiNotificationFacade);
          if (!interface_)
            throw lumiera::error::State ("unable to access GuiNotificationFacade");
        }
      
      friend class singleton::StaticCreate<GuiNotificationInterfaceProxy>;
      
      
      
      /* ======== forwarding through interface ========== */
      
      void
      displayInfo (string const& text)
        {
          interface_->displayInfo (cStr(text));
        }
      
      void
      triggerGuiShutdown (string const& cause)
        {
          interface_->triggerGuiShutdown (cStr(cause));
        }
      
      
    };
  
  namespace {
  
    singleton::UseSubclass<GuiNotificationInterfaceProxy> typeinfo_proxyInstance_to_create;
  }
  
  /** storage for the facade proxy factory used by client code to invoke through the interface */
  lumiera::SingletonSub<GuiNotification> GuiNotification::facade (typeinfo_proxyInstance_to_create);
  
  ///////////////////////////////////////TODO: this solution is not correct, because it doesn't detect when the interface is shut down!
  


} // namespace gui
