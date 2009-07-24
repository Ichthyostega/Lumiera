/*
  CommandUse2(Test)  -  usage aspects II
 
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
#include "proc/control/command-def.hpp"
//#include "lib/lumitime.hpp"
#include "lib/util.hpp"

#include "proc/control/test-dummy-commands.hpp"

#include <tr1/functional>
#include <boost/ref.hpp>
#include <boost/format.hpp>
//#include <iostream>
//#include <cstdlib>
#include <string>


namespace control {
namespace test    {


  using boost::format;
  using boost::str;
  //using lumiera::Time;
  //using util::contains;
  using std::tr1::function;
  using std::tr1::bind;
  using std::string;
  //using std::rand;
  //using std::cout;
  //using std::endl;
//  using lib::test::showSizeof;
//  using util::isSameObject;
  using util::contains;
  using boost::ref;

//  using session::test::TestClip;
//  using lumiera::P;
  
  
  //using lumiera::typelist::BuildTupleAccessor;
  using lumiera::error::LUMIERA_ERROR_EXTERNAL;
  
  
  
  
  
  /***************************************************************************
   * @test command usage aspects II: patterns of invoking commands.
   *       
   * @see Command
   * @see command-basic-test.cpp (simple usage example)
   */
  class CommandUse2_test : public Test
    {
    
      int randVal_;
      
      string randomTxt()
        {
          format fmt ("invoked( %2d )");
          
          randVal_ = rand() % 100;
          return str (fmt % randVal_);
        }
      
      bool blowUp_ = false;
      
      
      virtual void
      run (Arg) 
        {
          command2::check_.seekp(0);
          uint cnt_defs = Command::definition_count();
          uint cnt_inst = Command::instance_count();
          
          function<string()> randFun = bind (&CommandUse2_test::randomTxt, this);
          
          // prepare a command definition (prototype)
          CommandDef ("test.command2")
              .operation (command2::operate)
              .captureUndo (command2::capture)
              .undoOperation (command2::undoIt)
              .bind (randFun, ref(blowUp_));
          
              // note: blowUp_ is bound via reference_wrapper,
              //       so we can pull the trigger to provoke an exception
          blowUp_ = false;
          
          
          check_defaultHandlingPattern();
          check_ThrowOnError();
          
          
          Command::undef ("test.command2");
          Command::undef ("test.command2.1");
          ASSERT (cnt_defs == Command::definition_count());
          ASSERT (cnt_inst == Command::instance_count());
        }
      
      
      
      void
      check_defaultHandlingPattern()
        {
          Command com = Command::get("test.command2");
          
          ASSERT (!contains (command2::check_, "invoked"));
          
          bool res = com();
          
          ASSERT (res);
          ASSERT (contains (command2::check_, "invoked"));
          ASSERT (contains (command2::check_, randVal_));
          
          res = com.undo();
          ASSERT (res);        // UNDO invoked successfully
          ASSERT (!contains (command2::check_, randVal_));
          ASSERT (contains (command2::check_, "UNDO"));
          
          blowUp_ = true;
          string current = command2::check_.str();
          
          res = com();
          ASSERT (!res);       // not executed successfully (exception thrown)
          ASSERT (command2::check_.str() == current);
          ASSERT (LUMIERA_ERROR_EXTERNAL == lumiera_error());
          
          res = com.undo();
          ASSERT (!res);       // UNDO failed (exception thrown)
          ASSERT (command2::check_.str() == current);
          ASSERT (LUMIERA_ERROR_EXTERNAL == lumiera_error());
          
          blowUp_ = false;
        }
      
      
      
      void
      check_ThrowOnError()
        {
          Command com = Command::get("test.command2");
          
          blowUp_ = false;
          com.exec(HandlingPattern::THROW_SYNC);
          ASSERT (contains (command2::check_, randVal_));
          
          blowUp_ = true;
          string current = command2::check_.str();
          VERIFY_ERROR( EXTERNAL, com.exec(HandlingPattern::THROW_SYNC) );
          ASSERT (command2::check_.str() == current);
          
          // we can achieve the same effect,
          // after changing the default HandlingPatern for this command instance
          com.setHandlingPattern(HandlingPattern::THROW_SYNC);
          com.storeDef ("test.command2.1");
          
          Command com2 = Command::get("test.command2.1");
          VERIFY_ERROR( EXTERNAL, com2() );
          ASSERT (command2::check_.str() == current);

          blowUp_ = false;
          com2();
          ASSERT (command2::check_.str() > current);
          ASSERT (contains (command2::check_, randVal_));
          
          com2.undo();
          ASSERT (!contains (command2::check_, randVal_));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandUse2_test, "function controller");
      
      
}} // namespace control::test
