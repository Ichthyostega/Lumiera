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



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/linked-elements.hpp"
#include "lib/test/testdummy.hpp"

//#include <cstdlib>


namespace lib {
namespace test{
  
  namespace error = lumiera::error;
  
  
  namespace { // test data...
    
    const uint NUM_ELEMENTS = 500;
    
    LUMIERA_ERROR_DEFINE(SUBVERSIVE, "undercover action");
    
    class Nummy
      : public Dummy
      {
        
      public:
        Nummy* next;
        
        Nummy() 
          : Dummy()
          , next(0)
          { }
        
        explicit
        Nummy (int i)
          : Dummy(i)
          , next(0)
          { }
      };
    
    
    inline uint
    sum (uint n)
    {
      return n*(n+1) / 2;
    }
      
  }//(End) subversive test data
  
  
  
  
  using util::isnil;
  using util::isSameObject;
//  using lumiera::error::LUMIERA_ERROR_ITER_EXHAUST;
  
  typedef LinkedElements<Nummy> List;
  typedef LinkedElements<Nummy, linked_elements::NoOwnership> ListNotOwner;
  
  
  /********************************************************************
   *  @test ScopedCollection manages a fixed set of objects, but these
   *        child objects are noncopyable, may be polymorphic, an can
   *        be created either all at once or chunk wise. The API is
   *        similar to a vector and allows for element access
   *        and iteration.
   */
  class LinkedElements_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          iterating();
          
          verify_nonOwnership();
          verify_ExceptionSafety();
          populate_by_iterator();
          verify_RAII_safety();
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
            
            elements.pushNew<Nummy>(1);
            elements.pushNew<Nummy>(2);
            elements.pushNew<Nummy>(3);
            elements.pushNew<Nummy>(4);
            elements.pushNew<Nummy>(5);
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
            
            elements.pushNew();
            elements.pushNew();
            elements.pushNew();
            
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
              elements.pushNew<Nummy>(i);
            
            // since elements where pushed, 
            // they should appear in reversed order
            int check=NUM_ELEMENTS;
            List::iterator ii = elements.begin();
            while (ii)
              {
                CHECK (check == ii->getVal());
                CHECK (check == ii->acc(+5) - 5);
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
            CHECK (cii == elements.end());
            VERIFY_ERROR (ITER_EXHAUST, ++ii );
            VERIFY_ERROR (ITER_EXHAUST, ++cii );
            
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
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
            
            elements.pushNew<Nummy>(1);
            elements.pushNew<Nummy>(2);
            CHECK (1+2 == Dummy::checksum());
            
            VERIFY_ERROR (SUBVERSIVE, elements.pushNew<Nummy>(3) );
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
      
      
      void
      verify_RAII_safety()
        {
          CHECK (0 == Dummy::checksum());
          
          __triggerErrorAt(3);
          Populator yieldSomeElements(NUM_ELEMENTS);
          VERIFY_ERROR (SUBVERSIVE, List(yieldSomeElements) );
          
          CHECK (0 == Dummy::checksum());
          __triggerError_reset();
        }
      
      
      void
      verify_customAllocator()
        {
          CHECK (0 == Dummy::checksum());
          {
            AllocationCluster allocator;
            
            ListCustomAllocated elements(allocator);
            
            elements.pushNew<Num<1> > (2);
            elements.pushNew<Num<3> > (4,5);
            elements.pushNew<Num<6> > (7,8,9);
            
            CHECK (sum(9) == Dummy::checksum());
            CHECK (3 == allocator.size());
            CHECK (1 == allocator.count<Num<1> >());
            CHECK (1 == allocator.count<Num<3> >());
            CHECK (1 == allocator.count<Num<6> >());
            
            CHECK (3 == elements.size());
            CHECK (1+2 == elements[2].getVal());
            CHECK (3+4+5 == elements[1].getVal());
            CHECK (6+7+8+9 == elements[0].getVal());
            
            elements.clear();
            CHECK (0 == allocator.size());
            CHECK (0 == allocator.count<Num<1> >());
            CHECK (0 == allocator.count<Num<3> >());
            CHECK (0 == allocator.count<Num<6> >());
            CHECK (0 == Dummy::checksum());
          }
          CHECK (0 == Dummy::checksum());
        }
    };
  
  
  
  LAUNCHER (LinkedElements_test, "unit common");
  
  
}} // namespace lib::test

