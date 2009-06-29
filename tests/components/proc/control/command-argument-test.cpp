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
//#include "lib/meta/typelist.hpp"
//#include "lib/meta/tuple.hpp"
//#include "lib/util.hpp"

//#include <tr1/functional>
//#include <boost/format.hpp>
#include <iostream>
//#include <cstdlib>
#include <string>

//using std::tr1::bind;
//using std::tr1::placeholders::_1;
//using std::tr1::placeholders::_2;
//using std::tr1::function;
//using boost::format;
//using lumiera::Time;
//using util::contains;
using std::string;
//using std::rand;
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
  
  
  
  
  
  
  namespace {
  
    int testVal=0;  ///< used to verify the effect of testFunc ////////////////////TODO
  
  }
  
  
  
  /***************************************************************************
   * @test storing and retrieving command arguments of various types.
   *       
   * @see  control::CommandArgumentHolder
   * @see  command-basic-test.hpp
   */
  class CommandArgument_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          UNIMPLEMENTED ("create various argument tuples and re-access their contents");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandArgument_test, "unit controller");
  
  
}} // namespace control::test
