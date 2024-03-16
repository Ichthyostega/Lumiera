/*
  MultiFactSingleton(Test)  -  using MultiFact to manage a family of singletons

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file multifact-singleton-test.cpp
 ** unit test \ref MultiFactSingleton_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/multifact.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <string>



namespace lib {
namespace test{
  
  using boost::lexical_cast;
  using lib::test::showSizeof;
  using util::isSameObject;
  using util::isnil;
  using std::ostream;
  using std::string;
  
  using LERR_(INVALID);
  
  
  namespace { // hierarchy of test dummy objects
    
    struct Interface
      {
        virtual ~Interface() {};
        virtual operator string () const  =0;
      };
    
    
    enum theID
      { ONE = 1
      , TWO
      , THR
      , FOU
      };
    
    typedef factory::MultiFact<Interface&, theID> TestFactory;
    
    
    template<theID ii>
    class Implementation
      : public Interface
      {
        operator string()  const override
          {
            return "Impl-"+lexical_cast<string> (ii);
          }
      };
    
    /** Factory instance for the tests... */
    TestFactory theFact;
    
    // Configure the products to be fabricated....
    TestFactory::Singleton<Implementation<ONE>> holder1{theFact,ONE};
    TestFactory::Singleton<Implementation<TWO>> holder2{theFact,TWO};
    TestFactory::Singleton<Implementation<THR>> holder3{theFact,THR};
    TestFactory::Singleton<Implementation<FOU>> holder4{theFact,FOU};
  }
  
  
  
  
  
  /******************************************************************//**
   * @test verify the use of the MultiFact template to access Singletons.
   *       While generally speaking the MultiFact allows us to address
   *       and invoke several "production lines" by ID, an obvious
   *       use case would be to access a "family" of singletons
   *       through this mechanism. And indeed, \c MultiFact::Singleton
   *       is a preconfigured shortcut for this use case. The actual
   *       singleton access factories are placed into a static context
   *       (here in the anonymous namespace above) and their access
   *       operation is wired as "factory function".
   *       - we use a hierarchy of test dummy objects
   *       - we set up a singleton factory for several subclasses
   *       - the registration happens automatically in the ctor
   *       - we verify that we indeed get the correct flavour.
   * 
   * @see  lib::MultiFact
   */
  class MultiFactSingleton_test : public Test
    {
      void
      run (Arg) 
        {
          cout << theFact(ONE) << endl;
          cout << theFact(TWO) << endl;
          cout << theFact(THR) << endl;
          cout << theFact(FOU) << endl;
          cout << showSizeof (theFact) << endl;
          
          Interface & o1 = theFact(ONE);
          Interface & o2 = theFact(ONE);
          CHECK (isSameObject(o1,o2));
          
          TestFactory anotherFact;
          CHECK (isnil (anotherFact));
          VERIFY_ERROR (INVALID, anotherFact(ONE) );
          
          TestFactory::Singleton<Implementation<ONE>> anotherSingletonHolder (anotherFact,ONE);
          Interface & o3 = anotherFact(ONE);
          CHECK (isSameObject(o2,o3));
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (MultiFactSingleton_test, "unit common");
  
  
  
}} // namespace lib::test
