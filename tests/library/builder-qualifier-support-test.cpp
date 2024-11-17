/*
  BuilderQualifierSupport(Test)  -  demonstrate accepting arbitrary qualifier terms on a builder function

   Copyright (C)
     2022,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file builder-qualifier-support-test.cpp
 ** unit test \ref lib::test::BuilderQualifierSupport_test
 */




#include "lib/test/run.hpp"
#include "lib/builder-qualifier-support.hpp"
#include "lib/test/test-helper.hpp"

#include <string>


namespace lib {
namespace test{
  
  using std::string;
  
  
  namespace { // example strategy to use the builder-qualifier-support...
    
    
    /** Example "strategy" class, which can be configured
     *  with additional qualifiers at construction */
    class ExampleStrategy
      : BuilderQualifierSupport<ExampleStrategy>
      {
        
        friend Qualifier one();
        friend Qualifier two(string);
        
      public:
        ExampleStrategy() = default;
        
        template<class... QS>
        ExampleStrategy(Qualifier qual, QS... qs)
          : ExampleStrategy{}
          {
            qualify(*this, qual, qs...);
          }
        
        operator string ()  const
          {
            return "Strategy{"+prop_+"}";
          }
        
      private:
        /** a private property
         * to be manipulated by the qualifiers */
        string prop_{"∅"};
      };
    
    
    /** definition of a qualifier `one()` */
    ExampleStrategy::Qualifier
    one()
    {
      return ExampleStrategy::Qualifier{[](ExampleStrategy& strategy)
                                            {
                                              strategy.prop_ = "!one!";
                                            }};
    }
    
    /** definition of another qualifier `two(arg)`, accepting an additional argument */
    ExampleStrategy::Qualifier
    two(string additionalArg)
    {
      return ExampleStrategy::Qualifier{[=](ExampleStrategy& strategy)
                                            {
                                              strategy.prop_ += ".two("+additionalArg+")";
                                            }};
    }
    
  }//(End)Test example
  
  
  
  /***********************************************************************************//**
   * @test Demonstrate a technique to supply additional descriptive ctor arguments in a type safe way.
   *       - target and receiver may be some configurable Strategy etc.
   *       - the idea is to provide friend functors, which might tweak or reset internal settings;
   *       - these functors are packaged into free standing friend functions with intuitive naming...
   *       - which, on call-site, look like algebraic expressions/data-types.
   * @remark because the actual helper function is a free function, it may be integrated
   *       in various ways, but typically the support template will be mixed-in, as demonstrated
   *       here; this mechanism can be integrated into a constructor call, thus optionally allowing
   *       for arbitrary extra qualifiers, even with extra arguments.
   * 
   * @see builder-qualifier-support.hpp
   */
  class BuilderQualifierSupport_test : public Test
    {
      
      void
      run (Arg)
        {
          ExampleStrategy f0;
          CHECK (f0 == "Strategy{∅}"_expect);
          
          ExampleStrategy f1(one());
          CHECK (f1 == "Strategy{!one!}"_expect);
          
          ExampleStrategy f2(two("Ψ"));
          CHECK (f2 == "Strategy{∅.two(Ψ)}"_expect);
          
          ExampleStrategy f3(one(), two("↯"));
          CHECK (f3 == "Strategy{!one!.two(↯)}"_expect);
          
          ExampleStrategy f4(two("☭"), one());
          CHECK (f4 == "Strategy{!one!}"_expect);   // Note: evaluated from left to right, one() overwrites prop_
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BuilderQualifierSupport_test, "unit common");
  
  
}} // namespace lib::test
