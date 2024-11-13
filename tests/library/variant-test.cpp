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

/** @file variant-test.cpp
 ** unit test \ref Variant_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/variant.hpp"
#include "lib/util.hpp"

#include <string>


namespace lib {
namespace test{
  
  using ::Test;
  using meta::Types;
  using lib::time::Time;
  using lib::time::TimeVar;
  
  using util::contains;
  using std::string;
  
  using error::LUMIERA_ERROR_WRONG_TYPE;
  using error::LUMIERA_ERROR_LOGIC;
  
  
  // Test fixture...
  typedef Variant<Types<bool,int,string,Time>> TestVariant;
  
  
  
  
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
   * @see TreeMutator_test
   */
  class Variant_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          createVariant();
          accessVariant();
          acceptPredicate();
          verifyAssignment();
        }
      
      
      void
      createVariant()
        {
          Time someTime;
          TestVariant v0;
          TestVariant v1(11);
          TestVariant v2(string("lololo"));
          TestVariant v3(someTime);
          
          //// does not compile....
          // TestVariant evil(3.1415);
          
          cout << v0 <<endl
               << v1 <<endl
               << v2 <<endl
               << v3 <<endl;
          
          CHECK (contains (string(v0), "Variant"));
          CHECK (contains (string(v0), "bool"));
          CHECK (contains (string(v0), "false"));
          
          CHECK (contains (string(v1), "Variant"));
          CHECK (contains (string(v1), "int"));
          CHECK (contains (string(v1), "11"));
          
          CHECK (contains (string(v2), "Variant"));
          CHECK (contains (string(v2), "string"));
          CHECK (contains (string(v2), "lololo"));
          
          CHECK (contains (string(v3), "Variant"));
          CHECK (contains (string(v3), "Time"));
          CHECK (contains (string(v3), "0:00:00.000"));
        }
      
      
      void
      accessVariant()
        {
          int    someVal = rani(10000);
          string someStr = randStr(55);
          Time  someTime = randTime();
          
          TestVariant v3(someTime);
          TestVariant v2(someStr);
          TestVariant v1 = someVal;
          TestVariant v0; v0 = true;
          
          CHECK (true     == v0.get<bool>()   );
          CHECK (someVal  == v1.get<int>());
          CHECK (someStr  == v2.get<string>() );
          CHECK (someTime == v3.get<Time>()   );
          
          VERIFY_ERROR (WRONG_TYPE, v0.get<int>()    );
          VERIFY_ERROR (WRONG_TYPE, v0.get<string>() );
          VERIFY_ERROR (WRONG_TYPE, v0.get<Time>()   );
          
          VERIFY_ERROR (WRONG_TYPE, v1.get<bool>()   );
          VERIFY_ERROR (WRONG_TYPE, v1.get<string>() );
          VERIFY_ERROR (WRONG_TYPE, v1.get<Time>()   );
          
          VERIFY_ERROR (WRONG_TYPE, v2.get<bool>()   );
          VERIFY_ERROR (WRONG_TYPE, v2.get<int>()    );
          VERIFY_ERROR (WRONG_TYPE, v2.get<Time>()   );
          
          VERIFY_ERROR (WRONG_TYPE, v3.get<bool>()   );
          VERIFY_ERROR (WRONG_TYPE, v3.get<int>()    );
          VERIFY_ERROR (WRONG_TYPE, v3.get<string>() );
          
          //// does not compile...
          // v0.get<int>();
          // v1.get<double>();
          // v3.get<TimeVar>();
          
          struct Accessor
            : TestVariant::Visitor
            {
              bool b_ = false;
              int  i_ = 12;
              TimeVar t_;
              
              void handle (bool& b) { b_ = b; }
              void handle (Time& t) { t_ = t; }
              
              void handle (int& i6)
                {
                  i_ = i6;
                  ++i6;
                }
            };
          
          Accessor acs;
          CHECK (!acs.b_);
          CHECK (acs.i_ == 12);
          
          v0.accept (acs);
          CHECK (acs.b_);
          CHECK (acs.i_ == 12);
          
          v3.accept (acs);
          CHECK (acs.b_);
          CHECK (acs.i_ == 12);
          CHECK (acs.t_ == someTime);
          
          v2.accept (acs);
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
          CHECK (someVal+1 == v1.get<int>());
          v1.accept (acs);
          CHECK (someVal+2 == v1.get<int>());
          CHECK (someVal+1 == acs.i_);
        }
      
      
      void
      acceptPredicate()
        {
          const TestVariant v1(12);
          const TestVariant v2(string("123"));
          const TestVariant v3(Time::NEVER);
          
          struct Checker
            : TestVariant::Predicate
            {
              bool handle (int const& i)    { return i % 2; }
              bool handle (string const& s) { return s.length() % 2; }
            }
            check;
          
          CHECK (12 == v1.get<int>());
          CHECK ("123" == v2.get<string>());
          CHECK (Time::NEVER == v3.get<Time>());
          
          CHECK (!v1.accept(check));
          CHECK ( v2.accept(check));
          CHECK (!v3.accept(check));
          
          CHECK (12 == v1.get<int>());
          CHECK ("123" == v2.get<string>());
          CHECK (Time::NEVER == v3.get<Time>());
        }
      
      
      void
      verifyAssignment()
        {
          TestVariant v1(string("boo"));
          TestVariant v2(23);
          TestVariant v3(42);
          
          v1 = string("booo");
          v2 = v3;
          v3 = 24;
          CHECK ("booo" == v1.get<string>());
          CHECK (42 == v2.get<int>());
          CHECK (24 == v3.get<int>());
          
          VERIFY_ERROR (WRONG_TYPE, v1 = v2 );
          VERIFY_ERROR (WRONG_TYPE, v1 = 22);
          VERIFY_ERROR (WRONG_TYPE, v2 = string("2"));
          
          TestVariant v4  = Time();
          TestVariant v44 = Time(0,4,4,4);  // OK: copy initialisation
          VERIFY_ERROR (LOGIC, v4 = v44);   // Runtime Error: not assignable
          
          // v44 = Time(4,4);    // does not compile: Time is not assignable
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (Variant_test, "unit common");
  
  
}} // namespace lib::test
