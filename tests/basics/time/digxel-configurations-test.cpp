/*
  DigxelConfigurations(Test)  -  verify predefined standard Digxel configurations

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

/** @file digxel-configurations-test.cpp
 ** unit test \ref DigxelConfigurations_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/time/digxel.hpp"
#include "lib/format-cout.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

#include <cstdlib>

using lumiera::error::LERR_(ASSERTION);
using util::isSameObject;
using util::typeStr;
using std::rand;


namespace lib {
namespace time{
namespace test{
  
  
  
  
  
  
  /*******************************************************************//**
   * @test verify correctness of the predefined standard Digxels.
   *       Some widely used standard configurations, including
   *       - default Digxel for int and double values
   *       - sexagesimal Digxel
   *       - hex byte Digxel
   *       - ...more to come
   * @todo cover any newly added Digxel configurations.
   * @see Digxel_test
   */
  class DigxelConfigurations_test : public Test
    {
      virtual void
      run (Arg) 
        {
          verifyConfiguration<Digxel<int>   > (123);
          verifyConfiguration<Digxel<double>> (123.4567);
          verifyConfiguration<SexaDigit     > (42);
          verifyConfiguration<SexaDigit     > (-5);
          verifyConfiguration<HexaDigit     > (0xc);
          verifyConfiguration<HexaDigit     > (0x6f);
          verifyConfiguration<CountVal      > (-1234567890);
          
          verifySignum();
        }
      
      
      void
      verifySignum()
        {
          Signum sig;
          CHECK (1 == sig);
          
          sig = 123;
          CHECK (1 == sig);
          sig = -sig;
          CHECK (-1 == sig);
          sig += 98;
          CHECK (+1 == sig);
          CHECK (sig.show() == string(" "));
          sig *= -1;
          CHECK (sig.show() == string("-"));
        }
      
      
      template<class DIX, typename VAL>
      void
      verifyConfiguration (VAL testval)
        {
          DIX digxel;
          CHECK (0 == digxel);
          cout << typeStr(digxel) << "--empty--"<<digxel;
          
          digxel = testval;
          cout << "--(val="<<testval<<")--"<<digxel;
          
          // verify buffer overrun protection
          digxel = 123456789.12345;
          string formatted;
#if false ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
          VERIFY_ERROR (ASSERTION, formatted = digxel.show() ); // should trigger assertion
          formatted = digxel.show();                           //  second time doesn't reformat
          CHECK (formatted.length() == digxel.maxlen());
          cout << "--(clipped)--"<< digxel;
#endif    ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
          
          cout <<"|"<< endl;
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DigxelConfigurations_test, "unit common");
  
  
  
}}} // namespace lib::time::test
