/*
  OpaqueHolder(Test)  -  check the inline type erasure helper

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file opaque-holder-test.cpp
 ** unit test \ref OpaqueHolder_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"
#include "lib/util-foreach.hpp"
#include "lib/opaque-holder.hpp"

#include <iostream>
#include <vector>


namespace lib {
namespace test{
  
  using ::Test;
  using util::isnil;
  using util::for_each;
  using util::isSameObject;
  using LERR_(BOTTOM_VALUE);
  using LERR_(WRONG_TYPE);
  using LERR_(ASSERTION);
  
  using std::vector;
  using std::cout;
  using std::endl;
  
  namespace { // test dummy hierarchy
             //  Note: common storage but no vtable 
    
    long _checksum = 0;
    uint _create_count = 0;
    
    
    struct Base   
      {
        uint id_;
        
        Base(uint i=0)      : id_(i)     { _checksum +=id_; ++_create_count; }
        Base(Base const& o) : id_(o.id_) { _checksum +=id_; ++_create_count; }
        
        uint getIt() { return id_; }
      };
    
    
    template<uint ii>
    struct DD : Base
      {
        DD() : Base(ii)       { }
       ~DD() { _checksum -= ii; }  // doing the decrement here
      };                          //  verifies the correct dtor is called
    
    
    struct Special
      : DD<7>
      {
        ulong myVal_;
        
        Special (uint val)
          : myVal_(val)
          { }
        
        explicit
        operator bool()  const  ///< custom boolean "validity" check
          {
            return myVal_ % 2;
          }
      };
    
    
    /** maximum additional storage maybe wasted
     *  due to alignment of the contained object
     *  within OpaqueHolder's buffer
     */
    const size_t _ALIGN_ = sizeof(size_t);
    
  }
  
  typedef OpaqueHolder<Base> Opaque;
  typedef vector<Opaque> TestList;
  
  
  
  /******************************************************************************//**
   *  @test use the OpaqueHolder inline buffer to handle objects of a family of types
   *        through a common interface, without being forced to use heap storage
   *        or a custom allocator.
   *
   *  @todo this test doesn't cover automatic conversions and conversions using RTTI
   *        from the target objects, while `OpaqueHolder.template get()`
   *        would allow for such conversions. This is similar to Ticket #141, and
   *        actually based on the same code as variant.hpp (access-casted.hpp)
   */
  class OpaqueHolder_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          _checksum = 0;
          _create_count = 0;
          {
            TestList objs = createDummies ();
            for_each (objs, reAccess);
            checkHandling (objs);
            checkSpecialSubclass ();
          }
          CHECK (0 == _checksum); // all dead
        }
      
      
      TestList
      createDummies ()
        {
          TestList list;
          list.push_back (DD<1>());
          list.push_back (DD<3>());
          list.push_back (DD<5>());
          list.push_back (DD<7>());
          return list;
        } //note: copy
      
      
      static void
      reAccess (Opaque& elm)
        {
          cout << elm->getIt() << endl;
        }
      
      
      /** @test cover the basic situations of object handling,
       *        especially copy operations and re-assignments
       */
      void
      checkHandling (TestList& objs)
        {
          Opaque oo;
          CHECK (!oo);
          CHECK (isnil(oo));
          
          oo = objs[1];
          CHECK (oo);
          CHECK (!isnil(oo));
          
          typedef DD<3> D3;
          typedef DD<5> D5;
          D3 d3 (oo.get<D3>() );
          CHECK (3 == oo->getIt());    // re-access through Base interface
          CHECK (!isSameObject (d3, *oo));
          VERIFY_ERROR (WRONG_TYPE, oo.get<D5>() );
          
          // direct assignment of target into Buffer
          oo = D5();
          CHECK (oo);
          CHECK (5 == oo->getIt());
          VERIFY_ERROR (WRONG_TYPE, oo.get<D3>() );
          
          // can get a direct reference to contained object
          D5 &rd5 (oo.get<D5>()); 
          CHECK (isSameObject (rd5, *oo));
          
          CHECK (!isnil(oo));
          oo = objs[3];     // copy construction also works on non-empty object
          CHECK (7 == oo->getIt());
          
          // WARNING: direct ref has been messed up through the backdoor!
          CHECK (7 == rd5.getIt());
          CHECK (isSameObject (rd5, *oo));
          
          uint cnt_before = _create_count;
          
          oo.clear();
          CHECK (!oo);
          oo = D5();        // direct assignment also works on empty object
          CHECK (oo);
          CHECK (5 == oo->getIt());
          CHECK (_create_count == 2 + cnt_before);
          // one within buff and one for the anonymous temporary D5()
          
          
          // verify that self-assignment is properly detected...
          cnt_before = _create_count;
          oo = oo;
          CHECK (oo);
          CHECK (_create_count == cnt_before);
          oo = oo.get<D5>();
          CHECK (_create_count == cnt_before);
          oo = *oo;
          CHECK (_create_count == cnt_before);
          CHECK (oo);
          
          oo.clear();
          CHECK (!oo);
          CHECK (isnil(oo));
          VERIFY_ERROR (BOTTOM_VALUE, oo.get<D5>() );
#if false ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
          VERIFY_ERROR (ASSERTION, oo->getIt() );
#endif    ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
          // can't access empty holder...
          
          Opaque o1 (oo);
          CHECK (!o1);
          
          Opaque o2 (d3);
          CHECK (!isSameObject (d3, *o2));
          CHECK (3 == o2->getIt());
          
          CHECK (sizeof(Opaque) <= sizeof(Base) + sizeof(void*) + _ALIGN_);
        }
      
      
      /** @test OpaqueHolder with additional storage for subclass.
       *        When a subclass requires more storage than the base class or
       *        Interface, we need to create a custom OpaqueHolder, specifying the
       *        actually necessary storage. Such a custom OpaqueHolder behaves exactly
       *        like the standard variant, but there is protection against accidentally
       *        using a standard variant to hold an instance of the larger subclass.
       *        
       *  @test Moreover, if the concrete class has a custom operator bool(), it
       *        will be invoked automatically from OpaqueHolder's operator bool()
       * 
       */ 
      void
      checkSpecialSubclass ()
        {
          typedef OpaqueHolder<Base, sizeof(Special)> SpecialOpaque;
          
          cout << showSizeof<Base>() << endl;
          cout << showSizeof<Special>() << endl;
          cout << showSizeof<Opaque>() << endl;
          cout << showSizeof<SpecialOpaque>() << endl;
          
          CHECK (sizeof(Special) > sizeof(Base));
          CHECK (sizeof(SpecialOpaque) > sizeof(Opaque));
          CHECK (sizeof(SpecialOpaque) <= sizeof(Special) + sizeof(void*) + _ALIGN_);
          
          Special s1 (6);
          Special s2 (3);
          CHECK (!s1);               // even value
          CHECK (s2);                // odd value
          CHECK (7 == s1.getIt());   // indeed subclass of DD<7>
          CHECK (7 == s2.getIt());
          
          SpecialOpaque ospe0;
          SpecialOpaque ospe1 (s1);
          SpecialOpaque ospe2 (s2);
          
          CHECK (!ospe0);            // note: bool test (isValid)
          CHECK (!ospe1);            // also forwarded to contained object (myVal_==6 is even)
          CHECK ( ospe2);
          CHECK ( isnil(ospe0));     // while isnil just checks the empty state
          CHECK (!isnil(ospe1));
          CHECK (!isnil(ospe2));
          
          CHECK (7 == ospe1->getIt());
          CHECK (6 == ospe1.get<Special>().myVal_);
          CHECK (3 == ospe2.get<Special>().myVal_);
          
          ospe1 = DD<5>();            // but can be reassigned like any normal Opaque
          CHECK (ospe1);
          CHECK (5 == ospe1->getIt());
          VERIFY_ERROR (WRONG_TYPE, ospe1.get<Special>() );
          
          Opaque normal = DD<5>();
          CHECK (normal);
          CHECK (5 == normal->getIt());
#if false ////////////////////////////////////////////////////////TODO: restore throwing ASSERT
          // Assertion protects against SEGV
          VERIFY_ERROR (ASSERTION, normal = s1 );
#endif////////////////////////////////////////////////////////////
        }
    };
  
  
  LAUNCHER (OpaqueHolder_test, "unit common");
  
  
}} // namespace lib::test

