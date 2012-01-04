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



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/scoped-collection.hpp"
#include "lib/test/testdummy.hpp"

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
        acc (int i)
          {
            if (!i)
              return getVal() + trigger_;
            else
              return Dummy::acc(i);
          }
        
      public:
        SubDummy (int id, int trigger)
          : Dummy(id)
          , trigger_(trigger)
          {
            if (trigger == getVal())
              throw new error::Fatal ("Subversive Bomb", LUMIERA_ERROR_SUBVERSIVE);
          }
      };
    
  }//(End) subversive test data
  
  
  
  
  using util::isnil;
//using std::tr1::placeholders::_1;                     /////////////////////////////TODO
  using lumiera::error::LUMIERA_ERROR_ITER_EXHAUST;
  
  typedef ScopedCollection<Dummy> CollD;
  
  
  /********************************************************************
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
              coll.appendNew<Dummy>(i);
            
            int check=0;
            CollD::iterator ii = coll.begin();
            while (ii)
              {
                CHECK (check == ii->getVal());
                CHECK (check == ii->acc(+5) - 5);
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
      
      
      void
      building_RAII_Style()
        {
          CHECK (0 == Dummy::checksum());
          {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #877
                        /** Functor to populate the Collection */
                        class Populator
                          {
                            uint i_;
                            int off_;
                            
                          public:
                            Populator (int baseOffset)
                              : i_(0)
                              , off_(baseOffset)
                              { }
                            
                            void
                            operator() (int specialOffset, void* storage)
                              {
                                switch (i_ % 2) 
                                  {
                                  case 0:
                                    new(storage) Dummy(i_+off_);
                                    break;
                                    
                                  case 1:
                                    new(storage) SubDummy(i_+off_, specialOffset);
                                    break;
                              }   }
                          };
            
            
            int rr = rand() % 100;
            int trigger = 101;
            
            CollD coll (6, Populator(rr), trigger, _1 );
            
            CHECK (!isnil (coll));
            CHECK (6 == coll.size());
            CHECK (0 != Dummy::checksum());
            
            CHECK (coll[0].acc(0) == 0 + rr);
            CHECK (coll[1].acc(0) == 1 + rr + trigger);
            CHECK (coll[2].acc(0) == 2 + rr);
            CHECK (coll[3].acc(0) == 3 + rr + trigger);
            CHECK (coll[4].acc(0) == 4 + rr);
            CHECK (coll[5].acc(0) == 5 + rr + trigger);
            
            coll.clear();
            CHECK (0 == Dummy::checksum());
            
            // Verify Error handling while in creation:
            // SubDummy explodes on equal ctor parameters
            // which here happens for i==7
            VERIFY_ERROR (SUBVERSIVE, CollD(10, Populator(0), 7, _1 ) );
            
            // any already created object was properly destroyed
            CHECK (0 == Dummy::checksum());
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #877
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      void
      building_StackStyle()
        {
          CHECK (0 == Dummy::checksum());
          {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #877
              
            int rr = rand() % 100;
            
            CollD coll(3);
            CHECK (0 == coll.size());
            CHECK (0 == Dummy::checksum());

            Dummy& d0 = coll.appendNew();
            CHECK (1 == coll.size());
            
            Dummy& d1 = coll.appendNew<Dummy> (r1);
            CHECK (2 == coll.size());

            int sum = Dummy::checksum();

            // trigger the bomb
            VERIFY_ERROR (SUBVERSIVE, coll.appendNew<SubSummy>(r1,r1) );
            
            CHECK (  2 == coll.size()); // the other objects survived
            CHECK (sum == Dummy::checksum());
            
            Dummy& d2 = coll.appendNew<SubDummy> (r1, r1+1);
            CHECK (3 == coll.size());
            
            CHECK (sum + r1 == Dummy::checksum());
            
            VERIFY_ERROR (CAPACITY, coll.appendNew());
            VERIFY_ERROR (CAPACITY, coll.appendNew());
            VERIFY_ERROR (CAPACITY, coll.appendNew());
            
            CHECK (3 == coll.size());
            CHECK (sum + r1 == Dummy::checksum());
            
            
            CHECK (d0.acc(11) == coll[0].getVal() + 11 );
            CHECK (d1.acc(22) == r1 + 22);
            CHECK (d2.acc(33) == r1 + 33);
            CHECK (d2.acc(0)  == r1 + (r1+1) );     // SubDummy's special implementation of the acc()-function
                                                   //  returns the trigger value, when the argument is zero
            
            coll.clear();
            coll.appendNew<SubDummy> (11,22);
            
            CHECK (3 == coll.size());
            CHECK (11 == Dummy::checksum());
            
            // NOTE DANGEROUS:
            // The previously obtained references just point into the object storage.
            // Thus we're now accessing a different object, even a different type!
            CHECK (d0.acc(0) == 11 + 22);
            
            // The others even point into obsoleted storage holding zombie objects
            CHECK (d1.acc(44) == r1 + 44);
            
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #877
          }
          CHECK (0 == Dummy::checksum());
        }
    };
  
  
  LAUNCHER (ScopedCollection_test, "unit common");
  
  
}} // namespace lib::test

