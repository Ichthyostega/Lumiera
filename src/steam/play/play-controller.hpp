/*
  PLAY-CONTROLLER.hpp  -  frontend handle to control an play process

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

/** @file play-controller.hpp
 ** Functionality for driving and controlling an ongoing playback or render.
 **
 ** @see lumiera::DummyPlayer
 ** @see gui::PlaybackController usage example 
 */


#ifndef STEAM_PLAY_PLAY_CONTROLLER_H
#define STEAM_PLAY_PLAY_CONTROLLER_H


#include "steam/play/play-service.hpp"
//#include "include/display-facade.h"
//#include "common/instancehandle.hpp"
//#include "lib/singleton-ref.hpp"
#include "lib/nocopy.hpp"

//#include <string>


namespace steam {
namespace play {

  using std::string;
  
  
  /****************************************************************//**
   * TODO do we still need that??????
   * Isn't lumiera::Play::Controller already sufficient?
   */
  class PlayController
    : util::NonCopyable
    {
      
    public:
      
    private:
    };
  
  
  
  
  
  
}} // namespace steam::play
#endif
