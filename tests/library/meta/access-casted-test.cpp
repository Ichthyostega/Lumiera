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



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/access-casted.hpp"

#include <iostream>
#include <utility>
#include <string>

using std::move;
using std::string;
using std::ostream;
using std::cout;
using std::endl;

using lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE;


namespace util {
namespace test {
  
  
    
  
  
  
  namespace { // Test fixture...
    
    struct B {};
    struct D : B {};
    
    struct E : D
      {
        virtual ~E() {};
      };
    struct F : E {};
    
    using lib::test::tyAbbr;
    
    ostream& operator<< (ostream& s, const B& b) { return s << "B{} adr="<<&b<<" type: "<<tyAbbr(b); }
    ostream& operator<< (ostream& s, const D& d) { return s << "D{} adr="<<&d<<" type: "<<tyAbbr(d); }
    ostream& operator<< (ostream& s, const E& e) { return s << "E{} adr="<<&e<<" type: "<<tyAbbr(e); }
    ostream& operator<< (ostream& s, const F& f) { return s << "F{} adr="<<&f<<" type: "<<tyAbbr(f); }
    
  }//(End)Test fixture
  
  
  
  
  
  
  /*************************************************************************************************//**
   * @test verify a helper template for accessing values either through conversion or (dynamic) downcast.
   *       Typically, this helper is used in value holder containers or variant-like data structures,
   *       where the actual type is given at instantiation time of the template and possibly erased.
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
          
          F f;
          E& rE = f;
          E* pE = &f;
          D* pDE = pE;
          
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
          
          cout <<  "=== standard case: References ==="<<endl;
          cout <<  "Access(D  as D&)    --->" << AccessCasted<D&>::access(d)  <<endl;
          cout <<  "Access(D& as D&)    --->" << AccessCasted<D&>::access(rD) <<endl;
          
          cout <<  "=== build a value object ==="<<endl;
          cout <<  "Access(D  as D)     --->" << AccessCasted<D>::access(d) <<endl;
          cout <<  "Access(D& as D)     --->" << AccessCasted<D>::access(rD) <<endl;
          D dd1(d);
          cout <<  "Access(D&& as D)    --->" << AccessCasted<D>::access(move(dd1)) <<endl;
          
          cout <<  "=== take a pointer ==="<<endl;
          cout <<  "Access(D  as D*)    --->" << AccessCasted<D*>::access(d)  <<endl;
          cout <<  "Access(D& as D*)    --->" << AccessCasted<D*>::access(rD) <<endl;
          
          cout <<  "=== dereference a pointer ==="<<endl;
          cout <<  "Access(D* as D&)    --->" << AccessCasted<D&>::access(pD)  <<endl;
          cout <<  "Access(D* as D)     --->" << AccessCasted<D>::access(pD)  <<endl;
          D* pdd1(pD);
          cout <<  "Access(D*&& as D)   --->" << AccessCasted<D>::access(move(pdd1))  <<endl;
          D* pNull(0);
          VERIFY_ERROR(BOTTOM_VALUE, AccessCasted<D>::access(pNull));
          
          cout <<  "=== work cases: actual conversions ==="<<endl;
//        cout <<  "Access(B& as D&)    --->" << AccessCasted<D&>::access(rB) <<endl;
//        cout <<  "Access(D* as D*)    --->" << AccessCasted<D*>::access(pD) <<endl;
//        cout <<  "Access(B* as D*)    --->" << AccessCasted<D*>::access(pB) <<endl;
//        cout <<  "Access(D*& as D*&)  --->" << AccessCasted<D*&>::access(rpD) <<endl;
//        cout <<  "Access(B*& as D*&)  --->" << AccessCasted<D*&>::access(rpB) <<endl;
//        
//        cout <<  "Access(D  as B&)    --->" << AccessCasted<B&>::access(d)  <<endl;
//        cout <<  "Access(D& as B&)    --->" << AccessCasted<B&>::access(rD) <<endl;
//        cout <<  "Access(B& as B&)    --->" << AccessCasted<D&>::access(rB) <<endl;
//        cout <<  "Access(D* as B*)    --->" << AccessCasted<B*>::access(pD) <<endl;
//        cout <<  "Access(B* as B*)    --->" << AccessCasted<B*>::access(pB) <<endl;
//        cout <<  "Access(D*& as B*&)  --->" << AccessCasted<B*&>::access(rpD) <<endl;
//        cout <<  "Access(B*& as B*&)  --->" << AccessCasted<B*&>::access(rpB) <<endl;
//        
//        cout <<  "Access(D  as E&)    --->" << AccessCasted<E&>::access(d) <<endl;
//        cout <<  "Access(E& as F&)    --->" << AccessCasted<F&>::access(rE) <<endl;
//        cout <<  "Access(D(E)* as E*) --->" << AccessCasted<E*>::access(pDE) <<endl;
//        cout <<  "Access(D(E)* as F*) --->" << AccessCasted<F*>::access(pDE) <<endl;
//        cout <<  "Access(E* as F*)    --->" << AccessCasted<F*>::access(pE) <<endl;
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (AccessCasted_test, "unit common");
  
  
  
}} // namespace lib::meta::test
