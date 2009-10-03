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
#include "proc/control/command-mutation.hpp"
#include "proc/control/command-argument-holder.hpp"
#include "proc/control/memento-tie.hpp"
#include "lib/meta/tuple.hpp"

#include <tr1/functional>
#include <string>

namespace control {
namespace test    {
  
  
  
  using namespace lumiera::typelist;
  
  using std::tr1::function;
  using std::tr1::bind;
  using std::string;
  
  
  namespace {
    
    string check_;
    
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
    typedef MementoTie<Sig_oper, string> MemHolder;
    typedef ArgumentHolder<Sig_oper,string> ArgHolder;
    
    
    struct Testframe ///< test data set
      {
        char param;
        
        function<Sig_oper> o_Fun;
        function<Sig_capt> c_Fun;
        function<Sig_undo> u_Fun;
      };
    
    
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
          
          Testframe data1 = {'a', &oper_1, &capt_1, &undo_1};
          Testframe data2 = {'z', &oper_2, &capt_2, &undo_2};
          
          verifyMutationEquality (data1,data2);
          verifyMementoEquality  (data1,data2);
          verifyClosureEquality  (data1,data2);
          verifyCommandEquality  (data1,data2);
        }
      
      
      
      void
      verifyMutationEquality (Testframe d1, Testframe d2)
        {
          UNIMPLEMENTED ("cover Mutation equality test");
        }
      
      
      
      void
      verifyMementoEquality (Testframe d1, Testframe d2)
        {
          UNIMPLEMENTED ("cover Memento equality test");
          
#if false //////////////////////////////////////////////////////////////////////////////////////////////copied just for reference          
          function<void(short,int)> u1_fun;             // deliberately unbound
          function<void(short,int)> u2_fun  = undo;
          function< int(short)>     c1_fun;
          function< int(short)>     c2_fun  = capture;
          
          MemHolder m11 (u1_fun, c1_fun);
          MemHolder m12 (u1_fun, c2_fun);
          MemHolder m21 (u2_fun, c1_fun);
          MemHolder m22 (u2_fun, c2_fun);
          
          ASSERT (!m11 && !m12 && !m21 && !m22);
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
          
          testVal = 0;
          m22x.tieCaptureFunc() (1 + (rand() % 9));   // produce a random memento value != 0
          ASSERT (0 < m22x.getState());
          
          ASSERT (m22 != m22x);
          m22.tieCaptureFunc() (m22x.getState()); // get the same value into the memento within m22
          ASSERT (m22 == m22x);
#endif          
        }
      
      
      void
      verifyClosureEquality (Testframe d1, Testframe d2)
        {
          UNIMPLEMENTED ("cover Closure equality test");
        }
      
      
      void
      verifyCommandEquality (Testframe d1, Testframe d2)
        {
          UNIMPLEMENTED ("cover complete Command equality test");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandEquality_test, "function controller");
  
  
}} // namespace control::test
