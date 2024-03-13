/*
  DataCSV(Test)  -  verify data table with CSV storage support

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file data-csv-test.cpp
 ** unit test \ref DataCSV_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/stat/data.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/error.hpp"
//#include "lib/util-foreach.hpp"
#include "lib/util.hpp"
#include "lib/format-cout.hpp"            ///////////////////////TODO
#include "lib/test/diagnostic-output.hpp" ///////////////////////TODO

//#include <functional>
#include <string>
#include <vector>

//using lumiera::Error;
//using lumiera::LUMIERA_ERROR_EXCEPTION;
//using lumiera::error::LUMIERA_ERROR_ASSERTION;
//using lib::time::TimeVar;
//using lib::time::Time;

//using boost::algorithm::is_lower;
//using boost::algorithm::is_digit;
using util::isnil;
//using std::function;
using std::string;
using std::vector;


namespace lib {
namespace stat{
namespace test{
  
  namespace {//Setup for test
    
    /** Define the layout of a data row */
    struct TableForm
    {
        Column<string>  id{"ID"};
        Column<double> val{"Value"};
        Column<int>    off{"Offset"};
    
        auto allColumns()
        {   return std::tie(id
                           ,val
                           ,off
                           );
        }
    };
    
    using TestTab = DataFile<TableForm>;
    
  }//(End)Test setup
  
  
  
  /***********************************************************//**
   * @test Document and verify a data table component to maintain
   *       measurement data series, backed by CSV storage.
   * @see data.hpp
   * @see csv.hpp
   * @see Statistic_test
   */
  class DataCSV_test : public Test
    {
      void
      run (Arg)
        {
          simpleUsage();
          checkGarbageStr();
          checkThrowChecker();
          checkLocalManipulation();
        }
      
      
      /** @test add rows and data to a table without filename. */
      void
      simpleUsage ()
        {
          TestTab tab;
          CHECK (isnil (tab));
          tab.newRow();
          CHECK (not isnil (tab));
          CHECK (1 == tab.size());
          CHECK ( "" == string{tab.id});
          CHECK (0.0 == tab.val);
          CHECK ( 0  == tab.off);
          tab.id = "one";
          tab.val = 1.0;
          
          tab.dupRow();
          CHECK (2 == tab.size());
          CHECK ("one" == string{tab.id});
          CHECK ( 1.0  == tab.val);
          CHECK (   0  == tab.off);
          
          tab.id = "two";
          tab.val = 5.0;
          tab.off = -23;
          CHECK ("two" == string{tab.id});
          CHECK ( 5.0  == tab.val);
          CHECK ( -23  == tab.off);
          
          CHECK (tab.off.header == "Offset");
          CHECK (tab.off.data   == vector({0,-23}));
        }
      
      
      
      
      void
      checkGarbageStr()
        {
          UNIMPLEMENTED("Rabäääääh");
        }
      
      
      /** @test check the VERIFY_ERROR macro,
       *        which ensures a given error is raised.
       */
      void
      checkThrowChecker()
        {
        }
      
      
      /** @test check a local manipulations,
       *        which are undone when leaving the scope.
       */
      void
      checkLocalManipulation()
        {
        }
    };
  
  LAUNCHER (DataCSV_test, "unit calculation");
  
  
}}} // namespace lib::stat::test

