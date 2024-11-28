/*
  TEST-RAND-ONTOLOGY.hpp  -  placeholder for a domain-ontology working on dummy data frames

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file test-rand-ontology.hpp
 ** A faked »media calculation« environment to validate the render node network.
 */


#ifndef STEAM_ENGINE_TEST_RAND_ONTOLOGY_H
#define STEAM_ENGINE_TEST_RAND_ONTOLOGY_H


#include "lib/error.hpp"
#include "lib/depend.hpp"
#include "steam/engine/testframe.hpp"

#include <array>
#include <string>


namespace steam {
namespace engine{
namespace test  {
  
  using std::string;
  
  /** produce sequences of frames with (reproducible) random data */
  void generateFrame (TestFrame* buff, size_t frameNr =0, uint flavour =0);
  
  /** produce planar multi channel output of random data frames */
  void generateMultichan (TestFrame* buffArry, uint chanCnt, size_t frameNr =0, uint flavour =0);
  
  /** create an identical clone copy of the planar multi channel frame array */
  void duplicateMultichan (TestFrame* outArry, TestFrame* inArry, uint chanCnt);

  /** »process« a planar multi channel array of data frames in-place */
  void manipulateMultichan (TestFrame* buffArry, uint chanCnt, uint64_t param);
  
  /** »process« random frame date by hash-chaining with a parameter */
  void manipulateFrame (TestFrame* out, TestFrame const* in, uint64_t param);
  
  /** mix two random data frames by a parameter-controlled proportion */
  void combineFrames (TestFrame* out, TestFrame const* srcA, TestFrame const* srcB, double mix);
  


/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Dummy / Placeholder
  using NoArg   = std::array<char*, 0>;
  using SoloArg = std::array<char*, 1>;
  
  extern const string DUMMY_FUN_ID;
  
  /** @todo a placeholder operation to wire a prototypical render node
   */
  inline void
  dummyOp (NoArg in, SoloArg out)
  {
    UNIMPLEMENTED ("a sincerely nonsensical operation");
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Dummy / Placeholder
  /**
   * A fake _Domain Ontology_ to describe mocked »render operations« on
   * dummy data frames filled with random numbers.
   * 
   * @see TestFrame_test
   * @see NodeDevel_test
   * @see NodeLinkage_test
   * 
   */
  class TestRandOntology
    {
      
    public:
     ~TestRandOntology()  = default;
      TestRandOntology()  = default;
      
    private:
    };
  
  
  
  /** */
  
  
  
}}} // namespace steam::engine::test
#endif
