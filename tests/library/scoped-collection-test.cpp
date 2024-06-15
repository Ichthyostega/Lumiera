/*
  ScopedCollection(Test)  -  holding and owning a fixed collection of noncopyable objects

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

/** @file scoped-collection-test.cpp
 ** unit test \ref ScopedCollection_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/scoped-collection.hpp"
#include "lib/test/tracking-dummy.hpp"

#include <cstdlib>


namespace lib {
namespace test{
  
  namespace error = lumiera::error;
  
  
  namespace { // our explosive special Dummy
    
    LUMIERA_ERROR_DEFINE(SUBVERSIVE, "undercover action");
    
    class SubDummy
      : public Dummy
      {
        int trigger_;
        
        /** special variant of the dummy API operation:
         * @param i when zero, the trigger value will be revealed
         */
        virtual long
        calc (int i)
          {
            if (!i)
              return getVal() + trigger_;
            else
              return Dummy::calc(i);
          }
        
      public:
        SubDummy (int id, int trigger)
          : Dummy(id)
          , trigger_(trigger)
          {
            if (trigger == getVal())
              throw error::Fatal ("Subversive Bomb", LUMIERA_ERROR_SUBVERSIVE);
          }
        
        SubDummy()
          : Dummy()
          , trigger_(-1)
          { }
      };
    
    
    inline uint
    sum (uint n)
    {
      return n*(n+1) / 2;
    }
      
  }//(End) subversive test data
  
  
  
  
  using util::isnil;
  using LERR_(ITER_EXHAUST);
  
  typedef ScopedCollection<Dummy, sizeof(SubDummy)> CollD;
  
  
  /****************************************************************//**
   *  @test ScopedCollection manages a fixed set of objects, but these
   *        child objects are noncopyable, may be polymorphic, an can
   *        be created either all at once or chunk wise. The API is
   *        similar to a vector and allows for element access
   *        and iteration.
   */
  class ScopedCollection_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          building_RAII_Style();
          building_StackStyle();
          iterating();
          verify_defaultPopulator();
          verify_iteratorPopulator();
          verify_embeddedCollection();
        }
      
      
      void
      simpleUsage()
        {
          CHECK (0 == Dummy::checksum());
          {
            CollD container(5);
            CHECK (isnil (container));
            CHECK (0 == container.size());
            CHECK (0 == Dummy::checksum());
            
            container.populate();
            CHECK (!isnil (container));
            CHECK (5 == container.size());
            CHECK (0 != Dummy::checksum());
            
            container.clear();
            CHECK (isnil (container));
            CHECK (0 == container.size());
            CHECK (0 == Dummy::checksum());
            
            container.populate();
            CHECK (Dummy::checksum() == container[0].getVal()
                                      + container[1].getVal()
                                      + container[2].getVal()
                                      + container[3].getVal()
                                      + container[4].getVal());
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      void
      iterating()
        {
          CHECK (0 == Dummy::checksum());
          {
            CollD coll(50);
            for (uint i=0; i<coll.capacity(); ++i)
              coll.emplace<Dummy>(i);
            
            int check=0;
            CollD::iterator ii = coll.begin();
            while (ii)
              {
                CHECK (check == ii->getVal());
                CHECK (check == ii->calc(+5) - 5);
                ++check;
                ++ii;
              }
            
            
            // Test the const iterator
            CollD const& const_coll (coll);
            check = 0;
            CollD::const_iterator cii = const_coll.begin();
            while (cii)
              {
                CHECK (check == cii->getVal());
                ++check;
                ++cii;
              }
            
            
            // Test c++11 foreach iteration
            check = 0;
            for (auto& entry : coll)
              {
                CHECK (check == entry.getVal());
                ++check;
              }
            check = 0;
            for (auto const& entry : const_coll)
              {
                CHECK (check == entry.getVal());
                ++check;
              }
            
            
            // Verify correct behaviour of iteration end
            CHECK (! (coll.end()));
            CHECK (isnil (coll.end()));
            
            VERIFY_ERROR (ITER_EXHAUST, *coll.end() );
            VERIFY_ERROR (ITER_EXHAUST, ++coll.end() );
            
            CHECK (ii == coll.end());
            CHECK (cii == coll.end());
            VERIFY_ERROR (ITER_EXHAUST, ++ii );
            VERIFY_ERROR (ITER_EXHAUST, ++cii );
            
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      /** @test using the ScopedCollection to hold a variable
       * and possibly increasing number of elements, within the
       * fixed limits of the maximum capacity defined by the
       * ctor parameter. Any new elements will be created
       * behind the already existing objects. In case
       * of failure while creating an element, only
       * this element gets destroyed, the rest of
       * the container remains intact.
       */
      void
      building_StackStyle()
        {
          CHECK (0 == Dummy::checksum());
          {
              
            int rr = rand() % 100;
            
            CollD coll(3);
            CHECK (0 == coll.size());
            CHECK (0 == Dummy::checksum());
            
            Dummy& d0 = coll.emplaceElement();
            CHECK (1 == coll.size());
            
            Dummy& d1 = coll.emplace<Dummy> (rr);
            CHECK (2 == coll.size());
            
            int sum = Dummy::checksum();
            
            // trigger the bomb
            VERIFY_ERROR (SUBVERSIVE, coll.emplace<SubDummy>(rr,rr) );
            
            CHECK (  2 == coll.size()); // the other objects survived
            CHECK (sum == Dummy::checksum());
            
            Dummy& d2 = coll.emplace<SubDummy> (rr, rr+1);
            CHECK (3 == coll.size());
            
            CHECK (sum + rr == Dummy::checksum());
            
            VERIFY_ERROR (CAPACITY, coll.emplaceElement());
            VERIFY_ERROR (CAPACITY, coll.emplaceElement());
            VERIFY_ERROR (CAPACITY, coll.emplaceElement());
            
            CHECK (3 == coll.size());
            CHECK (sum + rr == Dummy::checksum());
            
            
            CHECK (d0.calc(11) == coll[0].getVal() + 11 );
            CHECK (d1.calc(22) == rr + 22);
            CHECK (d2.calc(33) == rr + 33);
            CHECK (d2.calc(0)  == rr + (rr+1) );     // SubDummy's special implementation of the acc()-function
                                                    //  returns the trigger value, when the argument is zero
            
            coll.clear();
            coll.emplace<SubDummy> (11,22);
            
            CHECK ( 1 == coll.size());
            CHECK (11 == Dummy::checksum());
            
            // NOTE DANGEROUS:
            // The previously obtained references just point into the object storage.
            // Thus we're now accessing a different object, even a different type!
            CHECK (d0.calc(0) == 11 + 22);
            
            // The others even point into obsoleted storage holding zombie objects
            CHECK (d1.calc(44) == rr + 44);
            
          }
          CHECK (0 == Dummy::checksum());
        }
        
        
      /** @test using the ScopedCollection according to the RAII pattern.
       * For this usage style, the collection is filled right away, during
       * construction. If anything goes wrong, the whole collection is
       * cleared and invalidated. Consequently there is no tangible "lifecycle"
       * at the usage site. Either the collection is fully usable, or not at all.
       * This requires the client to provide a functor (callback) to define
       * the actual objects to be created within the ScopedCollection. These
       * may as well be subclasses of the base type I, provided the general
       * element storage size #siz was chosen sufficiently large to hold
       * those subclass instances.
       * 
       * This test demonstrates the most elaborate usage pattern, where
       * the client provides a full blown functor object #Populator,
       * which even has embedded state. Generally speaking, anything
       * exposing a suitable function call operator is acceptable.
       */
      void
      building_RAII_Style()
        {
          CHECK (0 == Dummy::checksum());
          {
            int rr = rand() % 100;
            int trigger = 100 + 5 + 1;   // prevents the bomb from exploding (since rr < 100) 
            
            CollD coll (6, Populator(rr, trigger));
            
            CHECK (!isnil (coll));
            CHECK (6 == coll.size());
            CHECK (0 != Dummy::checksum());
            
            CHECK (coll[0].calc(0) == 0 + rr);
            CHECK (coll[1].calc(0) == 1 + rr + trigger);
            CHECK (coll[2].calc(0) == 2 + rr);
            CHECK (coll[3].calc(0) == 3 + rr + trigger);
            CHECK (coll[4].calc(0) == 4 + rr);
            CHECK (coll[5].calc(0) == 5 + rr + trigger);
            // what does this check prove?
            // - the container was indeed populated with DubDummy objects
            //   since the overridden version of Dummy::acc() did run and
            //   reveal the trigger value
            // - the population was indeed done with the anonymous Populator
            //   instance fed to the ctor, since this object was "marked" with
            //   the random value rr, and adds this mark to the built values.
            
            coll.clear();
            CHECK (0 == Dummy::checksum());
            
            // Verify Error handling while in creation:
            // SubDummy explodes on equal ctor parameters
            // which here happens for i==7
            VERIFY_ERROR (SUBVERSIVE, CollD(10, Populator(0, 7)) );
            
            // any already created object was properly destroyed
            CHECK (0 == Dummy::checksum());
            
          }
          CHECK (0 == Dummy::checksum());
        }
      
      /** Functor to populate the Collection */
      class Populator
        {
          uint i_;
          int off_;
          int trigg_;
          
        public:
          Populator (int baseOffset, int triggerCode)
            : i_(0)
            , off_(baseOffset)
            , trigg_(triggerCode)
            { }
          
          void
          operator() (CollD::ElementHolder& storage)
            {
              switch (i_ % 2) 
                {
                case 0:
                  storage.create<Dummy> (i_+off_);
                  break;
                  
                case 1:
                  storage.create<SubDummy> (i_+off_, trigg_);
                  break;
                }
              ++i_;
            }
        };
      
      
      
      /** @test for using ScopedCollection in RAII style,
       * several pre-defined "populators" are provided.
       * The most obvious one being just to fill the
       * collection with default constructed objects.
       */
      void
      verify_defaultPopulator()
        {
          CHECK (0 == Dummy::checksum());
          
          CollD coll (25, CollD::FillAll() );
          
          CHECK (!isnil (coll));
          CHECK (25 == coll.size());
          CHECK (0 != Dummy::checksum());
          
          for (CollD::iterator ii = coll.begin(); ii; ++ii)
            {
              CHECK ( INSTANCEOF (Dummy,    & (*ii)));
              CHECK (!INSTANCEOF (SubDummy, & (*ii)));
            }
        }
      
      
      void
      verify_subclassPopulator()
        {
          CHECK (0 == Dummy::checksum());
          
          CollD coll (25, CollD::FillWith<SubDummy>() );
          
          CHECK (!isnil (coll));
          CHECK (25 == coll.size());
          CHECK (0 != Dummy::checksum());
          
          for (CollD::iterator ii = coll.begin(); ii; ++ii)
            CHECK (INSTANCEOF (SubDummy, & (*ii)));
        }
      
      
      void
      verify_iteratorPopulator()
        {
          typedef ScopedCollection<uint> CollI;
          
          CollI source (25);
          for (uint i=0; i < source.capacity(); ++i)
            source.emplace<uint>(i);           // holding the numbers 0..24
          
          CollI coll (20, CollI::pull(source.begin()));
                                              // this immediately pulls in the first 20 elements 
          CHECK (!isnil (coll));
          CHECK (20 == coll.size());
          CHECK (25 == source.size());
          
          for (uint i=0; i < coll.size(); ++i)
            {
              CHECK (coll[i] == i        );
              CHECK (coll[i] == source[i]);
            }
          
          // note: the iterator is assumed to deliver a sufficient amount of elements
          VERIFY_ERROR (ITER_EXHAUST, CollI (50, CollI::pull (source.begin())));
        }
      
      
      /** @test simulate the typical situation of a manager
       * owning some embedded components. Here, our ManagerDemo
       * instance owns a collection of numbers 50..1. They are
       * created right while initialising the manager, and this
       * initialisation is done by invoking a member function
       * of the manager
       */
      void
      verify_embeddedCollection()
        {
          ManagerDemo object_with_embedded_Collection(50);
          CHECK (sum(50) == object_with_embedded_Collection.useMyNumbers());
        }
      
      class ManagerDemo
        {
          typedef ScopedCollection<uint> CollI;
          
          uint memberVar_;
          const CollI my_own_Numbers_;
          
          void
          buildNumbers (CollI::ElementHolder& storage)
            {
              storage.create<uint>(memberVar_);
              --memberVar_;
            }
          
        public:
          ManagerDemo(uint cnt)
            : memberVar_(cnt)
            , my_own_Numbers_(cnt, &ManagerDemo::buildNumbers, this)
            { 
              CHECK (0 == memberVar_);
              CHECK (cnt == my_own_Numbers_.size());
            }
          
          uint
          useMyNumbers()
            {
              uint sum(0);
              for (CollI::const_iterator ii = my_own_Numbers_.begin(); ii; ++ii)
                sum += *ii;
              return sum;
            }
        };
    };
  
  
  
  LAUNCHER (ScopedCollection_test, "unit common");
  
  
}} // namespace lib::test

