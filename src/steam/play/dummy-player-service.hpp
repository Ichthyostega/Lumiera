/*
  DUMMY-PLAYER-SERVICE.hpp  -  service implementing a dummy test player

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

*/

/** @file dummy-player-service.hpp
 ** A public service provided by the Steam-Layer, implementing a dummy/mockup playback process.
 ** This is a design sketch; Lumiera isn't able to generate rendered output as of 2/2009. The
 ** idea is, that for each ongoing calculation process, there is a ProcessImpl instance holding
 ** the necessary handles and allocations and providing an uniform API to the client side.
 ** Especially, this ProcessImpl holds a TickService, which generates periodic callbacks, and
 ** it uses an output handle (functor) to push the generated frames up.
 ** 
 ** This service is the implementation of a layer separation facade interface. Clients should use
 ** proc::play::DummyPlayer#facade to access this service. This header defines the interface used
 ** to \em provide this service, not to access it.
 ** 
 ** @see lumiera::DummyPlayer
 ** @see gui::PlaybackController usage example 
 */


#ifndef STEAM_DUMMYPLAYER_SERVICE_H
#define STEAM_DUMMYPLAYER_SERVICE_H


#include "include/dummy-player-facade.h"
#include "include/display-facade.h"
#include "common/instancehandle.hpp"
#include "lib/nocopy.hpp"

#include <memory>
#include <string>


namespace steam {
  namespace node {
    class DummyImageGenerator;
    class TickService;
  }
  
  namespace play {
  
    using std::string;
    using lumiera::Subsys;
    using lumiera::Display;
    using lumiera::DummyPlayer;
    
    using proc::node::DummyImageGenerator;
    using proc::node::TickService;
    
    
    
    /****************************************************************//**
     * Actual implementation of a single (dummy) playback process.
     * The DummyPlayerService (see below) maintains a collection of such
     * actively running playback processes, while the client code gets
     * DummyPlayer::Process handles to track any ongoing use. Users of
     * the plain C interface get a direct bare pointer to the respective
     * ProcessImpl instance and have to manage the lifecycle manually.
     */
    class ProcessImpl
      : public lumiera::DummyPlayer::ProcessImplementationLink,
        util::NonCopyable
      {
        uint fps_;
        bool play_;
        
        Display::Sink                        display_;
        std::unique_ptr<DummyImageGenerator> imageGen_;
        std::unique_ptr<TickService>         tick_;
        
        
      public:
        ProcessImpl(LumieraDisplaySlot) ;
       ~ProcessImpl() ;
        
        
        /* Implementation-level API */
        
        /** activate a playback process
         *  with given specification */
        void setRate (uint fps);
        
        bool isActive () { return fps_ != 0; }
        bool isPlaying() { return play_; }
        
        void  doPlay(bool yes);
        
        
        /* Lifecycle */
        
        DummyPlayer::Process createHandle();
        static void terminate(DummyPlayer::ProcessImplementationLink*);
        
      private:
        void doFrame (); ///< periodically invoked while playing
      };
    
    
    
    /**************************************************//**
     * Actual implementation of the DummyPlayer service.
     * Creating an instance of this class automatically
     * registers the interface lumieraorg_DummyPlayer with
     * the Lumiera Interface/Plugin system and creates
     * a forwarding proxy within the application core to
     * route calls through this interface.
     */
    class DummyPlayerService
      : util::NonCopyable
      {
        
        string error_;
        Subsys::SigTerm notifyTermination_;
        
        
        /* === Interface Lifecycle === */
        
        typedef lumiera::InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_DummyPlayer, 0)
                                       , DummyPlayer
                                       > ServiceInstanceHandle;
        ServiceInstanceHandle serviceInstance_;
        
      public:
        DummyPlayerService(Subsys::SigTerm terminationHandle);
       ~DummyPlayerService() { notifyTermination_(&error_); }
        
        
        
        /** conceptually, this serves as implementation
         *  of the DummyPlayer#start() function. But because
         *  this function sits \em behind the interface, it
         *  just returns an impl pointer.  */
        ProcessImpl* start (LumieraDisplaySlot viewerHandle);
        
      };
    
    
    
    
  } // namespace play

} // namespace steam
#endif

