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
 ** Primary service of the Player subsystem: Starting and controlling render/playback.
 ** This is the implementation level service interface for the "Player". Client code
 ** should access this service through the lumiera::Play facade interface.
 ** 
 ** The player and render control subsystem allows to start and control playback and
 ** rendering at any given collection of model ports (the conceptual exit points
 ** of the High-Level-Model). These model ports are the (side effect) result of a
 ** build process performed on the High-Level-Model, causing all nominal output
 ** designations within the model to be resolved as far as possible. Additionally,
 ** for _playback,_ the global level of the model (the global pipes of a timeline)
 ** need to be connected to a viewer component. This results in a further resolution
 ** step, mapping the output designations to the concrete output possibilities of
 ** the running Lumiera instance. This mapping information is kept embedded within
 ** an OutputManager instance, passed as parameter when invoking the player service.
 ** Alternatively, for _rendering,_ a similar kind of output mapping information
 ** needs to be provided, again embedded within an OutputManager instance, this
 ** time leading to an output file to be rendered.
 ** 
 ** The result of an invocation of the Player service is a Play::Controller
 ** frontend object. This smart-ptr like handle can be used by the client to
 ** control all aspects of playback or rendering; it behaves like a state machine.
 ** When the last copy of this Play::Controller frontend goes out of scope, behind
 ** the scenes the corresponding PlayProcess gets terminated and prepared for cleanup. 
 ** 
 ** @see engine::EngineService
 ** @todo started as draft in 11/2011
 ** @todo as of 2016 development in this area is stalled, but the design done thus far valid
 ** 
 */


#ifndef STEAM_PLAY_PLAY_SERVICE_H
#define STEAM_PLAY_PLAY_SERVICE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "include/play-facade.hpp"

#include <memory>
//#include <string>


namespace steam {
namespace play {

  using std::string;
//using lumiera::Subsys;
//using lumiera::Display;
//using lumiera::DummyPlayer;
  
  
//  class DummyImageGenerator;
//  class TickService;
  class ProcessTable;
  class RenderConfigurator;
  
  
  
  
  /**************************************************//**
   * Implementation access point: Player subsystem.
   * The PlayService is the primary way for clients to
   * get into using Lumiera's Play/Output Subsystem.
   * It allows to \em perform a timeline or model object.
   * Behind the scenes, this will create and outfit a
   * PlayProcess, which is accessible through the
   * Play::Controller returned as frontend/handle.
   * 
   * @internal the PlayService is instantiated and owned
   * by the OutputDirector, which acts as a central hub
   * for the Player subsystem. Clients should always
   * access this functionality through the
   * lumiera::Play facade interface. 
   */
  class PlayService
    : public lumiera::Play
    , util::NonCopyable
    {
      std::unique_ptr<ProcessTable>      pTable_;
      
      
      /** Implementation: build a PlayProcess */
      virtual Controller connect(ModelPorts, POutputManager);
      
      
    public:
      PlayService();   /////TODO Subsys::SigTerm terminationHandle);
      
     ~PlayService();    /////TODO notifyTermination_(&error_); }
      
    };
  
  
  LUMIERA_ERROR_DECLARE (CANT_PLAY); ///< unable to build playback or render process for this configuration
  
  
  
  
}} // namespace steam::play
#endif
