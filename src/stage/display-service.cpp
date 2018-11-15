/*
  DisplayService  -  service providing access to a display for outputting frames

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


/** @file display-service.cpp
 ** Implementation of _Displayer Service,_ exposed as a public interface.
 ** This service allows to set up _display slots,_ which can be handed over
 ** to client code in the course of the play process for outputting frames.
 ** 
 ** @deprecated This is a first draft as of 1/2009, and likely to be superseded
 **  by a better design, where rather the \em provider of an output facility
 **  registers with the OutputManager in the core.
 */


#include "stage/display-service.hpp"
#include "lib/depend.hpp"

extern "C" {
#include "common/interface-descriptor.h"
}


namespace stage {
  
  
  
  namespace { // hidden local details of the service implementation....
    
    
    
    /* ================== define an lumieraorg_Display instance ======================= */
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0
                               ,lumieraorg_DisplayFacade_descriptor
                               , NULL, NULL, NULL
                               , LUMIERA_INTERFACE_INLINE (name,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "Display"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (brief,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "UI Interface: service for outputting frames to a viewer or display"; }
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
    
    
    
    
    
    using lumiera::error::LERR_(LIFECYCLE);
    
    lib::Depend<DisplayService> _instance; ///< a backdoor for the C Language impl to access the actual SessionCommand implementation...
    
    
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_Display, 0
                               ,lumieraorg_DisplayService
                               , LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_DisplayFacade_descriptor)
                               , NULL /* on  open  */
                               , NULL /* on  close */
                               , LUMIERA_INTERFACE_INLINE (allocate,
                                                           void, (LumieraDisplaySlot slotHandle),
                                                             { 
                                                               if (!_instance)
                                                                 { 
                                                                   lumiera_error_set (LUMIERA_ERROR_LIFECYCLE, 0);
                                                                   return;
                                                                 }
                                                               
                                                               REQUIRE (slotHandle);
                                                               try
                                                                 {
                                                                 _instance().allocate (slotHandle,true); 
                                                                 }
                                                               catch (lumiera::Error&){ /* error state remains set */ }
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (release,
                                                           void, (LumieraDisplaySlot slotHandle),
                                                             { 
                                                               if (!_instance)
                                                                 { 
                                                                   lumiera_error_set (LUMIERA_ERROR_LIFECYCLE, 0);
                                                                   return;
                                                                 }
                                                               
                                                               REQUIRE (slotHandle);
                                                               _instance().allocate (slotHandle,false);
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (put,
                                                           void, (LumieraDisplaySlot slotHandle, LumieraDisplayFrame frame),
                                                             { 
                                                               //skipping full checks for performance reasons
                                                               REQUIRE (_instance && !lumiera_error_peek());
                                                               
                                                               REQUIRE (slotHandle);
                                                               DisplayerSlot& slot = _instance().resolve (slotHandle);
                                                               slot.put (frame);
                                                             }
                                                          )
                               );
    
    
    
    
  } // (End) hidden service impl details
  
  
  
  
  DisplayService::DisplayService()
    : error_{}
    , serviceInstance_( LUMIERA_INTERFACE_REF (lumieraorg_Display, 0, lumieraorg_DisplayService))
    {
      INFO (progress, "Display Facade opened.");
    }
  
  
  
  LumieraDisplaySlot
  DisplayService::setUp (FrameDestination const& outputDestination)
  {
    DisplayerTab& slots (_instance().slots_);
    return &slots.manage (new DisplayerSlot (outputDestination));
  }
  
  
  
  void
  DisplayService::allocate (LumieraDisplaySlot handle, bool doAllocate)
  {
    REQUIRE (handle);
    if (doAllocate)
      {
        if (handle->put_)
          throw lumiera::error::Logic("slot already allocated for output");
        else
          // Mark the handle as "allocated" and ready for output: 
          // Place the function pointer from the C interface into the handle struct.
          // calling it will invoke the implementing instance's "put" function
          // (see the LUMIERA_INTERFACE_INLINE above in this file!)
          handle->put_ = serviceInstance_.get().put;
      }
    else
      handle->put_ = 0;
  }
  
  
  
  DisplayerSlot& 
  DisplayService::resolve (LumieraDisplaySlot handle)
  {
    REQUIRE (handle);
    REQUIRE (handle->put_, "accessing a DisplayerSlot, which hasn't been locked for output");
    
    return *static_cast<DisplayerSlot*> (handle);
  }
  
  
  
  
  
  /* === DisplayerSlot Implementation === */
  
  
  DisplayerSlot::DisplayerSlot (FrameDestination const& outputDestination)
  : currBuffer_(0)
  { 
    put_ = 0; // mark as not allocated
    hasFrame_.connect (outputDestination);
    dispatcher_.connect (sigc::mem_fun (this, &DisplayerSlot::displayCurrentFrame));
  }
  
  
  DisplayerSlot::~DisplayerSlot() 
  {
    TRACE (gui_dbg, "Displayer Slot closing...");
  }
  
  
  void
  DisplayerSlot::displayCurrentFrame()
  {
    hasFrame_.emit (currBuffer_);
  }
  
  
} // namespace stage
