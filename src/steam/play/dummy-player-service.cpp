/*
  DummyPlayerService  -  access point and service implementing a dummy test player

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file dummy-player-service.cpp
 ** Implementation of a service for mock render output to support testing.
 */


#include "steam/play/dummy-player-service.hpp"
#include "steam/engine/worker/dummy-image-generator.hpp"
#include "steam/engine/worker/tick-service.hpp"
#include "lib/depend-inject.hpp"
#include "lib/depend.hpp"

extern "C" {
#include "common/interface-descriptor.h"
}

#include <functional>
#include <memory>
#include <string>



namespace steam  {
  namespace play{
  
    using std::string;
    using lumiera::Subsys;
    using std::unique_ptr;
    using std::bind;
    
    
    
    namespace { // hidden local details of the service implementation....
    
      /** details of how the DummyPlayer service can be started
       *  and used as independent "subsystem" within main()  */
      class DummyPlayerSubsysDescriptor
        : public Subsys
        {
          using ServiceHandle = lib::DependInject<DummyPlayerService>::ServiceInstance<>;
          
          /** manages the actual (single) instance of the player service impl */
          ServiceHandle thePlayer_{ServiceHandle::NOT_YET_STARTED};
          
          
          operator string ()  const { return "Dummy-Player"; }
          
          
          bool 
          shouldStart (lumiera::Option&)  override
            {
              return false; // for now the DummyPlayerService only comes "up" as dependency,
            }              //  but doesn't start as a subsystem on it's own.
          
          bool
          start (lumiera::Option&, Subsys::SigTerm terminationHandle)  override
            {
              ASSERT (!thePlayer_);
              
              thePlayer_.createInstance (terminationHandle);
              return true;
            }
          
          void
          triggerShutdown ()  noexcept override
            {
              thePlayer_.shutdown();
              // note: shutdown of the DummyPlayerService instance may block
             //        for a short period, until termination of all tick services
            }
          
          bool 
          checkRunningState ()  noexcept override
            {
              return bool(thePlayer_);
            }
        };
      
      lib::Depend<DummyPlayerSubsysDescriptor> theDummyPlayerDescriptor;
      
      
      
      
      
      /* ================== define an lumieraorg_DummyPlayer instance ======================= */
      
      LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0
                                 ,lumieraorg_DummyPlayerFacade_descriptor
                                 , NULL, NULL, NULL
                                 , LUMIERA_INTERFACE_INLINE (name,
                                                             const char*, (LumieraInterface ifa),
                                                               { (void)ifa;  return "DummyPlayer"; }
                                                            )
                                 , LUMIERA_INTERFACE_INLINE (brief,
                                                             const char*, (LumieraInterface ifa),
                                                               { (void)ifa;  return "Steam Interface: dummy player to test integration with the GUI"; }
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
                                                                   "  2009               Hermann Vosseler <Ichthyostega@web.de>";
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
      
      
      
      
      
      using LERR_(LIFECYCLE);
      lib::Depend<DummyPlayerService> _instance; ///< a backdoor for the C Language impl to access the actual SessionCommand implementation...
      
      typedef ProcessImpl* ProcP;
      
      
      LUMIERA_INTERFACE_INSTANCE (lumieraorg_DummyPlayer, 0
                                 ,lumieraorg_DummyPlayerService
                                 , LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_DummyPlayerFacade_descriptor)
                                 , NULL /* on  open  */
                                 , NULL /* on  close */
                                 , LUMIERA_INTERFACE_INLINE (startPlay,
                                                             LumieraPlayProcess, (LumieraDisplaySlot viewerHandle),
                                                               { 
                                                                 if (!_instance)
                                                                   { 
                                                                     lumiera_error_set (LUMIERA_ERROR_LIFECYCLE, 0);
                                                                     return 0;
                                                                   }
                                                                 
                                                                 return static_cast<LumieraPlayProcess> (_instance().start(viewerHandle)); 
                                                               }
                                                            )
                                 , LUMIERA_INTERFACE_INLINE (togglePlay,
                                                             void, (LumieraPlayProcess handle, bool doPlay),
                                                               { 
                                                                 if (!_instance)
                                                                   { 
                                                                     lumiera_error_set(LUMIERA_ERROR_LIFECYCLE, 0);
                                                                     return;
                                                                   }
                                                                 
                                                                 REQUIRE (handle);
                                                                 ProcP proc = static_cast<ProcP> (handle);
                                                                 
                                                                 proc->doPlay(doPlay);
                                                               }
                                                            )
                                 , LUMIERA_INTERFACE_INLINE (terminate,
                                                             void, (LumieraPlayProcess handle),
                                                               { 
                                                                 if (!_instance)
                                                                   { 
                                                                     lumiera_error_set(LUMIERA_ERROR_LIFECYCLE, 0);
                                                                     return;
                                                                   }
                                                                 
                                                                 REQUIRE (handle);
                                                                 ProcP proc = static_cast<ProcP> (handle);
                                                                 
                                                                 ProcessImpl::terminate (proc);
                                                               }
                                                            )
                                 );
      
      
      
      
    } // (End) hidden service impl details
    
    
    
    
    DummyPlayerService::DummyPlayerService (Subsys::SigTerm terminationHandle)
      : error_("")
      , notifyTermination_(terminationHandle)
      , serviceInstance_( LUMIERA_INTERFACE_REF (lumieraorg_DummyPlayer, 0, lumieraorg_DummyPlayerService))
      {
        INFO (progress, "DummyPlayer Facade opened.");
      }
    
    
    
    
    /** @par implementation note
     *  A new process (implementation) is created, configured
     *  and started here. This may include spawning a thread or
     *  allocating a timer. The newly created process is self-contained
     *  and will be just handed out, without caring for its lifecycle.
     *  If client code accesses this function via the plain C interface,
     *  the client is responsible for terminating this process, whereas
     *  when using the C++ interface, you'll get a Handle object which
     *  manages the lifecycle automatically.
     */
    ProcessImpl*
    DummyPlayerService::start (LumieraDisplaySlot viewerHandle)
      {
        unique_ptr<ProcessImpl> newProcess (new ProcessImpl (viewerHandle));

        REQUIRE (!newProcess->isActive());
        newProcess->setRate(25);
        
        return newProcess.release();
      }
    
    
    
    
    
    
    /* === Process Implementation === */
    
    
    ProcessImpl::ProcessImpl(LumieraDisplaySlot viewerHandle)
      : fps_{0}
      , play_{false}
      , display_{Display::facade().getHandle (viewerHandle)}
      , imageGen_{}
      , tick_{new TickService (bind (&ProcessImpl::doFrame, this))}
      { }
    
    
    ProcessImpl::~ProcessImpl()
    {
      INFO (proc_dbg, "Playback process halted...");
    }
    
    
    /** deleter function for lib::Handle */
    void
    ProcessImpl::terminate (DummyPlayer::ProcessImplementationLink* process)
    {
      if (process)
        delete process;
    }
    
    
    /**
     * activate a forwarding smart-Handle
     * to be used by client code for communication with the play process implementation.
     * The handle manages the lifecycle of the interface / play process connection.
     * This virtual function is used by the interface proxy to connect the client side
     * and the actual play process, after creating the latter through the interface system.
     */
    DummyPlayer::Process
    ProcessImpl::createHandle()
    {
      DummyPlayer::Process handle;
      DummyPlayer::ProcessImplementationLink *const implementationLink = this;
      handle.activate(implementationLink, &terminate);
      return handle;                    // note the deleter function...
    }
    
    
    
    void
    ProcessImpl::setRate (uint fps)
      {
        REQUIRE (fps==0 || fps_==0 );
        REQUIRE (fps==0 || !play_  );
        REQUIRE (tick_);
        
        fps_ = fps;
        play_ = (fps != 0);
        
        if (play_)
          imageGen_.reset(new DummyImageGenerator(fps));
        
        // callbacks with given frequency, starting now
        tick_->activate(fps); 
      }
    
    
    
    void
    ProcessImpl::doPlay(bool yes)
      {
        REQUIRE (isActive());
        tick_->activate (yes? fps_:0);
        play_ = yes;
      }
    
    
    
    void 
    ProcessImpl::doFrame()
      {
        REQUIRE (isActive());
        ASSERT (imageGen_);
        
        if (play_)
          display_(imageGen_->next());
        else
          display_(imageGen_->current());
      }
  
  
  
  } // namespace play
  
} // namespace steam





namespace lumiera {  
  
  /** @internal intended for use by main(). */
  lumiera::Subsys&
  DummyPlayer::getDescriptor()
  {
    return steam::play::theDummyPlayerDescriptor();
  }
  
  
} // namespace lumiera
