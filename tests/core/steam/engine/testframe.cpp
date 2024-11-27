/*
  TestFrame  -  test data frame (stub) for checking Render engine functionality

   Copyright (C)
     2011, 2024       Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file testframe.cpp
 ** Implementation of fake data frames to support unit testing.
 ** The data generation is based on a _discriminator seed value,_
 ** which is computed as a linear combination of a statically fixed anchor-seed
 ** combined with the family-number and sequence number. Based on this seed,
 ** the contents are then filled by a pseudo-random sequence.
 ** @note while initially drawn from real entropy, the anchor-seed can be
 **       reset from the default PRNG, which allows to establish a totally
 **       deterministically setup from test code, because the test itself
 **       can seed the default PRNG and thus establish a reproducible state.
 ** 
 ** Additionally, beyond this basic test-data feature, the contents can be
 ** manipulated freely, and a new checksum can be stored in the metadata,
 ** which allows to build pseudo media computation functions with a
 ** reproducible effect — so that the proper invocation of several
 ** computation steps invoked deep down in the render engine can
 ** be verified after completing a test invocation.
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
#include <deque>



namespace steam {
namespace engine{
namespace test  {
  namespace err = lumiera::error;
  
  using std::deque;
  using util::unConst;
  
  /** @note using a random-congruential engine to generate the payload data */
  using PseudoRandom = lib::RandomSequencer<std::minstd_rand>;
  
  
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
        uint64_t const& distinction_;
        
      public:
        DistinctNucleus(uint64_t const& anchor)
          : distinction_{anchor}
          { }
        
        uint64_t
        getSeed()  override
          {
            return distinction_;
          }
      };
    
    /** @return a stable characteristic memory marker for the metadata record */
    HashVal
    stampHeader()
    {
      static const HashVal MARK = lib::entropyGen.hash()
                                | 0b1000'1000'1000'1000'1000'1000'1000'1000;   //////////////////////////////TICKET #722 : not portable because HashVal ≡ size_t — should it be?
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
    
    
    /* ======= static TestFrame repository ======= */
    
    /** @internal table to hold test data frames in heap memory.
     * These frames are built on demand, but retained thereafter.
     * Some tests might rely on the actual memory locations, using the
     * test frames to simulate a real input frame data stream.
     * @note \ref TestFrame::reseed() also discards this storage, which
     *       otherwise is retained at stable location until process end.
     */
    struct TestFrameTable
      : deque<deque<TestFrame>>
      {
        TestFrameTable() = default;
        
        TestFrame&
        getFrame (uint seqNr, uint chanNr)
          {
            if (chanNr >= this->size())
                resize(chanNr+1);
            ENSURE (chanNr < this->size());
            deque<TestFrame>& channel = at(chanNr);
            
            if (seqNr >= channel.size())
              {
                INFO (test, "Growing channel #%d of test frames %d -> %d elements."
                          , chanNr, channel.size(), seqNr+1);
                for (uint nr=channel.size(); nr<=seqNr; ++nr)
                  channel.emplace_back (nr, chanNr);
              }
            ENSURE (seqNr < channel.size());
            return channel[seqNr];
          }
      };
      //
    std::unique_ptr<TestFrameTable> testFrames;
    //
  }// (End) hidden impl details
  
  
  
  
  TestFrame&
  testData (uint seqNr, uint chanNr)
  {
    if (not testFrames)
      testFrames = std::make_unique<TestFrameTable>();
    return testFrames->getFrame (seqNr, chanNr);
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
    testFrames.reset(); // discard existing test data repository
    dataSeed = drawSeed (lib::defaultGen);
  }
  
  
  
  
  
  /* ======= TestFrame class ======= */
  
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
      buildData();
      ASSERT (0 < header_.distinction);
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
        throw err::Logic ("target TestFrame already dead or unaccessible");
      if (not util::isSameAdr (this, o))
        {
          data() = o.data();
          header_ = o.header_;
          header_.stage = CREATED;
        }
      return *this;
    }
  
  
  /**
   * Sanity check on the metadata header.
   * @remark Relevant to detect memory corruption or when accessing some
   *  arbitrary memory location, which may or may not actually hold a TestFrame.
   *  Based on the assumption that it is unlikely that any given memory location
   *  just happens to hold our [marker word](\ref stampHeader()) by accident.
   * @note this is only the base level of verification, because in addition
   *  \ref isValid verifies the checksum and \ref isPristine additionally
   *  recomputes the data generation to see if it matches the Meta::distinction.
   */
  bool
  TestFrame::Meta::isPlausible()  const
  {
    return _MARK_ == stampHeader()
       and stage <= DISCARDED;
  }
  
  TestFrame::Meta&
  TestFrame::accessHeader()
  {
    if (not header_.isPlausible())
      throw err::Invalid{"TestFrame: missing or corrupted metadata"};
    return header_;
  }
  TestFrame::Meta const&
  TestFrame::accessHeader()  const
  {
    return unConst(this)->accessHeader();
  }
  
  TestFrame::StageOfLife
  TestFrame::currStage()  const
  {
    return header_.isPlausible()? header_.stage
                                : DISCARDED;
  }
  
  bool
  TestFrame::Meta::operator== (Meta const&o)  const
  {
    return isPlausible() and o.isPlausible()
       and stage == o.stage
       and checksum == o.checksum
       and distinction == o.distinction;
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
    return data() == o.data();
  }
  
  
  /**
   * Generate baseline data content based on the Meta::distinction seed.
   * @remark the seed is a [discriminator](\ref buildDiscriminator) based
   *         on both the »family« and the frameNo within this family;
   *         thus closely related frames are very unlikely to hold the same
   *         baseline data. Of course, follow-up manipulations could change
   *         the data, which should be documented by \ref markChecksum().
   */
  void
  TestFrame::buildData()
  {
    auto gen = buildDataGenFrom (accessHeader().distinction);
    for (uint64_t& dat : data64())
      dat = gen.u64();
    markChecksum();
  }
  
  /** verify the current data was not touched since initialisation
   * @remark implemented by regenerating the data sequence deterministically,
   *         based on the Meta::distinction mark recorded in the metadata. */
  bool
  TestFrame::matchDistinction()  const
  {
    auto gen = buildDataGenFrom (accessHeader().distinction);
    for (uint64_t const& dat : data64())
      if (dat != gen.u64())
        return false;
    return true;
  }
  
  /** @return a hash checksum computed over current data content */
  HashVal
  TestFrame::computeChecksum()  const
  {
    HashVal checksum{0};
    std::hash<char> getHash;
    for (char const& dat : data())
      lib::hash::combine (checksum, getHash (dat));
    return checksum;
  }
  
  /** @remark can be used to mark a manipulated new content as _valid_ */
  HashVal
  TestFrame::markChecksum()
  {
    return accessHeader().checksum = computeChecksum();
  }

  
  bool
  TestFrame::hasValidChecksum()  const
  {
    return accessHeader().checksum == computeChecksum();
  }
  
  bool
  TestFrame::isSane()  const
  {
    return header_.isPlausible();
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
    return isValid()
       and matchDistinction();
  }
  
  bool
  TestFrame::isAlive() const
  {
    return isSane()
       and not isDead();
  }
  
  bool
  TestFrame::isDead()  const
  {
    return isSane()
       and (DISCARDED == currStage());
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
    return candidate.isAlive();
  }
  
  bool
  TestFrame::isDead (void* memLocation)
  {
    TestFrame& candidate (accessAsTestFrame (memLocation));
    return candidate.isDead();
  }
  
  
}}} // namespace steam::engine::test
