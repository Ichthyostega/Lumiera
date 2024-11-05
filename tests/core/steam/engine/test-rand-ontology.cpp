/*
  TestRandOntoloy  -  implementation of a test framework processing dummy data frames

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file test-rand-ontology.cpp
 ** Implementation of fake data processing to verify invocation logic.
 */


#include "steam/engine/test-rand-ontology.hpp"
#include "lib/error.hpp"

//#include <vector>



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
  generateMultichan (uint chanCnt, TestFrame* buffArry, size_t frameNr, uint flavour)
  {
    REQUIRE (buffArry);
    for (uint i=0; i<chanCnt; ++i)
      new(buffArry+i) TestFrame{uint(frameNr), flavour+i};
  }
  
  /**
   * @param out   existing allocation to place the generated TestFrame into
   * @param in    allocation holding the input TestFrame data
   * @param param parameter to control the data manipulation (to be multiplied into the data)
   * @remark this function emulates „media data processing“: each byte of the input data is multiplied
   *         with the given \a param, wrapping each result into the corresponding output byte. The
   *         generated result TestFrame is marked with a valid checksum.
   */
  void
  manipulateFrame (TestFrame* out, TestFrame* in, int param)
  {
    REQUIRE (in);
    REQUIRE (out);
    for (size_t i=0; i < in->data().size(); ++i)
        out->data()[i] = char(param * in->data()[i]);
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
  combineFrames (TestFrame* out, TestFrame* srcA, TestFrame* srcB, int mix)
  {
    REQUIRE (srcA);
    REQUIRE (srcB);
    REQUIRE (out);
    for (size_t i=0; i < srcA->data().size(); ++i)
        out->data()[i] = char((1-mix) * srcA->data()[i] + mix * srcB->data()[i]);
  }
  
  
  
  
}}} // namespace steam::engine::test
