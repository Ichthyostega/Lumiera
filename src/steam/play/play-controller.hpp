/*
  PLAY-CONTROLLER.hpp  -  frontend handle to control an play process

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file play-controller.hpp
 ** Functionality for driving and controlling an ongoing playback or render.
 **
 ** @see lumiera::DummyPlayer
 ** @see stage::PlaybackController usage example 
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
