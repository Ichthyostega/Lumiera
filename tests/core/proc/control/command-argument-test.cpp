/*
  CommandArgument(Test)  -  checking storage of specifically typed command arguments

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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "proc/control/command-argument-holder.hpp"
#include "lib/scoped-ptrvect.hpp"
#include "lib/time/diagnostics.hpp"
#include "lib/meta/tuple.hpp"
#include "lib/format-string.hpp"
#include "lib/util-foreach.hpp"
#include "lib/util.hpp"

#include <functional>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>

using util::_Fmt;
using util::isnil;
using util::for_each;
using lib::time::Time;
using lib::time::TimeVar;
using lib::time::TimeValue;
using std::string;
using std::ostream;
using std::ostringstream;
using std::rand;
using std::cout;
using std::endl;


namespace proc {
namespace control {
namespace test    {
  
  using lib::test::showSizeof;
  using lib::test::randTime;
  using namespace lib::meta;
  
  
  
  
  
  namespace { // test helpers
    
    ostringstream protocol;     ///< used to verify the test function calls
    
    
    /**
     * watching the instance creation
     * of some parameter values
     */
    template<typename TY>
    struct Tracker
      {
        TY element_;
        static int instanceCnt;
        
        Tracker (TY init = TY())     : element_(init)         { ++instanceCnt; }
        Tracker (Tracker const& otr) : element_(otr.element_) { ++instanceCnt; }
       ~Tracker()                                             { --instanceCnt; }
       
        TY&
        operator* ()
          {
            return element_;
          }
        
        operator string()  const { return element_; }
        
        friend ostream&
        operator<< (ostream& out, const Tracker& tra)
          {
            return out << tra.element_;
          }
        
        friend bool
        operator== (Tracker const& tra1, Tracker const& tra2)
          {
            return tra1.element_ == tra2.element_;
          }
      };
    
    template<typename TY>
    int Tracker<TY>::instanceCnt (0);
    
    
    /** Dummy custom memento datatype
     *  @note memento needs to be equality comparable
     */
    struct Sint5
      {
        int i[5];
        
        friend bool
        operator== (Sint5 const& i1, Sint5 const& i2)
          {
            return i1.i == i2.i;
          }
      };
    
    
    
    /* === functions to implement test-"operation" & UNDO === */
    
    void
    doIt (Tracker<TimeVar> time, Tracker<string> str, int rand)
      {
        static _Fmt fmt ("doIt( Time=%s \"%s\" rand=%2d )");
        cout << "invoke operation..." << endl;
        protocol << fmt % *time % *str % rand;
      }
    
    Tracker<string>
    captureState (Tracker<TimeVar>, Tracker<string> xstr, int)
      {
        cout << "capture state..." << endl;
        return protocol.str() + *xstr;
      }
    
    void
    undoIt (Tracker<TimeVar> time, Tracker<string>, int, Tracker<string> memento)
      {
        cout << "undo... memento=" << memento << endl;
        protocol << "undoIt(time="<<time<<")----memento-:"<< *memento;
      }
    
    
    ///  another dummy-UNDO function
    void dummyU (int,int,int) { }
    int  dummyC (int u,int o) { return u + rand() % (o-u+1); }
    
    
    
    void
    showIt (CmdClosure& clo)
      {
        cout << clo << endl;
      }
    
    void
    checkSerialisation (CmdClosure& clo)
      {
        TODO ("implement serialisation/de-serialisation-Check");
        cout << "would be serialised....." << clo << endl;
        
        // serialise, then de-serialise into a new instance and compare both
      }
    
    
    int
    twoRandomDigits()
      {
        return 10 + rand() % 90;
      }
    
    
  } // test-helper implementation
  
  
  
  
  
  
  typedef lib::ScopedPtrVect<CmdClosure> ArgTuples;
  
  /***********************************************************************//**
   * @test Check storage handling of the command parameters and state memento.
   *
   * @see  control::CommandArgumentHolder
   * @see  command-basic-test.hpp
   */
  class CommandArgument_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          ArgTuples testTuples;
          Tracker<TimeVar>::instanceCnt = 0;
          Tracker<string>::instanceCnt = 0;
          
          createTuples (testTuples);
          checkArgumentComparison ();
          serialiseArgTuples (testTuples);
          testTuples.clear();
          
          simulateCmdLifecycle();
          
          // verify all dtors properly called...
          CHECK (0 == Tracker<TimeVar>::instanceCnt);
          CHECK (0 == Tracker<string>::instanceCnt);
        }
      
      
      typedef Tracker<TimeVar> TTime;
      typedef Tracker<string> Tstr;
      
      
      
      
      /** @test create various argument tuples and re-access their contents */
      void
      createTuples (ArgTuples& tup)
        {
          typedef ArgumentHolder<void(),            bool>  A1;
          typedef ArgumentHolder<void(int),         void*> A2;
          typedef ArgumentHolder<void(int,TimeVar), int>   A3;
          typedef ArgumentHolder<void(int,TimeVar), Sint5> A4;
          
          typedef ArgumentHolder<void(TTime,Tstr,int), Tstr>  A5;
          
          
          A1* arg1 = new A1(); tup.manage (arg1);
          A2* arg2 = new A2(); tup.manage (arg2);
          A3* arg3 = new A3(); tup.manage (arg3);
          A4* arg4 = new A4(); tup.manage (arg4);
          A5* arg5 = new A5(); tup.manage (arg5);
          
          CHECK (isnil (*arg1));
          CHECK (isnil (*arg2));
          CHECK (isnil (*arg3));
          CHECK (isnil (*arg4));
          CHECK (isnil (*arg5));
          
          for_each (tup, showIt);
          
          arg1->storeTuple (tuple::makeNullTuple());
          arg2->storeTuple (tuple::make (rand() % 10));
          arg3->storeTuple (tuple::make (rand() % 10, TimeVar(randTime())));
          arg4->storeTuple (tuple::make (rand() % 10, TimeVar(randTime())));
          
          arg5->storeTuple (tuple::make (TTime (randTime()), Tstr("glorious"), twoRandomDigits() ));
          
          CHECK (!arg5->canUndo());
          
          arg5->tie(undoIt, captureState)
                .tieCaptureFunc()                         // bind capturing function to memento storage,
                  (TTime(), Tstr("destruction"), 11);    //  then invoke the bound capturing mechanism
          
          CHECK (arg5->canUndo());
          CHECK (*arg5->memento() == "destruction");
          
          VERIFY_ERROR(MISSING_MEMENTO, arg4->memento().i[3] = 513 );
          
          for_each (tup, showIt);
        }
      
      
      
      /** @test serialise and de-serialise each tuple and check validity
       *  @todo unimplemented, waiting on Serialiser
       */
      void
      serialiseArgTuples (ArgTuples& tup)
        {
          for_each (tup, checkSerialisation);
        }
      
      
      
      /** @test verify the comparison operators */
      void
      checkArgumentComparison ()
        {
          ArgumentHolder<void(int,int), int> one, two;
          CHECK (one == two);               // empty, identically typed argument holders -->equal
          
          one.tie(dummyU,dummyC)
              .tieCaptureFunc()(1,9);
          CHECK (one != two);               // now one contains captured UNDO state
          
          two.tie(dummyU,dummyC)
              .tieCaptureFunc()(1,9);
          two.memento() = one.memento();     // put the same UNDO state in both
          CHECK (one == two);               // ...makes them equal again
          
          one.storeTuple (tuple::make (1,2));
          CHECK (one != two);               // verify argument tuple comparison
          CHECK (two != one);
          CHECK (!isnil (one));
          CHECK ( isnil (two));
          
          two.storeTuple (tuple::make (3,4));
          CHECK (!isnil (two));
          CHECK (one != two);
          CHECK (two != one);
          
          one.storeTuple (tuple::make (1,4));
          CHECK (!isnil (one));
          CHECK (one != two);
          CHECK (two != one);
          
          one.storeTuple (tuple::make (3,4));
          CHECK (!isnil (one));
          CHECK (one == two);
          CHECK (two == one);
          two.memento() = 12345;
          CHECK (!isnil (two));
          CHECK (one != two);
          CHECK (two != one);
        }
      
      
      
      /** @test simulate a complete command lifecycle with regards to the
       *        storage handling of the command parameters and state memento.
       */
      void
      simulateCmdLifecycle()
        {
          typedef void SIG_do(Tracker<TimeVar>, Tracker<string>, int);
          typedef ArgumentHolder<SIG_do, Tracker<string> >   Args;
          typedef MementoTie<SIG_do, Tracker<string> >  MemHolder;
          
          Args args;
          CHECK (isnil (args));
          cout << showSizeof(args) << endl;
          
          // store a set of parameter values, later to be used on invocation
          args.storeTuple (
            tuple::make (TTime(randTime()), Tstr("Lumiera rocks"), twoRandomDigits() ));
          CHECK (!isnil (args));
          cout << args << endl;
          
          CHECK (!args.canUndo());
          VERIFY_ERROR(MISSING_MEMENTO,  args.memento() );
          
          MemHolder& memHolder = args.tie(undoIt,captureState);
          CHECK (!memHolder);   // no stored memento....
          CHECK (!args.canUndo());
          
          function<SIG_do> doItFun = doIt;
          function<SIG_do> undoFun = memHolder.tieUndoFunc();
          function<SIG_do> captFun = memHolder.tieCaptureFunc();
          
          typedef function<void()> OpFun;
          
          // now close all the functions with the stored parameter values...
          OpFun bound_doItFun = std::bind (&CmdClosure::invoke, args, CmdFunctor(doItFun));
          OpFun bound_undoFun = std::bind (&CmdClosure::invoke, args, CmdFunctor(undoFun));
          OpFun bound_captFun = std::bind (&CmdClosure::invoke, args, CmdFunctor(captFun));
          
          protocol.seekp(0);
          protocol << "START...";
          
          bound_captFun();
          cout << "captured state: " << args.memento() << endl;
          CHECK (memHolder);
          CHECK (!isnil (*args.memento()));
          CHECK (args.canUndo());
          cout << args << endl;
          
          bound_doItFun();
          cout << protocol.str() << endl;
          bound_undoFun();
          cout << protocol.str() << endl;
          
          // Commands can serve as prototype to be copied....
          Args argsCopy (args);
          bound_captFun();
          protocol.seekp(0);
          protocol << "RESET...";
          
          args.storeTuple (
            tuple::make (TTime(TimeValue(123456)), Tstr("unbelievable"), twoRandomDigits() ));
          cout << "modified: " << args     << endl;
          cout << "copied  : " << argsCopy << endl;    // holds still the old params & memento
          
          bound_undoFun();
          cout << protocol.str() << endl;
        }
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (CommandArgument_test, "unit controller");
  
  
}}} // namespace proc::control::test
