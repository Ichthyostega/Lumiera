/*
  CustomSharedPtr(Test)  -  refcounting, equality and comparisons
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "common/test/run.hpp"
#include "common/util.hpp"

#include "common/p.hpp"

#include <boost/operators.hpp>

using std::string;


namespace asset
  {
  namespace test
    {
    using lumiera::P;
    using std::tr1::shared_ptr;
    using std::tr1::weak_ptr;
    
    
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
    
      
    /****************************************************************
     * @test assure correct behaviour of lumiera's custom shared-ptr,
     *       including ADL of operators, shared ownership, typing
     *       and ordering
     * @see  lumiera::P
     */
    class CustomSharedPtr_test : public Test
      {
        virtual void
        run (Arg arg) 
          {
            check_refcounting ();
            check_shared_ownership ();
            check_type_relations ();
            check_ordering ();
          }
        
        
        /** @test smart-ptr basic behaviour */
        void
        check_refcounting ()
          {
            P<X> p1 (new X(7));
            ASSERT (p1);
            ASSERT (1 == p1.use_count());
            ASSERT (7 == p1->x_);
            
              {
                P<X> p2 (new X(9));
                ASSERT (1 == p2.use_count());

                p2.swap (p1);
                ASSERT (1 == p1.use_count());
                ASSERT (1 == p2.use_count());
                
                p2 = p1;
                ASSERT (2 == p1.use_count());
                ASSERT (2 == p2.use_count());
              }
              
            ASSERT (1 == p1.use_count());
            ASSERT (9 == p1->x_);
            
            p1.reset();
            ASSERT (0 == p1.use_count());
            ASSERT (!p1);
          }
        
        
        /** @test cooperation with other shared-ptr types */
        void
        check_shared_ownership ()
          {
            std::auto_ptr<X> au (new X(22));
            ASSERT (au.get());
            
            P<X> pX (au);
            ASSERT (!au.get());
            ASSERT (pX);
            ASSERT (1 == pX.use_count());
            ASSERT (22 == pX->x_);
            
            weak_ptr<X> wX (pX);
            ASSERT (wX.lock());
            ASSERT (1 == pX.use_count());
            
            shared_ptr<X> sp1 (wX);
            shared_ptr<X> sp2 (pX); 
            shared_ptr<X> sp3; sp3 = pX;
            
            ASSERT (22 == sp3->x_);
            ASSERT (4 == pX.use_count());
            ASSERT (*pX  == *sp1);
            ASSERT (*sp1 == *sp2);
            ASSERT (*sp2 == *sp3);
            
            P<X> pX2;
            pX2.swap(pX);
            ASSERT (!pX);
            ASSERT (0 == pX.use_count());
            ASSERT (4 == pX2.use_count());
            
            P<X, P<X> > pXX (pX2);  // a different type, but compatible pointers
            pX2 = pX;
            ASSERT (!pX2);
            ASSERT (0 == pX2.use_count());
            ASSERT (4 == pXX.use_count());
            
            sp3 = sp2 = sp1 = pX;
            ASSERT (22 == pXX->x_);
            ASSERT (1 == pXX.use_count());
            ASSERT (!sp1);
            ASSERT (!sp2);
            ASSERT (!sp3);
            
            ASSERT (22 == wX.lock()->x_);
            ASSERT (1 == pXX.use_count());

            pXX.reset();
            ASSERT (!pXX);
            ASSERT (!wX.lock());
          }
        
        
        /** @test building type relationships on smart-ptrs */
        void
        check_type_relations ()
          {
            P<X> pX;                    // Base: shared_ptr<X>
            P<XX> pX1;                  // Base: shared_ptr<XX>
            P<XX,P<X> > pX2;            // Base: P<X>
            P<XX,shared_ptr<X> > pX3;   // Base: shared_ptr<X>
            P<XX,shared_ptr<long> > pLo;// Base: shared_ptr<long>       (quite a nonsene, but well...)
            P<X,string> pLoL;           // Base: std::string
            P<string> pLoLoL;           // Base: shared_ptr<string>
            
            ASSERT (INSTANCEOF (P<X>, &pX));
            ASSERT (INSTANCEOF (shared_ptr<X>, &pX));
            
            ASSERT ( INSTANCEOF (shared_ptr<XX>, &pX1));
//          ASSERT (!INSTANCEOF (shared_ptr<X>,  &pX1));     // doesn't compile (no RTTI) -- that's correct
//          ASSERT (!INSTANCEOF (P<X>,           &pX1));
            
            ASSERT ( INSTANCEOF (shared_ptr<X>,  &pX2));
//          ASSERT (!INSTANCEOF (shared_ptr<XX>, &pX2));
            ASSERT ( INSTANCEOF (P<X>,           &pX2));
            
            ASSERT ( INSTANCEOF (shared_ptr<X>,  &pX3));
//          ASSERT (!INSTANCEOF (shared_ptr<XX>, &pX3));
//          ASSERT (!INSTANCEOF (P<X>,           &pX3));
            
            ASSERT ( INSTANCEOF (shared_ptr<long>, &pLo));
//          ASSERT (!INSTANCEOF (shared_ptr<X>,    &pLo));
//          ASSERT (!INSTANCEOF (P<X>,             &pLo));
            
//          ASSERT (!INSTANCEOF (shared_ptr<long>, &pLoL));
//          ASSERT (!INSTANCEOF (shared_ptr<X>,    &pLoL));
//          ASSERT (!INSTANCEOF (P<X>,             &pLoL));
            ASSERT ( INSTANCEOF (string,           &pLoL));
            
            ASSERT ( INSTANCEOF (shared_ptr<string>, &pLoLoL));
//          ASSERT (!INSTANCEOF (string,             &pLoLoL));
//          ASSERT (!INSTANCEOF (shared_ptr<X>,      &pLoLoL));
            
            pX = pX1;   // OK: pointee subtype...
            pX = pX2;   // invokes shared_ptr<X>::operator= (shared_ptr<X> const&)
            pX = pX3;
//          pX = pLo;   // similar, but long*   not asignable to X*
//          pX = pLoL;  // similar, but string* not asignable to X*
//          pX = pLoLoL;   // same...
                          //  you won't be able to do much with the "LoLo"-Types, 
                         //   as their types and pointee types's relations don't match
            
            pX.reset (new XX(5));
            ASSERT (5 == *pX);      // implicit conversion from X to long
            
            pX2 = pX;               // works, because both are implemented in terms of shared_ptr<X>
            ASSERT (5 == pX2->x_); 
            ASSERT (6 == pX2->xx_); // using the XX interface (performing dynamic downcast)
            
            pX3.reset (new X(7));   // again works because implemented in terms of shared_ptr<X> 
            pX2 = pX3;              // same
            ASSERT (pX2);           // both contain indeed a valid pointer....
            ASSERT (pX3);
            ASSERT (! pX2.get());        // but dynamic cast to XX at access fails
            ASSERT (! pX3.get());
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
            
            ASSERT ( (pX1 == pX1));    // reflexivity
            ASSERT (!(pX1 != pX1));
            ASSERT (!(pX1 <  pX1));
            ASSERT (!(pX1 >  pX1));
            ASSERT ( (pX1 <= pX1));
            ASSERT ( (pX1 >= pX1));
            
            ASSERT (!(pX1 == pX2));    // compare to same ptr type with larger pointee of subtype
            ASSERT ( (pX1 != pX2));
            ASSERT ( (pX1 <  pX2));
            ASSERT (!(pX1 >  pX2));
            ASSERT ( (pX1 <= pX2));
            ASSERT (!(pX1 >= pX2));
            
            ASSERT (!(pX2 == pXX));    // compare to ptr subtype with larger pointee of same subtype
            ASSERT ( (pX2 != pXX));
            ASSERT ( (pX2 <  pXX));
            ASSERT (!(pX2 >  pXX));
            ASSERT ( (pX2 <= pXX));
            ASSERT (!(pX2 >= pXX));
            
            ASSERT (!(pX1 == pXX));    // transitively compare to ptr subtype with larger pointee of subtype
            ASSERT ( (pX1 != pXX));
            ASSERT ( (pX1 <  pXX));
            ASSERT (!(pX1 >  pXX));
            ASSERT ( (pX1 <= pXX));
            ASSERT (!(pX1 >= pXX));
            
            ASSERT ( (pX3 == pXX));    // compare ptr to subtype ptr both referring to same pointee
            ASSERT (!(pX3 != pXX));
            ASSERT (!(pX3 <  pXX));
            ASSERT (!(pX3 >  pXX));
            ASSERT ( (pX3 <= pXX));
            ASSERT ( (pX3 >= pXX));
            
            ASSERT ( (pX4 == pXX));    // compare ptr to subtype ptr both referring to different but equal pointees 
            ASSERT (!(pX4 != pXX));
            ASSERT (!(pX4 <  pXX));
            ASSERT (!(pX4 >  pXX));
            ASSERT ( (pX4 <= pXX));
            ASSERT ( (pX4 >= pXX));
            
            ASSERT (!(pXX == pX5));    // compare subtype ptr to empty ptr: "unequal but not orderable"
            ASSERT ( (pXX != pX5));
            ASSERT (!(pXX <  pX5));
            ASSERT (!(pXX >  pX5));
            ASSERT (!(pXX <= pX5));
            ASSERT (!(pXX >= pX5));
            
            ASSERT ( (pX5 == pX6));    // compare two empty ptrs: "equal, aequivalent but not orderable"
            ASSERT (!(pX5 != pX6));
            ASSERT (!(pX5 <  pX6));
            ASSERT (!(pX5 >  pX6));
            ASSERT ( (pX5 <= pX6));
            ASSERT ( (pX5 >= pX6));
          }
      };
    
    
    /** Register this test class... */
    LAUNCHER (CustomSharedPtr_test, "unit common");
    
    
    
  } // namespace test

} // namespace asset
