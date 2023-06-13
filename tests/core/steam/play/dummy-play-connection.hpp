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
 ** Dummy and test setup of playback and rendering, _omitting most_ of the Lumiera engine.
 ** Building this dummy configuration was driven by the need to test and verify the design
 ** in the course of building the foundations of the render engine. The design of Lumiera's
 ** engine is elaborate, and thus -- for a long time -- we have to live with a not-yet
 ** operational engine. While, at the same time, we need to start integrating components
 ** to see if and how the design works out. So, as a remedy, we create a fixture of
 ** "coordinated placeholders". These can be used to stand-in for the real services still
 ** to be written, allowing to invoke the high-level interfaces soon. And behind the scenes,
 ** these placeholders are connected, allowing to produce specific test situations and then
 ** verify the results after the test run.
 ** 
 ** # Use cases
 ** This dummy setup can be used in various circumstances
 ** - for unit tests we want to produce artificial test media frames: each TestFrame is
 **   produced with a reproducible pseudo-random sequence and can be verified to the last bit.
 ** - for integration tests, we want to generate test media data, either to send them to a file,
 **   or to a real system output
 ** - the GUI needs a dummy engine for being able to implement at least some operational pieces
 **   of functionality, instead of just creating windows, menus and icons.
 ** - on the long run, the application will need some kind of test data generator for the users
 **   to verify a more complicated wiring setup. Thus, the DummyPlayConnection is there to stay!
 ** 
 ** Because these are somewhat similar usage scenarios, where this and that part is to be exchanged
 ** for some, we prefer a _policy based design_ here: The DummyPlayConnection is templated
 ** to use _a strategy,_ filling in the variable parts.
 ** 
 ** # provided test services
 ** By using different strategy template parameters, we create different flavours of the dummy;
 ** each one counting as a separate setup (not related to each other, that is). The actual instance
 ** then can just be default created; it should be placed into an scope enduring the whole usage
 ** cycle. Repeated re-initialisation or re-loading is outside the intended usage scope here.
 ** 
 ** The *core interface* allows to retrieve dummy implementations of
 ** - a session model exposing exit node(s)
 ** - generator object(s) to live within this session model
 ** - corresponding generator nodes to serve as implementation of the former
 ** - a low-level render node network ready to use, relying on those generator nodes
 ** - OutputSlot implementations to serve as pseudo- or demo output facilities
 ** - an OutputManager exposing those output facilities.
 ** 
 ** The *test support interface* provides a test driver for performing a controlled
 ** playback or rendering for some time. Thus, a test routine may lock into a blocking wait,
 ** to investigate results after the planned test sequence was performed.
 ** 
 ** @todo this was invented in 2012 -- but development of the player subsystem stalled thereafter.
 **       As of 2016, I still consider this design valid and intend to pick up development when
 **       able to address this topic again. At the moment, the UI-Session connection is more urgent.
 ** 
 ** @see lumiera::DummyPlayer
 ** @see stage::PlaybackController usage example 
 */


#ifndef STEAM_PLAY_DUMMY_PLAY_CONNECTION_H
#define STEAM_PLAY_DUMMY_PLAY_CONNECTION_H


#include "steam/play/dummy-builder-context.hpp"
#include "steam/play/output-manager.hpp"
#include "lib/time/timequant.hpp"

#include "lib/iter-source.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <vector>


namespace steam{
namespace play {
namespace test {

//    using std::string;
  using lib::time::Duration;
  
  
  
  /// @todo placeholder as of 6/23
  struct PlayTestFrames_Strategy
    {
      
    };
  
  
  /****************************************************************//**
   * Framework for dummy playback and rendering.
   * A DummyPlayConnection provides a coherent set of placeholders,
   * allowing to start a data producing process while leaving out
   * various parts of the real engine implementation. The specific
   * mode of operation, suitable for various test scenarios, may be
   * fine tuned by the strategy object defined as template parameter.
   * 
   * @todo not-yet-implemented as of 2016 -- but the design can be considered valid.
   * @todo 6/23 still largely unimplemented, but used as generator for mock ModelPort 
   */
  template<class DEF>
  class DummyPlayConnection
    : util::NonCopyable
    {
      SimulatedBuilderContext mockBuilder_;
      
    public:
      
      ModelPorts
      getAllModelPorts()
        {
          return mockBuilder_.getAllModelPorts();
        }
      
      DummyOutputLink
      getModelPort (uint index)
        {
          return mockBuilder_.getModelPort (index);
        }
      
      /** search through all port <-> sink connections
       *  supported by this DummyPlayConnection
       * @return `true` if found both a math on port an sink.
       */
      bool
      isSupported (ModelPort port, DataSink sink)
        {
          uint i{0};
          for (auto it=getAllModelPorts(); bool{it}; ++it, ++i)
            if (port == *it)
              {
                auto [refPort, refSink] = getModelPort(i);
                if (refSink == sink)
                  return true;
              }
          return false;
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
  
  
  
  
}}} // namespace steam::play::test
#endif /*STEAM_PLAY_DUMMY_PLAY_CONNECTION_H*/
