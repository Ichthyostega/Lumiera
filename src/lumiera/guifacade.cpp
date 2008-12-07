/*
  GuiFacade  -  access point for communicating with the Lumiera GTK GUI
 
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


#include "gui/guifacade.hpp"
#include "include/guinotificationfacade.h"
#include "include/error.hpp"
#include "common/singleton.hpp"
#include "lumiera/instancehandle.hpp"

#include <boost/scoped_ptr.hpp>
#include <string>


namespace gui {
  
  using std::string;
  using boost::scoped_ptr;
  using lumiera::Subsys;
  using lumiera::InstanceHandle;
  
  
  
  
  struct GuiRunner
    : public GuiFacade
    {
      typedef InstanceHandle<LUMIERA_INTERFACE_INAME(lumieraorg_Gui, 1)> GuiHandle;
      
      GuiHandle theGUI_;
      
      
      GuiRunner (Subsys::SigTerm terminationHandle)
        : theGUI_("lumieraorg_Gui", 1, 1, "lumieraorg_GuiStarterPlugin") // load GuiStarterPlugin
        {
          ASSERT (theGUI_);
          if (!kickOff (terminationHandle))
            throw lumiera::error::Fatal("failed to bring up GUI",lumiera_error());
        }
      
      ~GuiRunner () {  }
      
      
      bool kickOff (Subsys::SigTerm& terminationHandle) 
        { 
          return theGUI_->kickOff (reinterpret_cast<void*> (&terminationHandle))
              && !lumiera_error_peek();
        }
    };
  
  
  
  
  namespace { // implementation details : start GUI through GuiStarterPlugin
    
    scoped_ptr<GuiRunner> facade (0);
    
    class GuiSubsysDescriptor
      : public lumiera::Subsys
      {
        operator string ()  const { return "Lumiera GTK GUI"; }
        
        bool 
        shouldStart (lumiera::Option&)
          {
            UNIMPLEMENTED ("determine, if a GUI is needed");
            return false;
          }
        
        bool
        start (lumiera::Option&, Subsys::SigTerm termination)
          {
            facade.reset (new GuiRunner (termination));  /////////////////////TODO: actually decorate the termSignal, in order to delete the facade
            return true;
          }
        
        void
        triggerShutdown ()  throw()
          {
            try { GuiNotification::facade().triggerGuiShutdown ("Application shutdown"); }
            
            catch (...){}
          }
        
      };
    
    lumiera::Singleton<GuiSubsysDescriptor> theDescriptor;
    
  } // (End) impl details
  
  
  
  
  
  
  /** @internal intended for use by main(). */
  lumiera::Subsys&
  GuiFacade::getDescriptor()
  {
    return theDescriptor();
  }
  
  
  bool
  GuiFacade::isUp ()
  {
    return (facade);
  }



} // namespace gui
