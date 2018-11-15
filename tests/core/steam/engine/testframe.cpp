/*
  TestFrame  -  test data frame (stub) for checking Render engine functionality

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

* *****************************************************/

/** @file testframe.cpp
 ** Implementation of fake data frames to support unit testing
 */


#include "proc/engine/testframe.hpp"
#include "lib/error.hpp"

#include <boost/random/linear_congruential.hpp>

#include <cstring>
#include <memory>
#include <vector>



namespace proc {
namespace engine {
namespace test   {
  
  using std::vector;
  using std::memcpy;
  
  typedef boost::rand48 PseudoRandom;
  
  
  namespace error = lumiera::error;
  
  namespace { // hidden local support facilities....
    
    /** @internal helper for generating unique test frames.
     * This "discriminator" is used as a random seed when
     * filling the test frame data buffers. It is generated
     * to be different on adjacent frames of the same series,
     * as well as to differ to all near by neighbouring channels.
     * @param seq the sequence number of the frame within the channel
     * @param family the channel this frame belongs to
     */
    uint64_t
    generateDistinction(uint seq, uint family)
    {
      // random offset, but fixed per executable run
      static uint base(10 + rand() % 990);
      
      // use the family as stepping
      return (seq+1) * (base+family);
    }
    
    
    TestFrame&
    accessAsTestFrame (void* memoryLocation)
    {
      REQUIRE (memoryLocation);
      return *reinterpret_cast<TestFrame*> (memoryLocation);
    }
    
    
    /**
     * @internal table to hold test data frames.
     * These frames are built on demand, but retained thereafter.
     * Some tests might rely on the actual memory locations, using the
     * test frames to simulate a real input frame data stream.
     * @param CHA the maximum number of channels to expect
     * @param FRA the maximum number of frames to expect per channel 
     * @warning choose the maximum number parameters wisely.
     *          We're allocating memory to hold a table of test frames
     *          e.g. sizeof(TestFrame) * 20channels * 100frames ≈ 2 MiB
     *          The table uses vectors, and thus will grow on demand,
     *          but this might cause existing frames to be relocated in memory;
     *          some tests might rely on fixed memory locations. Just be cautious!
     */
    template<uint CHA, uint FRA>
    struct TestFrameTable
      : vector<vector<TestFrame>>
      {
        typedef vector<vector<TestFrame>> VECT;
        
        TestFrameTable()
          : VECT(CHA)
          {
            for (uint i=0; i<CHA; ++i)
              at(i).reserve(FRA);
          }
        
        TestFrame&
        getFrame (uint seqNr, uint chanNr=0)
          {
            if (chanNr >= this->size())
              {
                WARN (test, "Growing table of test frames to %d channels, "
                            "which is > the default (%d)", chanNr, CHA);
                resize(chanNr+1);
              }
            ENSURE (chanNr < this->size());
            vector<TestFrame>& channel = at(chanNr);
            
            if (seqNr >= channel.size())
              {
                WARN_IF (seqNr >= FRA, test,
                         "Growing channel #%d of test frames to %d elements, "
                            "which is > the default (%d)", chanNr, seqNr, FRA);
                for (uint i=channel.size(); i<=seqNr; ++i)
                  channel.push_back (TestFrame (i,chanNr));
              }
            ENSURE (seqNr < channel.size());
            
            return channel[seqNr];
          }
      };
    
    const uint INITIAL_CHAN = 20;
    const uint INITIAL_FRAMES = 100;
    
    typedef TestFrameTable<INITIAL_CHAN,INITIAL_FRAMES> TestFrames;
    
    std::unique_ptr<TestFrames> testFrames;
    
    
    TestFrame&
    accessTestFrame (uint seqNr, uint chanNr)
    {
      if (!testFrames) testFrames.reset (new TestFrames);
      
      return testFrames->getFrame(seqNr,chanNr);
    }
    
  } // (End) hidden impl details
  
  
  
  
  TestFrame&
  testData (uint seqNr)
  {
    return accessTestFrame (seqNr, 0);
  }
  
  TestFrame&
  testData (uint chanNr, uint seqNr)
  {
    return accessTestFrame (seqNr,chanNr);
  }
  
  void
  resetTestFrames()
  {
    testFrames.reset(0);
  }
  
  
  
  
  /* ===== TestFrame class ===== */
  
  TestFrame::~TestFrame()
    {
      stage_ = DISCARDED;
    }
  
  
  TestFrame::TestFrame(uint seq, uint family)
    : distinction_(generateDistinction (seq,family))
    , stage_(CREATED)
    {
      ASSERT (0 < distinction_);
      buildData();
    }
  
  
  TestFrame::TestFrame (TestFrame const& o)
    : distinction_(o.distinction_)
    , stage_(CREATED)
    {
      memcpy (data_, o.data_, BUFFSIZ);
    }
  
  TestFrame&
  TestFrame::operator= (TestFrame const& o)
    {
      if (DISCARDED == stage_)
        throw new error::Logic ("target TestFrame is already dead");
      if (this != &o)
        {
          distinction_ = o.distinction_;
          stage_ = CREATED;
          memcpy (data_, o.data_, BUFFSIZ);
        }
      return *this;
    }
  
  
  
  /** @note performing an unchecked conversion of the given
   *        memory location to be accessed as TestFrame.
   *        The sanity of the data found at that location
   *        is checked as well, not only the lifecycle flag.
   */
  bool
  TestFrame::isAlive (void* memLocation)
  {
    TestFrame& candidate (accessAsTestFrame (memLocation));
    return candidate.isSane()
        && candidate.isAlive();
  }
  
  bool
  TestFrame::isDead (void* memLocation)
  {
    TestFrame& candidate (accessAsTestFrame (memLocation));
    return candidate.isSane()
        && candidate.isDead();
  }
  
  bool
  TestFrame::operator== (void* memLocation)  const
  {
    TestFrame& candidate (accessAsTestFrame (memLocation));
    return candidate.isSane()
        && candidate == *this;
  }
  
  bool
  TestFrame::contentEquals (TestFrame const& o)  const
  {
    for (uint i=0; i<BUFFSIZ; ++i)
      if (data_[i] != o.data_[i])
        return false;
    return true;
  }
  
  bool
  TestFrame::verifyData()  const
  {
    PseudoRandom gen(distinction_);
    for (uint i=0; i<BUFFSIZ; ++i)
      if (data_[i] != char(gen() % CHAR_MAX))
        return false;
    return true;
  }
  
  void
  TestFrame::buildData()
  {
    PseudoRandom gen(distinction_);
    for (uint i=0; i<BUFFSIZ; ++i)
      data_[i] = char(gen() % CHAR_MAX);
  }
  
  
  bool
  TestFrame::isAlive() const
  {
    return (CREATED == stage_)
        || (EMITTED == stage_);
  }
  
  bool
  TestFrame::isDead()  const
  {
    return (DISCARDED == stage_);
  }
  
  bool
  TestFrame::isSane()  const
  {
    return ( (CREATED == stage_)
           ||(EMITTED == stage_)
           ||(DISCARDED == stage_))
        && verifyData();
  }
  
  
  
}}} // namespace proc::engine::test
