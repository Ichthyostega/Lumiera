/*
  CommandEquality(Test)  -  verify equality comparisons on command's subsystems

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file command-equality-test.cpp
 ** unit test \ref CommandEquality_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/control/command-def.hpp"
#include "steam/control/command-mutation.hpp"
#include "steam/control/argument-erasure.hpp"
#include "steam/control/command-storage-holder.hpp"
#include "steam/control/command-simple-closure.hpp"
#include "steam/control/memento-tie.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/format-cout.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <functional>
#include <string>
#include <tuple>

namespace steam {
namespace control {
namespace test    {
  
  
  
  using namespace lib::meta;
  using lib::Symbol;
  using util::isnil;
  using util::isSameObject;
  
  using std::function;
  using std::bind;
  using std::string;
  using std::make_tuple;
  
  
  
  namespace {
    
    string check_;
    
    Symbol COMMAND1 ("test.equalityCommand1");
    Symbol COMMAND2 ("test.equalityCommand2");
    
    const string MARK_1 ("|_1_");
    const string MARK_2 ("|_2_");
    
    void oper_1 (char par)             { check_ += MARK_1 + par; }
    void oper_2 (char par)             { check_ += MARK_2 + par; }
    
    string capt_1 (char par)           { return check_ + MARK_1 + "|"+par+"|"; }
    string capt_2 (char par)           { return check_ + MARK_2 + "|"+par+"|"; }
    
    void undo_1 (char par, string mem) { check_ = mem + MARK_1 + par + "|";}
    void undo_2 (char par, string mem) { check_ = mem + MARK_2 + par + "|";}
    
    
    typedef void   Sig_oper(char);
    typedef string Sig_capt(char);
    typedef void   Sig_undo(char,string);
    
    typedef function<Sig_oper> Fun_o;
    typedef function<Sig_capt> Fun_c;
    typedef function<Sig_undo> Fun_u;
    
    using ArgTuple  = Tuple<Types<char>>;
    using ArgHolder = OpClosure<Sig_oper>;
    using MemHolder = MementoTie<Sig_oper, string>;
    using Closure   = SimpleClosure<Sig_oper>;
  }
  
  
  
  /*************************************************************************************//**
   * @test cover command equality detection.
   * Two commands are deemed equivalent, if they are based on the same CommandImpl record.
   * This means, we only rely on the _identity_ of those commands, but do not check the
   * _equivalence_ of their backing implementations. The latter can not be possibly
   * implemented in a totally airtight fashion, and for this reason, the C++ standard
   * decided not to support comparison between std::function objects.
   * 
   * @see  control::Command
   * @see  control::CommandImpl
   * @see  command-basic-test.hpp
   */
  class CommandEquality_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          CHECK (&oper_1 != &oper_2);
          CHECK (&capt_1 != &capt_2);
          CHECK (&undo_1 != &undo_2);
          
          CommandDef (COMMAND1)
            .operation (oper_1)
            .captureUndo (capt_1)
            .undoOperation (undo_1)
            ;
          CommandDef (COMMAND2)
            .operation (oper_2)
            .captureUndo (capt_2)
            .undoOperation (undo_2)
            ;
          
          Command c1 = Command::get(COMMAND1);
          Command c2 = Command::get(COMMAND2);
          CHECK (c1 == c1);
          CHECK (c1 != c2);
          CHECK (c2 != c1);
          
          Command cx = c1;
          CHECK (c1 == cx);
          CHECK (cx == c1);
          CHECK (!isSameObject (c1, c2));
          
          // verify equality matches behaviour
          string protocol1 = execCommand(c1);
          string protocolX = execCommand(cx);
          string protocol2 = execCommand(c2);
          
          CHECK (protocol1 == protocolX);
          CHECK (protocol1 != protocol2);
        }
      
      
      /** Helper: invoke and undo a command, 
       *  @return resulting operation protocol
       */
      string
      execCommand (Command com)
        {
          check_ = "(start)";
          com.bind('o');
          com();
          cout << com << ":" << check_ << endl;
          com.undo();
          cout << com << ":" << check_ << endl;
          return check_;
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandEquality_test, "function controller");
  
  
}}} // namespace steam::control::test
