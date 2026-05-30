/*
  BufferProxyAdaptor(Test)  -  verify accessing an output sink via BufferProvider protocol

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file buffer-proxy-adaptorr-test.cpp
 ** unit test \ref BufferProxyAdaptor_test
 */


#include "test/run.hpp"

#include "test/test-frame.hpp"
#include "test/test-rand-ontology.hpp"
#include "vault/mem/buffer-proxy-adaptor.hpp"
#include "lib/meta/prop-builder.hpp"
#include "test/diagnostic-output.hpp" ////////////////////TODO


using test::TestFrame;
using test::ont::manipulateFrame;

using lib::meta::PropBuilder;

namespace vault {
namespace mem   {
namespace test  {
  
  
  
  /***************************************************************//**
   * @test demonstrate a configuration of the BufferProvider implementation,
   *       rigged such that a pre-existing given buffer (address) can be
   *       passed through and exposed as BuffHandle.
   * @remark while this looks like a rather convoluted implementation setup,
   *       this feature was crucial to make the »Output Slot Protocol«
   *       mesh up well ([1415]) with the »Buffer Provider Protocol«,
   *       used internally within the Render Engine
   * @see output-buffer-proxy.hpp (the implementation actually used)
   * [1415]: https://issues.lumiera.org/ticket/1415]
   */
  class BufferProxyAdaptor_test : public Test
    {
      virtual void
      run (Arg)
        {
          BufferState state{NIL};
          
          // setup with notification callback
          BufferProxyAdaptor proxPro{PropBuilder()
                                      .define (PROP_FIELD(on_lock),   [&](BuffAlloc){ state = LOCKED; })
                                      .define (PROP_FIELD(on_emit),   [&](BuffAlloc){ state = EMITTED;})
                                      .define (PROP_FIELD(on_release),[&](BuffAlloc){ state = FREE;   })
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
  LAUNCHER (BufferProxyAdaptor_test, "unit play");
  
  
  
}}} // namespace vault::mem::test
