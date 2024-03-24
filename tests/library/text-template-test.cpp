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
  using LERR_(ITER_EXHAUST);
  
  
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
          simpeUsage();
          verify_parsing();
          verify_instantiation();
          verify_keySubstituton();
          verify_conditional();
          verify_iteration();
          verify_Map_binding();
          verify_ETD_binding();

        }
      
      
      /** @test simple point-and-shot usage...
       * @todo WIP 4/24 ✔ define ⟶ ✔ implement
       */
      void
      simpeUsage()
        {
          MapS snaps{{"whatever", "cruel world"}
                    ,{"greeting", "farewell"}};
          CHECK (TextTemplate::apply ("${greeting} ${whatever} ↯", snaps)
                 == "farewell cruel world ↯"_expect);
        }
      
      
      /** @test parsing of tag markup and compilation into a sequence of Action-codes
       * @note the regular expression \ref ACCEPT_FIELD is comprised of several
       *       alternatives and optional parts, which are marked by 5 sub-expressions
       *       - 1 ≙ an escaped field (which should not be processed)
       *       - 2 ≙ else token (which must be solitary)
       *       - 3 ≙ end token
       *       - 4 ≙ some logic token ("if" or "for")
       *       - 5 ≙ a key or key path
       * @todo WIP 4/24 ✔ define ⟶ ✔ implement
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
          
          
          // Parse matches of this regexp into well defined syntax elements
          auto parser = parse (input);
          CHECK (not isnil(parser));
          CHECK (parser->syntax == TagSyntax::KEYID);
          CHECK (parser->lead == "one "_expect);
          CHECK (parser->key  == "two"_expect);
          ++parser;
          CHECK (parser);
          CHECK (parser->syntax == TagSyntax::ESCAPE);
          CHECK (parser->lead == " three "_expect);
          CHECK (parser->key  == ""_expect);
          ++parser;
          CHECK (parser);
          CHECK (parser->syntax == TagSyntax::IF);
          CHECK (parser->lead == "\\${four} "_expect);
          CHECK (parser->key  == "high"_expect);
          ++parser;
          CHECK (isnil (parser));
          VERIFY_ERROR (ITER_EXHAUST, *parser);
          VERIFY_ERROR (ITER_EXHAUST, ++parser);
          
          
          
          // Generate sequence of Action tokens from parsing results
          input = R"~(
 Prefix-1 ${some.key} next one is \${escaped}
 Prefix-2 ${if cond1} active ${else} inactive ${end if
}Prefix-3 ${if cond2} active2${end if cond2} more
 Prefix-4 ${for data} fixed ${embedded}
    Pre-5 ${if nested}nested-active${
            else     }nested-inactive${ end
            if nested}loop-suffix${else}${end
for} tail...
)~";
          auto actions = TextTemplate::compile (input);
          CHECK (25 == actions.size());
          
          CHECK (actions[ 0].code == TextTemplate::Code::TEXT);
          CHECK (actions[ 0].val  == "\n Prefix-1 "_expect);                   // static text prefix
          CHECK (actions[ 0].refIDX == 0);
          
          CHECK (actions[ 1].code == TextTemplate::Code::KEY);                 // a placeholder to be substituted
          CHECK (actions[ 1].val  == "some.key"_expect);                       // use "some.key" for data retrieval
          
          CHECK (actions[ 2].code == TextTemplate::Code::TEXT);                // static text between active fields
          CHECK (actions[ 2].val  == " next one is "_expect);
          
          CHECK (actions[ 3].code == TextTemplate::Code::TEXT);                // since next tag was escaped, it appears in static segment
          CHECK (actions[ 3].val  == "\\${escaped}\n Prefix-2 "_expect);
          
          CHECK (actions[ 4].code == TextTemplate::Code::COND);                // start of an if-bracket construct
          CHECK (actions[ 4].val  == "cond1"_expect);                          // data marked with "cond1" will be used to determine true/false
          CHECK (actions[ 4].refIDX == 7 );                                    // IDX ≡ 7 marks start of the else-branch
          
          CHECK (actions[ 5].code == TextTemplate::Code::TEXT);                // this static block will only be included if "cond1" evaluates to true
          CHECK (actions[ 5].val  == " active "_expect);
          
          CHECK (actions[ 6].code == TextTemplate::Code::JUMP);                // unconditional jump at the end of the if-true-block
          CHECK (actions[ 6].val  == ""_expect);
          CHECK (actions[ 6].refIDX == 8 );                                    // IDX ≡ 8 points to the next element after the conditional construct
          
          CHECK (actions[ 7].code == TextTemplate::Code::TEXT);                // this static (else)-block will be included if "cond1" does not hold
          CHECK (actions[ 7].val  == " inactive "_expect);
          
          CHECK (actions[ 8].code == TextTemplate::Code::TEXT);                // again a static segment, displayed unconditionally
          CHECK (actions[ 8].val  == "Prefix-3 "_expect);                      // Note: no newline, since the closing bracket was placed at line start
          
          CHECK (actions[ 9].code == TextTemplate::Code::COND);                // again a conditional (but this time without else-branch)
          CHECK (actions[ 9].val  == "cond2"_expect);                          // data marked with "cond2" will be evaluated as condition
          CHECK (actions[ 9].refIDX == 11 );                                   // IDX ≡ 11 is the alternative route, this time pointing behind the conditional
          
          CHECK (actions[10].code == TextTemplate::Code::TEXT);                // static text block to be displayed as content of the conditional
          CHECK (actions[10].val  == " active2"_expect);
          
          CHECK (actions[11].code == TextTemplate::Code::TEXT);                // again an unconditional static segment (behind end of preceding conditional)
          CHECK (actions[11].val  == " more\n Prefix-4 "_expect);
          
          CHECK (actions[12].code == TextTemplate::Code::ITER);                // Start of a for-construct (iteration)
          CHECK (actions[12].val  == "data"_expect);                           // data marked with "data" will be used to find and iterate nested elements
          CHECK (actions[12].refIDX == 23 );                                   // IDX ≡ 23 points to the alternative "else" block, in case no iteration takes place
          
          CHECK (actions[13].code == TextTemplate::Code::TEXT);                // static block to appear for each nested "data" element
          CHECK (actions[13].val  == " fixed "_expect);
          
          CHECK (actions[14].code == TextTemplate::Code::KEY);                 // placeholder to be substituted
          CHECK (actions[14].val  == "embedded"_expect);                       // _typically_ the data "embedded" will live in the iterated, nested elements
          
          CHECK (actions[15].code == TextTemplate::Code::TEXT);                // again a static block, which however lives within the iterated segment
          CHECK (actions[15].val  == "\n    Pre-5 "_expect);
          
          CHECK (actions[16].code == TextTemplate::Code::COND);                // a nested conditional, thus nested on second level within the iteration construct
          CHECK (actions[16].val  == "nested"_expect);                         // data marked with "nested" will control the conditional (typically from iterated data elements)
          CHECK (actions[16].refIDX == 19 );                                   // IDX ≡ 19 points to the else-block of this nested conditional
          
          CHECK (actions[17].code == TextTemplate::Code::TEXT);                // static content to appear as nested if-true-section
          CHECK (actions[17].val  == "nested-active"_expect);
          
          CHECK (actions[18].code == TextTemplate::Code::JUMP);                // jump code at end of the true-section
          CHECK (actions[18].val  == ""_expect);
          CHECK (actions[18].refIDX == 20 );                                   // IDX ≡ 20 points behind the end of this nested conditional construct
          
          CHECK (actions[19].code == TextTemplate::Code::TEXT);                // static content comprising the else-section
          CHECK (actions[19].val  == "nested-inactive"_expect);                // Note: no whitespace due to placement of the tag brackets of "else" / "end if"
          
          CHECK (actions[20].code == TextTemplate::Code::TEXT);                // again an unconditional static segment, yet still within the looping construct
          CHECK (actions[20].val  == "loop-suffix"_expect);
          
          CHECK (actions[21].code == TextTemplate::Code::LOOP);                // the loop-end code, where evaluation will consider the next iteration
          CHECK (actions[21].val  == ""_expect);
          CHECK (actions[21].refIDX == 12 );                                   // IDX ≡ 12 points back to the opening ITER code
          
          CHECK (actions[22].code == TextTemplate::Code::JUMP);                // if however the iteration is complete, evaluation will jump over the "else" section
          CHECK (actions[22].val  == ""_expect);
          CHECK (actions[22].refIDX == 24 );
          
          CHECK (actions[23].code == TextTemplate::Code::TEXT);                // this static else-segment will appear whenever no iteration takes place
          CHECK (actions[23].val  == ""_expect);                               // Note: in this example there is an ${else}-tag, yet the content is empty
          
          CHECK (actions[24].code == TextTemplate::Code::TEXT);                // a final static segment after the last active tag
          CHECK (actions[24].val  == " tail...\n"_expect);
          CHECK (actions[24].refIDX == 0);
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
