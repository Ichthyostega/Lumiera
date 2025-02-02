/*
  SeveralBuilder(Test)  -  building a limited fixed collection of elements

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file several-builder-test.cpp
 ** unit test \ref SeveralBuilder_test
 */


#include "lib/test/run.hpp"
#include "lib/test/tracking-dummy.hpp"
#include "lib/test/tracking-allocator.hpp"
#include "lib/test/test-coll.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/allocation-cluster.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include "lib/several-builder.hpp"

#include <array>

using ::test::Test;
using std::array;

using lib::explore;
using util::isLimited;
using util::toString;
using util::isnil;
using util::join;


namespace lib {
namespace test{
  
  using LERR_(INDEX_BOUNDS);
  
  namespace { // invocation tracking diagnostic subclass...
    
    /**
     * Instance tracking sub-dummy
     * - implements the Dummy interface
     * - holds additional storage
     * - specific implementation of the virtual operation
     * - includes content of the additional storage into the
     *   checksum calculation, allowing to detect memory corruption
     */
    template<uint i>
    class Num
      : public test::Dummy
      {
        std::array<int,i> ext_;
        
      public:
        Num (uint seed=i)
          : Dummy(seed)
          {
            ext_.fill(seed);
            setVal ((i+1)*seed);
          }
       ~Num()
          {
            setVal (getVal() - explore(ext_).resultSum());
          }
        
        long
        calc (int ii)  override
          {
            return i+ii + explore(ext_).resultSum();
          }
        
        /// allow for move construction
        Num (Num && oNum)  noexcept
          : Num(0)
          {
            swap (*this, oNum);
          }
        Num&
        operator= (Num && oNum)
          {
            if (&oNum != this)
              swap (*this, oNum);
            return *this;
          }
        
        friend void
        swap (Num& num1, Num& num2)  ///< checksum neutral
        {
          std::swap (static_cast<Dummy&> (num1)
                    ,static_cast<Dummy&> (num2));
          std::swap (num1.ext_, num2.ext_);
        }
      };
    
    
    /**
     * A non-copyable struct with 16bit alignment
     * - not trivially default constructible
     * - but trivially destructible
     */
    struct ShortBlocker
      : util::NonCopyable
      {
        int16_t val;
        
        ShortBlocker (short r = 1 + rani(1'000))
          : val(r)
          { };
      };
    
  } // (END) test types
  
  
  
  
  
  
  
  /***************************************************************//**
   * @test use lib::Several to establish small collections of elements,
   *       possibly with sub-classing and controlled allocation.
   *     - the container is populated through a separate builder
   *     - the number of elements is flexible during population
   *     - the actual container allows random-access via base interface
   * @see several-builder.hpp
   */
  class SeveralBuilder_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          
          simpleUsage();
          check_Builder();
          check_ErrorHandling();
          check_ElementStorage();
          check_CustomAllocator();
        }
      
      
      
      /** @test demonstrate basic behaviour
       */
      void
      simpleUsage()
        {
          auto elms = makeSeveral({1,1,2,3,5,8,13}).build();
          CHECK (elms.size() == 7);
          CHECK (elms.back() == 13);
          CHECK (elms[3] == 3);
          CHECK (join (elms,"-") == "1-1-2-3-5-8-13"_expect);
          CHECK (toString(elms)  == "[1, 1, 2, 3, 5, 8, 13]"_expect);
        }
      
      
      /** @test various ways to build an populate the container
       *      - with a defined interface type \a I, instances of arbitrary subclasses
       *        can be added, assuming there is sufficient pre-allocated buffer space;
       *        all these subclass instances are accessed through the common interface.
       *      - yet the added elements can also be totally unrelated, in which case an
       *        *unchecked wild cast* will happen on access; while certainly dangerous,
       *        this behaviour allows for special low-level data layout tricks.
       *      - the results from an iterator can be used to populate by copy
       */
      void
      check_Builder()
        {
          // prepare to verify proper invocation of all constructors / destructors
          Dummy::checksum() = 0;
          
          { // Scenario-1 : Baseclass and arbitrary subclass elements
            SeveralBuilder<Dummy> builder;
            CHECK (isnil (builder));
            
            builder.emplace<Num<3>>()
                   .emplace<Num<2>>(1);
            CHECK (2 == builder.size());                                 // use information functions...
            CHECK (3 == builder[1].getVal());                            // to peek into contents assembled thus far...
            VERIFY_ERROR (INDEX_BOUNDS, builder[2] );                    // runtime bounds check on the builder (but not on the product!)
            builder.fillElm(2);
            CHECK (4 == builder.size());
            builder.fillElm(3, 5);
            CHECK (7 == builder.size());
            
            Several<Dummy> elms = builder.build();
            CHECK (    isnil(builder));
            CHECK (not isnil(elms));
            CHECK (7 == elms.size());
            CHECK (elms[0].getVal() == (3+1)*3);                         // indeed a Num<3> with default-seed ≡ 3
            CHECK (elms[0].calc(1)  == 3 + 1 + (3+3+3));                 // indeed called the overridden calc() operation
            CHECK (elms[1].getVal() == (2+1)*1);                         // indeed a Num<2> with seed ≡ 1
            CHECK (elms[1].calc(1)  == 2 + 1 + (1+1));                   // indeed the overridden calc() picking from the Array(1,1)
            CHECK (isLimited (1, elms[2].getVal(), 100'000'000));        // indeed a Dummy with default random seed
            CHECK (isLimited (1, elms[3].getVal(), 100'000'000));        // and this one too, since we filled in two instances
            CHECK (elms[4].getVal() == 5);                               // followed by tree instances Dummy(5)
            CHECK (elms[5].getVal() == 5);
            CHECK (elms[6].getVal() == 5);
            CHECK (elms[6].calc(1)  == 5+1);                             // indeed invoking the base implementation of calc()
          }
          
          { // Scenario-2 : unrelated element types
            SeveralBuilder<uint32_t> builder;
            
            auto urgh = array<char,5>{"Urgh"};
            auto phi  = (1+sqrtf(5))/2;
            
            builder.append (urgh, phi, -1);                              // can emplace arbitrary data
            CHECK (3 == builder.size());
            
            Several<uint32_t> elms = builder.build();                    // WARNING: data accessed by wild cast to interface type
            CHECK (3 == elms.size());
            CHECK (elms[0] == * reinterpret_cast<const uint32_t*> ("Urgh"));
            CHECK (elms[1] == * reinterpret_cast<uint32_t*> (&phi));
            CHECK (elms[2] == uint32_t(-1));
          }
          
          { // Scenario-3 : copy values from iterator
            SeveralBuilder<int> builder;
            
            VecI seq = getTestSeq_int<VecI> (10);
            builder.appendAll (seq);
            CHECK (10 == builder.size());
            
            auto elms = builder.build();
            CHECK (10 == elms.size());
            CHECK (join (elms,"-") == "0-1-2-3-4-5-6-7-8-9"_expect);
          }
          
          CHECK (0 == Dummy::checksum());
        }
      
      
      /** @test proper handling of exceptions during population
       *      - when the container is filled with arbitrary subclasses
       *        of a base interface with virtual destructor, the first element is used
       *        to accommodate the storage spread; larger elements or elements of a completely
       *        different type can not be accommodated and the container can not grow beyond
       *        the initially allocated reserve (defined to be 10 elements by default).
       *      - when the container is defined to hold elements of a specific fixed subclass,
       *        it can be filled with default-constructed instances, and the initial allocation
       *        can be expanded by move-relocation. Yet totally unrelated elements can not be
       *        accepted (due to unknown destructor); and when accepting another unspecific
       *        subclass instance, the ability to grow by move-relocation is lost.
       *      - a container defined for trivial data elements (trivially movable and destructible)
       *        can grow dynamically just by moving data around with `memmove`. Only in this case
       *        the _element spread_ can easily be adjusted after the fact, since a trivial element
       *        can be relocated to accommodate an increased spread. It is possible to add various
       *        different data elements into such a container, yet all will be accessed through an
       *        unchecked hard cast to the base element (`uint8_t` in this case). However, once we
       *        add a _non-copyable_ element, this capability for arbitrarily moving elements around
       *        is lost — we can not adapt the spread any more and the container can no longer
       *        grow dynamically.
       *      - all these failure conditions are handled properly, including exceptions emanating
       *        from element constructors; the container remains sane and no memory is leaked.
       */
      void
      check_ErrorHandling()
        {
          CHECK (0 == Dummy::checksum());
          
          { // Scenario-1 : Baseclass and arbitrary subclass elements
            SeveralBuilder<Dummy> builder;
            
            // The first element will _prime_ the container for a suitable usage pattern
            builder.emplace<Num<1>>();
            CHECK (1 == builder.size());
            
            // Notably the first element established the _spread_ between index positions,
            // which effectively limits the size of objects to be added. Moreover, since
            // the element type was detected to be non-trivial, we can not correct this
            // element spacing by shifting existing allocations (`memmove()` not possible)
            CHECK (sizeof(Num<1>) < sizeof(Num<5>));
            VERIFY_FAIL ("Unable to place element of type Num<5u> (size="
                        , builder.emplace<Num<5>>() );
            CHECK (1 == builder.size());
            
            // Furthermore, the first element was detected to be a subclass,
            // and the interface type `Dummy` has a virtual destructor;
            // all added elements must comply to this scheme, once established
            VERIFY_FAIL ("Unable to handle (trivial-)destructor for element type long, "
                         "since this container has been primed to use virtual-baseclass-destructors."
                        , builder.emplace<long>(55) );
            CHECK (1 == builder.size());
            
            // the initial allocation added some reserve buffer space (for 10 elements)
            // and we can fill that space with arbitrary subclass instances
            builder.fillElm (5);
            CHECK (6 == builder.size());
            
            // But the initial allocation can not be increased, since that would require
            // a re-allocation of a larger buffer, followed by copying the elements;
            // but since the established scheme allows for _arbitrary_ subclasses,
            // the builder does not know the exact type for safe element relocation.
            VERIFY_FAIL ("Several-container is unable to accommodate further element of type Dummy"
                        , builder.fillElm (20) );
            CHECK (10 == builder.size());
          }
          // in spite of all the provoked failures,
          // all element destructors were invoked
          CHECK (0 == Dummy::checksum());
          
          
          { // Scenario-2 : Baseclass and elements of a single fixed subclass
            SeveralBuilder<Dummy, Num<5>> builder;
            
            builder.fillElm(5);
            CHECK (5 == builder.size());
            
            // trigger re-alloc by moving into larger memory block
            builder.fillElm(14);
            CHECK (19 == builder.size());
            CHECK (builder.size() > INITIAL_ELM_CNT);
            
            // with the elements added thus far, this instance has been primed to
            // rely on a fixed well known element type for move-growth and to use
            // the virtual base class destructor for clean-up. It is thus not possible
            // to add another element that is not related to this baseclass...
            VERIFY_FAIL ("Unable to handle (trivial-)destructor for element type ShortBlocker, "
                         "since this container has been primed to use virtual-baseclass-destructors."
                        , builder.emplace<ShortBlocker>() );
            CHECK (19 == builder.size());
            
            CHECK (sizeof(ShortBlocker) < sizeof(Num<5>)); // it was not rejected due to size...
            
            // However, a subclass /different than the defined element type/ is acceptable,
            // but only to the condition to lock any further container growth by move-reallocation.
            // The rationale is that we can still destroy through the virtual base destructor,
            // but we aren't able to move elements safely any more, since we don't capture the type.
            builder.emplace<Num<1>>();
            CHECK (20 == builder.size());
            CHECK (20 == builder.capacity());
            CHECK ( 0 == builder.capReserve());
            
            // But here comes the catch: since we choose to accept arbitrary sub-types not identified in detail,
            // the container has lost its ability of move-reallocation; with 20 elements the current reserve
            // is exhausted and we are now unable to add any further elements beyond that point.
            VERIFY_FAIL ("unable to move elements of mixed unknown detail type, which are not trivially movable"
                        , builder.fillElm(5); );
            
            // the container is still sound however
            auto elms = builder.build();
            CHECK (20 == elms.size());
            // verify that member fields were not corrupted
            for (uint i=0; i<=18; ++i)
              CHECK (elms[i].calc(i) == 5 + i + (5+5+5+5+5));
            CHECK (elms.back().calc(0) == 1 + 0 + (1));
          }
          CHECK (0 == Dummy::checksum());
          
          
          { // Scenario-3 : arbitrary elements of trivial type
            SeveralBuilder<uint8_t> builder;
            
            builder.reserve(16);
            CHECK ( 0 == builder.size());
            CHECK (16 == builder.capacity());
            CHECK (16 == builder.capReserve());
            
            string BFR{"starship is"};
            builder.appendAll (BFR);
            CHECK (11 == builder.size());
            CHECK (16 == builder.capacity());
            CHECK ( 5 == builder.capReserve());
            
            // append element that is much larger than a char
            // => since elements are trivial, they can be moved to accommodate
            builder.append (int64_t(32));
            CHECK (12 == builder.size());
            CHECK (16 == builder.capacity());       // note: capacity remained nominally the same
            CHECK ( 4 == builder.capReserve());    //        while in fact the spread and thus the buffer were increased
            
            // emplace a completely unrelated object type,
            // which is also trivially destructible, but non-copyable
            builder.emplace<ShortBlocker> ('c');
            
            // can emplace further trivial objects, since there is still capacity left
            builder.append (int('o'), long('o'));
            CHECK (15 == builder.size());
            CHECK ( 1 == builder.capReserve());
            
            VERIFY_FAIL ("Unable to place element of type Num<5u>"
                        , builder.append (Num<5>{}) );
            CHECK (sizeof(Num<5>) > sizeof(int64_t));
            // not surprising: this one was too large,
            // and due to the non-copyable element we can not adapt anymore
            
            class NonTrivial
              {
              public:
               ~NonTrivial() { }
              };
            
            // adding data of a non-trivial type is rejected,
            // since the container does not capture individual element types
            // and thus does not know how to delete it
            CHECK (sizeof(NonTrivial) <= sizeof(int64_t));
            VERIFY_FAIL ("Unsupported kind of destructor for element type SeveralBuilder_test::check_ErrorHandling()::NonTrivial"
                        , builder.append (NonTrivial{}) );
            CHECK ( 1 == builder.capReserve());
            
            // space for a single one left...
            builder.append ('l');
            CHECK (16 == builder.size());
            CHECK ( 0 == builder.capReserve());
            
            // and now we've run out of space, and due to the non-copyable object, move-relocation is rejected
            VERIFY_FAIL ("Several-container is unable to accommodate further element of type char; "
                         "storage reserve (128 bytes ≙ 16 elms) exhausted and unable to move "
                         "elements of mixed unknown detail type, which are not trivially movable."
                        , builder.append ('!') );
            
            // yet the container is still fine....
            auto elms = builder.build();
            CHECK (16 == elms.size());
            CHECK (join(elms, "·") == "s·t·a·r·s·h·i·p· ·i·s· ·c·o·o·l"_expect);
          }
          CHECK (0 == Dummy::checksum());
          
          { // Scenario-4 : exception from element constructor
            SeveralBuilder<Dummy> builder;
            
            builder.emplace<Num<3>>(42);
            CHECK (1 == builder.size());
            
            Dummy::activateCtorFailure(true);
            try {
                builder.emplace<Num<3>>(23);
                NOTREACHED ("did not throw");
              }
            catch(...)
              {
                // Exception emanated from Dummy(baseclass) ctor;
                // at that point, the local val was set to the seed (≙23).
                // When a constructor fails in C++, the destructor is not called,
                // thus we have to compensate here to balance the checksum
                Dummy::checksum() -= 23;
              }
            CHECK (1 == builder.size());
            Dummy::activateCtorFailure(false);
            builder.emplace<Num<3>>(23);
            
            auto elms = builder.build();
            CHECK (2 == elms.size());
            CHECK (elms.front().calc(1)  == 3 + 1 + (42+42+42));
            CHECK (elms.back().calc(5)   == 3 + 5 + (23+23+23));
          }
          // all other destructors properly invoked...
          CHECK (0 == Dummy::checksum());
        }
      
      
      
      /** @test verify correct placement of instances within storage
       *      - use a low-level pointer calculation for this test to
       *        draw conclusions regarding the spacing of objects accepted
       *        into the lib::Several-container
       *      - demonstrate the simple data elements are packed efficiently
       *      - verify that special alignment requirements are observed
       *      - emplace several ''non copyable objects'' and then
       *        move-assign the lib::Several container instance; this
       *        demonstrates that the latter is just a access front-end,
       *        while the data elements reside in a fixed storage buffer
       */
      void
      check_ElementStorage()
        {
          auto loc        = [](auto& something){ return util::addrID (something);         };
          auto calcSpread = [&](auto& several){ return loc(several[1]) - loc(several[0]); };
          
          { // Scenario-1 : tightly packed values
            Several<int> elms = makeSeveral({21,34,55}).build();
            CHECK (21 == elms[0]);
            CHECK (34 == elms[1]);
            CHECK (55 == elms[2]);
            CHECK (3 == elms.size());
            CHECK (sizeof(elms) == sizeof(void*));
            
            CHECK (sizeof(int) == alignof(int));
            size_t spread = calcSpread (elms);
            CHECK (spread == sizeof(int));
            CHECK (loc(elms.back()) == loc(elms.front()) + 2*spread);
          }
          
          { // Scenario-2 : alignment
            struct Ali
              {
                alignas(64)
                  char charm = 'u';
              };
            
            auto elms = makeSeveral<Ali>().fillElm(5).build();
            CHECK (5 == elms.size());
            CHECK (sizeof(elms) == sizeof(void*));
            
            size_t spread = calcSpread (elms);
            CHECK (spread == alignof(Ali));
            CHECK (loc(elms.front()) % alignof(Ali) == 0);
            CHECK (loc(elms.back()) == loc(elms.front()) + 4*spread);
          }
          
          { // Scenario-3 : noncopyable objects
            auto elms = makeSeveral<ShortBlocker>().fillElm(5).build();
            
            auto v0 = elms[0].val;  auto p0 = loc(elms[0]);
            auto v1 = elms[1].val;  auto p1 = loc(elms[1]);
            auto v2 = elms[2].val;  auto p2 = loc(elms[2]);
            auto v3 = elms[3].val;  auto p3 = loc(elms[3]);
            auto v4 = elms[4].val;  auto p4 = loc(elms[4]);
            
            CHECK (5 == elms.size());
            auto moved = move(elms);
            CHECK (5 == moved.size());
            CHECK (loc(elms) != loc(moved));
            CHECK (isnil (elms));
            
            CHECK (loc(moved[0]) == p0);
            CHECK (loc(moved[1]) == p1);
            CHECK (loc(moved[2]) == p2);
            CHECK (loc(moved[3]) == p3);
            CHECK (loc(moved[4]) == p4);
            
            CHECK (moved[0].val == v0);
            CHECK (moved[1].val == v1);
            CHECK (moved[2].val == v2);
            CHECK (moved[3].val == v3);
            CHECK (moved[4].val == v4);
            
            CHECK (calcSpread(moved) == sizeof(ShortBlocker));
          }
        }
      
      
      
      /** @test demonstrate integration with a custom allocator
       *      - use the TrackingAllocator to verify balanced handling
       *        of the underlying raw memory allocations
       *      - use an AllocationCluster instance to manage the storage
       */
      void
      check_CustomAllocator()
        {
          // Setup-1: use the TrackingAllocator
          CHECK (0 == Dummy::checksum());
          CHECK (0 == TrackingAllocator::checksum());
          
          Several<Dummy> elms;
          size_t expectedAlloc;
          CHECK (0 == TrackingAllocator::numAlloc());
          CHECK (0 == TrackingAllocator::use_count());
          {
            auto builder = makeSeveral<Dummy>()
                              .withAllocator<test::TrackAlloc>()
                              .fillElm(55);
            
            size_t elmSiz = sizeof(Dummy);
            size_t buffSiz = elmSiz * builder.capacity();
            size_t headerSiz = sizeof(ArrayBucket<Dummy>);
            expectedAlloc = headerSiz + buffSiz;
            
            CHECK (TrackingAllocator::numBytes() == expectedAlloc);
            CHECK (TrackingAllocator::numAlloc() == 1);
            CHECK (TrackingAllocator::use_count()== 2);              // one instance in the builder, one in the deleter
            CHECK (TrackingAllocator::checksum() > 0);
            
            elms = builder.build();
          }
          CHECK (elms.size() == 55);
          CHECK (TrackingAllocator::numBytes() == expectedAlloc);
          CHECK (TrackingAllocator::numAlloc() == 1);
          CHECK (TrackingAllocator::use_count()== 1);                // only one allocator instance in the deleter left
          
          auto others = move(elms);
          CHECK (elms.size() == 0);
          CHECK (others.size() == 55);
          CHECK (TrackingAllocator::numBytes() == expectedAlloc);
          CHECK (TrackingAllocator::numAlloc() == 1);
          CHECK (TrackingAllocator::use_count()== 1);
          
          others = move(Several<Dummy>{});                           // automatically triggers de-allocation
          CHECK (others.size() == 0);
          
          CHECK (0 == Dummy::checksum());
          CHECK (0 == TrackingAllocator::numBytes());
          CHECK (0 == TrackingAllocator::numAlloc());
          CHECK (0 == TrackingAllocator::use_count());
          CHECK (0 == TrackingAllocator::checksum());
          
          
          
          {// Setup-2: use an AllocationCLuster instance
            AllocationCluster clu;
            size_t allotted = clu.numBytes();
            CHECK (allotted == 0);
            {
              auto builder = makeSeveral<Dummy>()
                                .withAllocator(clu)
                                .reserve(4)                          // use a limited pre-reservation
                                .fillElm(4);                         // fill all the allocated space with 4 new elements
              
              size_t buffSiz = sizeof(Dummy) * builder.capacity();
              size_t headerSiz = sizeof(ArrayBucket<Dummy>);
              expectedAlloc = headerSiz + buffSiz;
              CHECK (4 == builder.size());
              CHECK (4 == builder.capacity());
              CHECK (1 == clu.numExtents());                         // AllocationCluster has only opened one extent thus far
              CHECK (expectedAlloc == clu.numBytes());               // and the allocated space matches the demand precisely

              builder.append (Dummy{23});                            // now request to add just one further element
              CHECK (8 == builder.capacity());                       // ...which causes the builder to double up the reserve capacity

              buffSiz = sizeof(Dummy) * builder.capacity();
              expectedAlloc = headerSiz + buffSiz;
              CHECK (1 == clu.numExtents());                         // However, AllocationCluster was able to adjust allocation in-place
              CHECK (expectedAlloc == clu.numBytes());               // and thus the new increased buffer is still placed in the first extent

              // perform another unrelated allocation
              Dummy& extraDummy = clu.create<Dummy>(55);
              CHECK (1 == clu.numExtents());
              CHECK (clu.numBytes() > expectedAlloc+sizeof(Dummy));  // but now we've used some further space behind that point
              
              builder.reserve(9);                                    // ...which means that the AllocationCluster can no longer adjust dynamically
              CHECK (5 == builder.size());                           // .....because this is only possible on the latest allocation opened
              CHECK (9 <= builder.capacity());                       // And while we still got the increased capacity as desired,
              CHECK (2 == clu.numExtents());                         // this was only possible by wasting space and copying into a new extent
              buffSiz = sizeof(Dummy) * builder.capacity();
              expectedAlloc = headerSiz + buffSiz;
              CHECK (expectedAlloc <= AllocationCluster::max_size());
              CHECK (clu.numBytes() == AllocationCluster::max_size()
                                     + expectedAlloc);
              
              allotted = clu.numBytes();
              // request to throw away excess reserve
              builder.shrinkFit();
              CHECK (5 == builder.size());
              CHECK (5 == builder.capacity());
              CHECK (allotted > clu.numBytes());                     // dynamic adjustment was possible, since it is the latest allocation
              allotted = clu.numBytes();
              
              elms = builder.build();                                // Note: assigning to the existing front-end (which is storage agnostic)
              CHECK (5 == elms.size());
              CHECK (23 == elms.back().getVal());
              CHECK (55 == extraDummy.getVal());
            }                                                        // Now the Builder and the ExtraDummy is gone...
            CHECK (5 == elms.size());                                // while all created elements are still there, sitting in the Allocationcluster
            CHECK (23 == elms.back().getVal());
            CHECK (2 == clu.numExtents());
            CHECK (clu.numBytes() == allotted);
            
            CHECK (Dummy::checksum() > 0);
            elms = move(Several<Dummy>{});
            CHECK (Dummy::checksum() == 55);                         // all elements within Several were cleaned-up...
            CHECK (2 == clu.numExtents());                           // but the base allocation itself lives as long as the AllocationCluster
            CHECK (clu.numBytes() == allotted);
          }// AllocationCluster goes out of scope...
          CHECK (Dummy::checksum() == 0);                            // now the (already unreachable) extraDummy was cleaned up
        }                                                            // WARNING: contents in Several would now be dangling (if we haden't killed them)
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SeveralBuilder_test, "unit common");
  
  
  
}} // namespace lib::test
