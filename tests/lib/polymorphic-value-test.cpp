/*
  PolymorphicValue(Test)  -  verify handling of opaque polymorphic values

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/util-foreach.hpp"

#include "lib/polymorphic-value.hpp"
//#include "lib/bool-checkable.hpp"

#include <iostream>
//#include <cstdlib>
#include <vector>


namespace lib {
namespace test{
  
  using ::Test;
//  using util::isnil;
  using util::for_each;
  using util::unConst;
  using util::isSameObject;
//  using lumiera::error::LUMIERA_ERROR_INVALID;
  using lumiera::error::LUMIERA_ERROR_ASSERTION;
  
//  using std::vector;
//  using std::cout;
//  using std::endl;
  
  namespace { // test dummy hierarchy
             //  Note: largely varying space requirements
            //         correct function depending on concrete class
    
    
    struct Interface
      {
        virtual ~Interface() {}
        virtual long apiFunc()   =0;
        
        virtual long& localSum() =0;
        
        bool
        operator== (Interface const& o)  const
          {
            return unConst(this)->localSum()
                == unConst(o).localSum();
          }
      };
    
    
    const uint MAX_RAND = 1000;
    const uint MAX_SIZ  = sizeof(long[113]);  /////////////////////TODO: using just 111 causes SEGV ---> suspect the HandlingAdapter mixin to require additional storage
    
    long _checkSum = 0;
    long _callSum  = 0;
    uint _created  = 0;
    
    
    template<uint ii>
    struct Imp : Interface
      {
        long localData_[ii];
        
        Imp()
          {
            REQUIRE (0 < ii);
            localSum() = 0;
            note(ii);
            ++ _created;
          }
        
       ~Imp()
          {
            note (-1 * localSum());
            CHECK (0 == localSum());
          }
        
        
        long
        apiFunc()
          {
            long rr = ii * (rand() % MAX_RAND);
            note (rr);
            _callSum += rr;
            return rr;
          }
        
        long&
        localSum()
          {
            return localData_[ii-1];
          }
        
        void
        note (long markerValue)
          {
            localSum() += markerValue;
            _checkSum  += markerValue;
          }
      };
    
    
    /** maximum additional storage maybe wasted
     *  due to alignment of the contained object
     *  within the embedded Holder/Buffer
     */
    const size_t _ALIGN_ = sizeof(size_t);
    
  }
  
  typedef PolymorphicValue<Interface, MAX_SIZ> PolyVal;
  typedef std::vector<PolyVal> TestList;
  
  
  
  /**********************************************************************************
   *  @test build a bunch of PolymorphicValue objects. Handle them like copyable
   *        value objects, without knowing the exact implementation type; moreover
   *        execute implementation internals only disclosed to the concrete subtype.
   *        Verify correctness through checksums.
   */
  class PolymorphicValue_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          _checkSum = 0;
          _callSum  = 0;
          _created  = 0;
          
          {
            TestList objs = createOpaqueValues ();
            for_each (objs, operate);
          }
          CHECK (0 == _checkSum); // all dead
          
          verifyOverrunProtection();
        }
      
      
      TestList
      createOpaqueValues ()
        {
          TestList list;
          list.push_back (PolyVal::build<Imp<1> >  () );
          list.push_back (PolyVal::build<Imp<11> > () );
          list.push_back (PolyVal::build<Imp<111> >() );
          list.push_back (PolyVal::build<Imp<23> > () );
          list.push_back (PolyVal::build<Imp<5> >  () );
          return list;
        } //note: copy
      
      
      static void
      operate (PolyVal& elm)
        {
          PolyVal myLocalVal(elm);
          CHECK (elm == myLocalVal);
          
          long prevSum = _callSum;
          Interface& subject = myLocalVal;
          long randVal = subject.apiFunc();
          CHECK (prevSum + randVal == _callSum);
          CHECK (elm != myLocalVal);
          
          elm = myLocalVal;
          CHECK (elm == myLocalVal);
          CHECK (!isSameObject (elm, myLocalVal));
          
          CHECK (sizeof(myLocalVal) <= MAX_SIZ + _ALIGN_);
        }
      
      
      void
      verifyOverrunProtection()
        {
#if false ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
          VERIFY_ERROR (ASSERTION, PolyVal::build<Imp<112> >() );
#endif    ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
        }
    };
  
  
  LAUNCHER (PolymorphicValue_test, "unit common");
  
  
}} // namespace lib::test
