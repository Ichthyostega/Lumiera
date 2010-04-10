/*
  AdviceBasics(Test)  -  basic behaviour of the Advice collaboration
 
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
   * @test proof-of-concept for the Advice collaboration.
   *       Advice allows data exchange without coupling the participants tightly.
   *       This test demonstrates the basic expected behaviour in a simple but
   *       typical situation: two unrelated entities exchange a piece of data
   *       just by referring to a symbolic topic ID.
   * 
   * @todo partially unimplemented and thus commented out ////////////////////TICKET #605
   * 
   * @see advice.hpp
   * @see AdviceSituations_test
   * @see AdviceMultiplicity_test
   * @see AdviceConfiguration_test
   * @see AdviceBindingPattern_test
   * @see AdviceIndex_test implementation test
   */
  class AdviceBasics_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleExchange();
          createCollaboration();
          overwriting_and_retracting();
        }
      
      
      /** @test the very basic usage situation: the advisor sets an information value
       *        and the advised entity picks it up. */
      void
      simpleExchange()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
        }
      
      
      /** @test multiple ways how to initiate the advice collaboration */
      void
      createCollaboration()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
        }
      
      
      /** @test changing the provided advice, finally retracting it,
       *        causing fallback on the default value */
      void
      overwriting_and_retracting()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #605
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (AdviceBasics_test, "unit common");
  
  
}}} // namespace lib::advice::test
