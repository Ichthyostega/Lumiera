/*
  LocalBufferStage(Test)  -  verify thread-local buffer state management

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file local-buffer-stage-test.cpp
 ** unit test \ref LocalBufferStage_test
 */


#include "lib/error.hpp"
#include "test/run.hpp"
#include "test/test-frame.hpp"
#include "test/test-helper.hpp"
#include "vault/mem/buffhandle.hpp"
#include "vault/mem/buffer-metadata.hpp"
#include "vault/mem/local-buffer-stage.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <memory>


using std::strncpy;
using std::unique_ptr;
using test::TestFrame;
using test::testData;
using util::isSameObject;
using util::isnil;
using lib::randStr;
using lib::Literal;

namespace vault {
namespace mem   {
namespace test  {
  
  using LERR_(LOGIC);
//  using LERR_(FATAL);
//  using LERR_(INVALID);
  using LERR_(LIFECYCLE);
  
  
  namespace { // Test fixture
    
    template<typename TY>
    TY&
    accessAs (metadata::Entry& entry)
    {
      TY* ptr = reinterpret_cast<TY*> (entry.access());
      ASSERT (ptr);
      return *ptr;
    }
    
    template<typename X>
    Buff*
    mark_as_Buffer(X& something)
      {
        return reinterpret_cast<Buff*> (std::addressof(something));
      }
    
    
    const size_t TEST_MAX_SIZE = 1024 * 1024;
    
    HashVal JUST_SOMETHING = 123;
    auto SOME_POINTER = mark_as_Buffer(JUST_SOMETHING);
    
  }//(End) Test fixture and helpers
  
  
  
  
  
  
  /***************************************************************//**
   * @test verify mechanism for a thread-local BufferStage implementation
   *     - verify data synchronisation between several BufferMetadata registries
   *     - verify typical usage situations, where a thread-local registry
   *       has to synchronise itself with a central metadata hub.
   */
  class LocalBufferStage_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          verify_DataTransfer();
          verify_publishNewKey();
          verify_fetchNewKeyOnUse();
        }
      
      
      /** @test key-chains can be exported and imported */
      void
      verify_DataTransfer()
        {
          BufferMetadata meta1, meta2;
          
          const HashVal FAM = ranHash();
          const size_t SIZ_D = sizeof(double);
          const double RAND1 = 1.0 + defaultGen.uni();
          
          // create one type-key in Registry-1
          metadata::Key keyD = meta1.key (FAM,SIZ_D);
          CHECK (meta1.isKnown (keyD));
          CHECK (not meta2.isKnown (keyD));
          CHECK (meta1.get(keyD).isTypeKey());
          
          // create another type-key in Registry-2, this time with a constructor
          metadata::Key keyDD = meta2.key (FAM,SIZ_D, TypeHandler::create<double> (RAND1));
          CHECK (meta2.isKnown (keyDD));
          CHECK (not meta1.isKnown (keyDD));
          CHECK (meta2.get(keyDD).isTypeKey());
          
          // Keys are systematic => KeyD happens to be parent of KeyDD
          CHECK (keyD.storageSize() == keyDD.storageSize());
          CHECK (keyDD.parentKey() == HashVal(keyD));
          
          // synchronise a sub-type
          meta1.import (keyDD, meta2);
          CHECK (meta1.isKnown (keyDD));
          
          // now Registry-1 is able to use this subtype as well
          double testBuffer{0};
          metadata::Entry& eDD = meta1.markLocked (keyDD, mark_as_Buffer(testBuffer));
          CHECK (eDD.state() == LOCKED);
          CHECK (eDD.parentKey() == HashVal(keyDD));
          // and the »constructor« was indeed invoked...
          CHECK (testBuffer == RAND1);
          testBuffer += 1.1;   // add marker...
          
          // within the confines of allowed state transitions,
          // it is even possible to sync the state of active buffer entries
          CHECK (meta1.isKnown (eDD));
          CHECK (not meta2.isKnown (eDD));
          meta2.import (eDD, meta1);
          CHECK (meta2.isKnown (eDD));
          CHECK (meta2.get(eDD).state() == LOCKED);
          
          // importing the state record did not re-invoke the constructor
          CHECK (testBuffer == RAND1 + 1.1);
          
          meta2.get(eDD).mark(EMITTED);
          CHECK (meta2.get(eDD).state() == EMITTED);
          CHECK (meta1.get(eDD).state() == LOCKED);
          meta1.import (eDD, meta2);
          CHECK (meta1.get(eDD).state() == EMITTED);
          
          // now mark the buffer as released in Registry-1
          CHECK (eDD.state() == EMITTED);
          eDD.mark(FREE);
          CHECK (eDD.state() == FREE);
          CHECK (meta2.get(eDD).state() == EMITTED);
          
          // it is not allowed to mark de-allocated through synchronisation
          VERIFY_ERROR (LOGIC, meta2.import (eDD, meta1) );
          
          VERIFY_ERROR (LIFECYCLE, meta2.release(HashVal(eDD)) );
          CHECK (meta2.get(eDD).state() == EMITTED);
          meta2.get(eDD).invalidate (false);
          CHECK (meta2.get(eDD).state() == FREE);
          meta2.release(eDD);
          meta1.release(eDD);
        }
      
      
      /** @test simulate a standard buffer provider usage cycle
       */
      void
      verify_publishNewKey()
        {
        }
      
      
      void
      verify_fetchNewKeyOnUse()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (LocalBufferStage_test, "unit engine");
  
  
  
}}} // namespace vault::mem::test
