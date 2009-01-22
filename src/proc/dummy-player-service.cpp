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

extern "C" {
#include "common/interfacedescriptor.h"
}

#include <string>
#include <boost/scoped_ptr.hpp>


namespace proc {

  using std::string;
  using lumiera::Subsys;
  using boost::scoped_ptr;
  
  
  namespace { // hidden local details of the service implementation....
  
    /** details of how the DummyPlayer service can be started
     *  and used as independent "subsystem" within main()  */
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
            //      Probably it's better design to manage the processes in a separate thread anyway...
            
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
                               , LUMIERA_INTERFACE_INLINE (name, "\305\162\202\240\075\316\146\100\314\152\075\343\372\065\226\307",
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "DummyPlayer"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (brief, "\317\045\366\076\064\072\156\274\220\346\262\207\062\367\057\232",
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "Proc Interface: dummy player to test integration with the GUI"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (homepage, "\136\225\033\362\161\251\300\256\117\072\171\102\235\004\235\200",
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "http://www.lumiera.org/develompent.html" ;}
                                                          )
                               , LUMIERA_INTERFACE_INLINE (version, "\212\146\344\127\124\116\101\205\211\174\322\241\162\122\023\165",
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "0.1~pre"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (author, "\064\226\072\300\054\345\042\357\337\226\155\025\306\051\117\105",
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "Hermann Vosseler"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (email, "\041\075\220\112\246\304\261\135\003\135\060\202\230\327\303\206",
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "Ichthyostega@web.de"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (copyright, "\232\305\163\271\174\025\270\075\012\201\331\256\327\375\066\210",
                                                           const char*, (LumieraInterface ifa),
                                                             {
                                                               (void)ifa;
                                                               return
                                                                 "Copyright (C)        Lumiera.org\n"
                                                                 "  2009               Hermann Vosseler <Ichthyostega@web.de>";
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (license, "\136\136\073\173\145\357\151\062\040\013\323\272\051\352\305\060",
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
                               , LUMIERA_INTERFACE_INLINE (state, "\224\251\004\001\165\140\116\246\126\311\115\234\023\026\331\350",
                                                           int, (LumieraInterface ifa),
                                                             {(void)ifa;  return LUMIERA_INTERFACE_EXPERIMENTAL; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (versioncmp, "\267\155\303\046\353\222\323\014\145\027\043\100\370\311\257\126",
                                                           int, (const char* a, const char* b),
                                                             {return 0;}  ////////////////////////////////////////////TODO define version ordering
                                                          )
                               );
    
    
    
    
    
    using lumiera::facade::LUMIERA_ERROR_FACADE_LIFECYCLE;
    typedef lib::SingletonRef<DummyPlayer>::Accessor InstanceRef;

    InstanceRef _instance; ///< a backdoor for the C Language impl to access the actual DummyPlayer implementation...
    
    typedef DummyPlayer::Process* ProcP;
    
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_DummyPlayer, 0
                               ,lumieraorg_DummyPlayerFacade
                               , LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_DummyPlayerFacade_descriptor)
                               , NULL /* on  open  */
                               , NULL /* on  close */
                               , LUMIERA_INTERFACE_INLINE (startPlay, "\143\323\102\155\051\006\235\004\037\310\354\121\176\142\342\210",
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
                               , LUMIERA_INTERFACE_INLINE (pausePlay, "\275\157\316\220\210\053\226\134\057\016\273\265\240\053\112\307",
                                                           void, (LumieraPlayProcess handle, bool doPlay),
                                                             { 
                                                               if (!_instance)
                                                                 { 
                                                                   lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, 0);
                                                                   return;
                                                                 }
                                                               
                                                               REQUIRE (handle);
                                                               ProcP proc = static_cast<ProcP> (handle);
                                                               
                                                               proc->pause(doPlay);
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (terminate, "\005\265\115\021\076\143\010\215\373\252\370\174\235\136\340\004",
                                                           void, (LumieraPlayProcess handle),
                                                             { 
                                                               if (!_instance)
                                                                 { 
                                                                   lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, 0);
                                                                   return;
                                                                 }
                                                               
                                                               REQUIRE (handle);
                                                               ProcP proc = static_cast<ProcP> (handle);
                                                               
                                                               UNIMPLEMENTED ("terminate a running playback process");
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (getFrame, "\230\130\101\300\047\065\170\052\226\164\026\112\150\166\074\134",
                                                           void *, (LumieraPlayProcess handle),
                                                             { 
                                                               //skipping full checks for performance reasons
                                                               REQUIRE (_instance && !lumiera_error_peek());
                                                               
                                                               REQUIRE (handle);
                                                               ProcP proc = static_cast<ProcP> (handle);
                                                               
                                                               return const_cast<void*> (proc->getFrame());
                                                             }
                                                          )
                               );



    
  } // (End) hidden service impl details
  
  
  
  
  DummyPlayerService::DummyPlayerService (Subsys::SigTerm terminationHandle)
    : error_("")
    , notifyTermination_(terminationHandle)
    , implInstance_(this,_instance)
    , serviceInstance_( LUMIERA_INTERFACE_REF (lumieraorg_DummyPlayer, 0, lumieraorg_DummyPlayerFacade))
  {
    INFO (operate, "DummyPlayer Facade opened.");
  }
  
  
  
  
  /** @internal intended for use by main(). */
  Subsys&
  DummyPlayer::getDescriptor()
  {
    return theDescriptor();
  }
  
  // emit the vtable here into this translation unit within liblumieraproc.so ...
  DummyPlayer::~DummyPlayer()      { }
  DummyPlayer::Process::~Process() { }
  
  
  
  DummyPlayer::Process& 
  DummyPlayerService::start()
    {
      REQUIRE (!theProcess_.isActive());
      theProcess_.setRate(25);
      
      return theProcess_;
    }
  
  
  
  void
  ProcessImpl::setRate (uint fps)
    {
      REQUIRE (fps==0 || fps_==0 );
      REQUIRE (fps==0 || !play_  );
      
      fps_ = fps;
      play_ = (fps != 0);
    }
  
  
  
  void
  ProcessImpl::pause(bool doPlay)
    {
      REQUIRE (isActive());
      play_ = doPlay;
    }
  
  
  
  void* const 
  ProcessImpl::getFrame()
    {
      REQUIRE (isActive());
      
      UNIMPLEMENTED ("actually deliver a frame");
    }


} // namespace proc
