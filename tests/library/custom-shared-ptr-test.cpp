/*
  CustomSharedPtr(Test)  -  ref counting, equality and comparisons

  Copyright (C)         Lumiera.org
    2008, 2010,         Hermann Vosseler <Ichthyostega@web.de>

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

/** @file custom-shared-ptr-test.cpp
 ** unit test \ref CustomSharedPtr_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/p.hpp"

#include <boost/operators.hpp>



namespace lib {
namespace test{
  
  using std::string;
  using std::shared_ptr;
  using std::weak_ptr;
  
  using lumiera::error::LUMIERA_ERROR_ASSERTION;
  
  
  struct X
    : boost::totally_ordered<X>
    {
      long x_;
      
      X(long x=0)  : x_(x) {}
      operator long () { return x_; }
      
      bool operator<  (const X& ox)  const { return x_ <  ox.x_; }
      bool operator== (const X& ox)  const { return x_ == ox.x_; }
      
      virtual ~X() {} // using RTTI
    };
  
  struct XX : public X
    {
      long xx_;
      
      XX(long x=0)  : X(x), xx_(x+1) {}
    };
  
  
  /************************************************************//**
   * @test assure correct behaviour of lumiera's custom shared-ptr,
   *       including ADL of operators, shared ownership, typing
   *       and ordering
   * @see  lumiera::P
   */
  class CustomSharedPtr_test : public Test
    {
      virtual void
      run (Arg)
        {
          check_refcounting ();
          check_shared_ownership ();
          check_ownership_transfer ();
          check_type_relations ();
          check_ordering ();
        }
      
      
      /** @test smart-ptr basic behaviour */
      void
      check_refcounting ()
        {
          P<X> p1 (new X(7));
          CHECK (p1);
          CHECK (1 == p1.use_count());
          CHECK (7 == p1->x_);
          
            {
              P<X> p2 (new X(9));
              CHECK (1 == p2.use_count());
              
              p2.swap (p1);
              CHECK (1 == p1.use_count());
              CHECK (1 == p2.use_count());
              
              p2 = p1;
              CHECK (2 == p1.use_count());
              CHECK (2 == p2.use_count());
            }
          
          CHECK (1 == p1.use_count());
          CHECK (9 == p1->x_);
          
          p1.reset();
          CHECK (0 == p1.use_count());
          CHECK (!p1);
        }
      
      
      /** @test cooperation with other shared-ptr types */
      void
      check_shared_ownership ()
        {
          P<X> pX (new X(22));
          CHECK (pX);
          CHECK (1 == pX.use_count());
          
          weak_ptr<X> wX (pX);
          CHECK (wX.lock());
          CHECK (1 == pX.use_count());
          
          shared_ptr<X> sp1 (wX);
          shared_ptr<X> sp2 (pX);
          shared_ptr<X> sp3; sp3 = pX;
          
          CHECK (22 == sp3->x_);
          CHECK (4 == pX.use_count());
          CHECK (*pX  == *sp1);
          CHECK (*sp1 == *sp2);
          CHECK (*sp2 == *sp3);
          
          P<X> pX2;
          pX2.swap(pX);
          CHECK (!pX);
          CHECK (0 == pX.use_count());
          CHECK (4 == pX2.use_count());
          
          P<X, P<X>> pXX (pX2);  // a different type, but compatible pointers
          pX2 = pX;
          CHECK (!pX2);
          CHECK (0 == pX2.use_count());
          CHECK (4 == pXX.use_count());
          
          sp3 = sp2 = sp1 = pX;
          CHECK (22 == pXX->x_);
          CHECK (1 == pXX.use_count());
          CHECK (!sp1);
          CHECK (!sp2);
          CHECK (!sp3);
          
          CHECK (22 == wX.lock()->x_);
          CHECK (1 == pXX.use_count());
          
          pXX.reset();
          CHECK (!pXX);
          CHECK (!wX.lock());
        }
      
      
      void
      check_ownership_transfer ()
        {
          std::unique_ptr<X> up (new X(23));
          CHECK (up.get());
          
          P<X> pX (std::move(up));
          CHECK (!up.get());
          CHECK (pX);
          CHECK (1 == pX.use_count());
          CHECK (23 == pX->x_);
        }
      
      
      /** @test building type relationships on smart-ptrs */
      void
      check_type_relations ()
        {
          P<X> pX;                   // Base: shared_ptr<X>
          P<XX> pX1;                 // Base: shared_ptr<XX>
          P<XX,P<X>> pX2;            // Base: P<X>
          P<XX,shared_ptr<X>> pX3;   // Base: shared_ptr<X>
          P<XX,shared_ptr<long>> pLo;// Base: shared_ptr<long>       (rather nonsense, but well...)
          P<X,string> pLoL;          // Base: std::string
          P<string> pLoLoL;          // Base: shared_ptr<string>
          
          CHECK (INSTANCEOF (shared_ptr<X>, &pX));
          
          CHECK ( INSTANCEOF (shared_ptr<XX>, &pX1));
//        CHECK (!INSTANCEOF (shared_ptr<X>,  &pX1));      // doesn't compile (no RTTI) -- that's correct
//        CHECK (!INSTANCEOF (P<X>,           &pX1));      // similar, type mismatch detected by compiler
          
          CHECK ( INSTANCEOF (shared_ptr<X>,  &pX2));
//        CHECK (!INSTANCEOF (shared_ptr<XX>, &pX2));
          CHECK ( INSTANCEOF (P<X>,           &pX2));
          
          CHECK ( INSTANCEOF (shared_ptr<X>,  &pX3));
//        CHECK (!INSTANCEOF (shared_ptr<XX>, &pX3));
//        CHECK (!INSTANCEOF (P<X>,           &pX3));
          
          CHECK ( INSTANCEOF (shared_ptr<long>, &pLo));
//        CHECK (!INSTANCEOF (shared_ptr<X>,    &pLo));
//        CHECK (!INSTANCEOF (P<X>,             &pLo));
          
//        CHECK (!INSTANCEOF (shared_ptr<long>, &pLoL));
//        CHECK (!INSTANCEOF (shared_ptr<X>,    &pLoL));
//        CHECK (!INSTANCEOF (P<X>,             &pLoL));
          CHECK ( INSTANCEOF (string,           &pLoL));
          
          CHECK ( INSTANCEOF (shared_ptr<string>, &pLoLoL));
//        CHECK (!INSTANCEOF (string,             &pLoLoL));
//        CHECK (!INSTANCEOF (shared_ptr<X>,      &pLoLoL));
          
          pX = pX1;   // OK: pointee subtype...
          pX = pX2;   // invokes shared_ptr<X>::operator= (shared_ptr<X> const&)
          pX = pX3;
//        pX = pLo;   // similar, but long*   not assignable to X*
//        pX = pLoL;  // similar, but string* not assignable to X*
//        pX = pLoLoL;   // same...
                        //  you won't be able to do much with the "LoLo"-Types,
                       //   as their types and pointee types's relations don't match
          
          pX.reset (new XX(5));
          CHECK (5 == *pX);       // implicit conversion from X to long
          
          pX2 = pX;               // works, because both are implemented in terms of shared_ptr<X>
          CHECK (5 == pX2->x_);
          CHECK (6 == pX2->xx_);  // using the XX interface (performing dynamic downcast)
          
          pX3.reset (new X(7));   // again works because implemented in terms of shared_ptr<X>
          pX2 = pX3;              // same
          CHECK (pX2);            // both contain indeed a valid pointer....
          CHECK (pX3);
          CHECK (! pX2.get());    // but dynamic cast to XX at access fails
          CHECK (! pX3.get());
        }
      
      
      /** @test equality and ordering operators forwarded to pointee */
      void
      check_ordering ()
        {
          typedef P<X> PX;
          typedef P<XX,PX> PXX;
          
          PX pX1 (new X(3));
          PX pX2 (new XX(5));
          PX pX3, pX4, pX5, pX6;
          PXX pXX (new XX(7));
          
          pX3 = pXX;
          pX4.reset(new X(*pXX));
          
          CHECK ( (pX1 == pX1));    // reflexivity
          CHECK (!(pX1 != pX1));
          CHECK (!(pX1 <  pX1));
          CHECK (!(pX1 >  pX1));
          CHECK ( (pX1 <= pX1));
          CHECK ( (pX1 >= pX1));
          
          CHECK (!(pX1 == pX2));    // compare to same ptr type with larger pointee of subtype
          CHECK ( (pX1 != pX2));
          CHECK ( (pX1 <  pX2));
          CHECK (!(pX1 >  pX2));
          CHECK ( (pX1 <= pX2));
          CHECK (!(pX1 >= pX2));
          
          CHECK (!(pX2 == pXX));    // compare to ptr subtype with larger pointee of same subtype
          CHECK ( (pX2 != pXX));
          CHECK ( (pX2 <  pXX));
          CHECK (!(pX2 >  pXX));
          CHECK ( (pX2 <= pXX));
          CHECK (!(pX2 >= pXX));
          
          CHECK (!(pX1 == pXX));    // transitively compare to ptr subtype with larger pointee of subtype
          CHECK ( (pX1 != pXX));
          CHECK ( (pX1 <  pXX));
          CHECK (!(pX1 >  pXX));
          CHECK ( (pX1 <= pXX));
          CHECK (!(pX1 >= pXX));
          
          CHECK ( (pX3 == pXX));    // compare ptr to subtype ptr both referring to same pointee
          CHECK (!(pX3 != pXX));
          CHECK (!(pX3 <  pXX));
          CHECK (!(pX3 >  pXX));
          CHECK ( (pX3 <= pXX));
          CHECK ( (pX3 >= pXX));
          
          CHECK ( (pX4 == pXX));    // compare ptr to subtype ptr both referring to different but equal pointees
          CHECK (!(pX4 != pXX));
          CHECK (!(pX4 <  pXX));
          CHECK (!(pX4 >  pXX));
          CHECK ( (pX4 <= pXX));
          CHECK ( (pX4 >= pXX));
          
          CHECK (!(pXX == pX5));    // compare subtype ptr to empty ptr: "unequal but not orderable"
          CHECK ( (pXX != pX5));
          
          CHECK ( (pX5 == pX6));    // compare two empty ptrs: "equal, equivalent but not orderable"
          CHECK (!(pX5 != pX6));
          
          // order relations on NIL pointers disallowed
          
#if false ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
          VERIFY_ERROR (ASSERTION, pXX <  pX5 );
          VERIFY_ERROR (ASSERTION, pXX >  pX5 );
          VERIFY_ERROR (ASSERTION, pXX <= pX5 );
          VERIFY_ERROR (ASSERTION, pXX >= pX5 );
          
          VERIFY_ERROR (ASSERTION, pX5 <  pXX );
          VERIFY_ERROR (ASSERTION, pX5 >  pXX );
          VERIFY_ERROR (ASSERTION, pX5 <= pXX );
          VERIFY_ERROR (ASSERTION, pX5 >= pXX );
          
          VERIFY_ERROR (ASSERTION, pX5 <  pX6 );
          VERIFY_ERROR (ASSERTION, pX5 >  pX6 );
          VERIFY_ERROR (ASSERTION, pX5 <= pX6 );
          VERIFY_ERROR (ASSERTION, pX5 >= pX6 );
#endif    ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CustomSharedPtr_test, "unit common");
  
  
  
}} // namespace lib::test
