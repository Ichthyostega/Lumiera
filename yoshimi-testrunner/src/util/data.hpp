/*
 *  data - read and write a table with CSV data
 *
 *  Copyright 2021, Hermann Vosseler <Ichthyostega@web.de>
 *
 *  This file is part of the Yoshimi-Testsuite, which is free software:
 *  you can redistribute and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation,
 *  either version 3 of the License, or (at your option) any later version.
 *
 *  Yoshimi-Testsuite is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with yoshimi.  If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************/


/** @file data.hpp
 ** Manage a table with time series data, stored persistently as CSV.
 ** The Yoshimi Testsuite captures timing data, to detect the possible performance
 ** impact of code reworking. Due to the statistical nature of timing measurements
 ** and the dependency on the run environment, it is not sufficient just to rely on
 ** a single measurement to establish the runtime characteristics of a given test;
 ** rather, the statistical trend of the timings observed over several consecutive
 ** runs of the Testsuite must be established. Short of using a database, a modest
 ** amount of numeric data can be maintained in CSV files, which also allows for
 ** further manual evaluation within a spreadsheet or statistics application.
 **
 ** As a fundamental building block, this header provides a data table template
 ** with flexible column configuration to hold arbitrary, explicitly typed values.
 ** This solution is statically typed and does not carry any runtime type information;
 ** the actual data table object is then defined and accessed by means of _accessor_
 ** components for each column of data. A tuple of _current values_ corresponding to
 ** the most recent row of data can be accessed directly through these sub-components.
 **
 ** # Usage
 ** Create an actual instantiation of the DataFile template, passing a structure
 ** with util::Column descriptors. You may then directly access the values of the
 ** _actual column_ or save/load from a persistent CSV file.
 ** @note mandatory to define a method `allColumns()`
 ** \code
 ** struct Storage
 ** {
 **     Column<string> name{"theName"};
 **     Column<int>    n{"counter"};
 **     Column<double> x{"X value"};
 **     Column<double> y{"Y value"};
 **
 **     auto allColumns(){ return std::tie(name,n,x,y); }
 ** };
 **
 ** using Dataz = util::DataFile<Storage>;
 **
 ** Dataz daz("filename.csv");
 **
 ** daz.x = 123e-4;
 ** daz.y = -12345e-6;
 **
 ** std::vector<int>& counters = daz.n.data;
 ** \endcode
 **
 ** @see TimingObservation.hpp usage
 **
 */



#ifndef TESTRUNNER_UTIL_DATA_HPP_
#define TESTRUNNER_UTIL_DATA_HPP_


#include "util/nocopy.hpp"
#include "util/error.hpp"
#include "util/utils.hpp"
#include "util/file.hpp"
#include "util/csv.hpp"

#include <type_traits>
#include <utility>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <deque>
#include <tuple>


namespace util {

using std::tuple;
using std::vector;
using std::string;
using util::isnil;



/**
 * Helper: perform some arbitrary operation on each element of a tuple.
 * @note the given functor must be generic, since each position of the tuple
 *       may hold a data element of different type.
 * @remark credits to David Vandevoorde (member of C++ committee) for using
 *       std::apply to unpack the tuple's contents into an argument pack and
 *       then using a fold expression with the comma operator.
 */
template<class FUN, typename...ELMS>
void forEach(tuple<ELMS...>&& tuple, FUN fun)
{
    std::apply([&fun](auto&... elms)
                    {
                        (fun(elms), ...);
                    }
              ,tuple);
}



/**
 * Descriptor and Accessor for a data column within a DataFile table.
 * @tparam VAL type of values contained within this column;
 *             this type must be _default constructible_ and _copyable._
 */
template<typename VAL>
struct Column : util::NonCopyable
{
    string header;
    vector<VAL> data;

    using ValueType = VAL;


    Column(string headerID)
        : header{headerID}
        , data{}
    { }


    VAL& get()
    {
        if (isnil(data))
            throw error::State("No rows in DataTable yet");
        return data.back();
    }

    operator VAL&()
    {
        return get();
    }

    template<typename X>
    VAL& operator=(X&& newVal)
    {
        return get() = std::forward<X>(newVal);
    }
};





/**
 * Table with data values, stored persistently as CSV file.
 * Each row within the table represents a data record, holding a sequence
 * of values. Values are statically typed per column, i.e. one column may hold
 * strings, while the next column holds doubles. For actual usage it is thus necessary
 * to define the column layout, through a sequence of [column Descriptors](\ref util::Column).
 *
 * # Usage
 * Actually those Column objects serve as descriptors, but also as accessors -- and they hold
 * the actual data storage for each column, which is a `std::vector<VAL>` of value type `VAL`.
 * There is always a _current record_ -- corresponding to the actual data value and the newest
 * data row. For persistent storage, the sequence of rows is _reversed,_ so the newest data
 * appears at the top of the CSV file.
 * @tparam TAB a struct comprised of several Column objects, which hold the data and
 *         provide access to values of this specific column. Moreover, this type _must define_
 *         a function `allColumns()` to return a tuple with references to these column fields;
 *         the order of fields within this tuple also defines the order of columns
 *         within the table and persistent CSV storage.
 * @see suite::step::TimingObservation (relevant usage example)
 */
template<class TAB>
class DataFile
    : public TAB
    , util::NonCopyable
{
    fs::path filename_;

public:
    DataFile(fs::path csvFile)
        : filename_{consolidated(csvFile)}
    {
        loadData();
    }


    /* === Data Access === */

    static constexpr size_t columnCnt = std::tuple_size_v<decltype(std::declval<TAB>().allColumns())>;

    bool empty()  const
    {
        return 0 == this->size();
    }

    size_t size()  const
    {
        if (0 == columnCnt) return 0;
        size_t rowCnt = std::numeric_limits<size_t>::max();
        forEach(unConst(this)->allColumns(),
                [&](auto& col)
                {
                    rowCnt = std::min(rowCnt, col.data.size());
                }); // the smallest number of data points found in any column
        return rowCnt;
    }

    string dumpCSV()  const
    {
        string csv;
        for (uint i=0; i < size(); ++i)
            csv += formatCSVRow(i) + '\n';
        return csv;
    }



    /* === Manipulation === */

    void newRow()
    {
        forEach(TAB::allColumns(),
                [](auto& col)
                {
                    col.data.resize(col.data.size()+1);
                });
    }

    void dupRow()
    {
        if (empty())
            newRow();
        else
            forEach(TAB::allColumns(),
                [](auto& col)
                {
                    col.data.emplace_back(col.data.back());
                });
    }

    void reserve(size_t expectedCapacity)
    {
        forEach(TAB::allColumns(),
                [=](auto& col)
                {
                    col.data.reserve(expectedCapacity);
                });
    }


    /** @param lineLimit number of rows to retain, back from the newest */
    void save(size_t lineLimit =std::numeric_limits<size_t>::max())
    {
        fs::path newFilename{filename_};
        newFilename += ".tmp";

        std::ofstream csvFile{newFilename, std::ios_base::out | std::ios_base::trunc};
        if (not csvFile.good())
            throw error::State("Unable to create CSV output file "+formatVal(newFilename));
        saveData(csvFile, lineLimit);

        fs::path oldFile{filename_};
        oldFile += ".bak";
        if (fs::exists(filename_))
            fs::rename(filename_, oldFile);
        fs::rename(newFilename, filename_);
    }



private: /* === Implementation === */

    void loadData()
    {
        if (not (filename_.parent_path().empty()
                 or fs::exists(filename_.parent_path())))
            throw error::Invalid("DataFile("+formatVal(filename_.filename())
                                +") shall be placed into nonexistent directory "
                                +formatVal(filename_.parent_path()));
        if (not fs::exists(filename_))
            return; // leave the table empty

        std::ifstream csvFile(filename_);
        if (not csvFile.good())
            throw error::Misconfig{"unable to read CSV data file "+formatVal(filename_)};

        std::deque<string> rawLines;
        for (string line; std::getline(csvFile, line); )
            rawLines.emplace_back(move(line));

        if (rawLines.size() < 1) return;
        verifyHeaderSpec(rawLines[0]);

        // we know the number of rows now...
        reserve(rawLines.size() - 1);

        // storage in file is backwards, with newest data on top
        for (size_t row = rawLines.size()-1; 0<row; --row)
            if (not isnil(rawLines[row]))
                appendRowFromCSV(rawLines[row]);
    }


    void saveData(std::ofstream& csvFile, size_t lineLimit)
    {
        csvFile << generateHeaderSpec() << "\n";
        if (empty())
            return;
        lineLimit = size() > lineLimit? size()-lineLimit : 0;
        // store newest data first, possibly discard old data
        for (size_t row = size(); lineLimit < row; --row)
            csvFile << formatCSVRow(row-1) << "\n";
    }


    void verifyHeaderSpec(string headerLine)
    {
        CsvLine header(headerLine);
        forEach(TAB::allColumns(),
                [&](auto& col)
                {
                    if (*header != col.header)
                        throw error::Invalid("Header mismatch in CSV file. "
                                             "Expecting column("+formatVal(col.header)
                                            +") but found "+formatVal(*header));
                    ++header;
                });
    }

    string generateHeaderSpec()
    {
        string csv;
        forEach(TAB::allColumns(),
                [&](auto& col)
                {
                    appendCsvField(csv, col.header);
                });
        return csv;
    }


    void appendRowFromCSV(string line)
    {
        newRow();
        CsvLine csv(line);
        forEach(TAB::allColumns(),
                [&](auto& col)
                {
                    if (!csv)
                        if (csv.isParseFail())
                            csv.fail();
                        else
                            throw error::Invalid("Insufficient data; only "
                                                +str(csv.getParsedFieldCnt())
                                                +" fields, "+str(columnCnt)
                                                +" expected. Line="+line);

                    using Value = typename std::remove_reference<decltype(col)>::type::ValueType;
                    col.get() = parseAs<Value>(*csv);
                    ++csv;
                });
        if (csv)
            throw error::Invalid("Excess data fields in CSV. Expect "+str(columnCnt)+" fields. Line="+line);
    }


    string formatCSVRow(size_t rownum)  const
    {
        if (this->empty())
            throw error::LogicBroken("Attempt to access data from empty DataTable.");
        if (rownum >= this->size())
            throw error::LogicBroken("Attempt to access row #"+str(rownum)
                                    +" beyond range [0.."+str(size()-1)+"].");

        string csvLine;
        forEach(unConst(this)->allColumns(),
                [&](auto& col)
                {
                    appendCsvField(csvLine, col.data.at(rownum));
                });
        return csvLine;
    }
};



} // namespace util
#endif /*TESTRUNNER_UTIL_DATA_HPP_*/
