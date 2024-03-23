/*
  TextTemplate(Test)  -  verify the minimalistic text substitution engine

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file text-template-test.cpp
 ** unit test \ref TextTemplate_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"///////////////////////TODO
#include "lib/text-template.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"///////////////////////TODO
#include "lib/test/diagnostic-output.hpp"///////////////////////TODO
#include "lib/stat/csv.hpp"

//#include <chrono>
//#include <array>
#include <map>

//using std::array;
using std::regex_search;
using std::smatch;
using util::_Fmt;


namespace lib {
namespace test {
  
  using MapS = std::map<string, string>;
  
  
  /***************************************************************************//**
   * @test verify a minimalistic text substitution engine with flexible
   *       data binding, used for tool integration and script generation
   *       - cover the core parsing and templating functionality,
   *         using a direct binding
   *       - demonstrate the default-binding for a data map
   *       - cover the binding to Lumiera's »External Tree Description«
   * @see  text-template.hpp
   * @see  GnuplotGen_test
   */
  class TextTemplate_test : public Test
    {
      
      virtual void
      run (Arg)
        {
//          simpeUsage();
          verify_parsing();
          verify_instantiation();
          verify_keySubstituton();
          verify_conditional();
          verify_iteration();
          verify_Map_binding();
          verify_ETD_binding();

        }
      
      
      /** @test TODO simple point-and-shot usage...
       * @todo WIP 4/24 ✔ define ⟶ 🔁 implement
       */
      void
      simpeUsage()
        {
          MapS snaps{{"whatever", "cruel world"}
                    ,{"greeting", "farewell"}};
          CHECK (TextTemplate::apply ("${greeting} ${whatever} ↯", snaps)
                 == "farewell cruel world ↯"_expect);
        }
      
      
      /** @test TODO
       * @note the regular expression \ref ACCEPT_FIELD is comprised of several
       *       alternatives and optional parts, which are marked by 5 sub-expressions
       *       - 1 ≙ an escaped field (which should not be processed)
       *       - 2 ≙ else token (which must be solitary)
       *       - 3 ≙ end token
       *       - 4 ≙ some logic token ("if" or "for")
       *       - 5 ≙ a key or key path
       * @todo WIP 4/24 🔁 define ⟶ implement
       */
      void
      verify_parsing()
        {
          smatch mat;
          string input;
          CHECK (not regex_search (input, mat, ACCEPT_MARKUP));
          
          input = " Hallelujah ";
          CHECK (not regex_search (input, mat, ACCEPT_MARKUP));      // walk away ... nothing to see here...
          
          input = " stale${beer}forever";
          CHECK (regex_search (input, mat, ACCEPT_MARKUP));
          CHECK (mat.position() == 6);
          CHECK (mat.length() == 7);
          CHECK (mat.prefix() == " stale"_expect);
          CHECK (mat.suffix() == "forever"_expect);
          CHECK (mat[0]       == "${beer}"_expect);                  // so this first example demonstrates placeholder recognition 
          CHECK (not mat[1].matched);                                // Sub-1 : this is not an escaped pattern
          CHECK (not mat[2].matched);                                // Sub-2 : this pattern does not start with "else"
          CHECK (not mat[3].matched);                                // Sub-3 : no "end" keyword
          CHECK (not mat[4].matched);                                // Sub-4 : no further logic syntax
          CHECK (mat[5] == "beer"_expect);                           // Sub-5 : extracts the Key ID
          
          input = " watch ${for stale}${beer} whatever ";
          CHECK (regex_search (input, mat, ACCEPT_MARKUP));
          CHECK (mat.position() == 7);
          CHECK (mat.length() == 12);
          CHECK (mat.prefix() == " watch "_expect);
          CHECK (mat.suffix() == "${beer} whatever "_expect);        // (performing only one search here...)
          CHECK (mat[0]       == "${for stale}"_expect);             // Matched a regular opening iteration tag
          CHECK (not mat[2].matched);                                // Sub-2 does not trigger, since there is no "else" mark
          CHECK (not mat[3].matched);                                // Sub-3 does not trigger, no end mark either
          CHECK (mat[4] == "for"_expect);                            // Sub-4 picks the "for" keyword
          CHECK (mat[5] == "stale"_expect);                          // Sub-5 extracts a simple Key ≡ "stale"
          
          input = " work ${ end if  beer \t } however ";
          CHECK (regex_search (input, mat, ACCEPT_MARKUP));
          CHECK (mat.position() == 6);
          CHECK (mat.length() == 19);
          CHECK (mat.prefix() == " work "_expect);
          CHECK (mat.suffix() == " however "_expect);
          CHECK (mat[0]       == "${ end if  beer \t }"_expect);     // A regular end marker of an conditional
          CHECK (mat[3] == "end "_expect);                           // Sub-3 triggers on the "end" token
          CHECK (mat[4] == "if"_expect);                             // Sub-4 picks the "if" keyword
          CHECK (mat[5] == "beer"_expect);                           // Sub-5 extracts a simple Key ≡ "beer"
          
          input = " catch ${endgame stale}${endfor brown.beer} ever ";
          CHECK (regex_search (input, mat, ACCEPT_MARKUP));
          CHECK (mat.position() == 23);
          CHECK (mat.length() == 20);
          CHECK (mat.prefix() == " catch ${endgame stale}"_expect);// "while" is no valid keyword at the second position of the syntax
          CHECK (mat.suffix() == " ever "_expect);
          CHECK (mat[0]       == "${endfor brown.beer}"_expect);     // ...thus search proceeds to match on the second pattern installment
          CHECK (mat[3] == "end"_expect);                            // Sub-3 triggers on the "end" token
          CHECK (mat[4] == "for"_expect);                            // Sub-4 picks the "for" keyword
          CHECK (mat[5] == "brown.beer"_expect);                     // Sub-5 extracts a hierarchical key ID
          
          input = " catch ${else} ever ";
          CHECK (regex_search (input, mat, ACCEPT_MARKUP));
          CHECK (mat.position() == 7);
          CHECK (mat.length() == 7);
          CHECK (mat.prefix() == " catch "_expect);
          CHECK (mat.suffix() == " ever "_expect);
          CHECK (mat[0]       == "${else}"_expect);                  // Standard match on an "else"-tag
          CHECK (mat[2] == "else"_expect);                           // Sub-2 confirmed a solitary "else" keyword
          CHECK (not mat[1].matched);
          CHECK (not mat[3].matched);
          CHECK (not mat[4].matched);
          CHECK (not mat[5].matched);
          
          input = " catch ${else if} fever \\${can.beer} ";
          CHECK (regex_search (input, mat, ACCEPT_MARKUP));
          CHECK (mat.position() == 24);
          CHECK (mat.length() == 2);
          CHECK (mat.prefix() == " catch ${else if} fever "_expect); // Note: first pattern does not match as "else" must be solitary
          CHECK (mat.suffix() == "{can.beer} "_expect);              // Note: the following braced expression is tossed aside 
          CHECK (mat[0]       == "\\$"_expect);                      // Only the escaped pattern mark opening is picked up
          CHECK (not mat[2].matched);
          CHECK (not mat[3].matched);
          CHECK (not mat[4].matched);
          CHECK (not mat[5].matched);
          CHECK (mat[1] == "\\$"_expect);                            // Sub-1 picks the escaped mark (and the remainder is no complete tag)
          
          
          // Demonstration: can use this regular expression in a matching pipeline....
          input = "one ${two} three \\${four} ${if high} five";
          CHECK (util::join(
                    explore (util::RegexSearchIter{input, ACCEPT_MARKUP})
                      .transform ([](smatch mat){ return mat.str(); }))
                 ==
                    "${two}, \\$, ${if high}"_expect);
          
          auto render = [](TagSyntax& tag) -> string
                            { return _Fmt{"▶%s‖%d|%s‖▷"} % string{tag.lead} % uint(tag.syntax) % tag.key; };
          
          auto wau = parse(input)
                        .transform(render);
SHOW_EXPR(util::join(wau))
        }
      
      
      /** @test TODO
       * @todo WIP 4/24 🔁 define ⟶ implement
       */
      void
      verify_instantiation()
        {
        }
      
      
      /** @test TODO
       * @todo WIP 4/24 🔁 define ⟶ implement
       */
      void
      verify_keySubstituton()
        {
          UNIMPLEMENTED ("nebbich");
        }
      
      
      /** @test TODO
       * @todo WIP 4/24 🔁 define ⟶ implement
       */
      void
      verify_conditional()
        {
        }
      
      
      /** @test TODO
       * @todo WIP 4/24 🔁 define ⟶ implement
       */
      void
      verify_iteration()
        {
        }
      
      
      /** @test TODO
       * @todo WIP 4/24 🔁 define ⟶ implement
       */
      void
      verify_Map_binding()
        {
        }
      
      
      /** @test TODO
       * @todo WIP 4/24 🔁 define ⟶ implement
       */
      void
      verify_ETD_binding()
        {
        }
    };
  
  LAUNCHER (TextTemplate_test, "unit common");
  
  
}} // namespace lib::test
