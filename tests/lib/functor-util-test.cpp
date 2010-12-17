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



#include "lib/test/run.hpp"
//#include "lib/util.hpp"
#include "lib/functor-util.hpp"

#include <tr1/functional>
#include <iostream>

//using util::isnil;
//using std::string;
using std::cout;
using std::tr1::function;


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
  
  
  
  
  
  /*********************************************************************
   * @test verify some aspects of the functor-util's behaviour.
   *       Often, this is just a scrapbook for new ideas....
   */
  class FunctorUtil_test : public Test
    {
      virtual void
      run (Arg) 
        {
          verifyBruteForceComparison();
        }
      
      
      /** @test workaround for the missing functor comparison operator */
      void
      verifyBruteForceComparison()
        {
          typedef function<void(int)> Fvi;
          typedef function<int(void)> Fiv;
          
          typedef function<void(void)> Fvv;
          
          Fvi f0;
          Fvi f1 (fun1);
          Fvi f2 (fun2);
          
          CHECK (!rawComparison(f0, f1));
          CHECK (!rawComparison(f1, f2));
          CHECK (!rawComparison(f0, f2));
          
          Fvi f22 (f2);
          CHECK ( rawComparison(f2, f22));
          
          f1 = f2;
          CHECK ( rawComparison(f1, f2));
          
          CHECK (!rawComparison(f0, Fvi()));   // note: can't detect they are equivalent
          CHECK (!rawComparison(f0, Fiv()));
          
          f1 = bind (fun2, _1);
          CHECK (!rawComparison(f1, f2));
          
          Dummy dum1, dum2;
          Fvi fm1 = bind (&Dummy::gummi, dum1, _1);
          Fvi fm2 = bind (&Dummy::gummi, dum2, _1);
          Fvv fm3 = bind (&Dummy::gummi, dum1, 23);
          Fvv fm4 = bind (&Dummy::gummi, dum1, 24);
          Fvv fm5 = bind (&Dummy::gummi, dum2, 24);
          Fvv fm6 = bind (&Dummy::gummi, dum2, 24);
          
          CHECK (!rawComparison(f1, fm1));
          
          CHECK (!rawComparison(fm1, fm2));
          CHECK (!rawComparison(fm1, fm3));
          CHECK (!rawComparison(fm1, fm4));
          CHECK (!rawComparison(fm1, fm5));
          CHECK (!rawComparison(fm1, fm6));
          CHECK (!rawComparison(fm2, fm3));
          CHECK (!rawComparison(fm2, fm4));
          CHECK (!rawComparison(fm2, fm5));
          CHECK (!rawComparison(fm2, fm6));
          CHECK (!rawComparison(fm3, fm4));
          CHECK (!rawComparison(fm3, fm5));
          CHECK (!rawComparison(fm3, fm6));
          CHECK (!rawComparison(fm4, fm5));
          CHECK (!rawComparison(fm4, fm6));
          CHECK (!rawComparison(fm5, fm6));   // again: can't detect they are equivalent
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (FunctorUtil_test, "unit common");
  
  
  
}} // namespace util::test
