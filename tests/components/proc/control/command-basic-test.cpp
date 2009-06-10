/*
  CommandBasic(Test)  -  checking simple ProcDispatcher command definition and execution
 
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
//#include "proc/asset/media.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/edl.hpp"
//#include "proc/mobject/session/testclip.hpp"
//#include "proc/mobject/test-dummy-mobject.hpp"
#include "lib/p.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/placement-index.hpp"
//#include "proc/mobject/explicitplacement.hpp"
#include "proc/control/command-def.hpp"
//#include "lib/util.hpp"

#include <tr1/functional>
//#include <boost/format.hpp>
//#include <iostream>
#include <string>

//using std::tr1::bind;
using std::tr1::function;
//using boost::format;
//using lumiera::Time;
//using util::contains;
using std::string;
//using std::cout;


namespace control {
namespace test    {

//  using session::test::TestClip;
  using lib::P;

  ////////////////////////////////////////////TODO braindump
  
  template<typename SIG, typename MEM>
  struct BuildUndoCapturing_Signature;
  
  template<typename ARG, typename MEM>
  struct BuildUndoCapturing_Signature<function(ARG), MEM>
    {
      typedef function<MEM(ARG)> type;
    };
  
  template<typename SIG, typename MEM>
  struct BuildUndoOperation_Signature
    {
      typedef function<void(ExtendedArgs)> type;
    };
  
  class CommDef
    {
      Symbol id_;
      
      template<typename SIG, typename MEM>
      struct UndoDefinition
        {
          UndoDefinition
          undoOperation (typename BuildUndoOperation_Signature<SIG,MEM>::type how_to_Undo)
            {
              
            }
          
        };
      
      template<typename SIG>
      struct BasicDefinition
        {
          template<typename MEM>
          UndoDefinition<SIG,MEM>
          captureUndo (typename BuildUndoCapturing_Signature<function<SIG>,MEM>::type how_to_capture_UndoState)
            {
              
            }
          
        };
      
    public:
      CommDef (Symbol cmdID)
        : id_(cmdID)
        { }
      
      template<typename SIG>
      BasicDefinition<SIG>
      operation (function<SIG> operation_to_define)
        {
          
        }
    };
  
  
  
  
  /////////////////////////////
  /////////////////////////////

/*  
    bind: opFunc(a,b,c) -> op(void)
    
    curry(opFunc) (a) (b) (c)
    
    pAppl(func, x) ->  func2 (b, c)
    
    return bind( recursion(), param)
     
*/  
  namespace command1 {
    void
    command1_do (P<long> dummyObj, int randVal)
    {
      *dummyObj += randVal;
    }
    
    long
    command1_cap (P<long> dummyObj, int)
    {
      return *dummyObj;
    }
    
    void
    command1_undo (P<long> dummyObj, int, long oldVal)
    {
      *dummyObj = oldVal;
    }
  
  }
  
  ////////////////////////////////////////////TODO braindump
  
  
  /***************************************************************************
   * @test basic usage of the Proc-Layer command dispatch system.
   *       Shows how to define a simple command inline and another
   *       simple command as dedicated class. Finally triggers
   *       execution of both commands and verifies the command
   *       action has been invoked.
   *       
   * @todo currently rather a scrapbook for trying out first ideas on the command system !!!!!!!!!!!!!!!
   *       
   * @see  control::Command
   * @see  control::CommandDef
   * @see  mobject::ProcDispatcher
   */
  class CommandBasic_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          /////////////////////////////////TODO
        }
      
      void
      defineCommands ()
        {
          CommDef ("test.command1")
              .operation (command1_do)
              .captureUndo (command1_cap)
              .undoOperation (command1_undo)
              .bind (obj, randVal)
              ;
        }
      
      void
      checkExecution ()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandBasic_test, "unit controller");
      
      
}} // namespace control::test
