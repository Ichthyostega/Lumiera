/*
  Variant(Test)  -  verify the lightweight typesafe union record

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/util.hpp"


//#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
//#include <map>
//using boost::lexical_cast;
#include <vector>

using util::contains;
using std::string;
using std::cout;
using std::endl;


namespace lib {
namespace test{
  
  using ::Test;
  
  namespace { // test fixture...
    
  }//(End) test fixture
  
  typedef Variant<Types<bool,int64_t,string,Time>> TestVariant;
  
  
  
  /****************************************************************************//**
   * @test cover the essential properties of Lumiera's lightweight variant record.
   *       - this typesafe union can never be constructed empty
   *       - it defaults to the default constructed first type in list
   *       - it is copyable and assignable, but only with compatible type
   *       - value access requires knowledge of the contained type
   *       - generic visitor style access
   *       
   * @see lib::Variant
   * @see util::AccessCasted
   * @see lib::OpaqueHolder
   * @see GenericTreeMutator_test
   */
  class Variant_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          createVariant();
          accessVariant();
          verifyAssignment();
        }
      
      
      void
      createVariant()
        {
          TestVariant v0;
          TestVariant v1(11);
          TestVariant v2("lololo");
          
          auto v3 = TestVariant::empty<Time>();
          
          VERIFY_ERROR (WRONG_TYPE, TestVariant(3.1415));
          
          cout << v0 <<endl
               << v1 <<endl
               << v2 <<endl
               << v3 <<endl;
          
          CHECK (contains (string(v0), "Variant"));
          CHECK (contains (string(v0), "bool"));
          CHECK (contains (string(v0), "false"));
          
          CHECK (contains (string(v1), "Variant"));
          CHECK (contains (string(v1), "int64_t"));
          CHECK (contains (string(v1), "11"));
          
          CHECK (contains (string(v2), "Variant"));
          CHECK (contains (string(v2), "string"));
          CHECK (contains (string(v2), "lololo"));
          
          CHECK (contains (string(v3), "Variant"));
          CHECK (contains (string(v3), "lib::time::Time"));
          CHECK (contains (string(v3), "0:00:00:00"));
        }
      
      
      void
      accessVariant()
        {
          int someVal = rand() % 10000;
          int someStr = randStr(55);
          int someTime = randTime();
          
          TestVariant v3(someTime);
          TestVariant v2(someStr);
          
          auto v1 = TestVariant::empty<int64_t>();
          v1 = someVal;
          
          TestVariant v0;
          v0 = true;
          
          CHECK (true     == v0.get<bool>()   );
          CHECK (someVal  == v1.get<int64_t>());
          CHECK (someStr  == v2.get<string>() );
          CHECK (someTime == v3.get<Time>()   );
          
          VERIFY_ERROR (WRONG_TYPE, v0.get<int64_t>());
          VERIFY_ERROR (WRONG_TYPE, v0.get<string>() );
          VERIFY_ERROR (WRONG_TYPE, v0.get<Time>()   );
          
          VERIFY_ERROR (WRONG_TYPE, v1.get<bool>()   );
          VERIFY_ERROR (WRONG_TYPE, v1.get<string>() );
          VERIFY_ERROR (WRONG_TYPE, v1.get<Time>()   );
          
          VERIFY_ERROR (WRONG_TYPE, v2.get<bool>()   );
          VERIFY_ERROR (WRONG_TYPE, v2.get<int64_t>());
          VERIFY_ERROR (WRONG_TYPE, v2.get<Time>()   );
          
          VERIFY_ERROR (WRONG_TYPE, v3.get<bool>()   );
          VERIFY_ERROR (WRONG_TYPE, v3.get<int64_t>());
          VERIFY_ERROR (WRONG_TYPE, v3.get<string>() );
          
          // does not compile:
          v0.get<long>();
          v1.get<double>();
          v4.get<TimeVal>();
          
          struct Accessor
            : TestVariant::Visitor
            {
              bool b_ = false;
              int  i_ = 12;
              Time t_;
              
              void handle (bool b) { b_ = b; }
              void handle (Time t) { t_ = t; }
              
              void handle (int64_t& i6)
                {
                  i_ = i6;
                  ++i6;
                }
            };
          
          Accsssor acs;
          CHECK (!acs.b_);
          CHECK (acs.i_ == 12);
          
          v0.accept (acs);
          CHECK (acs.b_);
          CHECK (acs.i_ == 12);
          
          v4.accept (acs);
          CHECK (acs.b_);
          CHECK (acs.i_ == 12);
          CHECK (acs.t_ == someTime);
          
          v3.accept (acs);
          // nothing changed,
          // since we defined no accessor function
          CHECK (acs.b_);
          CHECK (acs.i_ == 12);
          CHECK (acs.t_ == someTime);
          
          v1.accept (acs);
          CHECK (acs.b_);
          CHECK (acs.t_ == someTime);
          CHECK (acs.i_ == someVal);
          
          // side-effect!
          CHECK (someVal+1 == v1.get<int64_t>());
          v1.accept (acs);
          CHECK (someVal+2 == v1.get<int64_t>());
          CHECK (someVal+1 == acs.i_);
        }
      
      
      void
      verifyAssignment()
        {
          TestVariant v1("boo");
          TestVariant v2(23);
          TestVariant v3(42);
          
          v1 = "booo";
          v2 = v3;
          v3 = 24;
          CHECK ("booo" == v1.get<string>());
          CHECK (42 == v2.get<int64_t>());
          CHECK (24 == v3.get<int64_t>());
          
          VERIFY_ERROR (WRONG_TYPE, v1 = v2 );
          VERIFY_ERROR (WRONG_TYPE, v1 = 22 );
          VERIFY_ERROR (WRONG_TYPE, v2 = "2");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (Variant_test, "unit common");
  
  
}} // namespace lib::test
