/*
  GeneratorCombinations(Test)  -  verify generating case combinations

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file generator-combinations-test.cpp
 ** unit test \ref GeneratorCombinations_test
 */


#include "lib/test/run.hpp"
#include "lib/meta/generator.hpp"
#include "lib/meta/generator-combinations.hpp"
#include "meta/typelist-diagnostics.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"


using ::test::Test;
using std::string;


namespace lib  {
namespace meta {
namespace test {
  
  
  namespace { // test cases and data....
    
    typedef Types< Num<1>
                 , Num<3>
                 , Num<5>
                 >        Types1;
    typedef Types< Num<2>
                 , Num<4>
                 , Num<6>
                 >        Types2;
    
    
    using util::_Fmt;
    
    /**
     * A Test-Template to be instantiated
     * for all possible combinations
     * of the {Types1} x {Types2}
     */
    template<class T1, class T2, class BASE>
    struct TestCase
      : BASE
      {
        static string
        visitAll()
          {
            T1 param1;
            T2 param2;
            return _Fmt("-<%u%u>%s") % uint(param1)
                                     % uint(param2)
                                     % BASE::visitAll();
          }
      };
    
    template<>
    struct TestCase<void,void,NullType>
      {
        static string
        visitAll()
          {
            return "-|";
          }
      };
    typedef TestCase<void,void,NullType> IterationEnd;
    
  } // (End) test data
  
  
  
  
  
  
  /*********************************************************************//**
   * @test check utilities for generating case combinations.
   *       - verify the Cartesian product is built properly
   *       - instantiate a two-parameter test template
   *         for all those cases, as given by the
   *         Cartesian product of two Type collections
   */
  class GeneratorCombinations_test : public Test
    {
      virtual void
      run (Arg) 
        {
          checkCartesian();
          checkCaseInstantiation();
        }
      
      
      void
      checkCartesian ()
        {
          typedef CartesianProduct<Types1,Types2> Cartesian;
          DISPLAY (Cartesian);
        }
      
      
      void
      checkCaseInstantiation ()
        {
          typedef InstantiateChainedCombinations< Types1,Types2
                                                , TestCase
                                                , IterationEnd > CombnationCases;
          
          cout << "All-Test-Combinations-" << CombnationCases::visitAll() << endl;
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (GeneratorCombinations_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
