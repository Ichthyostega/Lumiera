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
#include "lib/test/test-helper.hpp"
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
#include "lib/lumitime.hpp"
//#include "lib/util.hpp"

#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/generator.hpp"


#include <tr1/functional>
//#include <boost/format.hpp>
#include <iostream>
#include <cstdlib>
#include <string>

using std::tr1::bind;
//using std::tr1::placeholders::_1;
//using std::tr1::placeholders::_2;
using std::tr1::function;
//using boost::format;
using lumiera::Time;
//using util::contains;
using std::string;
using std::rand;
using std::cout;
using std::endl;


namespace control {
namespace test    {

  using lib::test::showSizeof;

//  using session::test::TestClip;
  using lumiera::P;
  
  
  //using lumiera::typelist::BuildTupleAccessor;
  
  
  
  
  
  
  /////////////////////////////
  /////////////////////////////
  
  //////////////////////////// start of the actual Test....
  
  
  
  namespace command1 {
    void
    operate (P<Time> dummyObj, int randVal)
    {
      *dummyObj += Time(randVal);
    }
    
    Time
    capture (P<Time> dummyObj, int)
    {
      return *dummyObj;
    }
    
    void
    undoIt (P<Time> dummyObj, int, Time oldVal)
    {
      *dummyObj = oldVal;
    }
  
  }
  
  
  
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
          P<Time> obj (new Time(5));
          int randVal ((rand() % 10) - 5);
          
          CommandDef ("test.command1")
              .operation (command1::operate)
              .captureUndo (command1::capture)
              .undoOperation (command1::undoIt)
              .bind (obj, randVal)
              ;
          
          
          Command& ourCmd = Command::get("test.command1");
          
          // invoke the command
          ASSERT (*obj == Time(5));
          ourCmd();
          ASSERT (*obj == Time(5) + Time(randVal));
          
          // undo the effect of the command
          ourCmd.undo();
          ASSERT (*obj == Time(5));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandBasic_test, "unit controller");
      
      
}} // namespace control::test
