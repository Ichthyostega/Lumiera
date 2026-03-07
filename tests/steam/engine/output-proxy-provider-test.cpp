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


#include "test/run.hpp"

#include "test/test-frame.hpp"
#include "test/test-rand-ontology.hpp"
#include "steam/engine/buffer-proxy-adaptor.hpp"
#include "lib/meta/prop-builder.hpp"
#include "test/diagnostic-output.hpp" ////////////////////TODO


using test::TestFrame;
using test::ont::manipulateFrame;

using lib::meta::PropBuilder;

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
          BufferState state{NIL};
          
          // setup with notification callback
          BufferProxyAdaptor proxPro{PropBuilder()
                                      .define (PROP_FIELD(on_lock),   [&]{ state = LOCKED; })
                                      .define (PROP_FIELD(on_emit),   [&]{ state = EMITTED;})
                                      .define (PROP_FIELD(on_release),[&]{ state = FREE;   })
                                    };
          
          // Assuming some data block is »given«
          seedRand();
          size_t frameNr = defaultGen.u64();
          TestFrame dataBlock (frameNr);
          CHECK (dataBlock.isPristine());
          CHECK (state == NIL);
          
          BuffHandle handle = proxPro.lockBuffer (dataBlock);
          CHECK (state == LOCKED);
          
          // Now a »client« can do awful things to the buffer...
          CHECK (handle.isValid());
          auto& data = handle.accessAs<TestFrame>();
          uint64_t param = defaultGen.u64();
          manipulateFrame (&data, &data, param);
          HashVal check = data.getChecksum();

          // »client« is done...
          handle.emit();
          CHECK (state == EMITTED);
          
          // end usage cycle
          handle.release();
          CHECK (state == FREE);
          CHECK (not handle.isValid());
          CHECK (not dataBlock.isPristine());
          CHECK (    dataBlock.isValid());
          
          CHECK (check == dataBlock.getChecksum());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (OutputProxyProvider_test, "unit play");
  
  
  
}}} // namespace steam::play::test
