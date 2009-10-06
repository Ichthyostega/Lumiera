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
#include "proc/control/command-def.hpp"
#include "lib/lumitime.hpp"
#include "lib/p.hpp"
//#include "lib/util.hpp"

#include <cstdlib>

using lumiera::Time;
//using util::contains;
using std::rand;


namespace control {
namespace test    {
  
  using lumiera::P;
  
  
  
  
  
  namespace  { // functions to be invoked through the command system
  
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
   *       Shows how to define a simple command inline and how to
   *       trigger execution and UNDO. Verifies the command action
   *       takes place and is reverted again by the UNDO function.
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
              .operation (operate)
              .captureUndo (capture)
              .undoOperation (undoIt)
              .bind (obj, randVal)
              ;
          
          
          Command ourCmd = Command::get("test.command1");
          
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
