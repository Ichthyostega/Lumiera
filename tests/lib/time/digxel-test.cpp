/*
  Digxel(Test)  -  cover behaviour of a generic number-element holder

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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/time/display.hpp"
#include "lib/time/digxel.hpp"
#include "lib/util.hpp"

#include <iostream>
#include <cstdlib>

using util::isSameObject;
using std::rand;
using std::cout;
using std::endl;


namespace lib {
namespace time{
namespace test{
  
  namespace { // Test data and setup
    
    const uint REPEAT     = 40;
    const uint RAND_RANGE = 100;
    const uint RAND_DENOM = 3;
    const uint TIMING_CNT = 10000000;
    
    inline double
    randomFrac()
      {
        double arbitrary = (rand() % RAND_RANGE);
        arbitrary /= (1 + rand() % RAND_DENOM);
        return arbitrary;
      }
    
    inline uint
    isOdd (uint i)
      {
        return i % 2;
      }
    
    
    double sum(0),
      checksum(0);
    
    double
    sideeffectSum (double val)
    {
      sum += val;
      return val;
    }
    
    
    /* === special Digxel configuration for this test === */
    
    double
    limitingMutator (double value2set)
    {
      return (+1 < value2set) ? 1.0
           : (-1 > value2set) ? -1.0
                              : value2set;
    }
    
    
    struct VerySpecialFormat
      : digxel::PrintfFormatter<double, 11>
      {
        VerySpecialFormat() : digxel::PrintfFormatter<double,11>("##%+5.1f ##") { }
      };
    
    typedef Digxel<double, VerySpecialFormat> TestDigxel;
    
  }//(End)Test setup
  
  
  
  
  
  
  
  /***********************************************************************
   * @test verify correct behaviour of an display "Digxel":
   *       A self-contained numeric element to support building displays.
   *       - build a Digxel
   *       - set a value
   *       - retrieve formatted display
   *       - performing side-effects from the setter-functor
   *       - formatted value caching
   */
  class Digxel_test : public Test
    {
      virtual void
      run (Arg) 
        {
          checkSimpleUsage ();
          checkMutation ();
          verifyMutatorInfluence ();
          verifyComparisons ();
          checkCopy ();
          checkDisplayOverrun ();
          verifyDisplayCaching ();
        } 
      
      
      void
      checkSimpleUsage ()
        {
          TestDigxel digi;
          CHECK (0 == digi);
          CHECK ("## +0.0 ##" == string(digi));
          cout << "empty____" << digi << endl;
          
          digi = -88.77;
          CHECK (-88.77 == digi);
          CHECK ("##-88.8 ##" == string(digi));
          cout << "value____" << digi << endl;
        }
      
      
      void
      checkMutation ()
        {
          TestDigxel digi;
          
          // configure what the Digxel does on "mutation"
          digi.mutator = sideeffectSum;
          
          CHECK (0 == digi);
          sum = checksum = 0;
          for (uint i=0; i < REPEAT; ++i)
            {
              double arbitrary = randomFrac();
              checksum += arbitrary;      // for verification
                                         //
              digi = arbitrary;         //...causes invocation of mutation functor
              
              CHECK (sum == checksum, "divergence after adding %f in iteration %d", arbitrary, i);
              CHECK (digi == arbitrary);
            }
          CHECK (0 < sum);
        }
      
      
      void
      verifyMutatorInfluence ()
        {
          TestDigxel digi;
          
          // using the default mutator
          CHECK (0 == digi);
          digi = 12.3;
          CHECK (12.3 == digi);
          
          // a special mutator to limit the value
          digi.mutator = limitingMutator;
          CHECK (12.3 == digi);
          digi = 12.3;
          CHECK (1 == digi);
          
          digi = 0.5;
          CHECK (0.5 == digi);
          digi = -0.678;
          CHECK (-0.678 == digi);
          digi = -9.1011;
          CHECK (-1 == digi);
          
          digi.setValueRaw(12.3);  // bypassing mutator
          CHECK (12.3 == digi);
        }
      
      
      void
      verifyComparisons ()
        {
          TestDigxel d1;
          TestDigxel d2;
          
          CHECK (d1 == d2);
          
          double someValue = randomFrac();
          d1 = someValue;
          
          CHECK (d1 == someValue);
          CHECK (d1 != d2);
          CHECK (d2 != d1);
          
          d2 = d1 + 22;
          CHECK (d1 <  d2);
          CHECK (d1 <= d2);
          
          CHECK (!(d1 > d2));
          CHECK (!(d1 >= d2));
          CHECK (!(d1 == d2));
        }
      
      
      void
      checkCopy ()
        {
          TestDigxel d1;
          
          double someValue = randomFrac();
          
          d1 = someValue;
          CHECK (d1 == someValue);
          
          TestDigxel d2(d1);
          CHECK (d2 == someValue);
          CHECK (!isSameObject (d1, d2));
          
          d1 = randomFrac();
          CHECK (d1 != d2);
          CHECK (d2 == someValue);
        }
      
      
      /** @test Digxel should be protected
       *        against display buffer overrun */
      void
      checkDisplayOverrun ()
        {
          TestDigxel digi;
          digi = 123456789.12345678;
          
          string formatted;
#if false ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
          VERIFY_ERROR (ASSERTION, formatted = digi.show() ); // should trigger assertion
          formatted = digi.show();                            // second time doesn't reformat                
#endif    ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
          
          CHECK (formatted.length() <= digi.maxlen());
        }
      
      
      /** @test verify caching of formatted values.
       *  Digxel avoids reformatting unchanged values;
       *  to verify the effectivity of this measure, we
       *  take some timings.
       *  @warning the results of such tests could be unreliable,
       *  but in this case here I saw a significant difference,
       *  with values of 0.5sec / 0.8sec  */
      void
      verifyDisplayCaching ()
        {
          TestDigxel digi;
          digi = 1;
          
          clock_t start(0), stop(0);
          start = clock();
          for (uint i=0; i < TIMING_CNT; ++i)
            {
              digi = 1;
              isOdd (i);
            }
          stop = clock();
          uint without_reformatting = stop - start;
          
          
          start = clock();
          for (uint i=0; i < TIMING_CNT; ++i)
            {
              digi = isOdd (i);
            }
          stop = clock();
          uint with_reformatting = stop - start;
          
          cout << "without reformatting = "<< double(without_reformatting)/CLOCKS_PER_SEC <<"sec"<< endl;
          cout << "with reformatting    = "<< double(with_reformatting   )/CLOCKS_PER_SEC <<"sec"<< endl;
          
          CHECK (without_reformatting < with_reformatting);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (Digxel_test, "unit common");
  
  
  
}}} // namespace lib::time::test
