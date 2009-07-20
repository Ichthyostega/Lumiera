/*
  CommandArgument(Test)  -  checking storage of specifically typed command arguments
 
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
//#include "proc/asset/media.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/edl.hpp"
//#include "proc/mobject/session/testclip.hpp"
//#include "proc/mobject/test-dummy-mobject.hpp"
//#include "lib/p.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/placement-index.hpp"
//#include "proc/mobject/explicitplacement.hpp"
#include "proc/control/command-argument-holder.hpp"
#include "lib/scoped-ptrvect.hpp"
#include "lib/lumitime-fmt.hpp"
//#include "lib/meta/typelist.hpp"
//#include "lib/meta/tuple.hpp"
#include "lib/util.hpp"

//#include <tr1/functional>
#include <boost/format.hpp>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>

//using std::tr1::bind;
//using std::tr1::placeholders::_1;
//using std::tr1::placeholders::_2;
//using std::tr1::function;
using util::isnil;
//using util::and_all;
using util::for_each;
using boost::format;
using lumiera::Time;
//using util::contains;
using std::string;
using std::rand;
using std::ostream;
using std::ostringstream;
using std::cout;
using std::endl;


namespace control {
namespace test    {
  
  using lib::test::showSizeof;
  using lib::test::randTime;
  
//  using session::test::TestClip;
//  using lumiera::P;
//  using namespace lumiera::typelist;
//  using lumiera::typelist::Tuple;
  
//  using control::CmdClosure;
  
  
  
  
  
  
  namespace { // test helpers
    
    ostringstream protocol;     ///< used to verify the test function calls
    
    
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
       
       friend ostream&
       operator<< (ostream& out, const Tracker& tra)
         {
           return out << tra.element_;
         }
       
       operator string()  const { return element_; }
      };
    
    template<typename TY>
    int Tracker<TY>::instanceCnt (0);
    
    
    
    void
    doIt (Tracker<Time> time, Tracker<string> str, int rand)
      {
        static format fmt ("doIt( Time=%s \"%s\" rand=%2d )");
        protocol << fmt % *time % *str % rand;
      }
    
    Tracker<string>
    captureState (Tracker<Time>, Tracker<string> xstr, int)
      {
        return protocol.str() + *xstr;
      }
    
    void
    undoIt (Tracker<Time> time, Tracker<string>, int, Tracker<string> memento)
      {
        protocol << "undoIt(time="<<time<<")---state-was-:"<< *memento;
      }
    
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
    
    
  } // test-helper implementation
  
  
  
  
  
  typedef lib::ScopedPtrVect<CmdClosure> ArgTuples;
  
  /***************************************************************************
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
          Tracker<Time>::instanceCnt = 0;
          Tracker<string>::instanceCnt = 0;
          
          createTuples (testTuples);
          checkArgumentComparison ();
          serialiseArgTuples (testTuples);
          testTuples.clear();
          
//        simulateCmdLifecycle();
          
          ASSERT (0 == Tracker<Time>::instanceCnt);
          ASSERT (0 == Tracker<string>::instanceCnt);
        }
      
      typedef Tracker<Time> TTime;
      typedef Tracker<string> Tstr;
      typedef struct{ int i[5]; } Sint5;
          
      
      /** @test create various argument tuples and re-access their contents */
      void
      createTuples (ArgTuples& tup)
        {
          typedef ArgumentHolder<void(),         bool>  A1;
          typedef ArgumentHolder<void(int),      void*> A2;
          typedef ArgumentHolder<void(int,Time), int>   A3;
          typedef ArgumentHolder<void(int,Time), Sint5> A4;
          
          typedef ArgumentHolder<void(TTime,Tstr,int), Tstr>  A5;
          
          
          A1* arg1 = new A1(); tup.manage (arg1);
          A2* arg2 = new A2(); tup.manage (arg2);
          A3* arg3 = new A3(); tup.manage (arg3);
          A4* arg4 = new A4(); tup.manage (arg4);
          A5* arg5 = new A5(); tup.manage (arg5);
          
          ASSERT (isnil (*arg1));
          ASSERT (isnil (*arg2));
          ASSERT (isnil (*arg3));
          ASSERT (isnil (*arg4));
          ASSERT (isnil (*arg5));
          
          for_each (tup, showIt);
          
          arg1->bind ();
          arg2->bind (rand() % 10);
          arg3->bind (rand() % 10, randTime());
          arg4->bind (rand() % 10, randTime());
          
          arg5->bind (TTime (randTime()), Tstr("glorious"), rand() % 25);
          
          ASSERT (!arg5->canUndo());
          
          arg5->tie(undoIt, captureState)
                .tieCaptureFunc()                         // bind capturing function to memento storage,
                  (TTime(), Tstr("destruction"), 11);    //  then invoke the bound capturing mechanism 
          
          ASSERT (arg5->canUndo());
          ASSERT (*arg5->memento() == "destruction");
          
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
          ASSERT (one == two);               // empty, identically typed argument holders -->equal
          
          one.tie(dummyU,dummyC)
              .tieCaptureFunc()(1,9);
          ASSERT (one != two);               // now one contains captured UNDO state
          
          two.tie(dummyU,dummyC)
              .tieCaptureFunc()(1,9);
          two.memento() = one.memento();     // put the same UNDO state in both
          ASSERT (one == two);               // ...makes them equal again 
          
          one.bind (1,2);                    // verify argument tuple comparison
          ASSERT (one != two);
          ASSERT (two != one);
          ASSERT (!isnil (one));
          ASSERT ( isnil (two));
          
          two.bind (3,4);
          ASSERT (!isnil (two));
          ASSERT (one != two);
          ASSERT (two != one);
          
          one.bind (1,4);
          ASSERT (!isnil (one));
          ASSERT (one != two);
          ASSERT (two != one);
          
          one.bind (3,4);
          ASSERT (!isnil (one));
          ASSERT (one == two);
          ASSERT (two == one);
          two.memento() = 12345;
          ASSERT (!isnil (two));
          ASSERT (one != two);
          ASSERT (two != one);
        }
      
      
#if false ////////////////////////////////////////////////////////////////////////////TODO.....          
      /** @test simulate a complete command lifecycle with regards to
       *  the storage handling of the command parameters and state memento.
       */
      void
      simulateCmdLifecycle()
        {
          typedef void SIG_do(Tracker<Time>, Tracker<string>, int);
          typedef ArgumentHolder<SIG_do, Tracker<string> > Args;
          typedef MementoTie<SIG_do, Tracker<string> > MemHolder;
          
          Args args;
          ASSERT (isnil (args));
          cout << showSizeof(args) << endl;
          
          args.bind (randTime(), "Lumiera rocks". rand() % 100);
          ASSERT (!isnil (args));
          cout << args << endl;
          
          ASSERT (!args.canUndo());
          ASSERT (isnil (args.memento()));
          MemHolder& memHolder = args.tie(undoIt,captureState);
          ASSERT (!memHolder);   // no stored memento.... 
          
          function<SIG_do> doItFun = doIt;
          function<SIG_do> undoFun = memHolder.tieUndoFunc();
          function<SIG_do> captFun = memHolder.tieCaptureFunc();
          
          CmdFunctor bound_doItFun = args.bindArguments (CmdFunctor(doItFun));
          CmdFunctor bound_undoFun = args.bindArguments (CmdFunctor(undoFun));
          CmdFunctor bound_captFun = args.bindArguments (CmdFunctor(captFun));
          
          protocol.seekp(0);
          protocol << "START...";
          
          bound_captFun();
          cout << "captured state: " << args.memento() << endl;
          ASSERT (memHolder);
          ASSERT (!isnil (args.memento()));
          ASSERT (args.canUndo());
          
          bound_doItFun();
          cout << protocol.str() << endl;
          bound_undoFun();
          cout << protocol.str() << endl;
          
          // Commands can serve as prototype to be copied....
          Args argsCopy (args);
          bound_captFun();
          protocol.seekp(0);
          protocol << "RESET...";
          
          args.bind (123456, "unbelievable", rand() %100);
          cout << "original: " << args << endl;
          cout << "copied  : " << argsCopy << endl;
          
          bound_undoFun();
          cout << protocol.str() << endl;
        }
#endif ////////////////////////////////////////////////////////////////////////////TODO.....          
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (CommandArgument_test, "unit controller");
  
  
}} // namespace control::test
