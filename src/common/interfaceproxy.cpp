/*
  InterfaceProxy  -  definition of forwarding proxies for the facade interfaces
 
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


#include "include/interfaceproxy.hpp"
#include "common/instancehandle.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

using util::cStr;

namespace lumiera {
  namespace facade {
    
    
    LUMIERA_ERROR_DEFINE (FACADE_LIFECYCLE, "facade interface currently not accessible");  
    
    
    template<class IHA>
    class Holder;
    
    template<class FA, class I>
    class Holder<InstanceHandle<I,FA> >
      : Accessor<FA>,
        protected FA
      {
      protected:
        typedef InstanceHandle<I,FA> IHandle;
        typedef Holder<IHandle> THolder;
        typedef Proxy<IHandle> TProxy;
        typedef Accessor<FA> Access;
        
        I& _i_;
        
        Holder (IHandle const& iha)
          : _i_(iha.get())
          {  }
        
      public:
        static TProxy& open(IHandle const& iha)
          {
            static char buff[sizeof(TProxy)];
            TProxy* p = new(buff) TProxy(iha);
            Access::implProxy_ = p;
            return *p;
          }
        
        static void close()
          {
            if (!Access::implProxy_) return;
            TProxy* p = static_cast<TProxy*> (Access::implProxy_);
            Access::implProxy_ = 0;
            p->~TProxy();
          }
      };
    
    
    template<class FA>
    FA* Accessor<FA>::implProxy_;
    
    
    template<class IHA>
    void
    openProxy (IHA const& iha)
      {
        Proxy<IHA>::open(iha);
      }
    
    template<class IHA>
    void
    closeProxy ()
      {
        Proxy<IHA>::close();
      }
    
   } // namespace facade
  
} // namespace lumiera





    /* ==================== GuiNotification =================================== */
    
#include "include/guinotification-facade.h"

namespace gui {
  
  /** storage for the facade proxy factory
   *  used by client code to invoke through the interface */
  lumiera::facade::Accessor<GuiNotification> GuiNotification::facade;

} // namespace gui



namespace lumiera {
  namespace facade {
    
    typedef InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_GuiNotification, 0)
                          , gui::GuiNotification
                          > IHandle_GuiNotification;
    
    
    template<>
    class Proxy<IHandle_GuiNotification>
      : public Holder<IHandle_GuiNotification>
      {
        //----Proxy-Implementation-of-GuiNotification--------
        
        void displayInfo (string const& text)           { _i_.displayInfo (cStr(text)); }
        void triggerGuiShutdown (string const& cause)   { _i_.triggerGuiShutdown (cStr(cause)); }
        
        
      public:
        Proxy (IHandle const& iha) : THolder(iha) {}
      };
    
    
    template  void openProxy<IHandle_GuiNotification>  (IHandle_GuiNotification const&);
    template  void closeProxy<IHandle_GuiNotification> (void);
    
   } // namespace facade
  
} // namespace lumiera





    /* ==================== gui::Display ====================================== */
    
#include "include/display-facade.h"

namespace lumiera {
  
  /** storage for the facade proxy factory
   *  used by client code to invoke through the interface */
  lumiera::facade::Accessor<Display> Display::facade;
  
  /// emit the vtable here into this translation unit within liblumieracommon.so...
  Display::~Display()      { }
  
} // namespace lumiera



namespace lumiera {
  namespace facade {
    
    typedef InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_Display, 0)
                          , lumiera::Display
                          > IHandle_Display;
    
    
    template<>
    class Proxy<IHandle_Display>
      : public Holder<IHandle_Display>
      {
        //----Proxy-Implementation-of-lumiera::Display--------
        
        Display::Sink
        getHandle (LumieraDisplaySlot display)
          {
            _i_.allocate (display);
            Sink sinkHandle;
            sinkHandle.activate (display, _i_.release);
            if (lumiera_error_peek() || !sinkHandle)
              throw lumiera::error::State("failed to allocate output DisplayerSlot",
                                          lumiera_error());
            return sinkHandle; 
          }
        
        
      public:
        Proxy (IHandle const& iha) : THolder(iha) {} 
      };
    
    
    template  void openProxy<IHandle_Display>  (IHandle_Display const&);
    template  void closeProxy<IHandle_Display> (void);
    
   } // namespace facade
  
} // namespace lumiera






    /* ==================== DummyPlayer ======================================= */
    
#include "proc/play/dummy-player-service.hpp"

namespace proc {
  namespace play {
  
  /** storage for the DummyPlayer facade proxy factory... */
  lumiera::facade::Accessor<DummyPlayer> DummyPlayer::facade;
  
} }


namespace lumiera {
  namespace facade {
    
    typedef lumiera::InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_DummyPlayer, 0)
                                     , proc::play::DummyPlayer
                                     > IHandle_DummyPlayer;
    
    
    template<>
    class Proxy<IHandle_DummyPlayer>
      : public Holder<IHandle_DummyPlayer>
      {
        //----Proxy-Implementation-of-DummyPlayer--------
        typedef proc::play::DummyPlayer::Process Process;
        typedef proc::play::ProcessImpl ProcessImpl;
        
        /** @note as an optimisation we hand out a direct reference
         *  to the implementing process object. While this ref could
         *  still be passed as handle to the C Language interface, using
         *  it directly within the client (=GUI) bypasses the C interface
         *  and thus leaves us only with one level of indirection,
         *  irrespective if using the C or C++ interface.
         */
        Process start(LumieraDisplaySlot viewerHandle)
          {
            ProcessImpl* pP = static_cast<ProcessImpl*> (_i_.startPlay (viewerHandle));
            
            if (!pP)
              throw lumiera::error::State("failed to start DummyPlayer", lumiera_error());
            
            return pP->createHandle();
          }
        
        
        
      public:
        Proxy (IHandle const& iha) : THolder(iha) {}
      };
    
    
    template  void openProxy<IHandle_DummyPlayer>  (IHandle_DummyPlayer const&);
    template  void closeProxy<IHandle_DummyPlayer> (void);
    
    
  } // namespace facade
  
} // namespace lumiera
