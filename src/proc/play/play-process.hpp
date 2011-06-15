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
#include <boost/noncopyable.hpp>
//#include <boost/scoped_ptr.hpp>
//#include <string>


namespace proc {
namespace play {

//    using std::string;
//    using lumiera::Subsys;
//    using lumiera::Display;
//    using lumiera::DummyPlayer;
  
  
  
  /******************************************************
   * Playback/Render process within the Lumiera Player.
   * This is a top-level implementation entity, created
   * by the PlayService when \em performing a timeline
   * or similar model object. A PlayProcess doesn't
   * perform calculations; rather it serves to group
   * all the handles, registrations, calculation contexts
   * and similar entries necessary to make the calculations
   * happen within the renderengine and to send the generated
   * data to an output sink.
   * 
   * @note PlayProcess entities are never directly visible.
   *       Client code handles and controls these processes
   *       through a lumiera::Play::Controller returned
   *       from the PlayService.
   */
  class PlayProcess
    : boost::noncopyable
    {
      
    public:
      
    };
  
  
  
  
}} // namespace proc::play
#endif
