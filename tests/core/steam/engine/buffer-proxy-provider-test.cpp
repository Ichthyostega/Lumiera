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
   * @test verify the design of OutputSlot and BufferProvider by
   *       implementing a delegating BufferProvider to expose
   *       output data buffers provided from _some implementation._
   * @todo WIP-WIP 12/2024 this turned out to be impossible,
   *       due to inconsistencies in the default implementation.  /////////////////////////////////////////////TICKET #1387 : need to consolidate BufferProvider default implementation
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
          
          BuffHandle handle = proxPro.lockBuffer (dataBlock); ///////////////////////////////////////////////TICKET #1387 : unable to implement this
          
          // Now a »client« can do awful things to the buffer...
          CHECK (handle.isValid());
          auto& data = handle.accessAs<TestFrame>();
          uint64_t param = defaultGen.u64();
          ont::manipulateFrame (&data, &data, param);

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
