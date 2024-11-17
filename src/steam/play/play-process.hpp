/*
  PLAY-PROCESS.hpp  -  state frame for an ongoing play/render process

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file play-process.hpp
 ** Organisational unit of an ongoing render- or playback process.
 ** A process object doesn't perform any work in itself, rather it's
 ** an entry in the process table maintained within the PlayService.
 ** This table entry is used to keep track of the individual data feeds,
 ** each corresponding to one of the global pipes to be "performed" in order
 ** to generate output data. Usually, these global pipes all belong to a given
 ** Timeline (but other setups are possible as well).
 ** 
 ** Each of these Feed objects comprising a play process is in turn responsible
 ** for getting one or multiple CalculationStream entities configured and operative
 ** within the actual render engine. Each of these calculation streams corresponds
 ** to a running series of calculations for consecutive frames, to be delivered
 ** in a time-bound fashion from the render engine into an OutputSlot, allocated
 ** for rendering this specific feed.
 ** 
 ** A PlayProcess isn't exposed directly to client code -- it's the body object,
 ** while a Play::Controller handle is returned to the client (PImpl pattern).
 ** Using this controller frontend, clients are allowed to control and change
 ** the playback or rendering state and goals, which then causes the PlayProcess
 ** to reconfigure the ongoing or planned calculations.
 ** 
 ** @see lumiera::DummyPlayer
 ** @see stage::PlaybackController usage example 
 */


#ifndef STEAM_PLAY_PLAY_PROCESS_H
#define STEAM_PLAY_PLAY_PROCESS_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
//#include "include/dummy-player-facade.h"
//#include "include/display-facade.h"
//#include "common/instancehandle.hpp"
//#include "lib/singleton-ref.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/engine/calc-stream.hpp"
#include "lib/iter-source.hpp"                 ////////////////////TICKET #493 : only using the IterSource base feature here
#include "lib/util.hpp"

#include <functional>
//#include <string>
#include <vector>


namespace steam {
namespace play {

//    using std::string;
//    using lumiera::Subsys;
//    using lumiera::Display;
//    using lumiera::DummyPlayer;
  using util::isnil;
  using steam::mobject::ModelPort;
  using std::function;
  
  typedef lib::IterSource<ModelPort>::iterator ModelPorts;
  
  namespace error = lumiera::error;
  
  
  
  
  /**
   * Rendering data feed, corresponding to a single
   * global pipe and to be delivered into a single OutputSlot.
   * A feed may still be comprised of multiple channels, but is
   * bound to operate on a single type of media data only.
   */
  class Feed
    {
      engine::CalcStreams renderStreams_;
      
    public:
      
      /** building a Feed effectively requires the definition
       *  of a rendering plan through the EngineService.
       * @param CalcStreams definition of the individual
       *        calculation "continuations" for the engine.
       *        These correspond to already running
       *        render calculations. */
      Feed (engine::CalcStreams const&);
    };
  
  typedef function<Feed(ModelPort)> FeedBuilder;
  typedef std::vector<Feed>         OutputFeeds;
    
  
  /**************************************************//**
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
    : util::NonCopyable
    {
      OutputFeeds outputFeeds_;
      
      PlayProcess (OutputFeeds feeds);
      
    public:
      static PlayProcess*
      initiate (ModelPorts dataGenerators, FeedBuilder);
    };
  
  
  
  
}} // namespace steam::play
#endif
