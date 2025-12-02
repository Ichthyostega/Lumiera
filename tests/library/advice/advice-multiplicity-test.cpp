/*
  AdviceMultiplicity(Test)  -  verify correctness when dispatching multiple Advices concurrently

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file advice-multiplicity-test.cpp
 ** unit test \ref AdviceMultiplicity_test
 */


#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"

//#include "lib/p.hpp"
//#include "steam/assetmanager.hpp"
//#include "steam/asset/inventory.hpp"
//#include "steam/mobject/session/clip.hpp"
//#include "steam/mobject/session/fork.hpp"
//#include "lib/meta/trait-special.hpp"
//#include "lib/util-foreach.hpp"
//#include "lib/symbol.hpp"
#include "common/advice.hpp"

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
   * @test stress test for the Advice system:
   *       Concurrently add a multitude of advice provisions and check correct
   *       matching and dispatch of all advice.
   * 
   * @todo advanced topic, deferred for now (4/10) ////////////////////TICKET #610
   * 
   * @see advice.hpp
   * @see AdviceBasics_test
   */
  class AdviceMultiplicity_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          check_highLoadSimpleMatch();
          check_multipleMatch();
          check_unlockedDispatch();
        }
      
      
      /** @test run simple pairs of collaborators in multiple threads.
       *        Intentionally, there should be just a single match per pair,
       *        but the timings of provision and pickup are chosen randomly
       */
      void
      check_highLoadSimpleMatch()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #610
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #610
        }
      
      
      /** @test here, one advice might reach multiple advised entities and
       *        one entity may receive multiple pieces of advice, overwriting
       *        previous advice provisions. The receiving clients (advised entities)
       *        are polling irregularly, but finally should each pick up the correct
       *        value. To check this, advice values are generated with a specific
       *        pattern, which can be check summed */
      void
      check_multipleMatch()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #610
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #610
        }
      
      
      /** @test when dispatching Advice, some of the locking can be left out.
       *        Obviously this means we can't be sure anymore the client will
       *        see the newly dispatched piece of advice. But the main focus
       *        of this test is to ensure the advice system itself is working
       *        properly even without complete locking.
       *        To verify the results, each thread performs a kind of "flush"
       *        (actually creates a memory barrier) before picking up the
       *        final value */
      void
      check_unlockedDispatch()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #610
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #610
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (AdviceMultiplicity_test, "function common");
  
  
}}} // namespace lumiera::advice::test
