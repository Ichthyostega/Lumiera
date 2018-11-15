/*
  AdviceConfiguration(Test)  -  cover the various policies to configure Advice

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file advice-configuration-test.cpp
 ** unit test \ref AdviceConfiguration_test
 */


#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"

#include "common/advice.hpp"
//#include "lib/p.hpp"
//#include "steam/assetmanager.hpp"
//#include "steam/asset/inventory.hpp"
//#include "steam/mobject/session/clip.hpp"
//#include "steam/mobject/session/fork.hpp"
//#include "lib/meta/trait-special.hpp"
//#include "lib/util-foreach.hpp"
//#include "lib/symbol.hpp"

//#include <string>

//using lib::test::showSizeof;
//using lib::test::randStr;
//using util::isSameObject;
//using util::and_all;
//using util::for_each;
//using util::isnil;
//using lib::Literal;
//using lib::Symbol;
//using lib::P;
//using std::string;



namespace lumiera {
namespace advice {
namespace test {
  
  namespace {
  }
  
  
  
  /***************************************************************************//**
   * @test this is a collection of tests to cover the configuration options
   *       available as policy arguments when establishing the collaboration.
   *       - default advice values, or blocking or throwing
   *       - using activation signals on both sides
   *       - dispatch without locking (TODO any chance we can cover this by test??)
   *       - more to come....
   * 
   * @todo partially unimplemented and thus commented out ////////////////////TICKET #605
   * 
   * @see advice.hpp
   * @see AdviceBasics_test
   * @see AdviceSituations_test
   */
  class AdviceConfiguration_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          checkDefaultAdvice();
          blockOnAdvice();
          checkSignals();
        }
      
      
      /** @test typically, advice is a default constructible value,
       *        so there is a natural fallback in case no active advice
       *        provision exists. Alternatively we may specify to throw.
       */
      void
      checkDefaultAdvice()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
        }
      
      
      /** @test when opening the advice collaboration, both sides may independently
       *        install a signal (callback functor) to be invoked when the actual
       *        advice solution gets established. 
       */
      void
      checkSignals()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
        }
      
      
      /** @test instead of allowing default advice values, both sides
       *        may enter a blocking wait until an advice solution is available.
       *        This is implemented as special case of using signals
       */
      void
      blockOnAdvice()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (AdviceConfiguration_test, "unit common");
  
  
}}} // namespace lumiera::advice::test
