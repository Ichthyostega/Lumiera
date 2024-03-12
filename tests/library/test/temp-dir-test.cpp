/*
  TempDir(Test)  -  verify automated temporary working directory

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file temp-dir-test.cpp
 ** unit test \ref TempDir_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/temp-dir.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/error.hpp"
//#include "lib/util-foreach.hpp"
#include "lib/format-cout.hpp"
#include "lib/test/diagnostic-output.hpp"

//#include <boost/algorithm/string.hpp>
#include <fstream>
//#include <functional>
//#include <string>

//using util::for_each;
//using lumiera::Error;
//using lumiera::LUMIERA_ERROR_EXCEPTION;
//using lumiera::error::LUMIERA_ERROR_ASSERTION;
//using lib::time::TimeVar;
//using lib::time::Time;

//using boost::algorithm::is_lower;
//using boost::algorithm::is_digit;
//using std::function;
//using std::string;


namespace lib {
namespace test{
namespace test{
  
  
  
  
  /***************************************************************//**
   * @test validate proper working of a temporary working directory,
   *       with automatic name allocation and clean-up.
   * @see temp-dir.hpp
   * @see DataCSV_test usage example
   */
  class TempDir_test : public Test
    {
      void
      run (Arg)
        {
          simpleUsage();
          verify_Lifecycle();
        }
      
      
      void
      simpleUsage()
        {
          TempDir temp;
          auto ff = temp.makeFile();
          CHECK (fs::exists (ff));
          CHECK (fs::is_empty (ff));
          
          std::ofstream out{ff, std::ios_base::out};
          auto scree = randStr(55);
          out << scree << endl;
          out.close();
          
          CHECK (fs::is_regular_file (ff));
          CHECK (not fs::is_empty (ff));
          
          std::ifstream in{ff};
          string readBack;
          in >> readBack;
          CHECK (readBack == scree);
        }
      
      
      
      /** @test prints "sizeof()" including some type name. */
      void
      verify_Lifecycle ()
        {
        }
    };
  
  LAUNCHER (TempDir_test, "unit common");
  
  
}}} // namespace lib::test::test

