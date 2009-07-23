/*
  CommandUse1(Test)  -  usage scenario 1
 
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
//#include "lib/test/test-helper.hpp"
//#include "proc/asset/media.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/edl.hpp"
//#include "proc/mobject/session/testclip.hpp"
//#include "proc/mobject/test-dummy-mobject.hpp"
//#include "lib/p.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/placement-index.hpp"
//#include "proc/mobject/explicitplacement.hpp"
#include "proc/control/command-def.hpp"
//#include "lib/lumitime.hpp"
#include "lib/util.hpp"

#include "proc/control/test-dummy-commands.hpp"

//#include <boost/format.hpp>
//#include <iostream>
//#include <cstdlib>
//#include <string>

//using boost::format;
//using lumiera::Time;
//using util::contains;
//using std::string;
//using std::rand;
//using std::cout;
//using std::endl;


namespace control {
namespace test    {

//  using lib::test::showSizeof;
  using util::isSameObject;

//  using session::test::TestClip;
//  using lumiera::P;
  
  
  //using lumiera::typelist::BuildTupleAccessor;
  
  
  
  
  
  
  /***************************************************************************
   * @test command usage scenario 1: defining commands in various ways.
   *       
   * @todo WIP
   */
  class CommandUse1_test : public Test
    {
    
      int randVal;
      int shuffle() { return randVal = 10 + (rand() % 40); }
      
      
      
      virtual void
      run (Arg) 
        {
          command1::checksum_ = 0;
          allInOneStep();
          definePrototype();
          usePrototype();
          
          ASSERT (0 == command1::checksum_);
        }
      
      
      void
      allInOneStep()
        {
          
          CommandDef ("test.command1.1")
              .operation (command1::operate)
              .captureUndo (command1::capture)
              .undoOperation (command1::undoIt)
              .bind (shuffle())
              .exec()
              ;
          
          ASSERT (randVal == checksum_);
          
          Command::get("test.command1.1").undo();
          ASSERT ( 0 == command1::checksum_);
        }
      
      
      void
      definePrototype()
        {
          CommandDef ("test.command1.2")
              .operation (command1::operate)
              .captureUndo (command1::capture)
              .undoOperation (command1::undoIt)
              .bind (shuffle())
              ;
          
          
          ASSERT (Command::get("test.command1.2").canExec());
        }
      
      
      void
      usePrototype()
        {
          Command c1 = Command::get("test.command1.2");
          ASSERT (c1);
          ASSERT (c1.canExec());
          ASSERT (!c1.canUndo());
          
          Command c2 = Command::get("test.command1.2");
          ASSERT (c1);
          ASSERT (c1.canExec());
          ASSERT (!c1.canUndo());
          
          ASSERT (c1 == c2);
          ASSERT (!isSameObject(c1, c2));
          
          ASSERT (0 == command1::checksum_);
          
          c1();
          
          ASSERT (randVal == command1::checksum_);
          ASSERT (c1.canUndo());
          ASSERT (c1 != c2);
          ASSERT (!c2.canUndo());
          
          c2();
          ASSERT (randVal + randVal == command1::checksum_);
          ASSERT (c2.canUndo());
          ASSERT (c1 != c2);
          
          c1.undo();
          ASSERT (0 == command1::checksum_);
          c2.undo();
          ASSERT (randVal == command1::checksum_);
          
          c2.bind(23);
          c2();
          ASSERT (randVal + 23 == command1::checksum_);
          
          // you should not use a command more than once (but it works...)
          c1();
          ASSERT (randVal + 23 + randVal == command1::checksum_);
          c1.undo();
          ASSERT (randVal + 23 == command1::checksum_);
          // note we've overwritten the previous undo state
          // and get the sate captured on the second invocation
          
          c2.undo()
          ASSERT (randVal == command1::checksum_);
          c1.undo();
          ASSERT (randVal + 23 == command1::checksum_);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandUse1_test, "function controller");
      
      
}} // namespace control::test
