/*
  CommandEquality(Test)  -  verify equality comparisons on command's subsystems

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

namespace proc {
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
   * Two commands are deemed equivalent, if they
   *  - build on the same Mutation functors
   *  - are either both incomplete or
   *  - are bound to equivalent arguments
   *  - hold equivalent undo state (memento)
   * To conduct this test, we set up two sets of functions, and then build both complete
   * command objects and command implementation facilities based on them.
   * 
   * @note The hidden problem with those comparisons is the equivalence of function objects.
   *       While required by TR1, unfortunately lib boost refuses to implement functor equality.
   *       Which forces us to resort to a low level hack, based on internals of the boost function
   *       implementation. This workaround reliably pinpoints differing functions, but sometimes
   *       fails to detect equivalent functions under specific circumstances (e.g. when there is
   *       binding involved, and / or the binders have been cloned). Bottom line: \c == is
   *       reliable, \c != might be wrong.
   * 
   * @see  control::Command
   * @see  control::CmdClosure
   * @see  control::Mutation
   * @see  control::UndoMutation
   * @see  control::MementoTie
   * @see  control::CommandImpl
   * @see  command-basic-test.hpp
   * @see  functor-util.hpp functor equality workaround
   */
  class CommandEquality_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          CHECK (&oper_1 != &oper_2);
          CHECK (&capt_1 != &capt_2);
          CHECK (&undo_1 != &undo_2);
          
          verifyMutationEquality();
          verifyMementoEquality();
          verifyClosureEquality();
          verifyCommandEquality();
        }
      
      
      
      void
      verifyMutationEquality()
        {
          Fun_o oFun_1 (oper_1);
          Fun_o oFun_2 (oper_2);
          Fun_o oFun_empty;
          
          Fun_u uFun_1 (undo_1);
          Fun_u uFun_empty;
          
          Mutation mut1 (oFun_1);
          Mutation muti (oFun_1);
          Mutation mut2 (oFun_2);
          CHECK (mut1 == mut1);
          CHECK (mut1 == muti);
          CHECK (muti == mut1);
          CHECK (mut1 != mut2);
          CHECK (mut2 != mut1);
          CHECK (muti != mut2);
          CHECK (mut2 != muti);
          
          Mutation umu (oFun_empty);    // empty operation function
          CHECK (mut1 != umu);
          
          Mutation mut_u0 (uFun_empty); // empty undo function
          CHECK (mut_u0 != umu);
          CHECK (mut_u0 != muti);
          
          Mutation mut_u1 (uFun_1); 
          CHECK (mut_u0 != mut_u1);    // function signatures differing
        }
      
      
      void
      verifyClosureEquality()
        {
          ArgHolder a1 (make_tuple ('a'));
          ArgHolder a2 (make_tuple ('a'));
          ArgHolder a3 (make_tuple ('z'));
          CHECK (a1 == a1);
          CHECK (a1 == a2);
          CHECK (a2 == a1);
          CHECK (a1 != a3);
          CHECK (a3 != a1);
          CHECK (a2 != a3);
          CHECK (a3 != a2);
          
          typedef StorageHolder<Sig_oper,string> Storage;
          Storage abuff1;
          Storage abuff2;
          CHECK (abuff1 == abuff2);
          
          TypedArguments<ArgTuple> newArgs (make_tuple ('z'));
          abuff1.bindArguments(newArgs);
          CHECK (abuff1 != abuff2);
          abuff2.bindArguments(newArgs);
          CHECK (abuff1 == abuff2);
          UndoMutation umu1 (abuff1.tie (undo_1, capt_1));
          CHECK (abuff1 != abuff2);                       // abuff2 isn't tied yet, i.e. has no undo/capture function
          UndoMutation umu2 (abuff2.tie (undo_1, capt_1));
          CHECK (abuff1 == abuff2);                       // same capture function, no memento state!
          
          Closure args {make_tuple ('u')};
          umu1.captureState(args);
          CHECK (abuff1 != abuff2);
          umu2.captureState(args);
          CHECK (abuff1 == abuff2); // same functions, same memento state
          
          check_ += "fake";          // manipulate the "state" to be captured
          umu2.captureState(args);     // capture again...
          CHECK (abuff1 != abuff2); // captured memento differs!
          
          UndoMutation umu3 (abuff2.tie (undo_1, capt_2));
          umu3.captureState(args);
          CHECK (abuff1 != abuff2); // differing functions detected
        }
      
      
      void
      verifyMementoEquality()
        {
          Fun_u uFun_1 (undo_1);
          Fun_u uFun_2 (undo_2);
          Fun_c cFun_1 (capt_1);
          Fun_c cFun_2 (capt_2);
          Fun_c cFun_empty;
          
          Fun_c empty_c;
          
          MemHolder m11 (uFun_1, cFun_1);
          MemHolder m12 (uFun_1, cFun_2);
          MemHolder m21 (uFun_2, cFun_empty);   // note: unbound capture function
          MemHolder m22 (uFun_2, cFun_2);
          
          CHECK (m11 == m11);
          CHECK (m12 == m12);
          CHECK (m21 == m21);
          CHECK (m22 == m22);
          CHECK (!(m11 != m11));
          
          CHECK (m11 != m12);
          CHECK (m11 != m21);
          CHECK (m11 != m22);
          CHECK (m12 != m11);
          CHECK (m12 != m21);
          CHECK (m12 != m22);
          CHECK (m21 != m11);
          CHECK (m21 != m12);
          CHECK (m21 != m22);
          CHECK (m22 != m11);
          CHECK (m22 != m12);
          CHECK (m22 != m21);
          
          MemHolder m22x (m22); // clone copy
          CHECK (!m22x);
          CHECK (m22 == m22x); // same functions, no state --> equal
          
          m22x.tieCaptureFunc() ('x');   // produce a memento state
          CHECK (!isnil (m22x.getState()));
          
          CHECK (m22 != m22x);
          m22.tieCaptureFunc() ('x'); // get same value into the memento within m22
          CHECK (m22 == m22x);
          
          // document shortcomings on UndoMutation comparisons
          UndoMutation umu11 (m11);
          UndoMutation umu12 (m11);    // note: due to cloning the embedded functor,
          CHECK (umu11 != umu12);     //       our hacked-in comparison operator fails
        }
      
      
      void
      verifyCommandEquality()
        {
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
  
  
}}} // namespace proc::control::test
