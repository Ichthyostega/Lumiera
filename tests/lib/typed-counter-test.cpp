/*
  TypedCounter(Test)  -  managing a set of type based contexts
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
 ** <P>
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
#include "lib/test/test-helper.hpp"
#include "lib/typed-counter.hpp"
#include "lib/scoped-ptrvect.hpp"
#include "backend/thread-wrapper.hpp"
#include "lib/sync.hpp"
#include "lib/util.hpp"

#include <tr1/functional>
#include <vector>
#include <cstdlib>


namespace lib {
namespace test{
  
  
  using backend::Thread;
  using backend::JoinHandle;
  using util::for_each;
  using util::isnil;
  using std::tr1::placeholders::_1;
  using std::tr1::bind;
  using std::tr1::ref;
  using std::vector;
  using std::rand;
  
  
  namespace { // test data and helpers...
      
      const uint MAX_FAMILIES   = 20;  ///< maximum separate "families", each sharing a TypedCounter
      const uint MAX_MEMBERS    = 30;  ///< maximum members per family (member == test thread)
      const uint MAX_ITERATIONS = 50;  ///< maximum iterations within a single test thread
      const uint MAX_DELAY_ms   =  3;  ///< maximum delay between check iterations
      
      
      /** 
       * Interface to a family of dummy types
       */
      class DummyType
        {
        public:
          virtual ~DummyType() { }
          
          /** core test operation: do a random increment or decrement
           *  on the provided TypedCounter instance, and also save an
           *  account to a local embedded checksum for verification */
          virtual void doCount               (TypedCounter&)  =0;
          
          virtual void collect_externalCount (TypedCounter&)  =0;
          virtual void collect_internalCount ()               =0;
        };
      
      
      /* === Checksums === */
      
      long sum_TypedCounter_;    ///< Sum1: calculated from TypedCounter
      long sum_internal_;        ///< Sum2: control value calculated from Dummy::localChecksum_
      
      void
      accountExternal (DummyType& target, TypedCounter& counter_to_use)
      {
        target.collect_externalCount (counter_to_use);
      }
      void
      accountInternal (DummyType& target)
      {
        target.collect_internalCount();
      }
      
      
      /**
       * To actually drive the TypedCounter invocations, we need a family
       * of different (but of course related) types. Actually, we use these
       * subclasses here also to carry out the invocations and the accounting
       * to build up the checksums for verification.
       */
      template<uint i>
      class Dummy
        : public DummyType
        , public lib::Sync<>
        {
          long localChecksum_;
          
          void
          record_internal (int increment)
            {
              Lock protect(this);
              localChecksum_ += increment;
            }
          
          
          void
          doCount (TypedCounter& counter)
            {
              // note: deliberately *not* synchronised
              
              if (rand() % 2)
                {
                  counter.inc<Dummy>();
                  record_internal (+1);
                }
              else
                {
                  counter.dec<Dummy>();
                  record_internal (-1);
                }
            }
          
          void
          collect_externalCount (TypedCounter& counter)
            {
              // Lock not necessary, because of invocation sequence
              sum_TypedCounter_  += counter.get<Dummy>();
            }
          
          void
          collect_internalCount ()
            {
              sum_internal_ += localChecksum_;
            }
          
        public:
          Dummy() : localChecksum_(0) {}
        };
      
      
      
      
      
      
      /** 
       * Collection of target functions,
       * to be invoked during the test run
       */
      struct DummyTarget
        {
          typedef ScopedPtrVect<DummyType> TargetVect;
          
          TargetVect targets_;
          
          DummyTarget ()
            : targets_(10)
            {
              targets_.manage(new Dummy<0>);
              targets_.manage(new Dummy<1>);
              targets_.manage(new Dummy<2>);
              targets_.manage(new Dummy<3>);
              targets_.manage(new Dummy<4>);
              targets_.manage(new Dummy<5>);
              targets_.manage(new Dummy<6>);
              targets_.manage(new Dummy<7>);
              targets_.manage(new Dummy<8>);
              targets_.manage(new Dummy<9>);
            }
          
          
          /** entry point for the SingleCheck instances
           *  to trigger off a single invocation
           */
          void
          torture (TypedCounter& counter_to_use)
            {
              uint victim = (rand() % 10);
              targets_[victim].doCount (counter_to_use);
            }
          
          /** allow Iteration over all targets in the TargetVect */
          TargetVect::iterator begin() { return targets_.begin(); }
          TargetVect::iterator end()   { return targets_.end();   }
        };
      
      DummyTarget targetCollection; 
      
      
      
      /**
       * Each single check runs in a separate thread
       * and performs a random sequence of increments
       * and decrements on random targets. 
       */
      class SingleCheck
        : JoinHandle,
          Thread
        {
        public:
          SingleCheck (TypedCounter& counter_to_use)
            : Thread("TypedCounter_test worker Thread"
                    , bind (&SingleCheck::runCheckSequence, this, ref(counter_to_use), (rand() % MAX_ITERATIONS))
                    , (backend::JoinHandle&)*this 
                    )
            { }
          
         ~SingleCheck () { this->join(); }
          
          
        private:
          void runCheckSequence(TypedCounter& counter, uint iterations)
            {
              do
                {
                  usleep (1000 * (rand() % MAX_DELAY_ms));
                  targetCollection.torture (counter);
                }
              while (iterations--);
            }
        };
      
      
      /** 
       * Family of individual checks, sharing
       * a common TypedCounter instance.
       */
      struct TestFamily
        {
          TypedCounter ourCounter_;
          ScopedPtrVect<SingleCheck> checks_;
          
          TestFamily()
            : checks_(MAX_MEMBERS)
            {
              uint members (1 + rand() % MAX_MEMBERS);
              while (members--)
                checks_.manage (new SingleCheck (ourCounter_));
            }
          
         ~TestFamily()
            {
              checks_.clear(); // blocks until all test threads finished
              account();
            }
          
          void
          account()
            {
              for_each ( targetCollection
                       , bind (accountExternal, _1, ourCounter_)
                       );
            }
        };
      
      
      /** a series of independent context sets */
      typedef ScopedPtrVect<TestFamily> FamilyTable;
      
  }
  
  
  
  /***************************************************************************************
   * @test build multiple sets of type-based contexts and run a simple counting operation
   *       in each of them concurrently. Check the proper allocation of type-IDs in each
   *       context and verify correct counting operation by checksum.
   * 
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
      simpleUsageTest ()
        {
          TypedCounter myCounter;
          ASSERT (isnil (myCounter));
          ASSERT (0==myCounter.size());
          
          ASSERT (0 == myCounter.get<short>());
          ASSERT (1 == myCounter.size());
          
          ASSERT (0 == myCounter.get<long>());
          ASSERT (2 == myCounter.size());
          
          ASSERT (-1 == myCounter.dec<short>());
          ASSERT (-2 == myCounter.dec<short>());
          ASSERT (+1 == myCounter.inc<long>());
          
          ASSERT (-2 == myCounter.get<short>());
          ASSERT (+1 == myCounter.get<long>());
          
          
          ASSERT (1 == TypedContext<TypedCounter>::ID<short>::get());
          ASSERT (2 == TypedContext<TypedCounter>::ID<long>::get());
          ASSERT (2 == myCounter.size());
        }
      
      
      void 
      tortureTest () 
        {
          sum_TypedCounter_ = 0;
          sum_internal_     = 0;
          
          uint num_Families (1 + rand() % MAX_FAMILIES);
          
          FamilyTable testFamilies(num_Families);
          for (uint i=0; i<num_Families; ++i)
            testFamilies.manage (new TestFamily);
          
          testFamilies.clear(); // blocks until all threads have terminated
          
          for_each (targetCollection, accountInternal);
          ASSERT (sum_TypedCounter_ == sum_internal_);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypedCounter_test, "unit common");
  
  
}} // namespace lib::test
