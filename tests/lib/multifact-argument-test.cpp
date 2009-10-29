/*
  MultiFactArgument(Test)  -  passing additional invocation arguments to registered factory functions
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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
#include "lib/test/test-helper.hpp"
#include "lib/multifact-arg.hpp"
//#include "lib/util.hpp"

//#include <boost/lexical_cast.hpp>
#include <iostream>
//#include <string>
#include <tr1/functional>



namespace lib {
namespace test{
  
//  using boost::lexical_cast;
  using lib::test::showSizeof;
//  using util::isSameObject;
//  using util::isnil;
//  using std::ostream;
//  using std::string;
  using std::cout;
  using std::endl;
  using std::tr1::bind;
  using std::tr1::function;
  using std::tr1::placeholders::_1;
  
//  using lumiera::error::LUMIERA_ERROR_INVALID;
  
  
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
    
  }
  
  
  
  
  
  /*******************************************************************
   * @test define a MultiFact (factory with dynamic registration),
   *       which accepts additional arguments and passes them
   *       through to the registered factory function(s).
   * @note we set up fabrication functions by binding such as to match
   *       the function signature declared in the factory; thereby one
   *       argument remains unclosed, which is the argument to be
   *       supplied on each factory invocation by the client code.
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
          theFact.defineProduction (ONE, bind (&fabricateNumberz, 1, _1));
          theFact.defineProduction (TWO, bind (&fabricateNumberz, 2, _1));
          
          cout << showSizeof (theFact) << endl;
          
          typedef TestFactory::Product PP;
          
          PP p1 = theFact(ONE, 2);
          PP p2 = theFact(TWO, 3);
          ASSERT (1*2 == p1->n_);
          ASSERT (2*3 == p2->n_);
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (MultiFactArgument_test, "unit common");
  
  
  
}} // namespace lib::test
