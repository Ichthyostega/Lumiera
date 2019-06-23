/*
  FunctorUtil(Test)  -  verifying function object and signal utilities

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file functor-util-test.cpp
 ** unit test \ref FunctorUtil_test
 */



#include "lib/test/run.hpp"
#include "lib/functor-util.hpp"

#include <functional>
#include <boost/functional/hash.hpp>
#include <iostream>

using lib::HashVal;
using std::cout;
using std::function;
using boost::hash;         // note: only boost::hash allows for easy defining of custom hash functions 


namespace util {
namespace test {
  
  
  namespace {
    
    void fun1 (int i) { cout << "fun1 (" << i << ")\n"; }
    void fun2 (int i) { cout << "fun2 (" << i << ")\n"; }
    
    struct Dummy
      {
        void gummi (int i) { cout << "gummi (" << i << ")\n"; }
      };
  }
  
  
  
  
  
  /*****************************************************************//**
   * @test verify some aspects of the functor-util's behaviour.
   *       At times, this is just a scrapbook for new ideas....
   */
  class FunctorUtil_test : public Test
    {
      virtual void
      run (Arg) 
        {
//          verifyBruteForceComparison();
//          verifyHashThroughBackdoor();
        }
      
      typedef function<void(int)> Fvi;
      typedef function<int(void)> Fiv;
      
      typedef function<void(void)> Fvv;
      
      
      /** @test workaround for the missing functor comparison operator */
//      void
//      verifyBruteForceComparison()
//        {
//          Fvi f0;
//          Fvi f1 (fun1);
//          Fvi f2 (fun2);
//          
//          CHECK (!rawComparison(f0, f1));
//          CHECK (!rawComparison(f1, f2));
//          CHECK (!rawComparison(f0, f2));
//          
//          Fvi f22 (f2);
//          CHECK ( rawComparison(f2, f22));
//          
//          f1 = f2;
//          CHECK ( rawComparison(f1, f2));
//          
//          CHECK (!rawComparison(f0, Fvi()));   // note: can't detect they are equivalent
//          CHECK (!rawComparison(f0, Fiv()));
//          
//          f1 = bind (fun2, _1);
//          CHECK (!rawComparison(f1, f2));
//          
//          Dummy dum1, dum2;
//          Fvi fm1 = bind (&Dummy::gummi, dum1, _1);
//          Fvi fm2 = bind (&Dummy::gummi, dum2, _1);
//          Fvv fm3 = bind (&Dummy::gummi, dum1, 23);
//          Fvv fm4 = bind (&Dummy::gummi, dum1, 24);
//          Fvv fm5 = bind (&Dummy::gummi, dum2, 24);
//          Fvv fm6 = bind (&Dummy::gummi, dum2, 24);
//          
//          CHECK (!rawComparison(f1, fm1));
//          
//          CHECK (!rawComparison(fm1, fm2));
//          CHECK (!rawComparison(fm1, fm3));
//          CHECK (!rawComparison(fm1, fm4));
//          CHECK (!rawComparison(fm1, fm5));
//          CHECK (!rawComparison(fm1, fm6));
//          CHECK (!rawComparison(fm2, fm3));
//          CHECK (!rawComparison(fm2, fm4));
//          CHECK (!rawComparison(fm2, fm5));
//          CHECK (!rawComparison(fm2, fm6));
//          CHECK (!rawComparison(fm3, fm4));
//          CHECK (!rawComparison(fm3, fm5));
//          CHECK (!rawComparison(fm3, fm6));
//          CHECK (!rawComparison(fm4, fm5));   //  note: same argument but different functor instance
//          CHECK (!rawComparison(fm4, fm6));
//          CHECK (!rawComparison(fm5, fm6));   // again: can't detect they are equivalent
//        }
      
      
      /** @test workaround for missing standard hash
       *        calculation for functor objects.
       *        Workaround relying on boost
       *        implementation internals */
//      void
//      verifyHashThroughBackdoor()
//        {
//          Fvi f0;
//          Fvi f1 (fun1);
//          Fvi f2 (fun2);
//          Fvi f22 (f2);
//          
//          hash<Fvi> calculateHash;
//          CHECK (calculateHash (f0));
//          CHECK (calculateHash (f1));
//          CHECK (calculateHash (f2));
//          CHECK (calculateHash (f22));
//          
//          HashVal h0 = calculateHash (f0);
//          HashVal h1 = calculateHash (f1);
//          HashVal h2 = calculateHash (f2);
//          HashVal h22 = calculateHash (f22);
//          
//          CHECK (h0 != h1);
//          CHECK (h0 != h2);
//          CHECK (h1 != h2);
//          
//          CHECK (h2 == h22);
//          
//          f1 = f2;
//          h1 = calculateHash (f1);
//          CHECK (h1 == h2);
//          CHECK (h1 != h0);
//          
//          CHECK (h0 != calculateHash (Fvi()));    // note: equivalence not detected
//          
//          // checking functors based on member function(s)
//          Dummy dum1, dum2;
//          Fvi fm1 = bind (&Dummy::gummi, dum1, _1);
//          Fvi fm2 = bind (&Dummy::gummi, dum2, _1);
//          Fvv fm3 = bind (&Dummy::gummi, dum1, 23);
//          Fvv fm4 = bind (&Dummy::gummi, dum1, 24);
//          Fvv fm5 = bind (&Dummy::gummi, dum2, 24);
//          Fvv fm6 = bind (&Dummy::gummi, dum2, 24);
//          
//          HashVal hm1 = calculateHash (fm1);
//          HashVal hm2 = calculateHash (fm2);
//          
//          hash<Fvv> calculateHashVV;
//          HashVal hm3 = calculateHashVV (fm3);
//          HashVal hm4 = calculateHashVV (fm4);
//          HashVal hm5 = calculateHashVV (fm5);
//          HashVal hm6 = calculateHashVV (fm6);
//          
//          CHECK (h1  != hm1);
//          
//          CHECK (hm1 != hm2);
//          CHECK (hm1 != hm3);
//          CHECK (hm1 != hm4);
//          CHECK (hm1 != hm5);
//          CHECK (hm1 != hm6);
//          CHECK (hm2 != hm3);
//          CHECK (hm2 != hm4);
//          CHECK (hm2 != hm5);
//          CHECK (hm2 != hm6);
//          CHECK (hm3 != hm4);
//          CHECK (hm3 != hm5);
//          CHECK (hm3 != hm6);
//          CHECK (hm4 != hm5);
//          CHECK (hm4 != hm6);
//          CHECK (hm5 != hm6);   // again: unable to detect the equivalence
//        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (FunctorUtil_test, "unit common");
  
  
  
}} // namespace util::test
