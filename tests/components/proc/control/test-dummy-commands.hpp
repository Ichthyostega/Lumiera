/*
  TEST-DUMMY-COMMANDS.hpp  -  dummy function used to build test commands
 
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
 
*/


/** @file test-dummy-commands.hpp
 ** Some dummy command functions used for building unit test cases. 
 ** Any of these functions comes in triples of operation function, undo state
 ** capturing function and und function. They are placed into a nested test
 ** namespace, together with some global variables used as a backdoor to
 ** verify the effect of calling these functions.
 **
 ** @see command-use1-test.cpp
 ** @see CommandBasic_test simple complete command definition example
 **
 */



#ifndef COMMAND_TEST_DUMMY_COMMANDS_H
#define COMMAND_TEST_DUMMY_COMMANDS_H

//#include "pre.hpp"
//#include "lib/test/run.hpp"
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
//#include "proc/control/command-def.hpp"
//#include "lib/lumitime.hpp"
//#include "lib/util.hpp"


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

  
  
  
  namespace command1 { ///< test command just adding a given value
    
    long checksum_ = 0;
      
    void
    operate (int someVal)
    {
      checksum_ += someVal;
    }
    
    long
    capture (int)
    {
      return checksum_;
    }
    
    void
    undoIt (int, long oldVal)
    {
      checksum_ = oldVal;
    }
  
  }
  
      
}} // namespace control::test
