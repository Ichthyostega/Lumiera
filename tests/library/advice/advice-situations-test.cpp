/*
  AdviceSituations(Test)  -  catalogue of standard Advice usage scenarios

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

/** @file advice-situations-test.cpp
 ** unit test \ref AdviceSituations_test
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
//using liab::P;
//using std::string;



namespace lumiera {
namespace advice {
namespace test {
  
  namespace {
  }
  
  
  
  /**************************************************************************************//**
   * @test documentation of the fundamental usage scenarios envisioned in the Advice concept.
   *       This test will be augmented and completed as the Lumiera application matures.
   * 
   * @todo partially unimplemented and thus commented out ////////////////////TICKET #335
   * 
   * @see advice.hpp
   * @see AdviceBasics_test
   * @see AdviceConfiguration_test
   */
  class AdviceSituations_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          check_ProxyRenderingAdvice();
          check_DependencyInjection();
          TODO ("more advice usage scenarios.....?");
        }
      
      
      /** @test usage scenario: switch a processing node into proxy mode. */
      void
      check_ProxyRenderingAdvice()
        {
          UNIMPLEMENTED ("anything regarding proxy rendering");
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #335
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #335
        }
      
      
      /** @test usage scenario: dependency injection for tests */
      void
      check_DependencyInjection()
        {
          UNIMPLEMENTED ("suitable advice to request and transfer test dependencies");
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #335
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #335
        }
      
      // more to come.....
       
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (AdviceSituations_test, "function common");
  
  
}}} // namespace lumiera::advice::test
