/*
  Cmdlinewrapper(Test)  -  build vector of tokens from cmdline, various conversions

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


#include "lib/test/run.hpp"
#include "lib/cmdline.hpp"
#include "lib/util-foreach.hpp"

#include <iostream>
#include <sstream>

#include <boost/lambda/lambda.hpp>

using std::cout;



namespace util {
namespace test {
  using boost::lambda::_1;
  using boost::lambda::var;
  
  
  
  /** @test for util::Cmdline, wrapping various example cmdlines */
  class CmdlineWrapper_test : public Test
    {
      void
      run (Arg)
        {
          testLine("");
          testLine("\n\t ");
          testLine("spam");
          testLine("\nspam");
          testLine("eat more spam");
          testLine(" oo _O()O_  ä + €");
          testLine("Ω\tooΩ\toΩo\tΩoo");
          
          testStandardCmdlineformat();
        }
      
      void
      testLine (const string cmdline)
        {
          cout << "wrapping cmdline:" << cmdline << "..." << "\n";
          
          int i=0;
          Cmdline theCmdline (cmdline);
          for_each(theCmdline, (cout << var(i)++ << "|" <<  _1 << "|\n"));
          cout << "-->" << theCmdline << "\n";
          
          // consistency checks
          std::ostringstream output;
          output << theCmdline;
          CHECK (output.str() == string(theCmdline));
          
          i=0;
          string token;
          std::istringstream input(theCmdline);
          while (input >> token)
            CHECK (token == theCmdline[i++]);
        }
      
      /** @test wrapping a (albeit faked) standard commandline
       *        given as (argc, argv[])
       */
      void
      testStandardCmdlineformat()
        {
          const char* fakeArg[3] = {"CMD", "one ", "two"};
          Cmdline theCmdline(3, fakeArg);
          cout << "Standard Cmdlineformat:" << theCmdline << "\n";
        }
    };
  
  LAUNCHER (CmdlineWrapper_test, "unit common");
  
  
}} // namespace util::test

