/*
  PLAYBACK-CONTROLLER.hpp  -  playback controller object

  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>

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


/** @file controller/playback-controller.hpp
 ** This file contains the definition of the playback controller object
 */


#ifndef PLAYBACK_CONTROLLER_HPP
#define PLAYBACK_CONTROLLER_HPP

#include "gui/gtk-base.hpp"
#include "include/dummy-player-facade.h"
#include "include/display-facade.h"

#include <boost/noncopyable.hpp>


namespace gui {
namespace controller {
  
  
  
  class PlaybackController
    : boost::noncopyable
    {

      volatile bool playing_;
      
      lumiera::DummyPlayer::Process playHandle_;
      
      LumieraDisplaySlot viewerHandle_;
      
    public:
    
      PlaybackController();
    
      void play();
      void pause();
      void stop();
      
      bool is_playing();
    
      void useDisplay (LumieraDisplaySlot display);
      
    private:
      
      void on_frame();
      
    };
  
  
}} // namespace gui::controller
#endif // PLAYBACK_CONTROLLER_HPP

