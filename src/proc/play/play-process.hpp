/*
  PLAY-PROCESS.hpp  -  state frame for an ongoing play/render process

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

/** @file play-process.hpp
 *  
 ** @see lumiera::DummyPlayer
 ** @see gui::PlaybackController usage example 
 */


#ifndef PROC_PLAY_PLAY_PROCESS_H
#define PROC_PLAY_PLAY_PROCESS_H


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
    class PlayerProcess
      : boost::noncopyable
      {
        
      public:
        
      };
    
    
    
    
  } // namespace play

} // namespace proc
#endif
