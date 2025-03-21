/*
  VerbFunctionDispatch(Test)  -  Concept to dispatch according to the verbs of a DSL

   Copyright (C)
     2014,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file verb-function-dispatch-test.cpp
 ** Demonstrate the concept of a _verb language_ based on double dispatch.
 ** @see diff-language.hpp
 */


#include "lib/test/run.hpp"
#include "lib/verb-token.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"

#include <string>
#include <vector>

using std::string;
using util::_Fmt;
using std::vector;


namespace lib {
namespace test{
  
  
  namespace { // Test Fixture
    
    /** the "visitor" interface to invoke */
    class Receiver
      {
      public:
        virtual ~Receiver() { } ///< this is an interface
        
        virtual string woof()   =0;
        virtual string honk()   =0;
        virtual string moo()    =0;
        virtual string meh()    =0;
      };
    
    const string BEGINNING("silence");
    
    using Verb = VerbToken<Receiver, string(void)>;
    using VerbSeq = vector<Verb>;
    
    
    Verb VERB(Receiver, woof);
    Verb VERB(Receiver, honk);
    Verb VERB(Receiver, moo);
    Verb VERB(Receiver, meh);
    
    
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
    
  }//(End) Test fixture
  
  
  
  
  
  
  /**********************************************************************//**
   * @test Demonstration/Concept: dispatch a specific function
   *       based on the given verbs of an embedded custom language.
   *       Actually what we want to achieve here is a specific form
   *       of double dispatch; thus the implementation relies on a
   *       variation of the visitor pattern.
   *       
   * @see DiffListApplication_test
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
  LAUNCHER (VerbFunctionDispatch_test, "unit common");
  
  
  
}} // namespace lib::test
