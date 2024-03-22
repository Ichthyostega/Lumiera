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
#include "lib/format-cout.hpp"///////////////////////TODO
#include "lib/test/diagnostic-output.hpp"///////////////////////TODO

//#include <chrono>
//#include <array>
#include <map>

//using std::array;
using std::regex_search;
using std::smatch;


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
       *       - 1 ≙ end token
       *       - 2 ≙ some logic token ("if" or "for")
       *       - 3 ≙ a key or key path
       *       - 4 ≙ else token (which must be solitary)
       *       - 5 ≙ an escaped field (which should not be processed)
       * @todo WIP 4/24 🔁 define ⟶ implement
       */
      void
      verify_parsing()
        {
          smatch mat;
          string input;
          CHECK (not regex_search (input, mat, ACCEPT_MARKUP));
          
          input = " Hallelujah ";
          CHECK (not regex_search (input, mat, ACCEPT_MARKUP));
          
          input = " stale${beer}forever ";
SHOW_EXPR(input)
          CHECK (regex_search (input, mat, ACCEPT_MARKUP));
SHOW_EXPR(mat.position())
          CHECK (mat.position() == 6);
SHOW_EXPR(mat.length())
          CHECK (mat.length() == 7);
SHOW_EXPR(mat.prefix())
          CHECK (mat.prefix() == " stale"_expect);
SHOW_EXPR(mat.suffix())
          CHECK (mat.suffix() == "forever "_expect);
SHOW_EXPR(string(mat[0]))
          CHECK (mat[0] == "${beer}"_expect);
SHOW_EXPR(string(mat[1]))
          CHECK (not mat[1].matched);
SHOW_EXPR(string(mat[2]))
          CHECK (not mat[2].matched);
SHOW_EXPR(string(mat[3]))
          CHECK (mat[3] == "beer"_expect);
SHOW_EXPR(string(mat[4]))
          CHECK (not mat[4].matched);
SHOW_EXPR(string(mat[5]))
          CHECK (not mat[5].matched);
          
          input = " watch ${for stale}${beer} whatever ";
SHOW_EXPR(input)
          CHECK (regex_search (input, mat, ACCEPT_MARKUP));
SHOW_EXPR(mat.position())
          CHECK (mat.position() == 7);
SHOW_EXPR(mat.length())
          CHECK (mat.length() == 12);
SHOW_EXPR(mat.prefix())
          CHECK (mat.prefix() == " watch "_expect);
SHOW_EXPR(mat.suffix())
          CHECK (mat.suffix() == "${beer} whatever "_expect);
SHOW_EXPR(string(mat[0]))
          CHECK (mat[0] == "${for stale}"_expect);
SHOW_EXPR(string(mat[1]))
          CHECK (not mat[1].matched);
SHOW_EXPR(string(mat[2]))
          CHECK (mat[2] == "for"_expect);
SHOW_EXPR(string(mat[3]))
          CHECK (mat[3] == "stale"_expect);
SHOW_EXPR(string(mat[4]))
SHOW_EXPR(string(mat[5]))
          
          input = " work ${end if  beer} however ";
SHOW_EXPR(input)
          CHECK (regex_search (input, mat, ACCEPT_MARKUP));
SHOW_EXPR(mat.position())
          CHECK (mat.position() == 6);
SHOW_EXPR(mat.length())
          CHECK (mat.length() == 15);
SHOW_EXPR(mat.prefix())
          CHECK (mat.prefix() == " work "_expect);
SHOW_EXPR(mat.suffix())
          CHECK (mat.suffix() == " however "_expect);
SHOW_EXPR(string(mat[0]))
          CHECK (mat[0] == "${end if  beer}"_expect);
SHOW_EXPR(string(mat[1]))
          CHECK (mat[1] == "end "_expect);
SHOW_EXPR(string(mat[2]))
          CHECK (mat[2] == "if"_expect);
SHOW_EXPR(string(mat[3]))
          CHECK (mat[3] == "beer"_expect);
SHOW_EXPR(string(mat[4]))
SHOW_EXPR(string(mat[5]))
          
          input = " catch ${end while stale}${endfor brown.beer} ever ";
SHOW_EXPR(input)
          CHECK (regex_search (input, mat, ACCEPT_MARKUP));
SHOW_EXPR(mat.position())
          CHECK (mat.position() == 25);
SHOW_EXPR(mat.length())
          CHECK (mat.length() == 20);
SHOW_EXPR(mat.prefix())
          CHECK (mat.prefix() == " catch ${end while stale}"_expect);
SHOW_EXPR(mat.suffix())
          CHECK (mat.suffix() == " ever "_expect);
SHOW_EXPR(string(mat[0]))
          CHECK (mat[0] == "${endfor brown.beer}"_expect);
SHOW_EXPR(string(mat[1]))
          CHECK (mat[1] == "end"_expect);
SHOW_EXPR(string(mat[2]))
          CHECK (mat[2] == "for"_expect);
SHOW_EXPR(string(mat[3]))
          CHECK (mat[3] == "brown.beer"_expect);
SHOW_EXPR(string(mat[4]))
SHOW_EXPR(string(mat[5]))
          
          input = " catch ${else} ever ";
SHOW_EXPR(input)
          CHECK (regex_search (input, mat, ACCEPT_MARKUP));
SHOW_EXPR(mat.position())
          CHECK (mat.position() == 7);
SHOW_EXPR(mat.length())
          CHECK (mat.length() == 7);
SHOW_EXPR(mat.prefix())
          CHECK (mat.prefix() == " catch "_expect);
SHOW_EXPR(mat.suffix())
          CHECK (mat.suffix() == " ever "_expect);
SHOW_EXPR(string(mat[0]))
          CHECK (mat[0] == "${else}"_expect);
SHOW_EXPR(string(mat[1]))
SHOW_EXPR(string(mat[2]))
SHOW_EXPR(string(mat[3]))
          CHECK (mat[3] == "else"_expect);
SHOW_EXPR(string(mat[4]))
SHOW_EXPR(string(mat[5]))
          
          input = " catch ${else if} fever \\${can.beer} ";
SHOW_EXPR(input)
          CHECK (regex_search (input, mat, ACCEPT_MARKUP));
SHOW_EXPR(mat.position())
          CHECK (mat.position() == 24);
SHOW_EXPR(mat.length())
          CHECK (mat.length() == 2);
SHOW_EXPR(mat.prefix())
          CHECK (mat.prefix() == " catch ${else if} fever "_expect);
SHOW_EXPR(mat.suffix())
          CHECK (mat.suffix() == "{can.beer} "_expect);
SHOW_EXPR(string(mat[0]))
          CHECK (mat[0] == "\\$"_expect);
SHOW_EXPR(string(mat[1]))
SHOW_EXPR(string(mat[2]))
          CHECK (not mat[2].matched);
SHOW_EXPR(string(mat[3]))
          CHECK (not mat[3].matched);
SHOW_EXPR(string(mat[4]))
          CHECK (not mat[4].matched);
SHOW_EXPR(string(mat[5]))
          CHECK (mat[5] == "\\$"_expect);
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
