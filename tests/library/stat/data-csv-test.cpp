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
#include "lib/test/temp-dir.hpp"
#include "lib/stat/data.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <sstream>
#include <string>
#include <vector>

using util::isnil;
using lib::time::Time;
using lib::test::TempDir;
using std::make_tuple;
using std::string;
using std::vector;



namespace lib {
namespace stat{
namespace test{
  
  namespace {//Setup for test
    
    
    /** Define the layout of a data row */
    struct TableForm
    {
        Column<string>  id{"ID"};     //   ◁────── names given here must match first storage line
        Column<double> val{"Value"};
        Column<int>    off{"Offset"};
    
        auto allColumns()         //   ◁────────── mandatory function; defines actual sequence of columns
        {   return std::tie(id
                           ,val
                           ,off
                           );
        }
    };
    
    using TestTab = DataFile<TableForm>;
    
  }//(End)Test setup
  
  using error::LUMIERA_ERROR_STATE;
  
  
  
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
          demonnstrate_CSV_Notation();
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
          
          meta::forEach (tab.allColumns()
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
          double val = 1.0 / 3;
          CHECK (util::toString(val)              == "0.33333333"_expect         );
          CHECK (util::showDecimal(val)           == "0.333333333333333"_expect  );
          CHECK (util::showComplete(val)          == "0.33333333333333331"_expect);
          CHECK (boost::lexical_cast<string>(val) == "0.33333333333333331"_expect);
          
          CHECK (format4Csv(double(1) / 3) == "0.333333333333333"_expect   );
          CHECK (format4Csv(float(1) / 3)  == "0.333333"_expect            );
          CHECK (format4Csv(f128(1) / 3)   == "0.333333333333333333"_expect);
          CHECK (format4Csv(bool(1))       == "true"_expect );
          CHECK (format4Csv(bool(0))       == "false"_expect);
          CHECK (format4Csv("Starship-3")  == "\"Starship-3\""_expect ); // 3rd test today ;-)
          CHECK (format4Csv(Time(1,2,25,13)) == "\"13:25:02.001\""_expect);
          
          
          string line;
          int64_t ii = -100000;
          bool boo   = true;
          
          appendCsvField (line, ii);
          CHECK (line == "-100000"_expect);
          appendCsvField (line, val);
          CHECK (line == "-100000,0.333333333333333"_expect);
          appendCsvField (line, boo);
          CHECK (line == "-100000,0.333333333333333,true"_expect);
          appendCsvField (line, "Raptor");
          CHECK (line == "-100000,0.333333333333333,true,\"Raptor\""_expect);
          
          
          CsvParser parse{line};
          CHECK (parse.isValid());
          CHECK (*parse == "-100000"_expect);
          CHECK (-100000 == parseAs<int>(*parse));
          ++parse;
          CHECK (parse.isValid());
          CHECK (*parse == "0.333333333333333"_expect);
          CHECK (0.333333343f == parseAs<float>(*parse));
          ++parse;
          CHECK (parse.isValid());

          CHECK (*parse == "true"_expect);
          CHECK (true == parseAs<bool>(*parse));
          ++parse;
          CHECK (parse.isValid());
          CHECK (*parse == "Raptor"_expect);
          CHECK ("Raptor" == parseAs<string>(*parse));
          ++parse;
          CHECK (not parse.isValid());
          
          line = " ◐0◑. ; \t \"' \" \n ,oh my ;";
          CsvParser horror{line};
          CHECK ("◐0◑." == *horror);           // as far as our CSV format is concerned, this is valid
          CHECK (0 == horror.getParsedFieldCnt());
          ++horror;
          CHECK (1 == horror.getParsedFieldCnt());
          CHECK ("' " == *horror);
          ++horror;
          CHECK ("oh" == *horror);
          CHECK (2 == horror.getParsedFieldCnt());
          
          // next field is not quoted, but contains space
          VERIFY_FAIL (",oh |↯|my ;", ++horror );
          
          CHECK (not horror.isValid());
          CHECK (horror.isParseFail());
          
          // CsvParser is a »Lumiera Forward Iterator«
          CHECK (meta::can_IterForEach<CsvParser>::value);
        }
      
      
      /** @test verify a table backed by persistent CSV data */
      void
      verify_persistentDataFile()
        {
          TempDir temp;
          // prepare a data file to load into the table...
          fs::path f = temp.makeFile("dataz.csv");
          std::ofstream content{f};
          content << R"("ID", "Value", "Offset")"<<endl
                  << R"(  "one" , 5.5 ;    +1  )"<<endl
                  << R"(;" 0 ";0)"               <<endl;  //   ◁────── demonstrating some leeway in storage format
          content.close();
          
          TestTab dat{f};
          CHECK (2 == dat.size());
          CHECK ("ID"  == dat.id.header);
          CHECK ("Value" == dat.val.header);
          CHECK ("Offset" == dat.off.header);
          //Note: data is reversed in storage — last/newest line first
          CHECK ("one" == string{dat.id});
          CHECK ( 5.5  == dat.val);
          CHECK (  1   == dat.off);
          CHECK (dat.id.data    == vector<string>({"","one"}));
          CHECK (dat.val.data   == vector<double>({0 ,5.5  }));
          CHECK (dat.off.data   == vector<int>   ({0 ,1    }));
          
          // can modify some values....
          dat.id = "mid";
          dat.dupRow();
          dat.id = "last";
          dat.off *= -1;
          // can dump the contents as CSV
          CHECK (dat.dumpCSV() ==
R"("",0,0
"mid",5.5,1
"last",5.5,-1
)"_expect);
          
          // save complete table in current state, overwriting on disk
          dat.save();
          
          // read back data rewritten on disk...
          std::ifstream readback{f};
          std::ostringstream inBuff;
          inBuff << readback.rdbuf();
          CHECK (inBuff.str() ==
R"("ID","Value","Offset"
"last",5.5,-1
"mid",5.5,1
"",0,0
)"_expect);
         // note again the reversed order in storage: last line at top
        }
      
      
      
      /** @test simplified notation of inline CSV data for tests */
      void
      demonnstrate_CSV_Notation()
        {
          CHECK (CSVLine(1,"2",3.4,5555/55) == "1,\"2\",3.4,101"_expect);
          CHECK (CSVLine(string{"himself"}) == "\"himself\""_expect);
          CHECK (CSVLine{CSVLine{1e9}}      == "1000000000"_expect);
          CHECK (CSVLine{}                  == ""_expect);
          
          auto appended = (CSVLine{} += 5.5) += Symbol();
          CHECK (appended == "5.5,\"⟂\""_expect);
          
          CHECK (CSVData({"eeny","meeny","miny","moe"}) == "\"eeny\",\"meeny\",\"miny\",\"moe\""_expect);
          CHECK (CSVData({"eeny , meeny","miny","moe"}) == "\"eeny , meeny\"\n\"miny\"\n\"moe\""_expect); // you dirty dirty dishrag you
          
          auto csv = CSVData{{"la","la","schland"}
                            ,{{3.2,1l,88}
                             ,{"mit", string{"mia"}, Literal("ned")}
                             ,CSVLine(string(";"))
                             ,{false}
                             ,{}
                             }};
          CHECK (csv.size() == 6);
          CHECK (string(csv) ==
R"("la","la","schland"
3.2,1,88
"mit","mia","ned"
";"
false
)"_expect);
        }
    };
  
  LAUNCHER (DataCSV_test, "unit calculation");
  
  
}}} // namespace lib::stat::test
