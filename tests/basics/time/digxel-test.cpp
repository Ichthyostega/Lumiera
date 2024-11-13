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

/** @file digxel-test.cpp
 ** unit test \ref Digxel_test
 */


#include "lib/test/run.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-string.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/time/digxel.hpp"
#include "lib/random.hpp"
#include "lib/util.hpp"

using lumiera::error::LUMIERA_ERROR_ASSERTION;
using util::isSameObject;
using util::isnil;
using lib::rani;


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
        double arbitrary = 1 + rani(RAND_RANGE);
        arbitrary /= 1 + rani(RAND_DENOM);
        
        static double prevVal;
        if (arbitrary != prevVal)
          {
            prevVal = arbitrary;
            return arbitrary;
          }
        else
          return randomFrac();
      }
    
    inline uint
    isOdd (uint i)
      {
        return i % 2;
      }
    
    
    
    /* === special Digxel configuration for this test === */
    
    struct VerySpecialFormat
      : digxel::PrintfFormatter<double, 11>
      {
        VerySpecialFormat() : digxel::PrintfFormatter<double,11>("##%+5.1f ##") { }
      };
    
    typedef Digxel<double, VerySpecialFormat> TestDigxel;
    
    
    double sum(0),
      checksum(0);
    
    void
    sideeffectSum (TestDigxel* digxel, double val)
    {
      sum += val;
      digxel->setValueRaw (val);
    }
    
    double preval(0), newval(0);
    
    void
    protocollingMutator (TestDigxel* digxel, double val)
    {
      preval = newval;
      newval = val;
      digxel->setValueRaw (val);
    }
    
    void
    limitingMutator (TestDigxel* digxel, double value2set)
    {
      digxel->setValueRaw ((+1 < value2set) ? +1.0
                        :  (-1 > value2set) ? -1.0
                               : value2set);
    }
    
    void
    trivialMutator (TestDigxel* digxel, double value2set)
    {
      digxel->setValueRaw (value2set);
    }
    
    void
    emptyMutator (TestDigxel*, double)
    {
      /* do nothing */
    }
    
  }//(End)Test setup
  
  
  
  
  
  
  
  /*******************************************************************//**
   * @test verify correct behaviour of an display "Digxel":
   *       A self-contained numeric element to support building displays.
   *       - build a Digxel
   *       - set a value
   *       - retrieve formatted display
   *       - verify comparisons and increments
   *       - performing side-effects from the setter-functor
   *       - formatted value caching
   */
  class Digxel_test : public Test
    {
      virtual void
      run (Arg arg) 
        {
          seedRand();
          
          checkSimpleUsage();
          checkMutation ();
          verifyMutatorInfluence();
          verifyAssignMutatingOperators();
          verifyComparisons();
          checkCopy ();
          checkDisplayOverrun();
          
          if (!isnil (arg))
            timingMeasurements();
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
          digi.installMutator (sideeffectSum, digi);
          
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
          digi.installMutator (limitingMutator, digi);
          CHECK (12.3 == digi);
          digi = 12.3;
          CHECK (12.3 == digi);    // triggered on real change only
          digi = 12.2;
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
      
      
      /** @test verify the self-assigning increment/decrement operators. 
       *  @note especially these need to invoke the mutator function,
       *        much like a direct assignment. We use a special mutator
       *        to protocol the previous / new value.
       */
      void
      verifyAssignMutatingOperators ()
        {
          TestDigxel digi;
          digi.installMutator (protocollingMutator, digi);
          
          digi = 12.3;
          CHECK ( 0.0 == preval && 12.3 == newval);
          digi += 10;
          CHECK (12.3 == preval && 22.3 == newval);
          digi -= 5;
          CHECK (22.3 == preval && 17.3 == newval);
          ++digi;
          CHECK (17.3 == preval && 18.3 == newval);
          digi++;
          CHECK (18.3 == preval && 19.3 == newval);
          --digi;
          CHECK (19.3 == preval && 18.3 == newval);
          digi--;
          CHECK (18.3 == preval && 17.3 == newval);
          
          double val = ++digi;
          CHECK (18.3 == digi && 18.3 == val);
          val = digi++;
          CHECK (19.3 == digi && 18.3 == val);
          val = --digi;
          CHECK (18.3 == digi && 18.3 == val);
          val = digi--;
          CHECK (17.3 == digi && 18.3 == val);
        }
      
      
      void
      verifyComparisons ()
        {
          TestDigxel d1;
          TestDigxel d2;
          
          CHECK (d1 == d2);
          
          double someValue = d1 + randomFrac();
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
      
      
      /** @test perform several timing measurements and 
       *  especially verify the effect of caching formatted values.
       *  Digxel avoids reformatting unchanged values; besides that
       *  it is possible to install a "mutator" functor for invoking
       *  all kinds of special behaviour on value changes. Of course
       *  doing so comes with a (considerable) price tag....
       * @note the measurement especially show the effects of
       *  locality, which can vary largely over several runs.
       */
      void
      timingMeasurements ()
        {
          TestDigxel digi;
          digi = 1;
          
          clock_t start(0), stop(0);
          util::_Fmt resultDisplay("timings(%s)%|36T.|%4.0fns\n");
          
#define   START_TIMINGS start=clock();
#define   DISPLAY_TIMINGS(ID)\
          stop = clock();     \
          uint ID = stop-start;\
          cout << resultDisplay % STRINGIFY (ID) % (double(ID)/CLOCKS_PER_SEC/TIMING_CNT*1e9) ;
          
          
          START_TIMINGS
          for (uint i=0; i < TIMING_CNT; ++i)
            {
              isOdd (i);
            }
          DISPLAY_TIMINGS (empty_loop)
          
          
          START_TIMINGS
          for (uint i=0; i < TIMING_CNT; ++i)
            {
              digi = 1;
              isOdd (i);
            }
          DISPLAY_TIMINGS (without_reformatting)
          
          
          START_TIMINGS
          for (uint i=0; i < TIMING_CNT; ++i)
            {
              digi = isOdd (i);
            }
          DISPLAY_TIMINGS (with_reformatting)
          
          
          digi.installMutator (emptyMutator, digi);
          
          START_TIMINGS
          for (uint i=0; i < TIMING_CNT; ++i)
            {
              digi = isOdd (i);
            }
          DISPLAY_TIMINGS (with_empty_mutator)
          
          
          digi.installMutator (trivialMutator, digi);
          
          START_TIMINGS
          for (uint i=0; i < TIMING_CNT; ++i)
            {
              digi = isOdd (i);
            }
          DISPLAY_TIMINGS (with_trivial_mutator)
          
          
          digi.installMutator (&TestDigxel::setValueRaw, digi);
          
          START_TIMINGS
          for (uint i=0; i < TIMING_CNT; ++i)
            {
              digi = isOdd (i);
            }
          DISPLAY_TIMINGS (with_memfun_mutator)
          
          
          CHECK (without_reformatting < with_reformatting);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (Digxel_test, "unit common");
  
  
  
}}} // namespace lib::time::test
