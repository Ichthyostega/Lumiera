/*
  GuiFacade  -  access point for communicating with the Lumiera GTK GUI

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


#include "gui/guifacade.hpp"
#include "include/gui-notification-facade.h"
#include "lib/sync.hpp"
#include "lib/error.hpp"
#include "lib/depend.hpp"
#include "lib/functor-util.hpp"
#include "common/instancehandle.hpp"

#include <boost/scoped_ptr.hpp>
#include <functional>
#include <string>


namespace gui {
  
  using std::string;
  using boost::scoped_ptr;
  using std::bind;
  using std::placeholders::_1;
  using lumiera::Subsys;
  using lumiera::InstanceHandle;
  using util::dispatchSequenced;
  using lib::Sync;
  using lib::RecursiveLock_NoWait;
  
  
  
  /** load and start the GUI as a plugin */
  struct GuiRunner
    : public GuiFacade
    {
      typedef InstanceHandle<LUMIERA_INTERFACE_INAME(lumieraorg_Gui, 1)> GuiHandle;
      
      GuiHandle theGUI_;
      
      
      GuiRunner (Subsys::SigTerm terminationHandle)
        : theGUI_("lumieraorg_Gui", 1, 1, "lumieraorg_GuiStarterPlugin") // load GuiStarterPlugin
        {
          ASSERT (theGUI_);
          bool res = this->launchUI (terminationHandle);
          
          if (!res || lumiera_error_peek())
            throw lumiera::error::Fatal("failed to bring up GUI",lumiera_error());
        }
      
      ~GuiRunner () {  }
      
      
      bool launchUI (Subsys::SigTerm& terminationHandle)  override
        { 
          return theGUI_->launchUI (reinterpret_cast<void*> (&terminationHandle));
        }
    };
  
  
  
  
  namespace { // implementation of GUI-"Subsystem" : start GUI through GuiStarterPlugin
    
    scoped_ptr<GuiRunner> facade (0);
    
    
    class GuiSubsysDescriptor
      : public lumiera::Subsys,
        public Sync<RecursiveLock_NoWait>
      {
        operator string ()  const { return "Lumiera GTK GUI"; }
        
        bool 
        shouldStart (lumiera::Option& opts)  override
          {
            if (opts.isHeadless() or 0 < opts.getPort())
              {
                INFO (guifacade, "*not* starting the GUI...");
                return false;
              }
            else
              return true;
          }
        
        bool
        start (lumiera::Option&, Subsys::SigTerm termination)  override
          {
            Lock guard (this);
            if (facade) return false; // already started
            
            facade.reset (
              new GuiRunner (                            // trigger loading load the GuiStarterPlugin...
                dispatchSequenced( closeOnTermination_  //  on termination call this->closeGuiModule(*) first
                                 , termination)));     //...followed by invoking the given termSignal
            return true;
          }
        
        /**
         * @warning there is a possible race here,
         *  when shutdown is triggered before the GUI was able to open the GuiNotification interface.
         *  However, the Lumiera thread handling wrapper/framework ensures that a new thread actually
         *  starts to execute (and picks up the arguments), prior to returning from the thread starting
         *  function. For this reason, it is rather unlikely this race actually happens in practice,
         *  since opening the GuiNotification interface is done early, while starting the UI-Bus.
         * @todo                                      ////////////////////////////////////////////////////////TICKET #1048 : use Thread.sync (barrier) to work around this!
         */
        void
        triggerShutdown ()  noexcept override
          {
            try { GuiNotification::facade().triggerGuiShutdown ("Application shutdown"); }
            
            ERROR_LOG_AND_IGNORE (guifacade, "trigger shutdown of the GUI");
          }
        
        bool 
        checkRunningState ()  noexcept override
          {
            return bool(facade);
          }
        
        
        void
        closeGuiModule (std::string *)
          {
            Lock guard (this);
            if (!facade)
              {
                WARN (guifacade, "Termination signal invoked, but GUI is currently closed. "
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
                WARN (guifacade, "GUI subsystem terminates, but GuiFacade isn't properly closed. "
                                 "Closing it forcedly; this indicates broken startup logic and should be fixed.");
                try { facade.reset (0); }
                ERROR_LOG_AND_IGNORE (guifacade, "forcibly closing the GUI");
                ENSURE (not lumiera_error_peek());
              }
          }
      };
    
    lib::Depend<GuiSubsysDescriptor> theDescriptor;
    
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
    return bool(facade);
  }



} // namespace gui

/*
// Local Variables:
// mode: C++
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
