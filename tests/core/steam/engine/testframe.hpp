/*
  TESTFRAME.hpp  -  test data frame (stub) for checking Render engine functionality

   Copyright (C)
     2011, 2024       Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file testframe.hpp
 ** Unit test helper to generate fake test data frames.
 ** Each TestFrame holds a 1k buffer of byte data, which can be
 ** verified, accessed and manipulated to emulate media computations.
 ** A [metadata header](\ref steam::engine::test::TestFrame::Meta) is placed
 ** in memory behind the working buffer, which allows to detect data corruption
 ** and stores a lifecycle phase and a data checksum.
 ** 
 ** The contents of each TestFrame are filled on creation with pseudo-random data,
 ** which is created from a _discriminator seed,_ based on a »family« and a »frameNr«
 ** within each family(≙channel). Due to the deterministic nature of these computations,
 ** the _pristine state_ of any frame can be determined. But the payload data is accessible
 ** and can be manipulated, and a new [checksum can be recorded](\ref TestFrame::markChecksum).
 ** 
 ** For ease of testing, a static [store of TestFrame instances](\ref #testData) is built and
 ** retained in heap memory, and an arbitrary memory location can be treated as TestFrame.
 */


#ifndef STEAM_ENGINE_TESTFRAME_H
#define STEAM_ENGINE_TESTFRAME_H


#include "lib/integral.hpp"
#include "lib/hash-value.h"

#include <array>


namespace steam {
namespace engine {
namespace test   {
  
  using lib::HashVal;
  
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
      
      static constexpr size_t BUFFSIZ = 1024;
      using _Arr = std::array<char,BUFFSIZ>;
      using _A64 = std::array<uint64_t, BUFFSIZ/sizeof(uint64_t)>;

      struct Meta
        {
          HashVal _MARK_;
          HashVal checksum;
          uint64_t distinction;
          StageOfLife stage;
          
          Meta (uint seq, uint family);
          bool isPlausible()             const;
          bool operator== (Meta const&)  const;
        };
      
      /** inline storage buffer for the payload media data */
      alignas(uint64_t)
        std::byte buffer_[sizeof(_Arr)];
      
      /** Metadata record located behind the data buffer */
      Meta header_;
      
    public:
      /** discard all cached #testData and recalibrate data generation */
      static void reseed();
      
     ~TestFrame();
      TestFrame (uint seq=0, uint family=0);
      TestFrame (TestFrame const&);
      TestFrame& operator= (TestFrame const&);
      
      /** recompute and store checksum based on current contents */
      HashVal markChecksum();
      HashVal getChecksum();
      
      /** Helper to verify that a given memory location holds
       *  an active TestFrame instance (created, not yet destroyed)
       * @return true if the TestFrame datastructure is intact and
       *         marked as still alive.
       */
      static bool isAlive (void* memLocation);
      
      /** Helper to verify a given memory location holds
       *  an already destroyed TestFrame instance */
      static bool isDead (void* memLocation);
      
      bool isAlive()    const;
      bool isDead()     const;
      bool isSane()     const;
      bool isValid()    const;
      bool isPristine() const;
      
      bool operator== (void* memLocation) const;
      
      friend bool operator== (TestFrame const& f1, TestFrame const& f2) { return  f1.contentEquals(f2); }
      friend bool operator!= (TestFrame const& f1, TestFrame const& f2) { return !f1.contentEquals(f2); }

      /** Array-style direct access to the payload data */
      _Arr&       data()         { return * std::launder (reinterpret_cast<_Arr* > (&buffer_));      }
      _Arr const& data()   const { return * std::launder (reinterpret_cast<_Arr const*> (&buffer_)); }
      _A64&       data64()       { return * std::launder (reinterpret_cast<_A64* > (&buffer_));      }
      _A64 const& data64() const { return * std::launder (reinterpret_cast<_A64 const*> (&buffer_)); }
      
    private:
      void buildData();
      Meta&       accessHeader();
      Meta const& accessHeader()  const;
      bool contentEquals (TestFrame const& o) const;
      bool matchDistinction()   const;
      StageOfLife currStage()   const;
      HashVal computeChecksum() const;
      bool hasValidChecksum()   const;
    };
  
  
  
  /** Helper to access a specific frame of test data at a fixed memory location.
   *  The series of test frames is generated on demand, but remains in memory thereafter,
   *  similar to real data accessible from some kind of source stream. Each of these generated
   *  test frames is filled with different yet reproducible pseudo random data.
   *  Client code is free to access and corrupt this data.
   * @note TestFrame::reseed() discards this data and draws a new base seed from `defaultGen`
   */
  TestFrame& testData (uint seqNr =0, uint chanNr =0);
  
  
}}} // namespace steam::engine::test
#endif
