/*
  RandomDraw(Test)  -  verify the component builder for random selected values

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file random-draw-test.cpp
 ** unit test \ref RandomDraw_test
 */




#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"
#include "lib/random-draw.hpp"
#include "lib/test/diagnostic-output.hpp"////////////////////TODO
//#include "lib/util.hpp"

//#include <cstdlib>



namespace lib {
namespace test{
  
//  using util::isSameObject;
//  using std::rand;
  
//  namespace error = lumiera::error;
//  using error::LUMIERA_ERROR_FATAL;
//  using error::LUMIERA_ERROR_STATE;
  
  
  namespace {
//    const Literal THE_END = "all dead and hero got the girl";
    
  }

  using Draw = lib::RandomDraw<uint, 10>;
  
  
  
  /***********************************************************************************//**
   * @test Verify a flexible builder for random-value generators; using a config template,
   *       these can be outfitted to use a suitable source of randomness and to produce
   *       values from a desired target type and limited range.
   *       - TODO
   * @see result.hpp
   * @see lib::ThreadJoinable usage example
   */
  class RandomDraw_test
    : public Test
    {
      
      void
      run (Arg)
        {
          simpleUse();
          
          verify_numerics();
          verify_buildProfile();
          verify_dynamicChange();
        }
      
      
      
      /** @test TODO demonstrate a basic usage scenario
       * @todo WIP 11/23 🔁 define ⟶ implement
       */
      void
      simpleUse()
        {
          auto draw = Draw().probability(0.5);
          CHECK (draw(0)   == 0);
          CHECK (draw(127) == 0);
          CHECK (draw(128) == 1);
          CHECK (draw(141) == 2);
          CHECK (draw(255) ==10);
          CHECK (draw(256) == 0);
        }
      
      
      
      /** @test TODO verify random number transformations
       * @todo WIP 11/23 🔁 define ⟶ implement
       */
      void
      verify_numerics()
        {
          UNIMPLEMENTED ("verify random number transformations");
        }
      
      
      
      /** @test TODO verify the Builder-API to define the profile of result values.
       * @todo WIP 11/23 🔁 define ⟶ implement
       */
      void
      verify_buildProfile()
        {
          UNIMPLEMENTED ("verify random number profile configuration");
        }
      
      
      
      /** @test TODO change the generation profile dynamically
       * @todo WIP 11/23 🔁 define ⟶ implement
       */
      void
      verify_dynamicChange()
        {
          UNIMPLEMENTED ("change the generation profile dynamically");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (RandomDraw_test, "unit common");
  
  
}} // namespace lib::test
