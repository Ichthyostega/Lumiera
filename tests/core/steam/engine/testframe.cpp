/*
  TestFrame  -  test data frame (stub) for checking Render engine functionality

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file testframe.cpp
 ** Implementation of fake data frames to support unit testing
 */


#include "lib/error.hpp"
#include "lib/random.hpp"
#include "lib/hash-standard.hpp"
#include "lib/hash-combine.hpp"
#include "steam/engine/testframe.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <climits>
#include <memory>
#include <vector>



namespace steam {
namespace engine {
namespace test   {
  
  using util::unConst;
  using std::vector;
  
  /** @note using a random-congruential engine to generate the payload data */
  using PseudoRandom = lib::RandomSequencer<std::minstd_rand>;
  
  
  namespace error = lumiera::error;
  
  namespace { // hidden local support facilities....
    
    /**
     * Offset to set the seed values of »families« apart.
     * The data in the test frames is generated from a distinctive ID-seed,
     * which is controlled by the _family_ and the _seq-No_ within each family.
     * The seeds for consecutive frames are spread apart by the #dataSeed,
     * and the SEQUENCE_SPREAD constant acts as minimum spread. While seed
     * values can wrap within the 64bit number range, this generation scheme
     * makes it very unlikely that neighbouring frames end up with the same seed.
     */
    const size_t SEQUENCE_SPREAD = 100;
    
    HashVal
    drawSeed (lib::Random& srcGen)
    {
      return srcGen.distribute(
                std::uniform_int_distribution<HashVal>{SEQUENCE_SPREAD
                                                      ,std::numeric_limits<HashVal>::max()-SEQUENCE_SPREAD});
    }
    
    /** @internal a static seed hash used to anchor the data distinction ID-seeds */
    HashVal dataSeed{drawSeed(lib::entropyGen)};
    
    /** @internal helper for generating unique test frames.
     * This »discriminator« is used as a random seed when filling the test frame data buffers.
     * It is generated to be very likely different on adjacent frames of the same series,
     * as well as to differ to all nearby neighbouring channels.
     * @note the #dataSeed hash is limited by #SEQUENCE_SPREAD to prevent „risky“ families;
     *   the extreme case would be dataSeed+family ≡ 0 (all frames would be equal then)
     * @param seq the sequence number of the frame within the channel
     * @param family the channel this frame belongs to
     */
    uint64_t
    generateDiscriminator(uint seq, uint family)
    {
      // use the family as stepping
      return (seq+1) * (dataSeed+family);
    }
    
    class DistinctNucleus
      : public lib::SeedNucleus
      , util::MoveOnly
      {
        uint64_t const& fixPoint_;
      public:
        DistinctNucleus(uint64_t const& anchor)
          : fixPoint_{anchor}
          { }
        
        uint64_t
        getSeed()  override
          {
            return fixPoint_;
          }
      };
    
    /** @return a stable characteristic memory marker for the metadata record */
    HashVal
    stampHeader()
    {
      static const HashVal MARK = lib::entropyGen.hash()
                                | 0b1000'1000'1000'1000'1000'1000'1000'1000;
      return MARK;
    }
    
    /** @internal build a PRNG starting from the referred fixed seed */
    auto
    buildDataGenFrom (uint64_t const& anchor)
    {
      DistinctNucleus seed{anchor};
      return PseudoRandom{seed};
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

  
  
  /**
   * @remark this function should be invoked at the start of any test
   *         which requires reproducible data values in the TestFrame.
   *         It generates a new base seed to distinguish individual data frames.
   *         The seed is drawn from the \ref lib::defaultGen, and thus will be
   *         reproducible if the latter has been reseeded beforehand.
   * @warning after invoking reseed(), the validity of previously generated
   *         frames can no longer be verified.
   */
  void
  TestFrame::reseed()
  {
    testFrames.reset();
    drawSeed (lib::defaultGen);
  }
  
  
  
  
  /* ===== TestFrame class ===== */
  
  TestFrame::Meta::Meta (uint seq, uint family)
    : _MARK_{stampHeader()}
    , checksum{0}
    , distinction{generateDiscriminator (seq,family)}
    , stage{CREATED}
    { }
  
  TestFrame::~TestFrame()
    {
      header_.stage = DISCARDED;
    }
  
  
  TestFrame::TestFrame (uint seq, uint family)
    : header_{seq,family}
    {
      ASSERT (0 < header_.distinction);
      header_.checksum = buildData();
      ENSURE (CREATED == header_.stage);
      ENSURE (isPristine());
    }
  
  
  TestFrame::TestFrame (TestFrame const& o)
    : header_{o.header_}
    {
      data() = o.data();
      header_.stage = CREATED;
    }
  
  TestFrame&
  TestFrame::operator= (TestFrame const& o)
    {
      if (not isAlive())
        throw new error::Logic ("target TestFrame already dead or unaccessible");
      if (not util::isSameAdr (this, o))
        {
          data() = o.data();
          header_ = o.header_;
          header_.stage = CREATED;
        }
      return *this;
    }
  
  
  TestFrame::Meta&
  TestFrame::accessHeader()
  {
    return header_;
  }
  TestFrame::Meta const&
  TestFrame::accessHeader()  const
  {
    ///////////////////////OOO detect if valid header present and else throw
    return unConst(this)->accessHeader();
  }
  
  TestFrame::StageOfLife
  TestFrame::currStage()  const
  {
    ///////////////////////OOO detect if valid header present and then access
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
      if (data()[i] != o.data()[i])
        return false;
    return true;
  }
  
  
  HashVal
  TestFrame::buildData()
  {
    auto gen = buildDataGenFrom (accessHeader().distinction);
    for (uint i=0; i<BUFFSIZ; ++i)
      data()[i] = char(gen.i(CHAR_MAX));
  }
  
  bool
  TestFrame::verifyData()  const
  {
    auto gen = buildDataGenFrom (accessHeader().distinction);
    for (uint i=0; i<BUFFSIZ; ++i)
      if (data()[i] != char(gen.i(CHAR_MAX)))
        return false;
    return true;
  }
  
  HashVal
  TestFrame::computeChecksum()  const
  {
    HashVal hash{0};
    ////////////////////////////////////////////OOO actually compute it
  }
  
  bool
  TestFrame::hasValidChecksum()  const
  {
    ////////////////////////////////////////////OOO actually compute checksum and verify
  }
  
  bool
  TestFrame::isAlive() const
  {
    return (CREATED == currStage())
        || (EMITTED == currStage());
  }
  
  bool
  TestFrame::isDead()  const
  {
    return (DISCARDED == currStage());
  }
  
  bool
  TestFrame::isSane()  const
  {
    return ( (CREATED == currStage())
           ||(EMITTED == currStage())
           ||(DISCARDED == currStage()))
        && verifyData();
  }
  
  bool
  TestFrame::isValid()  const
  {
    return isSane()
       and hasValidChecksum();
  }
  
  bool
  TestFrame::isPristine()  const
  {
    return isSane()
       and hasValidChecksum();
  }
  
  
  
}}} // namespace steam::engine::test
