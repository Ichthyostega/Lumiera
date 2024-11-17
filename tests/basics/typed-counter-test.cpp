/*
  TypedCounter(Test)  -  managing a set of type based contexts

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/


/** @file typed-counter-test.cpp
 ** Stress test to verify type-based contexts.
 ** Besides a simple usage (unit) test, this test performs a massively multithreaded
 ** test of the type-based contexts, through use of the TypedCounter. The idea behind
 ** this facility is to provide a context, in which type-IDs can be allocated. In the
 ** case of the TypedCounter, these type-IDs are used to index into a vector of counters,
 ** this way allowing to access a counter for a given type.
 ** 
 ** This test builds several "families", each sharing a TypedCounter. Each of these
 ** families runs a set of member threads, which concurrently access the TypedCounter of
 ** this family. After waiting for all threads to finish, we compare the checksum built
 ** within the target objects with the checksum collected through the TypedCounters.
 ** 
 ** @see thread-wrapper-test.cpp
 ** @see thread-wrapper-join-test.cpp
 **
 */



#include "lib/test/run.hpp"
#include "lib/typed-counter.hpp"
#include "lib/test/microbenchmark.hpp"
#include "lib/random.hpp"
#include "lib/util.hpp"

#include <cstdlib>
#include <utility>
#include <array>


namespace lib {
namespace test{
  
  using util::isnil;
  using lib::rani;
  
  
  namespace { // test parametrisation...
    const uint MAX_INDEX      =    10;  ///< number of distinct types / counters
    const uint NUM_THREADS    =   100;  ///< number of threads to run in parallel
    const uint NUM_ITERATIONS = 10000;  ///< number of repeated random accesses per Thread
  }
  
  
  
  /***********************************************************************************//**
   * @test verify the TypedCounter, which allows to maintain a counter-per-type.
   *       - demonstrate behaviour
   *       - concurrent test
   * @see TypedAllocationManager
   * @see typed-counter.hpp
   */
  class TypedCounter_test : public Test
    {
      
      void
      run (Arg)
        {
          simpleUsageTest();
          tortureTest();
        }
      
      
      void
      simpleUsageTest()
        {
          TypedCounter myCounter;
          CHECK (isnil (myCounter));
          CHECK (0==myCounter.size());
          
          CHECK (0 == myCounter.get<short>());
          CHECK (0 <  myCounter.size());
          // probably greater than 1;
          // other parts of the application allocate type-IDs as well
          
          // now allocate a counter for a type not seen yet
          struct X { };
          struct U { };
          
          CHECK (0 == myCounter.get<X>());
          size_t sX = myCounter.size();
          
          CHECK (0 == myCounter.get<U>());
          CHECK (sX + 1 == myCounter.size());
          CHECK (0 == myCounter.get<X>());
          CHECK (sX + 1 == myCounter.size());
          
          CHECK (-1 == myCounter.dec<X>());
          CHECK (-2 == myCounter.dec<X>());
          CHECK (+1 == myCounter.inc<U>());
          
          CHECK (-2 == myCounter.get<X>());
          CHECK (+1 == myCounter.get<U>());
          
          // each new type has gotten a new "slot" (i.e. a distinct type-ID)
          IxID typeID_short = TypedContext<TypedCounter>::ID<short>::get();
          IxID typeID_X     = TypedContext<TypedCounter>::ID<X>::get();
          IxID typeID_U     = TypedContext<TypedCounter>::ID<U>::get();
          
          CHECK (0 < typeID_short);
          CHECK (0 < typeID_X);
          CHECK (0 < typeID_U);
          CHECK (typeID_short < typeID_X);
          CHECK (typeID_X < typeID_U);
          // type-IDs are allocated in the order of first usage
          
          CHECK (sX + 1 == myCounter.size());
        }
      
      
      
      /** parametrised marker type to designate a counter to be incremented */
      template<size_t i>
      struct Dummy { };
      
      template<size_t i>
      static void
      increment (TypedCounter& counter)
      {
        counter.inc<Dummy<i>>();
      }

      /**
       * Helper for #tortureTest():
       * Build a table of functors, where the i-th entry invokes the function
       * increment<i>(), which leads to incrementing the counter for Dummy<i>.
       */
      template<size_t...I>
      static auto
      buildOperatorsTable(std::index_sequence<I...>)
      {
        using Operator = void(*)(TypedCounter&);
        return std::array<Operator, MAX_INDEX>{increment<I>...};
      }
      
      template<size_t...I>
      static size_t
      sumAllCounters(TypedCounter& counter, std::index_sequence<I...>)
      {
        return (counter.get<Dummy<I>>() + ... );
      }
      
      
      
      /**
       * @test verify TypedCounter concurrency safety
       *       - use a set of types `Dummy<i>` to access a corresponding counter
       *       - run a large number of threads in parallel, each incrementing
       *         a randomly picked counter; this is achieved by using a table
       *         of »increment operators«, where each one is tied to a specific
       *         Dummy<i>.
       */
      void
      tortureTest()
        {
          seedRand();
          
          using IDX = std::make_index_sequence<MAX_INDEX>;
          auto operators = buildOperatorsTable(IDX{});
          
          TypedCounter testCounter;
          
          auto testSubject = [&
                             ,gen = makeRandGen()]
                             (size_t) mutable -> size_t
                                {
                                  uint i = gen.i(MAX_INDEX);
                                  operators[i](testCounter);
                                  return 1;
                                };
          
          threadBenchmark<NUM_THREADS> (testSubject, NUM_ITERATIONS);
          
          size_t expectedIncrements = NUM_THREADS * NUM_ITERATIONS;
          CHECK (sumAllCounters(testCounter, IDX{}) == expectedIncrements);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypedCounter_test, "unit common");
  
  
}} // namespace lib::test
