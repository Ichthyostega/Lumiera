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
//#include "lib/verb-token.hpp"
#include "lib/verb-visitor.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"

#include <string>
#include <vector>

using std::string;
using util::_Fmt;
using std::vector;


namespace lib {
namespace test{
  
  
  class Receiver
    {
    public:
      virtual ~Receiver() { } ///< this is an interface
      
      virtual string woof()   =0;
      virtual string honk()   =0;
      virtual string moo()    =0;
      virtual string meh()    =0;
    };
  
  namespace {
    const string BEGINNING("silence");
    
    using Verb = VerbToken<Receiver, string(void)>;
    using VerbSeq = vector<Verb>;
    
    
    Verb VERB(Receiver, woof);
    Verb VERB(Receiver, honk);
    Verb VERB(Receiver, moo);
    Verb VERB(Receiver, meh);
  }
  
  
  /**
   * a receiver of verb-tokens,
   * which renders them verbosely
   */
  class VerboseRenderer
    : public Receiver
    {
      string woof() { return "Woof-Woof!"; }
      string honk() { return "Honk-Honk!"; }
      string moo()  { return "Moo-Moo!";   }
      string meh()  { return "Meh!";       }
    };
  
  
  /**
   * Statefull receiver of verb-tokens.
   */
  class RecollectingReceiver
    : public Receiver
    {
      string verb_;
      _Fmt fmt_;
      
      string
      buildResultTerm (string nextToken)
        {
          string resultExpression (fmt_ % verb_ % nextToken);
          verb_ = nextToken;
          return resultExpression;
        }
      
      
      string woof() { return buildResultTerm (VERB_woof); }
      string honk() { return buildResultTerm (VERB_honk); }
      string moo()  { return buildResultTerm (VERB_moo);  }
      string meh()  { return buildResultTerm (VERB_meh);  }
      
      
    public:
      RecollectingReceiver()
        : verb_(BEGINNING)
        , fmt_("%s followed by %s")
        { }
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
          VerbSeq tokens = build_test_feed();
          render_verbose (tokens);
          verify_dispatch (tokens);
          
          VerbPack<Receiver, string, sizeof(void*)> woof(&Receiver::woof, "woof");
          
//        profile.append_woof(1, 2);
        }
      
      
      /** prepare a sequence of verbs
       *  for the actual tests to work on */
      VerbSeq
      build_test_feed()
        {
          return {
            VERB_woof,
            VERB_honk,
            VERB_moo,
            VERB_meh
          };
        }
      
      
      /** @test demonstrate the dispatching
       *  based on the concrete verb token.
       *  Here the implementation just prints
       *  the name of the invoked verb
       */
      void
      render_verbose (VerbSeq tokens)
        {
          VerboseRenderer receiver;
          for (Verb verb : tokens)
              cout << "consuming " << verb
                   << " ->  '"
                   << verb.applyTo(receiver)
                   << "'\n";
        }
      
      
      /** @test verify the correct individual dispatch
       *  through a computation specific for the given verb
       */
      void
      verify_dispatch (VerbSeq tokens)
        {
          RecollectingReceiver receiver;
          string previous = BEGINNING;
          for (Verb verb : tokens)
            {
              CHECK (previous+" followed by "+string(verb) == verb.applyTo(receiver));
              previous = string(verb);
            }
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (VerbVisitorDispatch_test, "unit common");
  
  
  
}} // namespace lib::test
