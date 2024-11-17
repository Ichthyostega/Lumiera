/*
  CmdlineWrapper(Test)  -  build vector of tokens from cmdline, various conversions

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file cmdline-wrapper-test.cpp
 ** unit test \ref CmdlineWrapper_test
 */


#include "lib/test/run.hpp"
#include "lib/cmdline.hpp"
#include "lib/util-foreach.hpp"
#include "lib/format-cout.hpp"

#include <sstream>
#include <string>

using util::for_each;
using std::string;



namespace lib {
namespace test{
  
  
  
  /** @test for lib::Cmdline, wrapping various example cmdlines */
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
          testLine(" oo _O()O_  ☭ + €");
          testLine("Ω\tooΩ\toΩo\tΩoo");
          
          testStandardCmdlineformat();
        }
      
      void
      testLine (const string cmdline)
        {
          cout << "wrapping cmdline:" << cmdline << "..." << endl;
          
          int i=0;
          Cmdline theCmdline (cmdline);
          for_each(theCmdline, [&](string const& arg) { cout << i++ << "|" <<  arg << "|\n";});
          cout << "-->" << theCmdline << endl;
          
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
          cout << "Standard Cmdlineformat:" << theCmdline << endl;
        }
    };
  
  LAUNCHER (CmdlineWrapper_test, "unit common");
  
  
}} // namespace lib::test
