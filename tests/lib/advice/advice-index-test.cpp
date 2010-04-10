/*
  AdviceIndex(Test)  -  cover the index datastructure used to implement Advice dispatch
 
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
   * @test the Advice system uses an index datastructure to support matching
   *       the bindings to get pairs of participants to connect by an individual
   *       advice channel.
   *       
   *       This test covers the properties of this implementation datastucture.
   * 
   * @todo partially unimplemented and thus commented out ////////////////////TICKET #608
   * 
   * @see advice.hpp
   * @see AdviceBasics_test
   * @see AdviceBindingPattern_test
   */
  class AdviceIndex_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          checkInit();
          addEntry();
          checkLookup();
          removeEntry();
          checkCollisionHandling();
          checkCleanup();
        }
      
      
      void
      checkInit()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #608
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #608
        }
      
      
      void
      addEntry()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #608
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #608
        }
      
      
      void
      checkLookup()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #608
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #608
        }
      
      
      void
      removeEntry()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #608
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #608
        }
      
      
      void
      checkCollisionHandling()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #608
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #608
        }
      
      
      void
      checkCleanup()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #608
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #608
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (AdviceIndex_test, "function common");
  
  
}}} // namespace lib::advice::test
