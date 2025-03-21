/*
  PlayProcess  -  frontend to control an play process

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file play-controller.cpp
 ** Implementation of the control- / management frontend(s) exposed
 ** to clients for talking to an ongoing PlayProcess. This includes
 ** the implementation of lumiera::Play::Controller, exposed directly
 ** from the Player facade interface. Besides that primary control
 ** interface, this translation unit also contains the implementation
 ** for several extended play controllers targeted at special usage
 ** situations
 **
 ** @see lumiera::DummyPlayer
 ** @see stage::PlaybackController usage example 
 */


#include "steam/play/play-controller.hpp"
#include "steam/play/play-process.hpp"
#include "lib/time/timevalue.hpp"

//#include <string>
//#include <memory>
//#include <functional>

namespace lumiera {
  
  using lib::time::Time;
  using lib::time::Duration;
  using lib::time::TimeSpan;
  using steam::play::PlayProcess;
  
  
  
  /* ==== Play::Controller implementation ==== */
  
  /** toggle the play or pause state
   *  of the corresponding PlayProcess */
  void
  Play::Controller::play(bool)
  {
    UNIMPLEMENTED ("toggle play/pause state");
  }
  
  
  /** switch into scrubbing playback
   * 
   */
  void
  Play::Controller::scrub(bool)
  {
    UNIMPLEMENTED ("scrubbing playback");
  }
  
  
  void
  Play::Controller::adjustSpeed(double) ///< playback speed control
  {
    UNIMPLEMENTED ("adjust actual playback speed");
  }
  
  
  /** 
   * 
   */
  void
  Play::Controller::go(Time) ///< skip to the given point in time
  {
    UNIMPLEMENTED ("jump to given time");
  }
  
  
  /** 
   * 
   */
  void
  Play::Controller::controlPlayhead (time::Control<Time>     & ctrl)
  {
    UNIMPLEMENTED ("install a controller for influencing the 'playhead position'");
  }
  
  
  /** 
   * 
   */
  void
  Play::Controller::controlDuration (time::Control<Duration> & ctrl)
  {
    UNIMPLEMENTED ("install a controller for influencing the overall playback duration");
  }
  
  
  /** 
   * 
   */
  void
  Play::Controller::controlLooping  (time::Control<TimeSpan> & ctrl)
  {
    UNIMPLEMENTED ("install a controller for adjusting looped playback");
  }
  
  
  /** 
   * 
   */
  void
  Play::Controller::useProxyMedia (bool)
  {
    UNIMPLEMENTED ("toggle the use of proxy(placeholder) media");
  }
  
  
  /** 
   * 
   */
  void
  Play::Controller::setQuality (uint)
  {
    UNIMPLEMENTED ("adjust the quality level of the current render");
  }
  
  
  /** 
   * 
   */
  bool
  Play::Controller::is_playing()   const
  {
    UNIMPLEMENTED ("retrieve the playback state");
  }
  
  
  /** 
   * 
   */
  bool
  Play::Controller::is_scrubbing() const
  {
    UNIMPLEMENTED ("retrieve the state of scrubbing playback");
  }
  
  
  /** 
   * 
   */
  double
  Play::Controller::getSpeed()   const
  {
    UNIMPLEMENTED ("retrieve the current playback speed");
  }
  
  
  /** 
   * 
   */
  uint
  Play::Controller::getQuality()   const
  {
    UNIMPLEMENTED ("retrieve the current render quality setting");
  }
  
  
  /** 
   * 
   */
  bool
  Play::Controller::usesProxy()    const
  {
    UNIMPLEMENTED ("determine if the current render/playback uses proxy media");
  }
  
  
  /** @internal expose a weak reference to this handle object.
   *  Used by the ProcessTable to keep a link to all processes,
   *  without influencing their reference count */
  Play::Controller::operator weak_ptr<PlayProcess>()  const
  {
    return weak_ptr<PlayProcess> (this->smPtr_);
  }

  
  
}


namespace steam{
namespace play{
  
//    using std::string;
//    using lumiera::Subsys;
//    using std::bind;
    
    
    namespace { // hidden local details of the service implementation....
    
      
      
    } // (End) hidden service impl details
    
    
    
    
    /** */
  
  
}} // namespace steam::play
