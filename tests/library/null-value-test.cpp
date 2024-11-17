/*
  NullValue(Test)  -  verify the singleton holder for NIL or default values

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file null-value-test.cpp
 ** unit test \ref NullValue_test
 */




#include "lib/test/run.hpp"
#include "lib/null-value.hpp"
#include "lib/util.hpp"


namespace lib {
namespace test{
  
  using util::isSameObject;
  
  namespace { // test data and helpers...
    
    
    /** Test dummy to track ctor call */
    struct DummyType
      {
        static bool created;
        
        uint id_;
        
        DummyType()
          : id_(1 + rani(100))
          {
            created = true;
          }
      };
    
    bool DummyType::created  = false;
    
    
  }
  
  
  
  /***********************************************************************************//**
   * @test Access "NIL values" just by type, verify these are actually singletons
   *       and are indeed created on demand.
   * 
   * @see null-value.hpp
   * @see lib::advice::Request usage example
   */
  class NullValue_test : public Test
    {
      
      void
      run (Arg) 
        {
          seedRand();
          CHECK (long() == NullValue<long>::get());
          CHECK (short() == NullValue<short>::get());
          CHECK (isSameObject(NullValue<short>::get(), NullValue<short>::get()));
          
          CHECK (!DummyType::created);
          DummyType copy (NullValue<DummyType>::get());
          CHECK ( DummyType::created);
          
          CHECK ( copy.id_ == NullValue<DummyType>::get().id_);
          CHECK (!isSameObject(NullValue<DummyType>::get(), copy));
          CHECK ( isSameObject(NullValue<DummyType>::get(), NullValue<DummyType>::get()));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NullValue_test, "unit common");
  
  
}} // namespace lib::test
