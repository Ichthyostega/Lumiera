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

#include <string>


namespace gui {
  
  using std::string;
  using util::cStr;
  
  class GuiNotificationFacade
    : public GuiNotification
    {
      void
      displayInfo (string const& text)
        {
          INFO (operate, "GUI: display '%s' as notification message.", cStr(text));
        }
      
      void
      triggerGuiShutdown (string const& cause)
        {
          NOTICE (operate, "GUI: shutdown triggered with explanation '%s'....", cStr(cause));
        }
    };
  
    
//////////////////////////////////////////////TODO: define an instance lumieraorg_guinotification_facade by forwarding to GuiNotificationFacade    
  
} // namespace gui
