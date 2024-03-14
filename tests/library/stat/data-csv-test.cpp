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
using std::make_tuple;


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
  
  using error::LUMIERA_ERROR_STATE;
  using error::LUMIERA_ERROR_INVALID;
  
  
  
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
          verify_rowHandling();
          verify_CSV_Format();
          verify_persistentDataFile();
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
      verify_rowHandling()
        {
          TestTab tab;
          CHECK (3 == tab.columnCnt);
          
          CHECK (isnil (tab));
          CHECK (0 == tab.size());
          CHECK (0 == tab.id.data.size());
          CHECK (0 == tab.val.data.size());
          CHECK (0 == tab.off.data.size());
          CHECK ("ID"  == tab.id.header);
          CHECK ("Value" == tab.val.header);
          CHECK ("Offset" == tab.off.header);
          
          VERIFY_ERROR (STATE, tab.id.get() );
          VERIFY_ERROR (STATE, tab.val.get());
          VERIFY_ERROR (STATE, tab.off.get());
          VERIFY_ERROR (STATE, tab.off = 5  );
          VERIFY_ERROR (STATE,(tab.off == 5));
          
          // direct access to the data is possible and tolerated
          tab.val.data.push_back (5.5);
          CHECK (tab.val == 5.5);
          VERIFY_ERROR (STATE, (tab.off == 5));
          CHECK (1 == tab.val.data.size());
          CHECK (0 == tab.off.data.size());
          CHECK (0 == tab.id.data.size());
          CHECK (0 == tab.size());
          CHECK (isnil (tab));
          
          tab.newRow();
          CHECK ( "" == string{tab.id});
          CHECK (5.5 == tab.val);
          CHECK ( 0  == tab.off);
          CHECK (1 == tab.val.data.size());
          CHECK (1 == tab.off.data.size());
          CHECK (1 == tab.id.data.size());
          CHECK (1 == tab.size());
          CHECK (not isnil (tab));
          CHECK (tab.off.data   == vector({0}));
          CHECK (tab.val.data   == vector({5.5}));
          
          tab.allColumns() = make_tuple("●", 2.3, -11);
          CHECK ("●" == string{tab.id});
          CHECK (2.3 == tab.val);
          CHECK (-11 == tab.off);
          
          tab.dupRow();
          tab.val = 42;
          tab.id  = "◆";
          CHECK (tab.off.data   == vector({-11,-11}));
          CHECK (tab.val.data   == vector({2.3,42.0}));
          CHECK (tab.id.data    == vector<string>({"●","◆"}));
          
          tab.reserve(100);
          CHECK (tab.id.data.capacity()  >= 100);
          CHECK (tab.val.data.capacity() >= 100);
          CHECK (tab.off.data.capacity() >= 100);
          CHECK (tab.id.data.size()  == 2);
          CHECK (tab.val.data.size() == 2);
          CHECK (tab.off.data.size() == 2);
          CHECK (2 == tab.size());
          CHECK ("◆" == string{tab.id});
          CHECK ( 42 == tab.val);
          CHECK (-11 == tab.off);
          
          forEach(tab.allColumns()
                 ,[](auto& col){ col.data.resize(2); }
                 );
          CHECK (2 == tab.size());
          CHECK ("◆" == string{tab.id});
          CHECK ( 42 == tab.val);
          CHECK (-11 == tab.off);
          
          tab.dropLastRow();
          CHECK (1 == tab.size());
          CHECK ("●" == string{tab.id});
          CHECK (2.3 == tab.val);
          CHECK (-11 == tab.off);
          CHECK (tab.val.data.size() == 1);
          CHECK (tab.val.data.capacity() >= 100);
          
          tab.clear();
          CHECK (isnil (tab));
          CHECK (tab.val.data.size() == 0);
          CHECK (tab.val.data.capacity() >= 100);
        }
      
      
      /** @test validate the simple CSV conversion functions used by DataFile */
      void
      verify_CSV_Format()
        {
          string line;
          double val = 1.0 / 3;
          int64_t ii = -100000;
          
          appendCsvField (line, ii);
          CHECK (line == "-100000"_expect);
SHOW_EXPR(val)
SHOW_EXPL(val)
SHOW_EXPR(std::to_string(val))
SHOW_EXPR(format4Csv(val));
SHOW_EXPR(util::showDecimal(val))
SHOW_EXPR(util::showComplete(val))
          double vval = parseAs<double>(format4Csv(val));
SHOW_EXPR(vval)
SHOW_EXPL(vval)
SHOW_EXPR(std::to_string(vval))
SHOW_EXPR(format4Csv(vval));
SHOW_EXPR(util::showDecimal(val))
SHOW_EXPR(util::showComplete(val))
          vval = parseAs<double>(boost::lexical_cast<string>(val));
SHOW_EXPR(vval)
SHOW_EXPL(vval)
SHOW_EXPR(std::to_string(vval))
SHOW_EXPR(format4Csv(vval));
SHOW_EXPR(util::showDecimal(val))
SHOW_EXPR(util::showComplete(val))
          bool boo;
SHOW_EXPR(boo);
SHOW_EXPL(boo)
SHOW_EXPR(std::to_string(boo))
SHOW_EXPR(format4Csv(boo));
SHOW_EXPR(format4Csv(-42));
SHOW_EXPR(format4Csv(uint64_t(-42)));
          auto moo = f128(1) / 3;
SHOW_EXPR(moo)
SHOW_EXPL(moo)
SHOW_EXPR(std::to_string(moo))
SHOW_EXPR(format4Csv(moo));
SHOW_EXPR(util::showDecimal(moo))
SHOW_EXPR(util::showComplete(moo))
          auto oo = 1.0f / 3;
SHOW_EXPR(oo)
SHOW_EXPL(oo)
SHOW_EXPR(std::to_string(oo))
SHOW_EXPR(format4Csv(oo));
SHOW_EXPR(util::showDecimal(oo))
SHOW_EXPR(util::showComplete(oo))
SHOW_EXPR(format4Csv(lib::time::Time(1,2,3,4)));
        }
      
      
      /** @test verify a table backed by persistent CSV data
       */
      void
      verify_persistentDataFile()
        {
          UNIMPLEMENTED("Arrrröööh");
        }
    };
  
  LAUNCHER (DataCSV_test, "unit calculation");
  
  
}}} // namespace lib::stat::test

