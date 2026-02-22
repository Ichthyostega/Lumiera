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
#include "steam/engine/buffer-proxy-adaptor.hpp"
#include "steam/engine/test-rand-ontology.hpp"
#include "lib/test/diagnostic-output.hpp"


namespace steam {
namespace engine{
namespace test  {
  
  
  
  
  /***************************************************************//**
   * @test verify the design of OutputSlot and BufferProvider by
   *       implementing a delegating BufferProvider to expose
   *       output data buffers provided from _some implementation._
   * @todo 2/2026 now using this setup for prototyping        /////////////////////////////////////////////////TICKET #1415 : explore this design idea through prototyping
   */
  class OutputProxyProvider_test : public Test
    {
      virtual void
      run (Arg)
        {
          struct Callback
            {
              void on_lock() { cout << "LOCK" << endl; }
              void on_emit() { cout << "EMIT" << endl; }
              void on_release() { cout << "RELEASE" << endl; }
            };
          
          // setup with notification callback
          BufferProxyAdaptor proxPro{Callback()};
          
          // Assuming some data block is »given«
          seedRand();
          size_t frameNr = defaultGen.u64();
          TestFrame dataBlock (frameNr);
          CHECK (    dataBlock.isPristine());
          
          BuffHandle handle = proxPro.lockBuffer (dataBlock);
          
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
