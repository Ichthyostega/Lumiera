/*
  VerbVisitorDispatch(Test)  -  Setup to dispatch to arbitrary functions on a receiver interface

  Copyright (C)         Lumiera.org
    2019,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file verb-visitor-dispatch-test.cpp
 ** Demonstrate the extended concept of a _verb language_ based on double dispatch.
 ** @see body-canvas-widget.hpp
 */


#include "lib/test/run.hpp"
#include "lib/verb-visitor.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"

#include <string>
#include <vector>

using std::string;
using util::_Fmt;
using util::join;
using std::vector;


namespace lib {
namespace test{
  
  
  class Receiver
    {
    public:
      virtual ~Receiver() { } ///< this is an interface
      
      virtual string woof (bool huge, uint cnt) =0;
      virtual string honk (string)              =0;
      virtual string moo (size_t num)           =0;
      virtual string meh ()                     =0;
    };
  
  namespace {
    using Token = VerbPack<Receiver, string, sizeof(string)>;
    using TokenSeq = vector<Token>;
  }
  
  
  /**
   * a receiver of verb-tokens,
   * which renders them verbosely
   */
  class VerboseRenderer
    : public Receiver
    {
      string
      woof (bool huge, uint cnt)  override
        {
          string woof{huge? "Woof..":"haw-haw"};
          while (0 < cnt--)
            woof += woof;
          return woof;
        }
      string
      honk (string theHonk)  override
        {
          return theHonk+"-"+theHonk+"!";
        }
      string
      moo (size_t num)  override
        {
          return join (vector<string>{num, "Moo"}, "__");
        }
      string
      meh()  override
        {
          return "Meh!";
        }
    };
  
  
  
  
  
  
  
  /***********************************************************************//**
   * @test Demonstration/Concept: dispatch a specific function
   *       based on the given verbs of an embedded custom language.
   *       Actually what we want to achieve here is a specific form
   *       of double dispatch; thus the implementation relies on a
   *       variation of the visitor pattern.
   *       
   * @see DiffListApplication_test
   */
  class VerbVisitorDispatch_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          TokenSeq tokens = build_and_copy_tokens();
          render_verbose (tokens);
//        profile.append_woof(1, 2);
        }
      
      
      /** prepare a sequence of verbs
       *  for the actual tests to work on */
/*    VerbSeq
      build_test_feed()
        {
          return {
            VERB_woof,
            VERB_honk,
            VERB_moo,
            VERB_meh
          };
        }
*/      
      
      /** @test verify the correct individual dispatch
       *  through a computation specific for the given verb
       */
      TokenSeq
      build_and_copy_tokens ()
        {
          Token bigWoof(&Receiver::woof, "woof", true, 2u);
          Token littleWoof(&Receiver::woof, "woof", false, 3u);
          Token quack(&Receiver::honk, "honk", string{"quaack"});
          Token honk(&Receiver::honk, "honk", string{"Hoonk"});
          Token moo(&Receiver::moo, "moo", size_t(3));
          Token meh(&Receiver::meh, "meh");
          
          
          return TokenSeq{{littleWoof, quack,honk, bigWoof, moo, meh}};
        }
      
      /** @test demonstrate the dispatching
       *  based on the concrete verb token.
       *  Here the implementation just prints
       *  the name of the invoked verb
       */
      void
      render_verbose (TokenSeq& tokens)
        {
          VerboseRenderer receiver;
          for (Token tok : tokens)
              cout << "dispatching " << tok
                   << " ->  '"
                   << tok.applyTo(receiver)
                   << "'\n";
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (VerbVisitorDispatch_test, "unit common");
  
  
  
}} // namespace lib::test
