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
#include "common/singleton.hpp"
#include "lumiera/instancehandle.hpp"

extern "C" {
#include "lumiera/interface.h"
}

#include <boost/scoped_ptr.hpp>
#include <string>


namespace gui {
  
  using std::string;
  using boost::scoped_ptr;
  using lumiera::Subsys;
  
  /** interface of the GuiStarterPlugin */
  LUMIERA_INTERFACE_DECLARE (lumieraorg_Gui, 1
  );
  
  ///////////////////////////////////////////////////TODO: Placeholder
  LumieraInterface*
  getGuiStarterPlugin_InstanceDescriptor()
  {
    UNIMPLEMENTED ("implement the GuiStarterPlugin");
    return 0;
  }
  ///////////////////////////////////////////////////TODO: Placeholder
  
  
  struct GuiRunner
    : public GuiFacade
    {
      typedef lumiera::InstanceHandle<LUMIERA_INTERFACE_INAME(lumieraorg_Gui, 1)> GuiHandle;
      
      Subsys& guiSubsysHandle_;
      Subsys::SigTerm terminate_;
      
      GuiHandle theGUI_;
      
      
      GuiRunner (Subsys& handle, Subsys::SigTerm terminationSignal)
        : guiSubsysHandle_(handle),
          terminate_(terminationSignal),
          theGUI_(getGuiStarterPlugin_InstanceDescriptor())
        {
          ////TODO assert theGUI, i.e. implement a bool conversion there!
          TODO ("start gui thread, passing the terminationSignal");
        }
      
      ~GuiRunner ()
        {
          ////TODO any cleanup here?
        }
    };
  
  
  
  
  namespace { // implementation details
    
    scoped_ptr<GuiFacade> facade (0);
    
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
            UNIMPLEMENTED ("load and start the GUI and register shutdown hook");
            facade.reset (new GuiRunner (*this, termination));  /////////////////////TODO: actually decorate the termSignal, in order to delete the facade
            return false;
          }
        
        void
        triggerShutdown ()  throw()
          {
            UNIMPLEMENTED ("initiate closing the GUI");
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
