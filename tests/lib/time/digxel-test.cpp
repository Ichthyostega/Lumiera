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
#include "lib/time/digxel.hpp"
#include "lib/util.hpp"

//#include <boost/lexical_cast.hpp>
//#include <boost/algorithm/string/join.hpp>
#include <boost/lambda/lambda.hpp>
#include <iostream>  //TODO
#include <cstdlib>

//using boost::lexical_cast;
//using util::isnil;
//using util::contains;
using util::isSameObject;
using std::rand;
using std::cout;/////////TODO
using std::endl;

//using boost::algorithm::join;


namespace lib {
namespace time{
namespace test{
  
  namespace { // Test data
    
    // Placeholder to mark the function argument in a "lambda-expression"
    boost::lambda::placeholder1_type _1_;
    
    const uint REPEAT = 40;
    const uint RAND_RANGE = 100;
    const uint RAND_DENOM = 3;
    
    inline double
    randomFrac()
      {
        double arbitrary = (rand() % RAND_RANGE);
        arbitrary /= (1 + rand() % RAND_DENOM);
        return arbitrary;
      }


  }
  
  
  /***********************************************************************
   * @test verify correct behaviour of an display "Digxel":
   *       A self-contained numeric element to support building displays.
   *       - build a Digxel
   *       - set a value
   *       - retrieve formatted display
   *       - assign to invoke the setter-functor
   */
  class Digxel_test : public Test
    {
      virtual void
      run (Arg arg) 
        {
          checkSimpleUsage (ref);
        } 
      
      
      void
      checkSimpleUsage ()
        {
          TestDigxel digi;
          CHECK (0 == digi);
          CHECK ("## 0 ##" == digi.show());
          
          digi = 88;
          CHECK (88 == digi);
          CHECK ("## 88.0 ##" == digi.show());
        }
      
      
      void
      checkMutation ()
        {
          TestDigxel digi;
          
          uint sum(0), checksum(0);
          
          // configure what the Digxel does on "mutation"
          digi.mutator = ( var(sum) += _1_ );
          
          CHECK (0 == digi);
          for (uint i=0; i < REPEAT; ++i)
            {
              double arbitrary = randomFrac();
              checksum += arbitrary;
              
              digi.mutate (arbitrary);  // invoke the mutation functor 
              
              CHECK (sum == checksum, "divergence after adding %f in iteration %d", arbitrary, i);
              CHECK (0 == digi);     // the value itself is not touched
            }
        }
      
      
      void
      checkDefaultMutator ()
        {
          TestDigxel digi;
          
          CHECK (0 == digi);
          digi.mutate(12.3);
          CHECK (12.3 == digi);
        }
      
      
      void
      checkCopy ()
        {
          TestDigxel d1;
          
          double someValue = randomFrac();
          
          d1 = someValue;
          CHECK (d1 == someValue);
          
          TextDigxel d2(d1);
          CHECK (d2 == someValue);
          CHECK (!isSameObject (d1, d2));
          
          d1 = randomFrac();
          CHECK (d1 != d2);
          CHECK (d2 == someValue);
        }
      
      
      void
      checkDisplayOverrun ()
        {
          TestDigxel digi;
          digi = 123456789.12345678;
          
          string formatted (digi.show());  
          CHECK (formatted.length() <= digi.maxlen());
        }
      
      
      void
      verifyDisplayCaching ()
        {
          TestDigxel digi;
          digi = 1;
          
          clock_t start, stop;
          start = clock();
          for (uint i=0; i < TIMING_CNT; ++i)
            {
              val odd = i % 2;
              digi = 1;
            }
          stop = clock();
          uint without_reformatting = stop - start;
          
          
          start = clock();
          for (uint i=0; i < TIMING_CNT; ++i)
            {
              val odd = i % 2;
              digi = odd;
            }
          stop = clock();
          uint with_reformatting = stop - start;
          
          cout << "without = "<< without_reformatting << endl;
          cout << "with    = "<< with_reformatting << endl;
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (Digxel_test, "unit common");
  
  
  
}}} // namespace lib::time::test
