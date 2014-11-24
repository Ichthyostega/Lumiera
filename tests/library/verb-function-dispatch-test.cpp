/*
  VerbFunctionDispatch(Test)  -  Concept to dispatch according to the verbs of a DSL

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/format-string.hpp"
#include "lib/meta/function.hpp"
#include "lib/symbol.hpp"

#include <iostream>
#include <string>
#include <vector>

using std::string;
using util::_Fmt;
using std::vector;
using std::cout;


namespace lib {
namespace test{
  
  template<class REC, class SIG>
  class VerbToken
    {
      using Ret = typename lib::meta::_Fun<SIG>::Ret;
      
      typedef Ret (REC::*Handler) (void);
      
      Handler handler_;
      Literal token_;
      
    public:
      Ret
      applyTo (REC& receiver)
        {
          return (receiver.*handler_)();
        }
      
      operator string()
        {
          return string(token_);
        }
      
      VerbToken(Handler handlerFunction, Literal token)
        : handler_(handlerFunction)
        , token_(token)
        { }
    protected:
    };
  
  
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
    
    
    Verb WOOF(&Receiver::woof, "woof");
    Verb HONK(&Receiver::honk, "honk");
    Verb  MOO(&Receiver::moo,  "moo");
    Verb  MEH(&Receiver::meh,  "meh");
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
      
      
      string woof() { return buildResultTerm (WOOF); }
      string honk() { return buildResultTerm (HONK); }
      string moo()  { return buildResultTerm (MOO);  }
      string meh()  { return buildResultTerm (MEH);  }
      
      
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
   * @see session-structure-mapping-test.cpp
   */
  class VerbFunctionDispatch_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          VerbSeq tokens = build_test_feed();
          render_verbose (tokens);
          verify_dispatch (tokens);
        }
      
      
      /** prepare a sequence of verbs
       *  for the actual tests to work on */
      VerbSeq
      build_test_feed()
        {
          return {
            WOOF,
            HONK,
            MOO,
            MEH 
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
              cout << "consuming " << string(verb)
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
  LAUNCHER (VerbFunctionDispatch_test, "unit common");
  
  
  
}} // namespace lib::test
