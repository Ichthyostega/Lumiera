/*
  OpaqueHolder(Test)  -  check the inline type erasure helper
 
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
#include "lib/util.hpp"

#include "lib/opaque-holder.hpp"
#include "lib/bool-checkable.hpp"

#include <iostream>
#include <vector>


namespace lib {
namespace test{
  
  using ::Test;
  using util::isnil;
  using util::for_each;
  using util::isSameObject;
  using lumiera::error::LUMIERA_ERROR_INVALID;
  using lumiera::error::LUMIERA_ERROR_ASSERTION;
  
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
      , BoolCheckable<Special>
      {
        ulong myVal_;
        
        Special (uint val)
          : myVal_(val)
          { }
        
        bool
        isValid ()  const ///< custom boolean "validity" check
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
  
  
  
  /**********************************************************************************
   *  @test use the OpaqueHolder inline buffer to handle objects of a family of types
   *        through a common interface, without being forced to use heap storage
   *        or a custom allocator.
   *
   *  @todo this test doesn't cover automatic conversions and conversions using RTTI
   *        from the target objects, while \code OpaqueHolder.template get() \endcode
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
          ASSERT (0 == _checksum); // all dead
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
          ASSERT (!oo);
          ASSERT (isnil(oo));
          
          oo = objs[1];
          ASSERT (oo);
          ASSERT (!isnil(oo));
          
          typedef DD<3> D3;
          typedef DD<5> D5;
          D3 d3 (oo.get<D3>() );
          ASSERT (3 == oo->getIt());    // re-access through Base interface
          ASSERT (!isSameObject (d3, *oo));
          VERIFY_ERROR (WRONG_TYPE, oo.get<D5>() );
          
          // direct assignment of target into Buffer
          oo = D5();
          ASSERT (oo);
          ASSERT (5 == oo->getIt());
          VERIFY_ERROR (WRONG_TYPE, oo.get<D3>() );
          
          // can get a direct reference to contained object
          D5 &rd5 (oo.get<D5>()); 
          ASSERT (isSameObject (rd5, *oo));
          
          ASSERT (!isnil(oo));
          oo = objs[3];     // copy construction also works on non-empty object
          ASSERT (7 == oo->getIt());
          
          // WARNING: direct ref has been messed up through the backdoor!
          ASSERT (7 == rd5.getIt());
          ASSERT (isSameObject (rd5, *oo));
          
          uint cnt_before = _create_count;
          
          oo.clear();
          ASSERT (!oo);
          oo = D5();        // direct assignment also works on empty object
          ASSERT (oo);
          ASSERT (5 == oo->getIt());
          ASSERT (_create_count == 2 + cnt_before);
          // one within buff and one for the anonymous temporary D5()
          
          
          // verify that self-assignment is properly detected...
          cnt_before = _create_count;
          oo = oo;
          ASSERT (oo);
          ASSERT (_create_count == cnt_before);
          oo = oo.get<D5>();
          ASSERT (_create_count == cnt_before);
          oo = *oo;
          ASSERT (_create_count == cnt_before);
          ASSERT (oo);
          
          oo.clear();
          ASSERT (!oo);
          ASSERT (isnil(oo));
          VERIFY_ERROR (INVALID, oo.get<D5>() );
#if false ////////////////////////////////////////////////////////TODO: restore throwing ASSERT
          VERIFY_ERROR (ASSERTION, oo->getIt() );
#endif////////////////////////////////////////////////////////////
          // can't access empty holder...
          
          Opaque o1 (oo);
          ASSERT (!o1);
          
          Opaque o2 (d3);
          ASSERT (!isSameObject (d3, *o2));
          ASSERT (3 == o2->getIt());
          
          ASSERT (sizeof(Opaque) <= sizeof(Base) + sizeof(void*) + _ALIGN_);
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
          
          ASSERT (sizeof(Special) > sizeof(Base));
          ASSERT (sizeof(SpecialOpaque) > sizeof(Opaque));
          ASSERT (sizeof(SpecialOpaque) <= sizeof(Special) + sizeof(void*) + _ALIGN_);
          
          Special s1 (6);
          Special s2 (3);
          ASSERT (!s1);               // even value
          ASSERT (s2);                // odd value
          ASSERT (7 == s1.getIt());   // indeed subclass of DD<7>
          ASSERT (7 == s2.getIt());
          
          SpecialOpaque ospe0;
          SpecialOpaque ospe1 (s1);
          SpecialOpaque ospe2 (s2);
          
          ASSERT (!ospe0);            // note: bool test (isValid)
          ASSERT (!ospe1);            // also forwarded to contained object (myVal_==6 is even)
          ASSERT ( ospe2);
          ASSERT ( isnil(ospe0));     // while isnil just checks the empty state
          ASSERT (!isnil(ospe1));
          ASSERT (!isnil(ospe2));
          
          ASSERT (7 == ospe1->getIt());
          ASSERT (6 == ospe1.get<Special>().myVal_);
          ASSERT (3 == ospe2.get<Special>().myVal_);
          
          ospe1 = DD<5>();            // but can be reassigned like any normal Opaque
          ASSERT (ospe1);
          ASSERT (5 == ospe1->getIt());
          VERIFY_ERROR (WRONG_TYPE, ospe1.get<Special>() );
          
          Opaque normal = DD<5>();
          ASSERT (normal);
          ASSERT (5 == normal->getIt());
#if false ////////////////////////////////////////////////////////TODO: restore throwing ASSERT
          // Assertion protects against SEGV
          VERIFY_ERROR (ASSERTION, normal = s1 );
#endif////////////////////////////////////////////////////////////
        }
    };
  
  
  LAUNCHER (OpaqueHolder_test, "unit common");
  
  
}} // namespace lib::test

