/*
  CommandEquality(Test)  -  verify equality comparisons on command's subsystems
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "proc/control/command-def.hpp"
#include "proc/control/command-mutation.hpp"
#include "proc/control/argument-erasure.hpp"
#include "proc/control/command-argument-holder.hpp"
#include "proc/control/memento-tie.hpp"
#include "lib/meta/tuple.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <tr1/functional>
#include <iostream>
#include <string>

namespace control {
namespace test    {
  
  
  
  using namespace lumiera::typelist;
  using lib::Symbol;
  using util::isnil;
  using util::isSameObject;
  
  using std::tr1::function;
  using std::tr1::bind;
  using std::string;
  using std::cout;
  using std::endl;
  
  
  
  namespace {
    
    string check_;
    
    Symbol COMMAND1 ("test.equalityCommand1");
    Symbol COMMAND2 ("test.equalityCommand2");
    
    const string MARK_1 ("|_1_");
    const string MARK_2 ("|_2_");
    
    void oper_1 (char par)             { check_ += MARK_1 + par; }
    void oper_2 (char par)             { check_ += MARK_2 + par; }
    
    string capt_1 (char par)           { return MARK_1 + "|"+par+"|"; }
    string capt_2 (char par)           { return MARK_2 + "|"+par+"|"; }
    
    void undo_1 (char par, string mem) { check_ = mem + MARK_1 + par + "|";}
    void undo_2 (char par, string mem) { check_ = mem + MARK_2 + par + "|";}
    
    
    typedef void   Sig_oper(char);
    typedef string Sig_capt(char);
    typedef void   Sig_undo(char,string);
    
    typedef Tuple<Types<char> > ArgTuple;
    typedef Closure<Sig_oper> ArgHolder;
    typedef MementoTie<Sig_oper, string> MemHolder;
    
    
    struct Testframe ///< test data set
      {
        char param;
        
        function<Sig_oper> o_Fun;
        function<Sig_capt> c_Fun;
        function<Sig_undo> u_Fun;
      };
    
    
    Testframe data1 = {'a', &oper_1, &capt_1, &undo_1};
    Testframe data2 = {'z', &oper_2, &capt_2, &undo_2};
    
    Testframe nullD;
  }
  
  
  
  /*****************************************************************************************
   * @test cover command equality detection. Two commands are deemed equivalent, if they
   *       - build on the same Mutation functors
   *       - are either both incomplete or
   *       - are bound to equivalent arguments
   *       - hold equivalent undo state (memento)
   * To conduct this test, we set up two sets of functions, and then build both complete
   * command objects and command implementation facilities based on them.
   *   
   * The hidden problem with those comparisons is the equivalence of function objects,
   * which is required by TR1, but refused to implement by libboost.
   *       
   * @see  control::Command
   * @see  control::CmdClosure
   * @see  control::Mutation
   * @see  control::UndoMutation
   * @see  control::MementoTie
   * @see  control::CommandImpl
   * @see  command-basic-test.hpp
   */
  class CommandEquality_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          ASSERT (&oper_1 != &oper_2);
          ASSERT (&capt_1 != &capt_2);
          ASSERT (&undo_1 != &undo_2);
          
          verifyMutationEquality();
          verifyMementoEquality();
          verifyClosureEquality();
          verifyCommandEquality();
        }
      
      
      
      void
      verifyMutationEquality()
        {
          Mutation mut1 (data1.o_Fun);
          Mutation muti (data1.o_Fun);
          Mutation mut2 (data2.o_Fun);
          ASSERT (mut1 == mut1);
          ASSERT (mut1 == muti);
          ASSERT (muti == mut1);
          ASSERT (mut1 != mut2);
          ASSERT (mut2 != mut1);
          ASSERT (muti != mut2);
          ASSERT (mut2 != muti);
          
          Mutation umu (nullD.o_Fun);
          ASSERT (mut1 != umu);
          
          Mutation omu (nullD.u_Fun);
          ASSERT (omu != umu);
          ASSERT (omu != muti);
          
          omu = Mutation(data1.u_Fun);
          ASSERT (omu != muti);
        }
      
      
      void
      verifyClosureEquality()
        {
          ArgHolder a1 (tuple::make (data1.param));
          ArgHolder a2 (tuple::make (data2.param));
          ASSERT (a1 == a1);
          ASSERT (a1 != a2);
          ASSERT (a2 != a1);
          
          TypedArguments<ArgTuple> newArgs (tuple::make (data2.param));
          a1.bindArguments(newArgs);
          ASSERT (a1 == a2);
          ASSERT (a2 == a1);
          
          typedef ArgumentHolder<Sig_oper,string> AHImpl;
          AHImpl abuff1;
          AHImpl abuff2;
          ASSERT (abuff1 == abuff2);
          abuff1.bindArguments(newArgs);
          ASSERT (abuff1 != abuff2);
          abuff2.bindArguments(newArgs);
          ASSERT (abuff1 == abuff2);
          UndoMutation umu1 (abuff1.tie (data1.u_Fun, data1.c_Fun));
          ASSERT (abuff1 == abuff2);                                 // not detected, as the new memento holder isn't valid yet
          
          UndoMutation umu2 (abuff1.tie (data1.u_Fun, data2.c_Fun)); // note: using different capture function!
          ASSERT (abuff1 == abuff2);
          
          umu1.captureState(a1);
          umu2.captureState(a1);
          ASSERT (abuff1 != abuff2); // and now the different state (due to the differing capture function) is detected
          
          umu2 = UndoMutation(abuff1.tie (data1.u_Fun, data1.c_Fun)); // re-bind, now using the "right" capture function
          ASSERT (abuff1 != abuff2);
          umu2.captureState(a1);    
          ASSERT (abuff1 == abuff2); // same functions, same memento state
        }
      
      
      void
      verifyMementoEquality()
        {
          MemHolder m11 (data1.u_Fun, data1.c_Fun);
          MemHolder m12 (data1.u_Fun, data2.c_Fun);
          MemHolder m21 (data2.u_Fun, nullD.c_Fun); // note: unbound capture function
          MemHolder m22 (data2.u_Fun, data2.c_Fun);
          
          ASSERT ( (m11 == m11));
          ASSERT (!(m11 != m11));
          
          ASSERT (m11 != m12);
          ASSERT (m11 != m21);
          ASSERT (m11 != m22);
          ASSERT (m12 != m11);
          ASSERT (m12 != m21);
          ASSERT (m12 != m22);
          ASSERT (m21 != m11);
          ASSERT (m21 != m12);
          ASSERT (m21 != m22);
          ASSERT (m22 != m11);
          ASSERT (m22 != m12);
          ASSERT (m22 != m21);
         
          MemHolder m22x (m22); // clone copy
          ASSERT (!m22x);
          ASSERT (m22 == m22x); // same functions, no state --> equal
          
          m22x.tieCaptureFunc() ('x');   // produce a memento state
          ASSERT (!isnil (m22x.getState()));
          
          ASSERT (m22 != m22x);
          m22.tieCaptureFunc() ('x'); // get same value into the memento within m22
          ASSERT (m22 == m22x);
          
          // document shortcomings on UndoMutation comparisons
          UndoMutation umu11 (m11);
          UndoMutation umu12 (m12);
          UndoMutation umu21 (m21);
          UndoMutation umu22 (m22);
          ASSERT (m11 == m12);       // note: differing capture function not detected
          ASSERT (m11 != m21);
          ASSERT (m11 != m22);
          ASSERT (m12 != m21);
          ASSERT (m12 != m22);
          ASSERT (m21 == m22);       // note
        }
      
      
      void
      verifyCommandEquality()
        {
          CommandDef (COMMAND1)
            .operation (data1.o_Fun)
            .captureUndo (data1.c_Fun)
            .undoOperation (data1.u_Fun)
            ;
          CommandDef (COMMAND2)
            .operation (data2.o_Fun)
            .captureUndo (data2.c_Fun)
            .undoOperation (data2.u_Fun)
            ;
          
          Command c1 = Command::get(COMMAND1); 
          Command c2 = Command::get(COMMAND2); 
          ASSERT (c1 == c1);
          ASSERT (c1 != c2);
          ASSERT (c2 != c1);
          
          Command cx = c1;
          ASSERT (c1 == cx);
          ASSERT (cx == c1);
          ASSERT (!isSameObject (c1, c2));
          
          // verify equality matches behaviour
          string protocol1 = execCommand(c1); 
          string protocolX = execCommand(cx); 
          string protocol2 = execCommand(c2);
          
          ASSERT (protocol1 == protocolX);
          ASSERT (protocol1 != protocol2);
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
  
  
}} // namespace control::test
