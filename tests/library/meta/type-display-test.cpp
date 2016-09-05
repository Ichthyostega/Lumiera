/*
  TypeDisplay(Test)  -  human readable simplified display of C++ types

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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

//#include "lib/p.hpp"
//#include "lib/diff/gen-node.hpp"

#include "lib/meta/util.hpp"

#include <iostream>       //////////TODO
#include <string>

//using lib::P;
//using lib::newP;
//using lib::diff::GenNode;

using std::string;
using std::cout;       /////////////TODO
using std::endl;       /////////////TODO


namespace lib {
namespace meta{
namespace test{

  namespace { // test fixture
    
    template<class T>
    struct Outer
      {
        struct Inner { };
        
        static const T*
        cloak (Inner&&)
          {
            return nullptr;
          }
      };
    
    struct Space { };
    
    auto CHALLENGE_1 = "some::arbitrary::BullShit<oh::RLY>*";
    auto CHALLENGE_2 = "lib::Contrived<lib::meta::Barely,true>::ClusterFuck<const std::string& (const std::vector<proc::mobject::oh::RLY* const>)>";
    auto CHALLENGE_3 = "std::function<special::(anonymous namespace)::Shit(lib::P<proc::asset::Clip, std::shared_ptr<proc::asset::Clip>>)>";
    
  }//(end)fixture
  
  
  
  
  /***************************************************************************//**
   * @test verify post processing of demangled C++ type names.
   *  The purpose of those pretty printing functions is to support diagnostics
   *  and unit testing by making type names easier to digest. But of course
   *  we do not want to pick the wrong primary type for shortened display
   *  and we do not want mess up the semantic structure.
   *  - the first example should be passed through unaltered
   *  - the second example demonstrates various simplifications
   *    * strip some frequent namespace prefixes
   *      ** `std::`
   *      ** `lib::meta::`
   *      ** `proc::mobject::`
   *    * omit the standard allocator from STL containers
   *  - the third example demonstrates an embedded function signature
   *    * the primary type component is "`function`"
   *    * anything leading up to anonymous namespaces will be stripped
   *    * our special smart-ptr lib::P will be simplified
   * 
   * The remainder of the test concentrates on the corner cases of
   * lib::meta::primaryTypeComponent -- especially when the solution
   * approach breaks down, e.g. on pure function types.
   * 
   * @see format-cout.hpp
   * @see FormatCOUT_test
   * @see FormatHelper_test
   */
  class TypeDisplay_test
    : public Test
    {
      void
      run (Arg)
        {
          cout << "-----input----------------------"<<endl;
          cout << CHALLENGE_1                       <<endl;
          cout << CHALLENGE_2                       <<endl;
          cout << CHALLENGE_3                       <<endl;
          
          cout << "-----human-readable-------------"<<endl;
          cout << humanReadableTypeID (CHALLENGE_1) <<endl;
          cout << humanReadableTypeID (CHALLENGE_2) <<endl;
          cout << humanReadableTypeID (CHALLENGE_3) <<endl;
          
          cout << "-----primary-component----------" <<endl;
          cout << primaryTypeComponent (CHALLENGE_1) <<endl;
          cout << primaryTypeComponent (CHALLENGE_2) <<endl;
          cout << primaryTypeComponent (CHALLENGE_3) <<endl;
          
          cout << "-----sanitised-ID---------------"  <<endl;
          cout << sanitisedFullTypeName (CHALLENGE_1) <<endl;
          cout << sanitisedFullTypeName (CHALLENGE_2) <<endl;
          cout << sanitisedFullTypeName (CHALLENGE_3) <<endl;
          
          
          Outer<Space> ship;
          auto magic = ship.cloak;
          CHECK (typeStr(&magic)    == "Space const* (*)(Outer<Space>::Inner&&)");
          CHECK (typeSymbol(&magic) == "Function");
          
          CHECK (typeStr   <Outer<typeof(this)>::Inner>()    == "Outer<test::TypeDisplay_test*>::Inner");
          CHECK (typeSymbol<Outer<typeof(this)>::Inner>()    == "Inner"      );
          
          CHECK (primaryTypeComponent("")                    == "void"       );
          CHECK (primaryTypeComponent("Sym&")                == "Sym"        );
          CHECK (primaryTypeComponent("Sym const *")         == "Sym"        );
          CHECK (primaryTypeComponent("Sym const * const")   == "Sym"        );
          CHECK (primaryTypeComponent("Sym§$<>%&/'* const")  == "Sym§$<>%&/'"); // adornments stripped, but rest retained as-is
          CHECK (primaryTypeComponent("Sym§$<>%&/)&* const") == "Function"   ); // types ending with braces are classified as "Function"
          CHECK (primaryTypeComponent("Sym <§$&ää>")         == "Sym "       ); // note the trailing whitespace
          CHECK (primaryTypeComponent("One<§$&ää>::Two")     == "Two"        );
          CHECK (primaryTypeComponent("One::Two<§$&ää>")     == "Two"        );
          CHECK (primaryTypeComponent("One::Two<§$&ää>")     == "Two"        );
          CHECK (primaryTypeComponent("Sym<<xx>")            == "Sym<"       ); // unbalanced braces
          CHECK (primaryTypeComponent("Sym<>xx>")            == "void"       ); // opening brace never found
          CHECK (primaryTypeComponent("<x>xx>*")             == "void"       );
          CHECK (primaryTypeComponent("<x<xx>*")             == "<x"         );
          
          CHECK (sanitisedSymbol("bäälü9a/(6f*a☹☢☀s☭el_88☠") == "blafasel_88"); // note: picking up only valid identifier fragments
          
        }
    };
  
  LAUNCHER (TypeDisplay_test, "unit common");
  
  
}}} // namespace lib::meta::test

