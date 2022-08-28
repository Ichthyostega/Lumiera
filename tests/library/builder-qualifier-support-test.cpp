/*
  BuilderQualifierSupport(Test)  -  demonstrate accepting arbitrary qualifier terms on a builder function

  Copyright (C)         Lumiera.org
    2022,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file builder-qualifier-support-test.cpp
 ** unit test \ref lib::test::BuilderQualifierSupport_test
 */




#include "lib/test/run.hpp"
#include "lib/builder-qualifier-support.hpp"

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
          CHECK ("Strategy{∅}" == string(f0));
          
          ExampleStrategy f1(one());
          CHECK ("Strategy{!one!}" == string(f1));
          
          ExampleStrategy f2(two("Ψ"));
          CHECK ("Strategy{∅.two(Ψ)}" == string(f2));
          
          ExampleStrategy f3(one(), two("↯"));
          CHECK ("Strategy{!one!.two(↯)}" == string(f3));
          
          ExampleStrategy f4(two("☭"), one());
          CHECK ("Strategy{!one!}" == string(f4));   // Note: evaluated from left to right, one() overwrites prop_
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BuilderQualifierSupport_test, "unit common");
  
  
}} // namespace lib::test
