/*
  PLAY-SERVICE.hpp  -  interface: render- and playback control

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

/** @file play-service.hpp
 ** Player subsystem......
 **
 ** @see lumiera::DummyPlayer
 ** @see gui::PlaybackController usage example 
 */


#ifndef PROC_PLAY_PLAY_SERVICE_H
#define PROC_PLAY_PLAY_SERVICE_H


#include "lib/error.hpp"
#include "include/play-facade.h"

#include <boost/noncopyable.hpp>
//#include <boost/scoped_ptr.hpp>
//#include <string>


namespace proc {
  namespace play {
  
    using std::string;
    using lumiera::Subsys;
    using lumiera::Display;
    using lumiera::DummyPlayer;
    
    
//  class DummyImageGenerator;
//  class TickService;
    
    
    
    /******************************************************
     * Interface: Player subsystem.
     */
    class PlayService
      : boost::noncopyable
      {
        
      public:
        PlayService(Subsys::SigTerm terminationHandle);
        
       ~PlayService() { notifyTermination_(&error_); }
        
      };
    
    
    
    
  } // namespace play

} // namespace proc
#endif
