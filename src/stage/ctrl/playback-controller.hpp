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


/** @file playback-controller.hpp
 ** This file contains the definition of the playback controller object.
 ** 
 ** @deprecated this represents an early design of playback and will be reworked
 ** @remarks what we actually need is a PlaybackController as a shell or proxy
 **     to maintain a flexible link to ongoing processes in the core. But note,
 **     this is also related to the Displayer service, which needs to be offered
 **     by the UI, so we create a mutual dependency here, and there is not much
 **     that can be done about this.
 ** @warning as a temporary solution, 1/2017 the playback controller was moved
 **     into the viewer panel. Of course it can not work that way....
 ** @todo create a durable PlaybacController design    //////////////////////////////////////////////////////TICKET #1072
 */


#ifndef PLAYBACK_CONTROLLER_HPP
#define PLAYBACK_CONTROLLER_HPP

#include "stage/gtk-base.hpp"
#include "include/dummy-player-facade.h"
#include "include/display-facade.h"
#include "lib/nocopy.hpp"



namespace stage {
namespace ctrl {
  
  
  
  /** @deprecated we need a durable design for the playback process */
  class PlaybackController
    : util::NonCopyable
    {

      volatile bool playing_;
      
      lumiera::DummyPlayer::Process playHandle_;
      
      LumieraDisplaySlot viewerHandle_;
      
      static PlaybackController* instance;  ///////////////////////////////////////////////////////////////////TICKET #1067 shitty workaround to allow disentangling of top-level
      
    public:
    
      PlaybackController();
     ~PlaybackController();
      
      static PlaybackController& get();     ///////////////////////////////////////////////////////////////////TICKET #1067 shitty workaround to allow disentangling of top-level 
    
      void play();
      void pause();
      void stop();
      
      bool is_playing();
    
      void useDisplay (LumieraDisplaySlot display);
      
    private:
      
      void on_frame();
      
    };
  
  
}} // namespace stage::ctrl
#endif // PLAYBACK_CONTROLLER_HPP

