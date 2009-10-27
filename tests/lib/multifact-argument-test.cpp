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
#include "lib/multifact.hpp"
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
  
//  using lumiera::error::LUMIERA_ERROR_INVALID;
  
  
  namespace { // a test-dummy ID type, used to encapsulate additional arguments
    
    enum baseType
      { ONE = 1
      , TWO
      };
    
    struct DummyID
      {
        baseType bas;
        int additionalInfo;
      };
    
    
    struct Num { int n_; };
    
    /** dummy "factory" function to be invoked */
    Num*
    fabricateNumberz (int base, int offset)
    {
      cout << "fabricate("<<base<<", "<<offset<<")" << endl;
      Num* product = new Num;
      product->n_ = base*offset;
      return product;
    }
    
    
    typedef factory::MultiFact<Num, DummyID, factory::BuildRefcountPtr> TestFactory;
    
  }
  
  
  
  
  
  /*******************************************************************
   * @test define a MultiFact (factory with dynamic registration),
   *       which accepts additional arguments and passes them
   *       through to the registered factory function(s).
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
          
          DummyID id1 = {ONE, 2};
          DummyID id1 = {TWO, 3};
          
          PP p1 = theFact(id1);
          PP p2 = theFact(id2);
          ASSERT (1*2 == p1->n_);
          ASSERT (2*3 == p2->n_);
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (MultiFactArgument_test, "unit common");
  
  
  
}} // namespace lib::test
