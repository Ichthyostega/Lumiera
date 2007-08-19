/*
  Suite  -  helper class for running collections of tests
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


//#include <map>
//#include <memory>
//#include <tr1/memory>
//#include <iostream>
//#include <sstream>

#include "common/test/testoption.hpp"
#include "common/test/suite.hpp"
//#include "common/util.hpp"

#include "nobugcfg.h"
#include "common/error.hpp"


namespace test
  {
//  using std::map;
//  using std::auto_ptr;
//  using std::tr1::shared_ptr;

//  using util::isnil;
  
  
  
  /** set up an options parser to use the current commandline.
   *  reconizes the following options
   *  \code
   *  --help
   *  --group <groupID>
   *  \endcode
   */
  TestOption::TestOption (int argc, const char* argv[])
    : vm(), cmdline()
    {
      cmdline.reserve (10);
      parseOptions (argc,argv);
    }
  
  
  /** variant of the ctor taking a "fake" cmdline */
  TestOption::TestOption (string line)
    : vm(), cmdline()
    {
      cmdline.reserve (10);
      const char* fakeArg[3] = {"test", line.c_str() };
//      fakeArg[1] = line.c_str();
      parseOptions (1,fakeArg);
    }
  
  
  /** do the comandline parsing for the ctors */
  void TestOption::parseOptions (int argc, const char* argv[])
    {
      TODO("define options");
      UNIMPLEMENTED("actual commandline parsing!!");
    }
  
  
  /** @return the Tests-Group as given on cmdline, or Suite::ALLGROUP as default
   */
  const string & 
  TestOption::getTestgroup ()
    {
      FIXME("actual commandline parsing!!");
      return Suite::ALLGROUP;
    }
  
  string booooh = "boooh!";
  /** @return ID of a single test to run, empty if not specified
   */
  const string &
  TestOption::getTestID ()
    {
      UNIMPLEMENTED("actual commandline parsing");
      return booooh;
    }
  
  
  /** gather all remaining unknown cmd line tokens into a vector.
   *  @Note: the actual vector remains a member of this object, but isn't const 
   */
  vector<string>& 
  TestOption::remainingCmdline ()
    {
      UNIMPLEMENTED("get unknown remaining options");
      return cmdline;
    }
  
  
  /** */
  TestOption::operator string const ()
    {
      UNIMPLEMENTED("convert the remaining Cmndline to string");
      return 0;
    }

  
  
} // namespace test
