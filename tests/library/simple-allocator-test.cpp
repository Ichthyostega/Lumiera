/*
  SimpleAllocator(Test)  -  check interface for simple custom allocations

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file simple-allocator-test.cpp
 ** unit test \ref SimpleAllocator_test
 */


#include "lib/test/run.hpp"
#include "lib/simple-allocator.hpp"
#include "lib/util.hpp"

#include <string>


namespace lib {
namespace test{
  
  using util::isSameObject;
  using std::string;
  
  
  
  namespace { // test data and helpers...
  
    long checksum_ = 0;
    
    /**
     * Yet-another ctor/dtor-tracking test dummy object....
     */
    template<uint siz>
    class DummyObj
      {
        char crap_[siz];
        
      public:
        DummyObj()
          {
            REQUIRE (siz);
            for (uint i=0; i<siz; ++i)
              checksum_ += (crap_[i] = rani(128));
          }
        
        DummyObj (DummyObj const& o)
          {
            REQUIRE (siz);
            for (uint i=0; i<siz; ++i)
              checksum_ += (crap_[i] = o.crap_[i]);
          }
        
       ~DummyObj()
          {
            for (uint i=0; i<siz; ++i)
              checksum_ -= crap_[i];
          }
      };
    
    typedef Types<DummyObj<1>,DummyObj<23>,string> SupportedTypes;
    typedef SimpleAllocator<SupportedTypes, UseInstantiationCounting> TestAllocator;
  }
  
  
  
  /***********************************************************************************//**
   * @test cover the basic operations of a custom allocator, delegating to mpool.
   *       The SimpleAllocator doesn't provide any ref-counting or tracking facilities,
   *       nor does he support bulk de-allocation. The advantage over using std::allocator
   *       directly is the shortcut for (placement) construction, and -- of course -- the
   *       ability to exchange the memory model at one central location.
   * 
   * @todo as of 9/11 we do heap allocation, but we should use mpool --   see also Ticket #835
   *        
   * @see engine::BufferMetadata
   * @see TypedAllocationManager_test
   */
  class SimpleAllocator_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          CHECK (0 == checksum_);
          seedRand();
          
          TestAllocator allocator;
          
          typedef string *       PS;
          typedef DummyObj<1> *  PD1;
          typedef DummyObj<23> * PD23;
          CHECK (sizeof(DummyObj<1>) != sizeof(DummyObj<23>));
          
          PD1  pD11 = allocator.create<DummyObj<1>>();
          PD1  pD12 = allocator.create<DummyObj<1>>();
          PD23 pD21 = allocator.create<DummyObj<23>>();
          PD23 pD22 = allocator.create<DummyObj<23>>();
          PS   pS11 = allocator.create<string> ("Lumiera");
          PS   pS12 = allocator.create<string> ("the paradox");
          
          CHECK (pD11);
          CHECK (pD12);
          CHECK (pD21);
          CHECK (pD22);
          CHECK (pS11);
          CHECK (pS12);
          CHECK (!isSameObject (*pD11, *pD12));
          CHECK (!isSameObject (*pD11, *pD21));
          CHECK (!isSameObject (*pD11, *pD22));
          CHECK (!isSameObject (*pD11, *pS11));
          CHECK (!isSameObject (*pD11, *pS12));
          CHECK (!isSameObject (*pD12, *pD21));
          CHECK (!isSameObject (*pD12, *pD22));
          CHECK (!isSameObject (*pD12, *pS11));
          CHECK (!isSameObject (*pD12, *pS12));
          CHECK (!isSameObject (*pD21, *pD22));
          CHECK (!isSameObject (*pD21, *pS11));
          CHECK (!isSameObject (*pD21, *pS12));
          CHECK (!isSameObject (*pD22, *pS11));
          CHECK (!isSameObject (*pD22, *pS12));
          CHECK (!isSameObject (*pS11, *pS12));
          
          CHECK (*pS11 == "Lumiera");
          CHECK (*pS12 == "the paradox");
          
          PD23 pDxx = allocator.create<DummyObj<23>> (*pD21);
          PS   pSxx = allocator.create<string>       (*pS12);
          
          CHECK (*pS12 == *pSxx);
          CHECK (!isSameObject (*pS12, *pSxx));
          
          allocator.destroy (pD11);
          allocator.destroy (pD12);
          allocator.destroy (pD21);
          allocator.destroy (pD22);
          allocator.destroy (pS11);
          allocator.destroy (pS12);
          allocator.destroy (pDxx);
          allocator.destroy (pSxx);
          
          CHECK (0 == allocator.numSlots<DummyObj<1>>());
          CHECK (0 == allocator.numSlots<DummyObj<23>>());
          CHECK (0 == allocator.numSlots<string>());
          CHECK (0 == checksum_);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SimpleAllocator_test, "unit common");
  
  
}} // namespace lib::test
