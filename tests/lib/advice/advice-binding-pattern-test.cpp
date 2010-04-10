/*
  AdviceBindingPattern(Test)  -  cover pattern matching used to dispatch Advice
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"

#include "lib/advice.hpp"
//#include "lib/p.hpp"
//#include "proc/assetmanager.hpp"
//#include "proc/asset/inventory.hpp"
//#include "proc/mobject/session/clip.hpp"
//#include "proc/mobject/session/track.hpp"
//#include "lib/meta/trait-special.hpp"
//#include "lib/util-foreach.hpp"
//#include "lib/symbol.hpp"

//#include <iostream>
//#include <string>

//using lib::test::showSizeof;
//using lib::test::randStr;
//using util::isSameObject;
//using util::and_all;
//using util::for_each;
//using util::isnil;
//using lib::Literal;
//using lib::Symbol;
//using lumiera::P;
//using std::string;
//using std::cout;
//using std::endl;



namespace lib {
namespace advice {
namespace test {
  
  namespace {
  }
  
  
  
  /*******************************************************************************
   * @test the pattern matching machinery used to find an Advice solution.
   *       Each advice::Provision and advice::Request specifies a binding, used
   *       to discern various pieces of advice. Whenever patterns on the two sides
   *       match, an Advice channel is created, causing the advice provision to
   *       get visible to the advised entity.
   *       
   *       This test creates various patterns and verifies matching behaves
   *       as specified and documented.
   * 
   * @todo partially unimplemented and thus commented out ////////////////////TICKET #605
   * 
   * @see advice.hpp
   * @see AdviceBasics_test
   * @see AdviceMultiplicity_test
   * @see AdviceIndex_test implementation test
   */
  class AdviceBindingPattern_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verifyPatternNormalisation();
          verifyStaticMatch();
          verifyDynamicMatch();
        }
      
      
      void
      verifyPatternNormalisation()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
        }
      
      
      void
      verifyStaticMatch()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
        }
      
      
      /** @test match against patterns containing variables,
       *        verify the created solution arguments
       *  @todo this is a future extension
       */
      void
      verifyDynamicMatch()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (AdviceBindingPattern_test, "unit common");
  
  
}}} // namespace lib::advice::test
