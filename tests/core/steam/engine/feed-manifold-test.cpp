/*
  FeedManifold(Test)  -  check consistency of buffer table chunk allocation

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file feed-manifold-test.cpp
 ** unit test \ref FeedManifold_test
 */


#include "lib/test/run.hpp"
#include "lib/error.hpp"

#include "steam/engine/proc-node.hpp"
#include "steam/engine/feed-manifold.hpp"
#include "lib/format-cout.hpp" ////////////////TODO

#include <memory>

using test::Test;


namespace steam {
namespace engine{
namespace test  {

  namespace { // used internally
    
    const uint TABLE_SIZ = 100000;
    const uint CHUNK_MAX = 8000;
    const uint WIDTH_MAX = 3;
    
    
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
    /** just some crap to pass in as ctor argument... */
    template<class E>
    struct DummyArray : lib::RefArray<E>
      {
        E decoy;
        E const& operator[] (size_t)  const  { return decoy; }
        size_t size()                 const  { return CHUNK_MAX;}
      };
    DummyArray<ChannelDescriptor> dummy1;
    DummyArray<InChanDescriptor>  dummy2;
    
    
    /** a "hijacked" Connectivity descriptor requesting
     *  a random number of inputs and outputs */
    struct MockSizeRequest
      : Connectivity
      {
        uint ii,oo;
        
        MockSizeRequest()
          : Connectivity(dummy1,dummy2,0,NodeID()),
            ii(rani (CHUNK_MAX)),
            oo(rani (CHUNK_MAX))
          { }
        
        virtual uint getNrI()  const { return ii; }
        virtual uint getNrO()  const { return oo; }
        
        virtual BuffHandle callDown (StateClosure_OBSOLETE&, uint) const
        { throw lumiera::Error("not intended to be called"); }
      };
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
    
    
    
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #852
    void*
    detect_start_level (BuffTableStorage& sto)
    {
      return BuffTableChunk(MockSizeRequest(), sto ).outHandle;
    }       // address of first available storage element
    
    
    inline void*
    first_behind (BuffTable const& thisChunk, const uint nrI)
    {
      return &thisChunk.inHandle[nrI];
    }
    
    
    inline bool
    not_within(void* candidate, void* lower, void* upper)
    {
      return (candidate < lower) || (upper <= candidate);
    }
    
    
    bool
    consistencyCheck (BuffTable const& b, Connectivity const& num, void* lastLevel)
    {
      return (b.outHandle == lastLevel )  // storage is allocated continuously
          && (b.outBuff   <= b.inBuff  )  // input slots are behind the output slots
          && (b.outHandle <= b.inHandle)
          && (b.inBuff   == &b.outBuff  [num.nrO])
          && (b.inHandle == &b.outHandle[num.nrO])
          && (not_within(b.outBuff, b.outHandle, &b.inHandle[num.nrO])) // storage doesn't overlap
          && (not_within(b.inBuff,  b.outHandle, &b.inHandle[num.nrO]))
          && (not_within(b.outHandle, b.outBuff, &b.inBuff[num.nrO]))
          && (not_within(b.inHandle,  b.outBuff, &b.inBuff[num.nrO]))
           ;
    }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #852
  } // (End) internal defs
  
  
  
  /***************************************************************//**
   * @test create a random pattern of recursive invocations, each
   *       allocating a chunk out of a global buffer table storage.
   *       After returning, each allocation should be cleanly
   *       deallocated and the internal level in the storage vector
   *       should have doped to zero again.
   */
  class FeedManifold_test : public Test
    {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #852
      using PSto = std::unique_ptr<BuffTableStorage>;
      
      PSto pStorage;
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #852
      ulong counter;
      
      void
      run(Arg)  override 
        {
          seedRand(); ////////////////TODO RLY?
           counter = 0;
           
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #852
           // allocate storage block to be used chunk wise
           pStorage.reset (new BuffTableStorage (TABLE_SIZ));
           
           invocation (0, detect_start_level(*pStorage));
           
           pStorage.reset(0);  // dtor throws assertion error if corrupted
           
           cout << "BuffTable chunks allocated: "<<counter<< "\n";
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #852
        }
      
      
      /** recurse down randomly
       *  until exhausting storage
       */
      void invocation (uint consumed, void* lastLevel)
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #833
          MockSizeRequest numbers;
          consumed += numbers.getNrI()+numbers.getNrO();
          if (TABLE_SIZ <= consumed)
            return; // end recursion
          
          ++counter;
          BuffTableChunk thisChunk (numbers, *pStorage);
          CHECK (consistencyCheck (thisChunk, numbers, lastLevel));
          
          uint nrBranches ( 1 + rani(WIDTH_MAX));
          while (nrBranches--)
            invocation (consumed, first_behind (thisChunk,numbers.getNrI()));
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #833
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (FeedManifold_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
