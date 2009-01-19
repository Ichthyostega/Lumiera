/*
  DummyPlayerService  -  access point and service implementing a dummy test player
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "proc/dummy-player-service.hpp"
#include "lib/singleton.hpp"

#include <string>
#include <boost/scoped_ptr.hpp>


namespace proc {

  using std::string;
  using lumiera::Subsys;
  using boost::scoped_ptr;
  
  
  namespace { // hidden local details of the service implementation....
  
    class DummyPlayerSubsysDescriptor
      : public Subsys
      {
        operator string ()  const { return "Dummy-Player"; }
        
        bool 
        shouldStart (lumiera::Option&)
          {
            return false; // for now the DummyPlayerService only comes "up" as dependency,
          }              //  but doesn't start as a subsystem on it's own.
        
        bool
        start (lumiera::Option&, Subsys::SigTerm terminationHandle)
          {
            ASSERT (!thePlayer_);
            
            thePlayer_.reset (new DummyPlayerService (terminationHandle));
            return true;
          }
        
        /** manages the actual (single) instance of the player service impl */
        scoped_ptr<DummyPlayerService> thePlayer_;
        
        
        void
        triggerShutdown ()  throw()
          {
            TODO ("implement waiting for any playback processes to terminate gracefully");
            //..... but this would require us to use a separate thread, so I skip it for now.
            //      Probably it's better design to mange the processes in a separate thread anyway...
            
            thePlayer_.reset(0);
          }
        
        bool 
        checkRunningState ()  throw()
          {
            //note: not locking here...
            return (thePlayer_);
          }
      };
  
    lumiera::Singleton<DummyPlayerSubsysDescriptor> theDescriptor;
    
    
    
    
    
    /* ================== define an lumieraorg_GuiNotification instance ======================= */
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0
                               ,lumieraorg_DummyPlayerFacade_descriptor
                               , NULL, NULL, NULL
                               , LUMIERA_INTERFACE_INLINE (name, LUIDGEN,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "DummyPlayer"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (brief, LUIDGEN,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "Proc Interface: dummy player to test integration with the GUI"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (homepage, LUIDGEN,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "http://www.lumiera.org/develompent.html" ;}
                                                          )
                               , LUMIERA_INTERFACE_INLINE (version, LUIDGEN,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "0.1~pre"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (author, LUIDGEN,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "Hermann Vosseler"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (email, LUIDGEN,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "Ichthyostega@web.de"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (copyright, LUIDGEN,
                                                           const char*, (LumieraInterface ifa),
                                                             {
                                                               (void)ifa;
                                                               return
                                                                 "Copyright (C)        Lumiera.org\n"
                                                                 "  2009               Hermann Vosseler <Ichthyostega@web.de>";
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (license, LUIDGEN,
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
                               , LUMIERA_INTERFACE_INLINE (state, LUIDGEN,
                                                           int, (LumieraInterface ifa),
                                                             {(void)ifa;  return LUMIERA_INTERFACE_EXPERIMENTAL; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (versioncmp, LUIDGEN,
                                                           int, (const char* a, const char* b),
                                                             {return 0;}  ////////////////////////////////////////////TODO define version ordering
                                                          )
                               );
    
    
    
    
    
    using lumiera::facade::LUMIERA_ERROR_FACADE_LIFECYCLE;
    typedef lib::SingletonRef<GuiNotification>::Accessor InstanceRef;

    InstanceRef _instance; ///< a backdoor for the C Language impl to access the actual GuiNotification implementation...
    
    typedef DummyPlayer::Process* ProcP;
    
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_DummyPlayer, 0
                               ,lumieraorg_DummyPlayerFacade
                               , LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_DummyPlayerFacade_descriptor)
                               , NULL /* on  open  */
                               , NULL /* on  close */
                               , LUMIERA_INTERFACE_INLINE (startPlay, LUIDGEN,
                                                           LumieraPlayProcess, (void),
                                                             { 
                                                               if (!_instance)
                                                                 { 
                                                                   lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, 0);
                                                                   return 0;
                                                                 }
                                                               
                                                               return static_cast<LumieraPlayProcess> (& (_instance->start())); 
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (pausePlay, LUIDGEN,
                                                           void, (LumieraPlayProcess handle, bool doPlay),
                                                             { 
                                                               if (!_instance)
                                                                 { 
                                                                   lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, 0);
                                                                   return;
                                                                 }
                                                               
                                                               REQUIRE (handle);
                                                               ProcP proc = dynamic_cast<ProcP> (handle);
                                                               ASSERT (proc);
                                                               
                                                               proc->pause(doPlay);
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (terminate, LUIDGEN,
                                                           void, (LumieraPlayProcess handle),
                                                             { 
                                                               if (!_instance)
                                                                 { 
                                                                   lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, 0);
                                                                   return;
                                                                 }
                                                               
                                                               REQUIRE (handle);
                                                               ProcP proc = dynamic_cast<ProcP> (handle);
                                                               ASSERT (proc);
                                                               
                                                               UNIMPLEMENTED ("terminate a running playback process");
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (getFrame, LUIDGEN,
                                                           void *, (LumieraPlayProcess handle),
                                                             { 
                                                               if (!_instance)
                                                                 { 
                                                                   lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, 0);
                                                                   return 0;
                                                                 }
                                                               
                                                               REQUIRE (handle);
                                                               ProcP proc = dynamic_cast<ProcP> (handle);
                                                               ASSERT (proc);
                                                               
                                                               return const_cast<void*> (proc->getFrame());
                                                             }
                                                          )
                               );



    
  } // (End) hidden service impl details
  
  
  
  
  DummyPlayerService::DummyPlayerService (Subsys::SigTerm terminationHandle)
    : notifyTermination_(terminationHandle)
    , implInstance_(this,_instance)
    , serviceInstance_( LUMIERA_INTERFACE_REF (lumieraorg_DummyPlayer, 0, lumieraorg_DummyPlayerFacade))
  {
    INFO (operate, "GuiNotification Facade opened.");
  }
  
  
  
  
  /** @internal intended for use by main(). */
  Subsys&
  DummyPlayer::getDescriptor()
  {
    return theDescriptor();
  }



} // namespace proc
