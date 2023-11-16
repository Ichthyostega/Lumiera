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

/** @file polymorphic-value-test.cpp
 ** unit test \ref PolymorphicValue_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util-foreach.hpp"

#include "lib/polymorphic-value.hpp"

#include <vector>


namespace lib {
namespace test{
  
  using ::Test;
  using util::for_each;
  using util::unConst;
  using util::isSameObject;
  using lumiera::error::LUMIERA_ERROR_ASSERTION;
  
  
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
    const uint MAX_ELM  = 111;
    const uint MAX_SIZ  = sizeof(long[MAX_ELM]);
    
    /* Checksums to verify proper ctor-dtor calls and copy operations */
    long _checkSum = 0;
    long _callSum  = 0;
    uint _created  = 0;
    
    
    /**
     * Template to generate concrete implementation classes.
     * @note the generated classes vary largely in size, and
     *       moreover the actual place to store the checksum
     *       also depends on that size parameter.
     */
    template<uint ii, class BASE=Interface>
    struct Imp : BASE
      {
        long localData_[ii];
        
       ~Imp()
          {
            mark (-1 * localSum());
            CHECK (0 == localSum());
          }
        
        Imp()
          {
            REQUIRE (0 < ii);
            localSum() = 0;
            mark(ii);
            ++ _created;
          }
        
        Imp (Imp const& o)
          {
            ++ _created;
            copyData (o);
            _checkSum  += localSum();
          }// adjust, because we've gotten two identical instances
        
        Imp&
        operator= (Imp const& o)
          {
            _checkSum  -= localSum();
            copyData (o);
            _checkSum  += localSum();
            return *this;
          }
        
      private:
        virtual long
        apiFunc()
          {
            long rr = ii * (1 + rand() % MAX_RAND);
            mark (rr);
            _callSum += rr;
            return rr;
          }
        
        long&
        localSum()
          {
            return localData_[ii-1];
          }
        
        void
        mark (long markerValue)
          {
            localSum() += markerValue;
            _checkSum  += markerValue;
          }
        
        void
        copyData (Imp const& o)
          {
            for (uint i=0; i<ii; ++i)
              localData_[i] = o.localData_[i];
          }
      };
    
    
    /** maximum additional storage maybe wasted
     *  due to alignment of the contained object
     *  within the embedded Holder/Buffer
     */
    const size_t _ALIGN_ = sizeof(size_t);
    
  }
  
  using PolyVal = PolymorphicValue<Interface, MAX_SIZ>;
  using TestList = std::vector<PolyVal> ;
  
  
  
  
  
  
  /******************************************************************************//**
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
          
          verifyBasics();
          
          {
            TestList objs = createOpaqueValues ();
            for_each (objs, operate);
          }
          CHECK (0 == _checkSum); // all dead
          
          verifyOverrunProtection();
          verifyCopySupportDetectionMetafunctions();
        }
      
      
      TestList
      createOpaqueValues ()
        {
          TestList list;
          list.push_back (PolyVal::build<Imp<1>>  () );
          list.push_back (PolyVal::build<Imp<11>> () );
          list.push_back (PolyVal::build<Imp<111>>() );
          list.push_back (PolyVal::build<Imp<23>> () );
          list.push_back (PolyVal::build<Imp<5>>  () );
          return list;
        } //note: copy
      
      
      static void
      operate (PolyVal& elm)
        {
          PolyVal myLocalVal(elm);
          CHECK (elm == myLocalVal);
          
          long prevSum = _callSum;
          long randVal = myLocalVal->apiFunc();
          CHECK (prevSum + randVal == _callSum);
          CHECK (elm != myLocalVal);
          
          elm = myLocalVal;
          CHECK (elm == myLocalVal);
          CHECK (!isSameObject (elm, myLocalVal));
          
          CHECK (sizeof(myLocalVal) <= MAX_SIZ + polyvalue::Trait<Interface>::ADMIN_OVERHEAD + _ALIGN_);
        }
      
      
      void
      verifyBasics()
        {
          typedef Imp<MAX_ELM> MaximumSizedImp;
          
          // Standard case: no copy support by client objects
          verifyCreation_and_Copy<PolyVal, MaximumSizedImp>();
          
          // Special case: client objects expose extension point for copy support
          using CopySupportAPI    = polyvalue::CopySupport<Interface>;                    // Copy support API declared as sub-interface
          using CopySupportingImp = Imp<MAX_ELM,CopySupportAPI>;                          // insert this sub-interface between public API and Implementation
          using OptimalPolyVal    = PolymorphicValue<Interface, MAX_SIZ, CopySupportAPI>; // Make the Holder use this special attachment point
          CHECK (sizeof(OptimalPolyVal) < sizeof(PolyVal));                               // results in smaller Holder and less implementation overhead
          
          verifyCreation_and_Copy<OptimalPolyVal, CopySupportingImp>();
        }
      
      
      template<class PV,class IMP>
      void
      verifyCreation_and_Copy()
        {
          using Holder  = PV;
          using ImpType = IMP;
          using Api     = typename PV::Interface ;
          
          long prevSum = _checkSum;
          uint prevCnt = _created;
          
          Holder val = Holder::template build<ImpType>();
          CHECK (prevSum+111 == _checkSum);            // We got one primary ctor call
          CHECK (prevCnt+1   <= _created);             // Note: usually, the compiler optimises
          CHECK (prevCnt+2   >= _created);             //       and skips the spurious copy-operation
          CHECK (sizeof(Holder) >= sizeof(ImpType));
          Api& api = val;
          CHECK (isSameObject(api,val));
          CHECK (INSTANCEOF(ImpType, &api));
          
          prevCnt = _created;
          Holder val2(val);       // invoke copy ctor without knowing the implementation type
          api.apiFunc();
          CHECK (val != val2);    // invoking the API function had an sideeffect on the state
          val = val2;             // assignment of copy back to the original...
          CHECK (val == val2);    // cancels the side effect
          
          CHECK (prevCnt+1 == _created); // one new embedded instance was created by copy ctor
        }
      
      
      void
      verifyOverrunProtection()
        {
          typedef Imp<MAX_ELM+1> OversizedImp;
          CHECK (MAX_SIZ < sizeof(OversizedImp));
#if false ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
          VERIFY_ERROR (ASSERTION, PolyVal::build<OversizedImp>() );
#endif    ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
        }
      
      
      /** @test internally, PolymorphicValue uses some metafunctions
       * to pick a suitable code path, based on the presence of helper functions
       * on the API of the embedded objects. Default is no support by these objects,
       * which then requires to use a more expensive implementation. Sometimes it's
       * desirable to support _cloning only_ (copy ctor), but no assignment after
       * the fact. In this special case, a support API with only a `cloneInto()` member
       * can be implemented, causing the PolymorphicValue container to raise an 
       * exception in case the copy operator is invoked.
       */
      void
      verifyCopySupportDetectionMetafunctions()
        {
          typedef polyvalue::CopySupport<Interface> CopySupportAPI;
          typedef polyvalue::CloneValueSupport<Interface> CloneOnlyAPI;
          
          CHECK ( !polyvalue::exposes_CloneFunction<Interface>::value );
          CHECK (  polyvalue::exposes_CloneFunction<CopySupportAPI>::value );
          CHECK (  polyvalue::exposes_CloneFunction<CloneOnlyAPI>::value );
          
          CHECK (  polyvalue::allow_Clone_but_no_Copy<CloneOnlyAPI>::value );
          CHECK ( !polyvalue::allow_Clone_but_no_Copy<CopySupportAPI>::value );
       }
    };
  
  
  LAUNCHER (PolymorphicValue_test, "unit common");
  
  
}} // namespace lib::test
