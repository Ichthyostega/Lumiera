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
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/placement-index.hpp"
//#include "proc/mobject/explicitplacement.hpp"
#include "proc/control/command-def.hpp"
//#include "lib/util.hpp"

//#include <boost/format.hpp>
//#include <iostream>
#include <string>

//using boost::format;
//using lumiera::Time;
//using util::contains;
using std::string;
//using std::cout;


namespace control {
namespace test    {

//  using session::test::TestClip;

  namespace command1 {
  
    ////////////////////////////////////////////TODO braindump
    ////////////////////////////////////////////TODO braindump
  
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
      typedef shared_ptr<PlacementIndex> PIdx;
      
      virtual void
      run (Arg) 
        {
          /////////////////////////////////TODO
        }
      
      void
      checkExecution ()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandBasic_test, "unit controller");
      
      
}} // namespace control::test
