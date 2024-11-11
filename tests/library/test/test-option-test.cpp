/*
  TestOption(Test)  -  parsing of cmd line options for running Testcases

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/

/** @file test-option-test.cpp
 ** unit test \ref TestOption_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/testoption.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <memory>

using std::make_unique;
using lib::Cmdline;
using util::isnil;

namespace test {
  
  /************************************************************//**
   * invokes the TestOption parser for various example commandlines
   * @test for test::TestOption, parsing of commandline options
   * @see test::Suite
   * @see util::Cmdline
   */
  class TestOption_test : public Test
    {
      void
      run (Arg)
        {
          noOptions();
          help();
          groupID();
          singleTest();
          groupFilter1();
          groupFilter2();
          additionalCmd();
          additionalCmd2();
          verify_seed();
        }
      
      
      /** @test performs the actual test for the option parser test::TestOption */
      auto
      doIt (const string cmdline)
        {
          cout << "Testing invocation with cmdline: " << cmdline << "..." << endl;
          
          Cmdline args(cmdline);
          auto optparser = make_unique<TestOption>(args);
          const string testID = optparser->getTestID();
          cout << "--> Testgroup=" << optparser->getTestgroup() << endl;
          cout << "--> Test-ID  =" << (isnil(testID)? "--missing--" : testID ) << endl;
          cout << "--> remaining=" << args << endl;
          return std::move (optparser);
        }
      
      void noOptions()      { doIt (""); }
      void help()           { doIt ("--help"); }
      void groupID()        { doIt ("--group TestGroupID"); }
      void singleTest()     { doIt (" SingleTestID"); }
      void groupFilter1()   { doIt (" SingleTestID --group TestGroupID"); }
      void groupFilter2()   { doIt (" --group TestGroupID SingleTestID "); }
      void additionalCmd()  { doIt (" --group TestGroupID SingleTestID spam eggs"); }
      void additionalCmd2() { doIt ("\t\tSingleTestID spam --group TestGroupID    \t --eggs"); }
      
      void
      verify_seed()
        {
          CHECK ( not   doIt("ham --group spam")->optSeed());
          CHECK (7 == * doIt("ham --seed 7 spam")->optSeed());
          CHECK (0 == * doIt("ham --seed 0 spam")->optSeed());
          
          VERIFY_FAIL("argument (\'spam\') for option \'--seed\' is invalid"
                     , doIt("ham --seed spam"));
          
          // yet negative numbers are parsed and force-converted
          CHECK (std::numeric_limits<uint64_t>::max()
                   == * doIt("--seed=-1")->optSeed());
        }
    };
  
  LAUNCHER (TestOption_test, "function common");    
  
  
} // namespace test
