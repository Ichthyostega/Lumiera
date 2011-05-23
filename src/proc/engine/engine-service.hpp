/*
  ENGINE-SERVICE.hpp  -  primary service access point for using the renderengine

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file engine-service.hpp
 ** A public service provided by
 **
 ** @see lumiera::DummyPlayer
 ** @see gui::PlaybackController usage example 
 */


#ifndef PROC_ENGINE_ENGINE_SERVICE_H
#define PROC_ENGINE_ENGINE_SERVICE_H


//#include "include/dummy-player-facade.h"
//#include "include/display-facade.h"
//#include "common/instancehandle.hpp"
//#include "lib/singleton-ref.hpp"
//
//#include <boost/noncopyable.hpp>
//#include <boost/scoped_ptr.hpp>
//#include <string>


namespace proc {
namespace play {

//    using std::string;
//    using lumiera::Subsys;
//    using lumiera::Display;
//    using lumiera::DummyPlayer;
  
  
  
  
  
  /******************************************************
   * Actual implementation of the DummyPlayer service.
   * Creating an instance of this class automatically
   * registers the interface lumieraorg_DummyPlayer with
   * the Lumiera Interface/Plugin system and creates
   * a forwarding proxy within the application core to
   * route calls through this interface.
   */
  class EngineService
    : boost::noncopyable
    {
      
      string error_;
      Subsys::SigTerm notifyTermination_;
      
      
      /* === Interface Lifecycle === */
      
      typedef lumiera::InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_DummyPlayer, 0)
                                     , DummyPlayer
                                     > ServiceInstanceHandle;
      
      lib::SingletonRef<DummyPlayerService> implInstance_;
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
} // namespace proc
#endif
