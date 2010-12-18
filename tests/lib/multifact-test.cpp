/*
  MultiFact(Test)  -  unittest for the configurable object-family creating factory

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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/multifact.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>



namespace lib {
namespace test{
  
  using boost::lexical_cast;
  using lib::test::showSizeof;
  using util::isSameObject;
  using util::isnil;
  using std::ostream;
  using std::string;
  using std::cout;
  using std::endl;
  
  using lumiera::error::LUMIERA_ERROR_INVALID;
  
  
  namespace { // hierarchy of test dummy objects
    
    struct Interface
      {
        virtual ~Interface() {};
        virtual operator string () =0;
      };
    
    inline ostream& operator<< (ostream& os, Interface& ifa) { return os << string(ifa); }
    
    
    enum theID
      { ONE = 1
      , TWO
      , THR
      , FOU
      };
    
    typedef factory::MultiFact<Interface, theID, factory::PassReference> TestFactory;
    
    
    template<theID ii>
    class Implementation
      : public Interface
      {
        operator string()
          {
            return "Impl-"+lexical_cast<string> (ii);
          }
        
      public:
        static theID getTypeID() { return ii; }
      };
    
    /** Factory instance for the tests... */
    TestFactory theFact;
    
    // Configure the products to be fabricated....
    TestFactory::Singleton<Implementation<ONE> > holder1 (theFact,ONE);
    TestFactory::Singleton<Implementation<TWO> > holder2 (theFact,TWO);
    TestFactory::Singleton<Implementation<THR> > holder3 (theFact,THR);
    TestFactory::Singleton<Implementation<FOU> > holder4 (theFact,FOU);
  }
  
  
  
  
  
  /*******************************************************************
   * @test verify simple setup of the MultiFact template.
   *       Define a hierarchy of test dummy objects, in order to
   *       register them automatically for creation through a suitable
   *       instantiation of MultiFact. Verify we get the correct product
   *       when invoking this MultiFac flavour.
   * @see  lib::MultiFact
   */
  class MultiFact_test : public Test
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
          
          TestFactory::Singleton<Implementation<ONE> > anotherSingletonHolder (anotherFact,ONE);
          Interface & o3 = anotherFact(ONE);
          CHECK (isSameObject(o2,o3));
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (MultiFact_test, "unit common");
  
  
  
}} // namespace lib::test
