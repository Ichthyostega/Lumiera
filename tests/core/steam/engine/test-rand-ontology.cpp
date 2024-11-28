/*
  TestRandOntoloy  -  implementation of a test framework processing dummy data frames

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file test-rand-ontology.cpp
 ** Implementation of fake data processing to verify invocation logic.
 ** The emulated »media computations« work on TestFrame data buffers, which can be
 ** filled with deterministically generated pseudo-random data, that can be verified afterwards.
 ** Computations manipulate or combine individual data points, and mark the result again with a
 ** valid checksum. Hash-chaining computations are used in order to ensure that the resulting
 ** data values depend on all input- and parameter values, and the _exact order_ of processing.
 ** All computations are reproducible, and thus a test can verify a computation carried out
 ** within the context of the Render-Engine code.
 */


#include "steam/engine/test-rand-ontology.hpp"
#include "lib/hash-combine.hpp"
#include "lib/iter-zip.hpp"

#include <cmath>

using std::lround;
using lib::zip;


namespace steam {
namespace engine{
namespace test  {
//  namespace err = lumiera::error;
  
  namespace { // hidden local support facilities....
    
  } // (End) hidden impl details
  
  
  const string DUMMY_FUN_ID{"dummyFun(TestFrame)"};

  /* ========= Dummy implementation of Media processing ========= */

  /**
   * @param buff    a sufficiently sized allocation to place the result data into
   * @param frameNr the frame of the »source feed« to generate (determines actual random data)
   * @param flavour a further seed parameter to determine the actual (reproducibly) random data
   */
  void
  generateFrame (TestFrame* buff, size_t frameNr, uint flavour)
  {
    REQUIRE (buff);
    new(buff) TestFrame{uint(frameNr), flavour};
  }
  
  /**
   * @param chanCnt  size of the array of frames to generate
   * @param buffArry pointer to an allocation sufficiently sized to hold `TestFrame[chanCnt]`
   * @param frameNr  the frame of the »source feed« to use commonly on all those frames in the output
   * @param flavour  a further seed parameter used as starting offest for the output's `family` parameters
   * @remark this is a variation of the [dummy data generator](\ref #generateFrame),
   *         which immediately generates a planar block of related frames with random data,
   *         all seeded with the _same_ `frameNr` and _consecutive_ `family` parameters,
   *         which will be offset commonly by adding the \a flavour parameter.
   */
  void
  generateMultichan (TestFrame* buffArry, uint chanCnt, size_t frameNr, uint flavour)
  {
    REQUIRE (buffArry);
    for (uint i=0; i<chanCnt; ++i)
      new(buffArry+i) TestFrame{uint(frameNr), flavour+i};
  }
  
  /**
   * @param chanCnt size of the array of frames to clone
   * @param inArry  pointer to storage holding a TestFrame[chanCnt]
   * @param outArry pointer to allocated storage sufficient to hold a clone copy of these
   */
  void
  duplicateMultichan (TestFrame* outArry, TestFrame* inArry, uint chanCnt)
  {
    REQUIRE (inArry);
    REQUIRE (outArry);
    for (uint i=0; i<chanCnt; ++i)
      new(outArry+i) TestFrame{inArry[i]};
  }
  
  /**
   * @param chanCnt  size of the array of frames to manipulate
   * @param buffArry pointer to an array of several frames (channels)
   * @param param parameter to control or »mark« the data manipulation (hash-combining)
   * @remark this function in-place processing of several channels in one step: data is processed
   *         in 64-bit words, by hash-chaining with \a param and then joining in the data items.
   *         All data buffers will be manipulated and marked with as valid with a new checksum.
   */
  void
  manipulateMultichan (TestFrame* buffArry, uint chanCnt, uint64_t param)
  {
    REQUIRE (buffArry);
    const uint SIZ = buffArry->data64().size();
    for (uint i=0; i<SIZ; ++i)
      {
        uint64_t feed{param};
        for (uint c=0; c<chanCnt; ++c)
          {
            auto& data = buffArry[c].data64()[i];
            lib::hash::combine(feed, data);
            data = feed;
          }
      }
    for (uint c=0; c<chanCnt; ++c)
      buffArry[c].markChecksum();
  }
  
  /**
   * @param out   existing allocation to place the generated TestFrame into
   * @param in    allocation holding the input TestFrame data
   * @param param parameter to control or »mark« the data manipulation (hash-combining)
   * @remark this function emulates „media data processing“: data is processed in 64-bit words,
   *         by hash-chaining with \a param. The generated result is marked with a valid checksum.
   */
  void
  manipulateFrame (TestFrame* out, TestFrame const* in, uint64_t param)
  {
    REQUIRE (in);
    REQUIRE (out);
    auto calculate = [](uint64_t chain, uint64_t val){ lib::hash::combine(chain,val); return chain; };
    for (auto& [res,src] : zip (out->data64(), in->data64()))
        res = calculate(param, src);
    out->markChecksum();
  }
  
  /**
   * @param out  existing allocation to receive the calculated result TestFrame
   * @param srcA a buffer holding the input data for feed-A
   * @param srcB a buffer holding the input data for feed-B
   * @param mix  degree of mixing (by integer arithmetics): 100 means 100% feed-B
   * @remark this function emulates a mixing or overlaying operation:
   *         each result byte is the linear interpolation between the corresponding inputs.
   */
  void
  combineFrames (TestFrame* out, TestFrame const* srcA, TestFrame const* srcB, double mix)
  {
    REQUIRE (srcA);
    REQUIRE (srcB);
    REQUIRE (out);
    for (auto& [res,inA,inB] : zip (out->data()
                                   ,srcA->data()
                                   ,srcB->data()))
        res = lround((1-mix)*inA + mix*inB);
    out->markChecksum();
  }
  
  
  
  
}}} // namespace steam::engine::test
