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
  
  using std::vector;
  using std::cout;
  using std::endl;
  
  namespace { // test dummy hierarchy
             //  Note: no vtable and common storage
    
    long _checksum = 0;
    
    
    struct Base   
      {
        uint id_;
        
        Base(uint i=0) : id_(i) { _checksum +=id_; }
        
        uint getIt() { return id_; }
      };
    
    template<uint ii>
    struct DD : Base
      {
        DD() : Base(ii)       { }
       ~DD() { _checksum -= ii; }
      };
    
    
    struct Special
      : DD<7>
      , BoolCheckable<Special>
      {
        uint myVal_;
        
        Special (uint val)
          : myVal_(val)
          { }
        
        bool
        isValid ()  const
          {
            return myVal_ % 2;
          }
      };
          
    
  }
  
  typedef OpaqueHolder<Base> Opaque;
  typedef vector<Opaque> TestList;
  
  /**********************************************************************************
   *  @test use the OpaqueHolder inline buffer to handle a family of classes
   *        through a common interface, without being forced to use heap storage
   *        or a custom allocator.
   */
  class OpaqueHolder_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          _checksum = 0;
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
        }
      
      
      static void
      reAccess (Opaque& elm)
        {
          cout << elm->getIt() << endl;
        }
      
      
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
          ASSERT (3 == oo->getIt());
          ASSERT (isSameObject (d3, *oo));
          VERIFY_ERROR (WRONG_TYPE, oo.get<D5>() );
          
          oo = D5();
          ASSERT (oo);
          ASSERT (5 == oo->getIt());
          VERIFY_ERROR (WRONG_TYPE, oo.get<D3>() );
          
          oo.clear();
          ASSERT (!oo);
          ASSERT (isnil(oo));
          
          Opaque o1 (oo);
          ASSERT (!o1);
          
          Opaque o2 (d3);
          ASSERT (!isSameObject (d3, *oo));
          ASSERT (3 == oo->getIt());
          
          ASSERT (sizeof(Opaque) == sizeof(Base) + sizeof(void*));
        }
      
      
      void
      checkSpecialSubclass ()
        {
          // define a "special" OpaqueHolder with enough storage to hold a Special 
          typedef OpaqueHolder<Base, sizeof(Special)> SpecialOpaque;
          
          ASSERT (sizeof(Special) > sizeof(Base));
          ASSERT (sizeof(SpecialOpaque) > sizeof(Opaque));
          ASSERT (sizeof(SpecialOpaque) == sizeof(Special) + sizeof(void*));
          
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
          ASSERT (5 == ospe1.get<Special>().myVal_);
          ASSERT (6 == ospe2.get<Special>().myVal_);
          
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

