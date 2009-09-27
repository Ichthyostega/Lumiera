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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
//#include "proc/control/command.hpp"
#include "lib/typed-counter.hpp"
//#include "proc/control/command-def.hpp"
//#include "lib/lumitime.hpp"
//#include "lib/symbol.hpp"
#include "lib/sync.hpp"
#include "lib/scoped-ptrvect.hpp"
#include "backend/thread-wrapper.hpp"
#include "lib/util.hpp"

#include <tr1/functional>

//#include <tr1/memory>
//#include <boost/ref.hpp>
//#include <boost/format.hpp>
//#include <iostream>
#include <vector>
#include <cstdlib>
//#include <string>


namespace lib {
namespace test{


//  using boost::format;
//  using boost::str;
  //using lumiera::Time;
  //using util::contains;
  using backend::Thread;
  using backend::JoinHandle;
//  using std::tr1::shared_ptr;
  using std::tr1::bind;
  using std::tr1::placeholders::_1;
//  using std::string;
  using std::vector;
  using std::rand;
  //using std::cout;
  //using std::endl;
//  using lib::test::showSizeof;
//  using util::isSameObject;
  using util::for_each;
//  using util::contains;

//  using session::test::TestClip;
//  using lib::Symbol;
//  using lumiera::P;
  
  
  //using lumiera::typelist::BuildTupleAccessor;
//  using lumiera::error::LUMIERA_ERROR_EXTERNAL;
  
  namespace { // test data and helpers...
      
      const uint MAX_FAMILIES   = 20;  ///< maximum separate "families", each sharing a TypedCounter
      const uint MAX_MEMBERS    =  5;  ///< maximum members per family (member == test thread)
      const uint MAX_ITERATIONS =  5;  ///< maximum iterations within a single test thread
      const uint MAX_DELAY_ms   = 50;  ///< maximum delay between check iterations
      
      
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
          TypedCounter& counterSet_;
          long localChecksum_;
          
        public:
          SingleCheck (TypedCounter& counter_to_use)
            : Thread("TypedCounter_test worker Thread",
                     bind (&SingleCheck::runCheckSequence, this, (rand() % MAX_ITERATIONS)),
                     (backend::JoinHandle&)*this 
                    )
            , counterSet_(counter_to_use)
            , localChecksum_(0)
            { }
          
         ~SingleCheck () { this->join(); }
          
          
        private:
          void runCheckSequence(uint iterations)
            {
              while (--iterations)
                {
                  usleep (1000 * (rand() % MAX_DELAY_ms));
                  targetCollection.torture (counterSet_);
            }   }
        };
      
      
      /** 
       * Family of individual checks, sharing
       * a common TypedCounter instance.
       */
      struct TestFamily
        {
          ScopedPtrVect<SingleCheck> checks_;
          TypedCounter ourCounter_;
          
          TestFamily()
            : checks_(MAX_MEMBERS)
            {
              uint members (1 + rand() % MAX_MEMBERS);
              while (members--)
                checks_.manage (new SingleCheck (ourCounter_));
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
   * @todo using currently (9/09) a simple implementation based on static variables.
   *       This causes waste of storage for the type-based tables. It would be better
   *       if each set would build it's own Type-IDs. Ticket #
   *        
   * @see TypedAllocationManager
   * @see typed-counter.hpp
   */
  class TypedCounter_test : public Test
    {
      
      void
      run (Arg) 
        {
          sum_TypedCounter_ = 0;
          sum_internal_     = 0;
          
          uint num_Families (1 + rand() % MAX_FAMILIES);
          
          FamilyTable testFamilies(num_Families);
          for (uint i=0; i<num_Families; ++i)
            testFamilies.manage (new TestFamily);
          
          testFamilies.clear(); // blocks until all threads have terminated
          
          for (uint i=0; i<num_Families; ++i)
            testFamilies[i].account();
          for_each (targetCollection, accountInternal);
          
          ASSERT (sum_TypedCounter_ == sum_internal_);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypedCounter_test, "unit common");
  
  
}} // namespace lib::test
