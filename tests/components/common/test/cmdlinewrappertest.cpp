/*
  Cmdlinewrapper(Test)  -  build vector of tokens from cmdline, various conversions
 
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


#include "nobugcfg.h"

#include "common/test/run.hpp"
#include "common/cmdline.hpp"
#include "common/util.hpp"

#include <iostream>
#include <sstream>

#include <boost/lambda/lambda.hpp>

using namespace boost::lambda;
using std::cout;



namespace util
  {
  namespace test
    {
    
  
    ///// @test for util::Cmndline, wrapping various example cmdlines
    class CmdlineWrapper_test : public Test
      {
        virtual void run (Arg arg)
          {
            testLine("");
            testLine("\n\t ");
            testLine("spam");
            testLine("\nspam");
            testLine("eat more spam");
            testLine(" oo _O()O_  ä + €");
            testLine("\0\too\0\to\0o\t\0oo");
            
            testStandardCmdlineformat();
          }
        
        void testLine (const string cmdline)
          {
            cout << "wrapping cmdline:" << cmdline << "..." << "\n";
            
            int i=0;
            Cmdline theCmdline (cmdline);
            for_each(theCmdline, (cout << var(i)++ << "|" <<  _1 << "|\n"));
            cout << "-->" << theCmdline << "\n";
            
            // consistency checks
            std::ostringstream output;
            output << theCmdline;
            ENSURE (output.str() == string(theCmdline));
            
            i=0;
            string token;
            std::istringstream input(theCmdline);
            while (input >> token)
              ENSURE (token == theCmdline[i++]);
          }
        
        void testStandardCmdlineformat()
          {
            char* fakeArg[3] = {"CMD", "one ", "two"};
            Cmdline theCmdline(3, fakeArg);
            cout << "Standard Cmdlineformat:" << theCmdline << "\n";
          }
      };
    
      LAUNCHER (CmdlineWrapper_test, "unit common");

      
  } // namespace test
    
} // namespace util

