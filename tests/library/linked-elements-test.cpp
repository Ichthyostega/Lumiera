/*
  LinkedElements(Test)  -  verify the intrusive single linked list template

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file linked-elements-test.cpp
 ** unit test \ref LinkedElements_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/allocation-cluster.hpp"
#include "lib/linked-elements.hpp"
#include "lib/test/tracking-dummy.hpp"
#include "lib/iter-source.hpp"

#include <memory>



namespace lib {
namespace test{
  
  namespace error = lumiera::error;
  
  using util::isnil;
  using util::isSameObject;
  using LERR_(ITER_EXHAUST);
  
  
  namespace { // test data...
    
    LUMIERA_ERROR_DEFINE(PROVOKED_FAILURE, "provoked failure");
    
    const uint NUM_ELEMENTS = 500;
    int exception_trigger = -1;
    
    inline void __triggerErrorAt(int i) { exception_trigger = i; }
    inline void __triggerError_reset()  { exception_trigger =-1; }
    
    
    /**
     * Test-Element, supporting intrusive linked list storage.
     * Also tracks ctor/dtor calls by virtue of the Dummy baseclass.
     */
    struct Nummy
      : Dummy
      {
        Nummy* next;
        
        Nummy()
          : Dummy()
          , next{0}
          { }
        
        explicit
        Nummy (int i)
          : Dummy{i}
          , next{0}
          {
            if (i == exception_trigger)
              throw error::Fatal("simulated error", LUMIERA_ERROR_PROVOKED_FAILURE);
          }
      };
    
    
    /**
     * to demonstrate holding subclasses
     */
    template<uint I>
    struct Num
      : Nummy
      {
        void* storage[I];   // note size depends on template parameter
        
        Num (int i=0, int j=0, int k=0)
          : Nummy(I+i+j+k)
          { }
      };
    
    
    
    /**
     * Helper to produce a pre-determined series
     * of objects to populate a LinkedElements list.
     * @note just happily heap allocating new instances
     *       and handing them out. The LinkedElements list
     *       will take ownership of them and care for
     *       clean de-allocation.
     */
    class NummyGenerator
      : public IterSource<Nummy>
      {
        uint maxNum_;
        
        virtual Pos
        firstResult()
          {
            return new Nummy(1);
          }
        
        virtual void
        nextResult(Pos& num)
          {
            uint current = num->getVal();
            if (maxNum_ <= current)
              num = 0;
            else
              num = new Nummy(current+1);
          }
        
      public:
        NummyGenerator (uint maxElms)
          : maxNum_(maxElms)
          { }
      };
    
    /** Iterator-Frontend to generate this series of objects */
    class Populator
      : public NummyGenerator::iterator
      {
      public:
        explicit
        Populator (uint numElms)
          : NummyGenerator::iterator (
              NummyGenerator::build (new NummyGenerator(numElms)))
          { }
      };
    
    
    
    
    inline uint
    sum (uint n)
    {
      return n*(n+1) / 2;
    }
      
  }//(End) test data and helpers
  
  
  
  
  
  /// default case: ownership for heap allocated nodes
  using List = LinkedElements<Nummy>;
  
  /// managing existing node elements without taking ownership
  using ListNotOwner = LinkedElements<Nummy, linked_elements::NoOwnership>;
  
  
  
  /****************************************************************//**
   *  @test cover our custom single linked list template,
   *        in combination with Lumiera Forward Iterators
   *        and the usage of a custom allocator.
   */
  class LinkedElements_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          iterating();
          reverseList();
          
          verify_nonOwnership();
          verify_ExceptionSafety();
          populate_by_iterator();
          verify_RAII_safety();
          verify_customAllocator();
        }
      
      
      void
      simpleUsage()
        {
          CHECK (0 == Dummy::checksum());
          {
            List elements;
            CHECK (isnil (elements));
            CHECK (0 == elements.size());
            CHECK (0 == Dummy::checksum());
            
            elements.emplace<Nummy>(1);
            elements.emplace<Nummy>(2);
            elements.emplace<Nummy>(3);
            elements.emplace<Nummy>(4);
            elements.emplace<Nummy>(5);
            CHECK (!isnil (elements));
            CHECK (5 == elements.size());
            CHECK (0 != Dummy::checksum());
            
            CHECK (Dummy::checksum() == elements[0].getVal()
                                      + elements[1].getVal()
                                      + elements[2].getVal()
                                      + elements[3].getVal()
                                      + elements[4].getVal());
            
            elements.clear();
            CHECK (isnil (elements));
            CHECK (0 == elements.size());
            CHECK (0 == Dummy::checksum());
            
            elements.emplace<Nummy>();
            elements.emplace<Nummy>();
            elements.emplace<Nummy>();
            
            CHECK (3 == elements.size());
            CHECK (0 != Dummy::checksum());
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      void
      iterating()
        {
          CHECK (0 == Dummy::checksum());
          {
            List elements;
            for (uint i=1; i<=NUM_ELEMENTS; ++i)
              elements.emplace<Nummy>(i);
            
            // since elements where pushed,
            // they should appear in reversed order
            int check=NUM_ELEMENTS;
            List::iterator ii = elements.begin();
            while (ii)
              {
                CHECK (check == ii->getVal());
                CHECK (check == ii->calc(+5) - 5);
                --check;
                ++ii;
              }
            CHECK (0 == check);
            
            
            // Test the const iterator
            List const& const_elm (elements);
            check = NUM_ELEMENTS;
            List::const_iterator cii = const_elm.begin();
            while (cii)
              {
                CHECK (check == cii->getVal());
                --check;
                ++cii;
              }
            CHECK (0 == check);
            
            
            // Verify correct behaviour of iteration end
            CHECK (! (elements.end()));
            CHECK (isnil (elements.end()));
            
            VERIFY_ERROR (ITER_EXHAUST, *elements.end() );
            VERIFY_ERROR (ITER_EXHAUST, ++elements.end() );
            
            CHECK (ii == elements.end());
            CHECK (ii == List::iterator());
            CHECK (cii == elements.end());
            CHECK (cii == List::const_iterator());
            VERIFY_ERROR (ITER_EXHAUST, ++ii );
            VERIFY_ERROR (ITER_EXHAUST, ++cii );
            
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      void
      reverseList()
        {
          CHECK (0 == Dummy::checksum());
          {
            List list;
            CHECK (isnil (list));
            list.reverse();
            CHECK (isnil (list));
            CHECK (0 == Dummy::checksum());
            
            list.emplace<Nummy>(1);
            CHECK (not isnil (list));
            CHECK (1 == list[0].getVal());
            CHECK (1 == Dummy::checksum());
            list.reverse();
            CHECK (1 == Dummy::checksum());
            CHECK (1 == list[0].getVal());
            CHECK (not isnil (list));
            
            list.emplace<Nummy>(2);
            CHECK (not isnil (list));
            CHECK (2 == list.size());
            CHECK (2 == list[0].getVal());
            CHECK (2+1 == Dummy::checksum());
            list.reverse();
            CHECK (1+2 == Dummy::checksum());
            CHECK (1 == list[0].getVal());
            CHECK (2 == list.size());
            
            list.emplace<Nummy>(3);
            CHECK (3 == list.size());
            CHECK (3 == list.top().getVal());
            CHECK (3+1+2 == Dummy::checksum());
            list.reverse();
            CHECK (2 == list[0].getVal());
            CHECK (1 == list[1].getVal());
            CHECK (3 == list[2].getVal());
            List::iterator ii = list.begin();
            CHECK (2 == ii->getVal());
            ++ii;
            CHECK (1 == ii->getVal());
            ++ii;
            CHECK (3 == ii->getVal());
            ++ii;
            CHECK (isnil (ii));
            CHECK (2+1+3 == Dummy::checksum());

            list.emplace<Nummy>(4);
            CHECK (4 == list.top().getVal());
            CHECK (3 == list[3].getVal());
            list.reverse();
            CHECK (3 == list[0].getVal());
            CHECK (1 == list[1].getVal());
            CHECK (2 == list[2].getVal());
            CHECK (4 == list[3].getVal());
            CHECK (3+1+2+4 == Dummy::checksum());
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      /** @test add some node elements to the LinkedElements list
       *        but without taking ownership or performing any
       *        memory management. This usage pattern is helpful
       *        when the node elements are already managed elsewhere.
       *  @note we're still (intrusively) using the next pointer
       *        within the node elements. This means, that still
       *        a given node can't be member in multiple lists.
       */
      void
      verify_nonOwnership()
        {
          CHECK (0 == Dummy::checksum());
          {
            ListNotOwner elements;
            CHECK (isnil (elements));
            
            Num<22> n2;
            Num<44> n4;
            Num<66> n6;
            CHECK (22+44+66 == Dummy::checksum());
            
            elements.push(n2);
            elements.push(n4);
            elements.push(n6);
            CHECK (!isnil (elements));
            CHECK (3 == elements.size());
            CHECK (22+44+66 == Dummy::checksum());   // not altered: we're referring the originals
            
            CHECK (66 == elements[0].getVal());
            CHECK (44 == elements[1].getVal());
            CHECK (22 == elements[2].getVal());
            CHECK (isSameObject(n2, elements[2]));
            CHECK (isSameObject(n4, elements[1]));
            CHECK (isSameObject(n6, elements[0]));
            
            elements.clear();
            CHECK (isnil (elements));
            CHECK (22+44+66 == Dummy::checksum());   // referred elements unaffected
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      void
      verify_ExceptionSafety()
        {
          CHECK (0 == Dummy::checksum());
          {
            List elements;
            CHECK (isnil (elements));
            
            __triggerErrorAt(3);
            
            elements.emplace<Nummy>(1);
            elements.emplace<Nummy>(2);
            CHECK (1+2 == Dummy::checksum());
            
            VERIFY_ERROR (PROVOKED_FAILURE, elements.emplace<Nummy>(3) );
            CHECK (1+2 == Dummy::checksum());
            CHECK (2 == elements.size());
            
            CHECK (2 == elements[0].getVal());
            CHECK (1 == elements[1].getVal());
            
            elements.clear();
            CHECK (0 == Dummy::checksum());
            __triggerError_reset();
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      void
      populate_by_iterator()
        {
          CHECK (0 == Dummy::checksum());
          {
            Populator yieldSomeElements(NUM_ELEMENTS);
            List elements (yieldSomeElements);
            
            CHECK (!isnil (elements));
            CHECK (NUM_ELEMENTS == elements.size());
            CHECK (sum(NUM_ELEMENTS) == Dummy::checksum());
            
            int check=NUM_ELEMENTS;
            List::iterator ii = elements.begin();
            while (ii)
              {
                CHECK (check == ii->getVal());
                --check;
                ++ii;
              }
            CHECK (0 == check);
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      /** @test to support using LinkedElements within RAII-style components,
       *        all the elements might be added in one sway, by pulling them
       *        from a Lumiera Forward Iterator. In case this is done in the
       *        ctor, any exception while doing so will trigger cleanup
       *        of all elements (and then failure of the ctor altogether)
       */
      void
      verify_RAII_safety()
        {
          CHECK (0 == Dummy::checksum());
          
          __triggerErrorAt(3);
          Populator yieldSomeElements(NUM_ELEMENTS);
          VERIFY_ERROR (PROVOKED_FAILURE, List elements(yieldSomeElements) );
          
          CHECK (0 == Dummy::checksum());
          __triggerError_reset();
        }
      
      
      
          /** Policy to use an Allocation cluster,
           *  but also to invoke all object destructors */
          struct UseAllocationCluster
            {
              typedef AllocationCluster& CustomAllocator;
              
              CustomAllocator cluster_;
              
              UseAllocationCluster (CustomAllocator clu)
                : cluster_(clu)
                { }
              
              template<class TY, typename...ARGS>
              TY*
              create (ARGS&& ...args)
                {
                  return & cluster_.create<TY> (std::forward<ARGS> (args)...);
                }
              
              void dispose (void*) { /* does nothing */ }
            };
      
      /** @test use custom allocator to create list elements
       *      - a dedicated policy allows to refer to an existing AllocationCluster
       *        and to arrange for all object destructors to be called when this
       *        cluster goes out of scope
       *      - a C++ standard allocator can also be used; as an example, again an
       *        AllocationCluster is used, but this time with the default adapter,
       *        which places objects tight and skips invocation of destructors;
       *        however, since the LinkedElements destructor is called, it
       *        walks all elements and delegates through std::allocator_traits,
       *        which will invoke the (virtual) base class destructors.
       */
      void
      verify_customAllocator()
        {
          CHECK (0 == Dummy::checksum());
          {
            AllocationCluster cluster;
            
            LinkedElements<Nummy, UseAllocationCluster> elements(cluster);
            
            elements.emplace<Num<1>> (2);
            elements.emplace<Num<3>> (4,5);
            elements.emplace<Num<6>> (7,8,9);
            
            const size_t EXPECT = sizeof(Num<1>) + sizeof(Num<3>) + sizeof(Num<6>)
                                + 3*2*sizeof(void*);  // ◁┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄overhead for storing 3 dtor-invokers
            CHECK (EXPECT == cluster.numBytes());
            CHECK (sum(9) == Dummy::checksum());
            
            CHECK (3 == elements.size());
            CHECK (1+2 == elements[2].getVal());
            CHECK (3+4+5 == elements[1].getVal());
            CHECK (6+7+8+9 == elements[0].getVal());
            
            elements.clear();
            CHECK (EXPECT == cluster.numBytes());
            CHECK (sum(9) == Dummy::checksum());
            // note: elements won't be discarded unless
            //       the AllocationCluster goes out of scope
          }
          CHECK (0 == Dummy::checksum());
          {
            // now use AllocationCluster through the default allocator adapter...
            AllocationCluster cluster;
            using Allo = AllocationCluster::Allocator<Nummy>;
            using Elms = LinkedElements<Nummy, linked_elements::OwningAllocated<Allo>>;
            
            Elms elements{cluster.getAllocator<Nummy>()};
            
            elements.emplace<Num<1>> (2);
            elements.emplace<Num<3>> (4,5);
            
            const size_t EXPECT = sizeof(Num<1>) + sizeof(Num<3>);
            CHECK (EXPECT == cluster.numBytes());
            CHECK (sum(5) == Dummy::checksum());
            
            CHECK (2 == elements.size());
            CHECK (1+2 == elements[1].getVal());
            CHECK (3+4+5 == elements[0].getVal());
            // note: this time the destructors will be invoked
            //       from LinkedElements::clear(), but not from
            //       the destructor of AllocationCluster
          }
          CHECK (0 == Dummy::checksum());
        }
    };
  
  
  
  LAUNCHER (LinkedElements_test, "unit common");
  
  
}} // namespace lib::test
