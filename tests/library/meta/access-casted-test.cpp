/*
  AccessCasted(Test)  -  verify helper to cast or convert as appropriate

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file access-casted-test.cpp
 ** unit test \ref AccessCasted_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/access-casted.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <utility>
#include <string>

using std::move;
using std::string;
using std::ostream;

using util::isSameObject;

using LERR_(BOTTOM_VALUE);
using LERR_(WRONG_TYPE);


namespace util {
namespace test {
  
  
  
  namespace { // Test fixture...
    
    struct B {};
    struct D : B {};
    
    struct E : D
      {
        virtual ~E() {};
      };
    
    struct X
      {
        char x = 'x';
      };
    
    struct F
      : X
      , E
      { };
    
    
    ostream& operator<< (ostream& s, const B& b) { return s << "B{} adr="<<&b; }
    ostream& operator<< (ostream& s, const D& d) { return s << "D{} adr="<<&d; }
    ostream& operator<< (ostream& s, const E& e) { return s << "E{} adr="<<&e; }
    ostream& operator<< (ostream& s, const F& f) { return s << "F{} adr="<<&f; }
    
  }//(End)Test fixture
  
  
  
  
  
  
  /*************************************************************************************************//**
   * @test verify a helper template for accessing values either through conversion or (dynamic) downcast.
   *       Typically, this helper is used in value holder containers or variant-like data structures,
   *       where the actual type is given at instantiation time of the template and possibly erased.
   * 
   * @warning we can not really cover the negative cases, which ought to be rejected by the compiler.
   *          These cases have been verified one by one, and are retained commented out. You ought
   *          to re-check these cases manually when using a new compiler.
   */
  class AccessCasted_test : public Test
    {
      virtual void
      run (Arg)
        {
          
          D d;
          D* pD =&d;
          B* pB =pD;
          D& rD = *pD;
          B& rB = *pB;
          
          D*& rpD = pD;
          B*& rpB = pB;
          
          E e;
          E& rE = e;
          F f;
          E& rEF = f;
          E* pEF = &f;
          X* pXF = &f;
          F* pF  = &f;
          
          cout <<  "can_downcast<B,D>     = " << can_downcast<B,D>::value <<endl;
          cout <<  "can_downcast<B*,D*>   = " << can_downcast<B*,D*>::value <<endl;
          cout <<  "can_downcast<B&,D&>   = " << can_downcast<B&,D&>::value <<endl;
          cout <<  "can_downcast<B&,D*>   = " << can_downcast<B&,D*>::value <<endl;
          cout <<  "can_downcast<B*,D&>   = " << can_downcast<B*,D&>::value <<endl;
          cout <<  "can_downcast<B*&,D*&> = " << can_downcast<B*&,D*&>::value <<endl;
          cout <<  "can_downcast<D*&,D*&> = " << can_downcast<D*&,D*&>::value <<endl;
          
          cout <<  "can_downcast<D*,E*>   = " << can_downcast<D*,E*>::value <<endl;
          cout <<  "can_downcast<E*,F*>   = " << can_downcast<E*,F*>::value <<endl;
          
          cout <<  "has_RTTI<D*> = " << has_RTTI<D*>::value <<endl;
          cout <<  "has_RTTI<E*> = " << has_RTTI<E*>::value <<endl;
          cout <<  "has_RTTI<F*> = " << has_RTTI<F*>::value <<endl;
          
          cout <<  "is_convertible<D,D&>  = " << std::is_convertible<D,D&>::value <<endl;
          cout <<  "is_convertible<D&,D>  = " << std::is_convertible<D&,D>::value <<endl;
          
          
          cout <<  "can_use_dynamic_downcast<D,D&>  = " << can_use_dynamic_downcast<D,D&>::value <<endl;
          cout <<  "can_use_conversion<D,D&>        = " << can_use_conversion<D,D&>::value <<endl;
          cout <<  "can_use_dynamic_downcast<B*,D*> = " << can_use_dynamic_downcast<B*,D*>::value <<endl;
          cout <<  "can_use_conversion<D*,B*>       = " << can_use_conversion<D*,B*>::value <<endl;
          
          cout <<  "can_use_dynamic_downcast<D*&,D*&> = " << can_use_dynamic_downcast<D*&,D*&>::value <<endl;
          cout <<  "can_use_conversion<D*&,D*&>       = " << can_use_conversion<D*&,D*&>::value <<endl;
          cout <<  "can_use_conversion<D*,E*>         = " << can_use_conversion<D*,E*>::value <<endl;
          cout <<  "can_use_dynamic_downcast<D*&,E*>  = " << can_use_dynamic_downcast<D*&,E*>::value <<endl;
          cout <<  "can_use_conversion<E*,F*>         = " << can_use_conversion<E*,F*>::value <<endl;
          cout <<  "can_use_dynamic_downcast<E*,F*>   = " << can_use_dynamic_downcast<E*,F*>::value <<endl;
          
          
          
          cout <<  "=== standard case: References ==="<<endl;
          cout <<  "Access(D  as D&)    --->" << AccessCasted<D&>::access(d)  <<endl;
          cout <<  "Access(D& as D&)    --->" << AccessCasted<D&>::access(rD) <<endl;
          D dd1(d);
          // AccessCasted<D&>::access(move(dd1)); // does not compile since it would be dangerous; we can't take a l-value ref
          // AccessCasted<D&&>::access(rD);       // from a r-value (move) reference and we can't move a l-value ref
          // AccessCasted<D&&>::access(d);        //
          
          cout <<  "=== build a value object ==="<<endl;
          cout <<  "Access(D  as D)     --->" << AccessCasted<D>::access(d) <<endl;
          cout <<  "Access(D& as D)     --->" << AccessCasted<D>::access(rD) <<endl;
          cout <<  "Access(D&& as D)    --->" << AccessCasted<D>::access(move(dd1)) <<endl;
          
          cout <<  "=== take a pointer ==="<<endl;
          cout <<  "Access(D  as D*)    --->" << AccessCasted<D*>::access(d)  <<endl;
          cout <<  "Access(D& as D*)    --->" << AccessCasted<D*>::access(rD) <<endl;
          // AccessCasted<D*>::access(move(dd1)); // should not take value moved by r-value-ref as pointer, otherwise the moved object would be lost
          
          cout <<  "=== dereference a pointer ==="<<endl;
          cout <<  "Access(D* as D&)    --->" << AccessCasted<D&>::access(pD)  <<endl;
          cout <<  "Access(D* as D)     --->" << AccessCasted<D>::access(pD)  <<endl;
          D* pdd1(pD);
          cout <<  "Access(D*&& as D)   --->" << AccessCasted<D>::access(move(pdd1))  <<endl;
          D* pNull(0);
          VERIFY_ERROR (BOTTOM_VALUE, AccessCasted<D>::access(pNull));  // run-time NULL check
          // AccessCasted<D&&>::access(pD);       // should not move away a value accessed through a pointer, there might be other users
          
          cout <<  "=== const correctness ==="<<endl;
          cout <<  "Access(D  as D const&)       --->" << AccessCasted<D const&>::access(d)  <<endl;
          cout <<  "Access(D& as D const&)       --->" << AccessCasted<D const&>::access(rD) <<endl;
          cout <<  "Access(D  as const D)        --->" << AccessCasted<const D>::access(d) <<endl;
          cout <<  "Access(D& as const D)        --->" << AccessCasted<const D>::access(rD) <<endl;
          cout <<  "Access(D  as const D*)       --->" << AccessCasted<const D*>::access(d)  <<endl;
          cout <<  "Access(D& as const D*)       --->" << AccessCasted<const D*>::access(rD) <<endl;
          cout <<  "Access(D* as D const&)       --->" << AccessCasted<D const&>::access(pD)  <<endl;
          cout <<  "Access(D* as const D)        --->" << AccessCasted<const D>::access(pD)  <<endl;
          const D cD(d);
          D const& rcD(d);
          const D* pcD(&cD);
          cout <<  "Access(const D  as D const&) --->" << AccessCasted<D const&>::access(cD)  <<endl;
          cout <<  "Access(D const& as D const&) --->" << AccessCasted<D const&>::access(rcD) <<endl;
          cout <<  "Access(const D  as const D)  --->" << AccessCasted<const D>::access(cD) <<endl;
          cout <<  "Access(D const& as const D)  --->" << AccessCasted<const D>::access(rcD) <<endl;
          cout <<  "Access(const D  as const D*) --->" << AccessCasted<const D*>::access(cD)  <<endl;
          cout <<  "Access(D const& as const D*) --->" << AccessCasted<const D*>::access(rcD) <<endl;
          cout <<  "Access(const D* as D const&) --->" << AccessCasted<D const&>::access(pcD)  <<endl;
          cout <<  "Access(const D* as const D)  --->" << AccessCasted<const D>::access(pcD)  <<endl;
          cout <<  "Access(D const& as D)        --->" << AccessCasted<D>::access(rcD)       <<endl;   // it's OK to construct a new (non-const) object from const ref
          const D cD1(cD);                                                                             // likewise it's OK to construct from move-ref. Actually, we're not
          cout <<  "Access(D const&& as D)       --->" << AccessCasted<D>::access(move(cD1)) <<endl;   // moving anything, but it's up to the receiving ctor to prevent that
          // AccessCasted<D&>::access(rcD);       // normal ref from const ref is not const correct
          // AccessCasted<D*>::access(rcD);       // likewise, regular pointer from const ref prohibited
          // AccessCasted<D&>::access(pcD);       // likewise, regular ref from pointer-to-const
          // AccessCasted<D*>::access(pcD);       // and regular pointer from pointer-to-const
          // AccessCasted<D&&>::access(rcD);      // ruled out already because moving a reference is invalid
          // AccessCasted<D&&>::access(pcD);      // ruled out already because moving a dereferenced pointer is invalid
          // AccessCasted<D&>::access(move(cD));  // ruled out already because taking reference from moved value is invalid
          // AccessCasted<D*>::access(move(cD));  // and same for taking pointer from a moved value.
          
          cout <<  "=== work cases: actual conversions ==="<<endl;
          cout <<  "Access(B& as B&)             --->" << AccessCasted<B&>::access(rB) <<endl;
          cout <<  "Access(D& as B&)             --->" << AccessCasted<B&>::access(rD) <<endl;
          cout <<  "Access(B* as B*)             --->" << AccessCasted<B*>::access(pB) <<endl;
          cout <<  "Access(D* as B*)             --->" << AccessCasted<B*>::access(pD) <<endl;
          cout <<  "Access(D& as B*)             --->" << AccessCasted<B*>::access(rD) <<endl;
          cout <<  "Access(D* as B&)             --->" << AccessCasted<B&>::access(pD) <<endl;
          cout <<  "Access(B*& as B*&)           --->" << AccessCasted<B*&>::access(rpB) <<endl;
          cout <<  "Access(D*& as D*&)           --->" << AccessCasted<D*&>::access(rpD) <<endl;
          cout <<  "Access(D& as const B*)       --->" << AccessCasted<const B*>::access(rD) <<endl;
          cout <<  "Access(D* as B const&)       --->" << AccessCasted<B const&>::access(pD) <<endl;
          cout <<  "Access(D const& as const B*) --->" << AccessCasted<const B*>::access(rcD) <<endl;
          cout <<  "Access(const D* as B const&) --->" << AccessCasted<B const&>::access(pcD) <<endl;
          // AccessCasted<B*&>::access(rpD);      // ruled out, since it would allow to sneak-in a non-D object into the D*
          // AccessCasted<D&>::access(rB);        // any down-casts are ruled out,
          // AccessCasted<D*>::access(pB);        // since neither B nor D has RTTI
          // AccessCasted<D&>::access(pB);        //
          // AccessCasted<D*>::access(rB);        //
          // AccessCasted<E&>::access(rD);        // we need RTTI on both ends to perform a safe dynamic downcast.
          // AccessCasted<D*>::access((B*)pD);    // dangerous, since we have no way to know for sure it's indeed a D object
          // AccessCasted<E*>::access(pDE);       // same here, since E has RTTI but D hasn't, we have no way to find out the real type
          
          VERIFY_ERROR (WRONG_TYPE, AccessCasted<F&>::access(rE));                 // allowed by typing, but fails at runtime since it isn't an F-object
          cout <<  "Access(E(F)& as F&)          --->" << AccessCasted<F&>::access(rEF) <<endl;
          cout <<  "Access(E(F)* as F*)          --->" << AccessCasted<F*>::access(pEF) <<endl;
          cout <<  "Access(E(F)* as F&)          --->" << AccessCasted<F&>::access(pEF) <<endl;
          cout <<  "Access(E(F)& as F*)          --->" << AccessCasted<F*>::access(pEF) <<endl;
          cout <<  "Access(F* as X*)             --->" << AccessCasted<X*>::access(pF)  <<endl; // upcast to the other mixin is OK
          cout <<  "Access(X(F)* as X*)          --->" << AccessCasted<X*>::access(pXF) <<endl; //  (and note: address adjustment due to mixin layout)
          cout <<  "Access(F* as B&)             --->" << AccessCasted<B&>::access(pF)  <<endl; // upcast to base
          cout <<  "Access(F* as E&)             --->" << AccessCasted<E&>::access(pF)  <<endl; // upcast to parent (retaining the RTTI)
          // AccessCasted<X*>::access(pEF);       // cross-cast not supported (to complicated to implement)
          // AccessCasted<F*>::access(pXF);       // downcast not possible, since X does not provide RTTI
          
          int i = 2;
          float fp = 3.1415;
          cout <<  "Access(int as double) --->" << AccessCasted<double>::access(i) <<endl;
          cout <<  "Access(float as long) --->" << AccessCasted<long>::access(fp) <<endl;
          // AccessCasted<double&>::access(i);    // would undermine the type system, thus ruled out
          // AccessCasted<double const&>::access(i); // allowed, but warning: returning reference to temporary (and the warning is justified)
          
          CHECK (isSameObject (d, AccessCasted<B&>::access(d)));
          CHECK (isSameObject (rD, AccessCasted<B&>::access(pD)));
          CHECK (isSameObject (d, AccessCasted<B const&>::access(pD)));
          CHECK (!isSameObject (d, AccessCasted<B>::access(rD)));
          
          CHECK (isSameObject (f, AccessCasted<F&>::access(rEF)));
          CHECK (!isSameObject (f, AccessCasted<X&>::access(pF))); // note: address adjustment due to layout of mixin object X
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (AccessCasted_test, "unit common");
  
  
  
}} // namespace lib::meta::test
