/*
  DummyPlayer(Proxy)  -  access point and service implementing a dummy test player

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



/** @file dummy-player-interface-proxy.hpp
 ** This is an implementation fragment, intended to be included into common/interfaceproxy.cpp
 ** 
 ** The purpose is to define a proxy implementation of lumiera::DummyPlayer, in order to
 ** redirect any calls through the associated C Language Interface "lumieraorg_DummyPlayer"
 **
 ** @see dummy-player-facade.hpp
 ** @see dummy-player-service.hpp actual implementation within the Proc-Layer 
 */






    /* ==================== DummyPlayer ======================================= */
    
#include "proc/play/dummy-player-service.hpp"

namespace lumiera {
  
  /** storage for the DummyPlayer facade proxy factory... */
  lumiera::facade::Accessor<DummyPlayer> DummyPlayer::facade;
  
  
  
  
  
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
