/*
  MultiFactArgument(Test)  -  passing additional invocation arguments to registered factory functions

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file multifact-argument-test.cpp
 ** unit test \ref MultiFactArgument_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-cout.hpp"
#include "lib/multifact.hpp"

#include <functional>



namespace lib {
namespace test{
  
  using std::bind;
  using std::function;
  using std::placeholders::_1;
  using lib::test::showSizeof;
  
  
  
  namespace { // dummy fabrication function, creating wrapped numbers, controlled by an additional argument
    
    enum prodID
      { ONE = 1
      , TWO
      };
    
    struct Num { int n_; };
    
    /** dummy "factory" function to be invoked
     *  @return pointer to heap allocated product object
     *  @note this function needs to deliver the product in a form
     *        which can be accepted by the concrete wrapper, which
     *        is going to be configured into the factory.
     */
    Num*
    fabricateNumberz (int base, int offset)
    {
      cout << "fabricate("<<base<<", "<<offset<<")" << endl;
      Num* product = new Num;
      product->n_ = base*offset;
      return product;
    }
    
    
    /** the factory instantiation used for this test */
    typedef factory::MultiFact< Num(int)                    // nominal signature of fabrication
                              , prodID                     //  select factory function by prodID
                              , factory::BuildRefcountPtr //   wrapper: manage product by smart-ptr
                              > TestFactory;
    
    // for reference: type of an equivalent dispatcher table...
    typedef std::map<prodID, function<Num(int)> > DispatcherMap;
  }
  
  
  
  
  
  /***************************************************************//**
   * @test define a MultiFact (factory with dynamic registration),
   *       which accepts additional arguments and passes them
   *       through to the registered factory function(s).
   * @note we set up fabrication functions by binding the functions 
   *       in such a way as to match the function signature declared
   *       in the factory; thereby one argument remains unclosed,
   *       which is the argument to be supplied on each
   *       factory invocation by the client code.
   * 
   * @see  lib::MultiFact
   * @see  query-resolver.cpp
   */
  class MultiFactArgument_test : public Test
    {
      void
      run (Arg)
        {
          TestFactory theFact;
          theFact.defineProduction (ONE, bind (&fabricateNumberz, 1, _1 ));
          theFact.defineProduction (TWO, bind (&fabricateNumberz, 2, _1 ));
          
          cout << showSizeof (theFact) << endl;
          CHECK (sizeof(theFact) == sizeof(DispatcherMap));
          
          typedef TestFactory::Product PP;
          
          PP p1 = theFact(ONE, 2);
          PP p2 = theFact(TWO, 3);
          CHECK (1*2 == p1->n_);
          CHECK (2*3 == p2->n_);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MultiFactArgument_test, "unit common");
  
  
  
}} // namespace lib::test
