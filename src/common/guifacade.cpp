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
#include "lib/sync.hpp"
#include "lib/error.hpp"
#include "lib/singleton.hpp"
#include "lib/functorutil.hpp"
#include "common/instancehandle.hpp"

#include <boost/scoped_ptr.hpp>
#include <tr1/functional>
#include <string>


namespace gui {
  
  using std::string;
  using boost::scoped_ptr;
  using std::tr1::bind;
  using std::tr1::placeholders::_1;
  using lumiera::Subsys;
  using lumiera::InstanceHandle;
  using util::dispatchSequenced;
  using lib::Sync;
  using lib::RecursiveLock_NoWait;
  
  
  
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
      : public lumiera::Subsys,
        public Sync<RecursiveLock_NoWait>
      {
        operator string ()  const { return "Lumiera GTK GUI"; }
        
        bool 
        shouldStart (lumiera::Option& opts)
          {
            if (opts.isHeadless() || 0 < opts.getPort())
              {
                INFO (lumiera, "*not* starting the GUI...");
                return false;
              }
            else
              return true;
          }
        
        bool
        start (lumiera::Option&, Subsys::SigTerm termination)
          {
            Lock guard (this);
            if (facade) return false; // already started
            
            facade.reset (
              new GuiRunner (                            // trigger loading load the GuiStarterPlugin...
                dispatchSequenced( closeOnTermination_  //  on termination call this->closeGuiModule(*) first
                                 , termination)));     //...followed by invoking the given termSignal
            return true;
          }
        
        void
        triggerShutdown ()  throw()
          {
            try { GuiNotification::facade().triggerGuiShutdown ("Application shutdown"); }
            
            catch (...){}
          }
        
        bool 
        checkRunningState ()  throw()
          {
            Lock guard (this);
            return (facade);
          }
        
        void
        closeGuiModule (lumiera::Error *)
          {
            Lock guard (this);
            if (!facade)
              {
                WARN (operate, "Termination signal invoked, but GUI is currently closed. "
                               "Probably this is due to some broken startup logic and should be fixed.");
              }
            else
              facade.reset (0);
          }
        
        
        Subsys::SigTerm closeOnTermination_;
        
      public:
        GuiSubsysDescriptor()
          : closeOnTermination_ (bind (&GuiSubsysDescriptor::closeGuiModule, this, _1))
          { }
        
        ~GuiSubsysDescriptor()
          {
            if (facade)
              {
                WARN (operate, "GUI subsystem terminates, but GuiFacade isn't properly closed. "
                               "Closing it forcedly; this indicates broken startup logic and should be fixed.");
                try { facade.reset (0); }
                catch(...) { WARN_IF (lumiera_error_peek(), operate, "Ignoring error: %s", lumiera_error()); }
              }
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
