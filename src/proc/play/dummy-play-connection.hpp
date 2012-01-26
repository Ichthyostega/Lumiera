/*
  DUMMY-PLAY-CONNECTION.hpp  -  simplified test setup for playback

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

/** @file dummy-play-connection.hpp
 ** to \em provide this service, not to access it.
 **
 ** @see lumiera::DummyPlayer
 ** @see gui::PlaybackController usage example 
 */


#ifndef PROC_PLAY_DUMMY_PLAY_CONNECTION_H
#define PROC_PLAY_DUMMY_PLAY_CONNECTION_H


//#include "include/dummy-player-facade.h"
//#include "include/display-facade.h"
//#include "common/instancehandle.hpp"
//#include "lib/singleton-ref.hpp"
#include "proc/play/output-manager.hpp"
#include "proc/mobject/model-port.hpp"
#include "lib/time/timequant.hpp"
#include "lib/iter-source.hpp"
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
  using lib::time::Duration;
  
  typedef lib::IterSource<proc::mobject::ModelPort>::iterator ModelPorts;  
  
  
  struct PlayTestFrames_Strategy
    {
      
    };
  
  
  /********************************************************************
   */
  template<class DEF>
  class DummyPlayConnection
    : boost::noncopyable
    {
      
    public:
      
      ModelPorts
      provide_testModelPorts()
        {
          UNIMPLEMENTED ("provide a set of test model ports");
        }
      
      POutputManager
      provide_testOutputSlot()
        {
          UNIMPLEMENTED ("provide a suitable output sink simulation");
        }
      
      
      /* === Test Support API === */
      
      bool
      isWired()
        {
          UNIMPLEMENTED ("is this dummy in activated state?");
        }
      
      Duration
      getPlannedTestDuration()
        {
          UNIMPLEMENTED ("manage the a planned test duration");
        }
      
      /** test helper: blocking wait during an output test.
       *  The waiting time should be in accordance with the
       *  \link #getPlannedTestduration planned value \endlink,
       */
      void
      waitUntilDue()
        {
          UNIMPLEMENTED ("do a blocking wait, while an output test is performed in other threads");
        }
      
      bool
      gotCorrectOutput()
        {
          UNIMPLEMENTED ("verify proper operation by inspecting the provided test dummy components");
        }
    };
  
  
  
  
}} // namespace proc::play
#endif
