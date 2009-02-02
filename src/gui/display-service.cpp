/*
  DisplayService  -  service providing access to a display for outputting frames
 
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


#include "gui/display-service.hpp"
//#include "lib/singleton.hpp"

extern "C" {
#include "common/interfacedescriptor.h"
}

#include <string>
//#include <memory>
//#include <boost/scoped_ptr.hpp>


namespace gui {
  
  using std::string;
//    using boost::scoped_ptr;
  
  
  namespace { // hidden local details of the service implementation....
    
    
    
    /* ================== define an lumieraorg_GuiNotification instance ======================= */
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0
                               ,lumieraorg_DisplayFacade_descriptor
                               , NULL, NULL, NULL
                               , LUMIERA_INTERFACE_INLINE (name, LUIDGEN,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "Display"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (brief, LUIDGEN,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "UI Interface: service for outputting frames to a viewer or display"; }
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
    typedef lib::SingletonRef<DisplayService>::Accessor InstanceRef;
    
    InstanceRef _instance; ///< a backdoor for the C Language impl to access the actual DummyPlayer implementation...
    
    typedef ProcessImpl* ProcP;   ///////////////TODO
    
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_Display, 0
                               ,lumieraorg_DisplayService
                               , LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_DisplayFacade_descriptor)
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
                                                               
                                                               return static_cast<LumieraPlayProcess> (_instance->start()); 
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (togglePlay, LUIDGEN,
                                                           void, (LumieraPlayProcess handle, bool doPlay),
                                                             { 
                                                               if (!_instance)
                                                                 { 
                                                                   lumiera_error_set(LUMIERA_ERROR_FACADE_LIFECYCLE, 0);
                                                                   return;
                                                                 }
                                                               
                                                               REQUIRE (handle);
                                                               ProcP proc = static_cast<ProcP> (handle);
                                                               
                                                               proc->doPlay(doPlay);
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
                                                               ProcP proc = static_cast<ProcP> (handle);
                                                               
                                                               ProcessImpl::terminate (proc);
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (getFrame, LUIDGEN,
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
  
  
  
  
  DisplayService::DisplayService()
    : error_("")
    , implInstance_(this,_instance)
    , serviceInstance_( LUMIERA_INTERFACE_REF (lumieraorg_Display, 0, lumieraorg_DisplayService))
  {
    INFO (progress, "Display Facade opened.");
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
  DisplayService::start()
    {
      auto_ptr<ProcessImpl> newProcess (new ProcessImpl);

      REQUIRE (!newProcess->isActive());
      newProcess->setRate(25);
      
      return newProcess.release();
    }
  
  
  /* === Forwarding functions on the Process handle === */
  
  void DummyPlayer::Process::play(bool yes)    { impl().doPlay(yes);       }
  void* const DummyPlayer::Process::getFrame() { return impl().getFrame(); }
  
  
  
  
  
  /* === DisplayerSlot Implementation === */
  
  
  DisplayerSlot::DisplayerSlot (FrameDestination const& outputDestination)
    : currBuffer_(0)
  { 
    hasFrame_.connect (outputDestination);
    dispatcher_.connect (sigc::mem_fun (this, &DisplayerSlot::displayCurrentFrame));
  }
  
  
  void
  DisplayerSlot::displayCurrentFrame()
  {
    hasFrame_.emit (currentBuffer_);
  }
  
  
  DummyPlayer::Process
  ProcessImpl::createHandle()
  {
    DummyPlayer::Process handle;
    handle.activate(this, &terminate);
    return handle;
  }
  
  
  void
  ProcessImpl::terminate (ProcessImpl* process)
  {
    if (process)
      delete process;
  }
  
  
  
  void
  ProcessImpl::setRate (uint fps)
    {
      REQUIRE (fps==0 || fps_==0 );
      REQUIRE (fps==0 || !play_  );
      
      fps_ = fps;
      play_ = (fps != 0);
      
      if (play_)
        imageGen_.reset(new DummyImageGenerator(fps));
    }
  
  
  
  void
  ProcessImpl::doPlay(bool yes)
    {
      REQUIRE (isActive());
      play_ = yes;
    }
  
  
  
  void* const 
  ProcessImpl::getFrame()
    {
      REQUIRE (isActive());
      ASSERT (imageGen_);
      
      if (play_)
        return imageGen_->next();
      else
        return imageGen_->current();
    }
  
  
  



  
  // emit the vtable here into this translation unit within liblumieraproc.so ...
  DummyPlayer::~DummyPlayer()      { }



} // namespace proc
