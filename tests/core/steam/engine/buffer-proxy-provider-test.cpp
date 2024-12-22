/*
  OutputProxyProvider(Test)  -  verify accessing an output sink via BufferProvider protocol

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file output-proxy-provider-test.cpp
 ** unit test \ref OutputProxyProvider_test
 */


#include "lib/test/run.hpp"

//#include "steam/play/diagnostic-output-slot.hpp"
#include "steam/engine/buffer-proxy-provider.hpp"
#include "steam/engine/test-rand-ontology.hpp"



namespace steam {
namespace engine{
namespace test  {
  
  
  
  
  /***************************************************************//**
   * @test verify the OutputSlot interface and base implementation
   *       by performing full data exchange cycle. This is a
   *       kind of "dry run" for documentation purposes,
   *       both the actual OutputSlot implementation
   *       as the client using this slot are Mocks.
   */
  class OutputProxyProvider_test : public Test
    {
      virtual void
      run (Arg) 
        {
          size_t seenID{0};
          BufferState lastState{NIL};
          auto listener = [&](size_t id, BufferState state)
                              {
                                seenID = id;
                                lastState = state;
                              };
          // setup with notification callback
          BufferProxyProvider proxPro{listener};
          
          // Assuming some data block is »given«
          seedRand();
          TestFrame::reseed();
          size_t frameNr = defaultGen.u64();
          TestFrame dataBlock (frameNr);
          CHECK (    dataBlock.isPristine());
          
          BuffHandle handle = proxPro.lockBuffer (dataBlock);
          
          // Now a »client« can do awful things to the buffer...
          CHECK (handle.isValid());
          auto& data = handle.accessAs<TestFrame>();
          uint64_t param = defaultGen.u64();
          manipulateFrame (&data, &data, param);

          // »client« is done...
          handle.emit();
          
          // end usage cycle
          handle.release();
          CHECK (not handle.isValid());
          CHECK (not dataBlock.isPristine());
          CHECK (    dataBlock.isValid());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (OutputProxyProvider_test, "unit play");
  
  
  
}}} // namespace steam::play::test
