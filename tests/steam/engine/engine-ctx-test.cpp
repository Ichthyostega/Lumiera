/*
  EngineCtx(Test)  -  calculation of (internal) buffer metadata type keys

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file engine-ctx-test.cpp
 ** unit test \ref EngineCtx_test
 */


#include "lib/error.hpp"
#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "steam/engine/engine-ctx.hpp"
#include "vault/mem/buffer-provider.hpp"
#include "lib/util.hpp"


using util::isSameObject;
using vault::mem::BuffHandle;
using vault::mem::BufferProvider;

namespace steam {
namespace engine{
namespace test  {
  
  using LERR_(LIFECYCLE);
  
  
  
  /*****************************************************************************//**
   * @test verify context with generic management services for Dependency Injection,
   *       used within the Render Node Network to hide global setup state.
   */
  class EngineCtx_test : public Test
    {
      
      
      virtual void
      run (Arg)
        {
          auto& ctx = EngineCtx::access;
          BufferProvider& provider = ctx().mem;
          
          // in the default config (for tests) there is no separate cache provider
          CHECK (isSameObject (provider, ctx().cache));
          
          BuffHandle buff = provider.lockBufferFor<int>();
          CHECK (buff.isValid());
          CHECK (sizeof(int) <= buff.size());
          
          buff.accessAs<int>() = -LIFE_AND_UNIVERSE_4EVER;
          CHECK (buff.accessAs<int>() == -int(LIFE_AND_UNIVERSE_4EVER));
          
          uint alternativeTruth = buff.accessAs<uint>();
          CHECK (alternativeTruth == -LIFE_AND_UNIVERSE_4EVER);
          
          buff.release();
          CHECK (not buff.isValid());
          VERIFY_ERROR (LIFECYCLE, buff.accessAs<int>() );
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (EngineCtx_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
