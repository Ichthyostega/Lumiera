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
#include <strstream>
//#include <cstdlib>
#include <string>

//using std::tr1::bind;
//using std::tr1::placeholders::_1;
//using std::tr1::placeholders::_2;
//using std::tr1::function;
using util::isnil;
using util::and_all;
using boost::format;
using lumiera::Time;
//using util::contains;
using std::string;
//using std::rand;
using std::ostream;
using std::ostrstream;
using std::cout;
using std::endl;


namespace control {
namespace test    {
  
  using lib::test::showSizeof;
  
//  using session::test::TestClip;
//  using lumiera::P;
//  using namespace lumiera::typelist;
//  using lumiera::typelist::Tuple;
  
//  using control::CmdClosure;
  
  
  
  
  
  
  namespace { // test helpers
    
    ostrstream protocol;     ///< used to verify the test function calls
    
    
    template<typename TY>
    struct Tracker
      {
        TY element_;
        static int instanceCnt;
        
        Tracker (TY init = TY())
          : element_(init)
          {
            ++instanceCnt;
          }
        
       ~Tracker()
          {
            --instanceCnt; 
          }
       
       TY&
       operator* () const
         {
           return element_;
         }
       
       friend ostream&
       operator<< (ostream& out, const Tracker& tra)
         {
           return out << element_;
         }
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
    captureState (Tracker<Time> time, Tracker<string> str, int rand)
      {
        return protocol.str();
      }
    
    void
    undoIt (Tracker<Time> time, Tracker<string> str, int rand, Tracker<string> memento)
      {
        protocol << "undoIt(time="<<time<<")---state-was-:"<< *memento;
      }
    
    
    
    void
    showIt (CmdClosure& clo)
      {
        cout << clo << endl;
      }
    
    void
    checkSerialisation (CmdClosure& clo)
      {
        TODO ("implement serialisation/de-serialisation-Check");
        cout << "would be serialised....." << clo;
        
        // serialise, then de-serialise into a new instance and compare both
      }
    
    Time
    randTime ()
      {
        UNIMPLEMENTED ("create a random but not insane Time value");
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
          
          createArgTuples (testTuples);
          checkArgumentComparison ();
          serialiseArgTuples (testTuples);
          
          simulateCmdLifecycle();
          
          ASSERT (0 == Tracker<Time>::instanceCnt);
          ASSERT (0 == Tracker<string>::instanceCnt);
        }
      
      
      /** @test create various argument tuples and re-access their contents */
      void
      createTuples (ArgTuples& tup)
        {
          tup.manage (new ArgumentHolder<void()>);
          tup.manage (new ArgumentHolder<void(int)>);
          tup.manage (new ArgumentHolder<void(int,Time)>);
          tup.manage (new ArgumentHolder<void(int,Time), Time>);
          tup.manage (new ArgumentHolder<void(int,Time), int[5]>);
          
          ASSERT (and_all (tup, isnil));
          
          for_each (tup, showIt);
          
          tup[1].bind (rand() % 20);
          tup[2].bind (rand() % 20, randTime());
          tup[3].bind (rand() % 20, randTime());
          tup[4].bind (rand() % 20, randTime());
          
          tup[1].memento() = 42;
          tup[4].memento()[3] = 513;
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
          ArgumentHolder<void()> one, two;
          ASSERT (one == two);
          
          one.memento() = 5;
          ASSERT (one != two);
          
          ArgumentHolder<void(int,int)> three, four;
          ASSERT (three == four);
          three.bind (1,2);
          ASSERT (three != four);
          ASSERT (four != three);
          ASSERT (!isnil (three));
          ASSERT ( isnil (four));

          four.bind (3,4);
          ASSERT (!isnil (four));
          ASSERT (three != four);
          ASSERT (four != three);
          
          three.bind (3,4);
          ASSERT (!isnil (three));
          ASSERT (three == four);
          ASSERT (four == three);
          four.memento() = 12345;
          ASSERT (!isnil (four));
          ASSERT (three != four);
          ASSERT (four != three);
        }
      
      
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
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (CommandArgument_test, "unit controller");
  
  
}} // namespace control::test
