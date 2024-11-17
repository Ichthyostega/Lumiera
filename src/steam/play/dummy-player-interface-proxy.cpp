/*
  DummyPlayer(Proxy)  -  access point and service implementing a dummy test player

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/



/** @file dummy-player-interface-proxy.cpp
 ** Interface-Proxy for the lumiera::DummyPlayer facade (experimental/obsolete).
 ** The purpose is to define a proxy implementation of lumiera::DummyPlayer, in order to
 ** redirect any calls through the associated C Language Interface "lumieraorg_DummyPlayer"
 ** 
 ** @remarks the implementation of this interface proxy for the DummyPlayer highlighted
 **          some interesting design issues. The way we're defining our interfaces and
 **          bindings seems to have some shortcomings when attempting to express an
 **          interface (here DummyPlayer) in terms of another abstraction
 **          (here the DummyPlayer::Process), since the implementation of this
 **          abstraction has to be mapped and indirected via the interface-system
 **          as well. This forces us to duplicate all of these secondary interface
 **          functions several times, and incurs a further forwarding through the
 **          smart-Handle, since our interface system doesn't support suitable
 **          lifecycle support out of the box and instead places this burden
 **          onto the client code (or, as in this case here, the intermediary
 **          proxy used by the client code to access the interface).
 ** 
 ** @see dummy-player-facade.hpp
 ** @see dummy-player-service.hpp actual implementation within the Steam-Layer
 ** @deprecated obsolete early design draft from 2009;
 **             as of 2018 only kept in source to compile some likewise obsolete UI widgets.
 */


#include "include/dummy-player-facade.h"
#include "include/interfaceproxy.hpp"

namespace lumiera {
  
  // emit the vtable here into this translation unit within liblumieracommon.so ...
  DummyPlayer::~DummyPlayer()  { }
  DummyPlayer::ProcessImplementationLink::~ProcessImplementationLink() { };
  
  
  /** static storage for the facade access front-end */
  lib::Depend<DummyPlayer> DummyPlayer::facade;
  
  
  
  
  namespace facade {
    
    
    
    /* ==================== DummyPlayer ======================================= */
    
    using Interface = LUMIERA_INTERFACE_INAME(lumieraorg_DummyPlayer, 0);
    using Facade    = lumiera::DummyPlayer;
    
    using IHandle   = InstanceHandle<Interface, Facade>;
    
    
    template<>
    class Proxy<IHandle>
      : public Binding<IHandle>
      {
        //----Proxy-Implementation-of-DummyPlayer--------
        typedef lumiera::DummyPlayer::Process Process;
        typedef steam::play::ProcessImpl ProcessImpl;
        
        /** @note as an optimisation we hand out a direct reference
         *  to the implementing process object. While this ref could
         *  still be passed as handle to the C Language interface, using
         *  it directly within the client (=GUI) bypasses the C interface
         *  and thus leaves us only with one level of indirection,
         *  irrespective if using the C or C++ interface.
         *  @note in hindsight this turned out as a very bad idea,
         *  since it complicated the definition of the facade proxy
         *  and created quite involved library dependency problems.
         */
        Process start(LumieraDisplaySlot viewerHandle)  override
          {
            ProcessImplementationLink* pP = static_cast<ProcessImplementationLink*> (_i_.startPlay (viewerHandle));
            
            if (!pP)
              throw lumiera::error::State("failed to start DummyPlayer", lumiera_error());
            
            return pP->createHandle();
          }
        
        
        
      public:
        using Binding<IHandle>::Binding;
      };
    
    
    /**  emit proxy code here... */
    template
    class Link<Interface,Facade>;
    
  } // namespace facade

  
   /* === Forwarding function(s) on the Process handle === */
  
  void
  DummyPlayer::Process::play (bool yes)
    {
      // access the implementation via smart-Handle
      impl().doPlay(yes);
    }
  
  
} // namespace lumiera
