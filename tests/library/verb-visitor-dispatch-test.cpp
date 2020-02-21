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
#include "lib/meta/tuple-helper.hpp"
#include "lib/iter-tree-explorer.hpp"

#include <string>
#include <vector>
#include <tuple>

using std::string;
using util::_Fmt;
using util::join;
using std::vector;
using std::forward;
using std::make_tuple;


namespace lib {
namespace test{
  
  namespace { // Test Fixture
    
    /** the "visitor" interface used by all VerbPacks in this test */
    class Receiver
      {
      public:
        virtual ~Receiver() { } ///< this is an interface
        
        virtual string woof (bool huge, uint cnt) =0;
        virtual string honk (string)              =0;
        virtual string moo (size_t num)           =0;
        virtual string meh ()                     =0;
      };
    
    
    using Token = VerbPack<Receiver, string, sizeof(string)>;   // the argument list for honk(string) requires the most inline storage
    using TokenSeq = vector<Token>;
    
    
    /**
     * a concrete receiver of verb-tokens,
     * which renders them verbosely
     */
    class VerboseRenderer
      : public Receiver
      {
        string
        woof (bool huge, uint cnt)  override
          {
            string woof;
            while (0 < cnt--)
              woof += isnil(woof)? string {huge? "Woof..":"haw-haw"}
                                 : woof;
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
            return "Meh?";
          }
      };
    
    /**
     * another concrete receiver to report any invocation with arguments
     */
    class DiagnosticRenderer
      : public Receiver
      {
        string woof (bool huge, uint cnt)  override { return report("woof", huge, cnt); }
        string honk (string theHonk)       override { return report("honk", theHonk); }
        string moo (size_t num)            override { return report("moo", num); }
        string meh()                       override { return report("meh"); }
        
        template<typename...ARGS>
        string
        report (Literal func, ARGS&&...args)
          {
            return string(func)
                +  meta::dump (make_tuple (forward<ARGS>(args)...));
          }
      };
    
  }//(End)Test Fixture
  
  
  
  
  
  
  /***********************************************************************//**
   * @test Demonstration/Concept: build pre-bound VerbPack objects to
   *       embody a specific operation on the "receiver" interface,
   *       then invoke them on a given concrete implementation.
   *       
   * @see DiffListApplication_test
   */
  class VerbVisitorDispatch_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          TokenSeq tokens = build_and_copy_tokens();
          apply_VerboseRenderer (tokens);
          apply_different_receivers (tokens);
          verify_copy_and_equality (tokens);
        }
      
      
      /** @test verify the correct individual dispatch
       *  through a computation specific for the given verb
       */
      TokenSeq
      build_and_copy_tokens ()
        {
          Token littleWoof(&Receiver::woof, "woof", 0, 3);    // NOTE: argument type conversion
          Token bigWoof(&Receiver::woof, "woof", true, 2);
          Token quack(&Receiver::honk, "honk", "quaack");
          Token honk(&Receiver::honk, "honk", "Hoonk");
          Token moo(&Receiver::moo, "moo", 3);
          Token meh(&Receiver::meh, "meh");
          
          CHECK (sizeof(Token) == sizeof(string)              // storage size reserved for the arguments (by declaration of type Token)
                                + sizeof(void*)               // additional overhead for the VTable for dispatch
                                + sizeof(void(Receiver::*)()) // storage for the function pointer to the interface function
                                + sizeof(Literal)             // storage for the verb symbol (for diagnostics and equality comparisions)
                                );
          
          // add LVal-copies to result sequence
          return TokenSeq{{littleWoof, quack, honk, bigWoof, moo, meh}};
        }
      
      /** @test demonstrate the dispatching
       *  based on the concrete verb token.
       *  Here the implementation just prints
       *  the name of the invoked verb
       */
      void
      apply_VerboseRenderer (TokenSeq& tokens)
        {
          VerboseRenderer receiver;
          for (Token& tok : tokens)
              cout << "dispatching " << tok
                   << " ->  '"
                   << tok.applyTo(receiver)
                   << "'\n";
        }
      
      /** @test demonstrate that another receiver
       *  indeed invokes different implementations
       *  of the Interface functions embedded within
       *  the Verb. Here the implementation just prints
       *  the name of the invoked verb and the arguments.
       */
      void
      apply_different_receivers (TokenSeq& tokens)
        {
          VerboseRenderer verbose;
          DiagnosticRenderer diagnostic;
          auto render = [&](Receiver& renderer)
                          {
                            return join (lib::treeExplore(tokens)
                                             .transform ([&](Token tok)
                                                           {
                                                             return tok.applyTo (renderer);
                                                           })
                                        ,"-");
                          };
          
          CHECK (render(diagnostic) == "woof(false,3)-honk(quaack)-honk(Hoonk)-woof(true,2)-moo(3)-meh()");
          CHECK (render(verbose)    == "haw-hawhaw-hawhaw-hawhaw-haw-quaack-quaack!-Hoonk-Hoonk!-Woof..Woof..-Moo__Moo__Moo-Meh?");
        }
      
      
      void
      verify_copy_and_equality (TokenSeq& tokens)
        {
          Token bigWoof = tokens[3];
          Token oldWoof{&Receiver::woof, "woof", true, 1};
          Token oldWolf{&Receiver::woof, "wolf", true, 0};
          
          CHECK (bigWoof == oldWoof);
          CHECK (bigWoof != oldWolf);
          
          CHECK (not util::isSameObject (bigWoof, oldWoof));
          CHECK (string(bigWoof) == "VerbPack(woof)");
          CHECK (string(oldWoof) == "VerbPack(woof)");
          CHECK (string(oldWolf) == "VerbPack(wolf)");
          
          VerboseRenderer bark;
          CHECK (bigWoof.applyTo(bark) == "Woof..Woof..");
          CHECK (oldWoof.applyTo(bark) == "Woof..");
          CHECK (oldWolf.applyTo(bark) == "");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (VerbVisitorDispatch_test, "unit common");
  
  
  
}} // namespace lib::test
