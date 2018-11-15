/*
  TESTFRAME.hpp  -  test data frame (stub) for checking Render engine functionality

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

/** @file testframe.hpp
 ** Unit test helper to generate fake test data frames
 */


#ifndef STEAM_ENGINE_TESTFRAME_H
#define STEAM_ENGINE_TESTFRAME_H


#include <cstdlib>
#include <stdint.h>


namespace steam {
namespace engine {
namespace test   {
  
  
  /**
   * Mock data frame for simulated rendering.
   * A test frame can be created and placed instead of a real data frame.
   * It doesn't depend on any external libraries and will be self-maintaining.
   * Placeholder functions are provided for assignment (simulating the actual
   * calculations); additional diagnostic functions allow to verify the
   * performed operations after-the fact
   * 
   * Each TestFrame is automatically filled with pseudo random data;
   * multiple frames are arranged in sequences and channels, causing the random data
   * to be reproducible yet different within each frame. TestFrame's lifecycle is
   * tracked and marked in an embedded state field. Moreover, the contents of the
   * data block can be verified, because the sequence of bytes is reproducible,
   * based on the channel and sequence number of the test frame.
   * 
   * @see TestFrame_test
   * @see OutputSlotProtocol_test
   * 
   */
  class TestFrame
    {
      enum StageOfLife {
          CREATED, EMITTED, DISCARDED
        };
      
      static const size_t BUFFSIZ = 1024;
      
      uint64_t distinction_;
      StageOfLife stage_;
      
      char data_[BUFFSIZ];
      
    public:
     ~TestFrame();
      TestFrame (uint seq=0, uint family=0);
      TestFrame (TestFrame const&);
      TestFrame& operator= (TestFrame const&);
      
      /** Helper to verify that a given memory location holds
       *  an active TestFrame instance (created, not yet destroyed)
       * @return true if the TestFrame datastructure is intact and
       *         marked as still alive.
       */
      static bool isAlive (void* memLocation);
      
      /** Helper to verify a given memory location holds
       *  an already destroyed TestFrame instance */
      static bool isDead (void* memLocation);
      
      bool isAlive() const;
      bool isDead()  const;
      bool isSane()  const;
      
      bool operator== (void* memLocation) const;
      
      friend bool operator== (TestFrame const& f1, TestFrame const& f2) { return  f1.contentEquals(f2); }
      friend bool operator!= (TestFrame const& f1, TestFrame const& f2) { return !f1.contentEquals(f2); }
      
    private:
      bool contentEquals (TestFrame const& o)  const;
      bool verifyData()  const;
      void buildData ();
    };
  
  
  
  /** Helper to access a specific frame of test data at a fixed memory location.
   *  The series of test frames is generated on demand, but remains in memory thereafter,
   *  similar to real data accessible from some kind of source stream. Each of these generated
   *  test frames filled with different yet reproducible pseudo random data.
   *  Client code is free to access and corrupt this data.
   */
  TestFrame& testData (uint seqNr);
  
  TestFrame& testData (uint chanNr, uint seqNr);
  
  
  /** discards all the TestFrame instances and
   *  initialises an empty table of test frames */
  void resetTestFrames();
  
  
  
}}} // namespace steam::engine::test
#endif
