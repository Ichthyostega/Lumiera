/*
  AdviceSituations(Test)  -  catalogue of standard Advice usage scenarios

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file advice-situations-test.cpp
 ** unit test \ref AdviceSituations_test
 */


#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"

#include "common/advice.hpp"



namespace lumiera {
namespace advice {
namespace test {
  
  namespace {
  }
  
  
  
  /**************************************************************************************//**
   * @test documentation of the fundamental usage scenarios envisioned in the Advice concept.
   *       This test will be augmented and completed as the Lumiera application matures.
   * 
   * @todo yet more use cases to come....  ////////////////////////////////////////////////////////TICKET #335
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
          pattern01_justPickAndBeHappy();
          pattern02_pickIfPresent();
          pattern03_installOnlyOnce();
          TODO ("more advice usage scenarios.....?");
        }
      
      
      /** @test usage pattern 01: simply consume Advice -- irrespective if set explicitly. */
      void
      pattern01_justPickAndBeHappy()
        {
          Request<int> generic{"solution(life_and_universe_and_everything)"};
          CHECK (0 == generic.getAdvice());  // the early bird gets the worm...
          
          Provision<int> universal{"solution(life_and_universe_and_everything)"};
          universal.setAdvice(5);
          CHECK (5 == generic.getAdvice());  // ...while the bad girls go everywhere
          
          universal.retractAdvice();
          CHECK (0 == generic.getAdvice());  // nothing to see here, just move on
        }
      
      
      /** @test usage pattern 01: detect if specific advice was given. */
      void
      pattern02_pickIfPresent()
        {
          Request<int> request{"something(special)"};
          CHECK (not request.isMatched());
          
          Provision<int> info{"something(special)"};
          info.setAdvice(55);
          CHECK (request.isMatched());
          CHECK (55 == request.getAdvice());
          
          info.retractAdvice();
          CHECK (not request.isMatched());
        }
      
      
      /** @test usage pattern 01: . */
      void
      pattern03_installOnlyOnce()
        {
          Provision<int> info{"something(special)"};
          CHECK (not info.isGiven());

          Request<int> question{"something(special)"};
          CHECK (0 == question.getAdvice());
          CHECK (not question.isMatched());
          
          auto publish = [&](int i)
                            {
                              if (not info.isGiven())
                                info.setAdvice (i);
                            };
          
          for (uint i=0; i<5; ++i)
            if (i % 2)
              publish (i);
          
          CHECK (1 == question.getAdvice());
          CHECK (question.isMatched());
          
          info.retractAdvice();
          CHECK (not info.isGiven());
          CHECK (not question.isMatched());
        }
      
      
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #335
      /** @test usage pattern ∞ : somewhat literally fantastic */
      void
      check_SevenMoreWondersOfTheWorld()
        {
          UNIMPLEMENTED ("suitable advice to save the world");
        }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #335
      
      // more to come.....
       
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (AdviceSituations_test, "function common");
  
  
}}} // namespace lumiera::advice::test
