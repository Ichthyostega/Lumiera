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
 ** Manage a table with time sequence data, stored persistently as CSV.
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
 ** with a flexible column configuration to hold arbitrary, explicitly typed values.
 ** This solution is statically typed and does not carry any runtime type information;
 ** the actual data table object is then defined and accessed by means of _accessor_
 ** components for each column of data. A tuple of _current values_ corresponding to
 ** the most recent row of data can be accessed directly through these sub-components.
 ** 
 ** @todo WIP as of 9/21
 ** @see TimingObservation.hpp usage
 ** 
 */



#ifndef TESTRUNNER_UTIL_DATA_HPP_
#define TESTRUNNER_UTIL_DATA_HPP_


#include "util/nocopy.hpp"
#include "util/error.hpp"
#include "util/utils.hpp"
#include "util/csv.hpp"

//#include <string>
//#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include <limits>
#include <tuple>


namespace util {

using std::tuple;
using std::vector;
using util::isnil;



/**
 * perform some arbitrary operation on each element of a tuple.
 * @note the given functor must be generic, since each position of the tuple
 *       may hold a data element of a different type.
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



template<class TAB>
class DataFile
    : public TAB
    , util::NonCopyable
{

public:
    static constexpr size_t columnCnt = std::tuple_size_v<decltype(std::declval<TAB>().allColumns())>;

    DataFile()
    {
        newRow();
    }

    void newRow()
    {
        forEach(TAB::allColumns(),
                [](auto& col)
                {
                    col.data.resize(col.data.size()+1);
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
                                                +formatVal(csv.getParsedFieldCnt())
                                                +" fields. Line="+line);

                    using Value = typename std::remove_reference<decltype(col)>::type::ValueType;
                    col.get() = parseAs<Value>(*csv);
                    ++csv;
                });
    }

    string formatCSVRow(size_t rownum)
    {
        if (this->empty())
            throw error::LogicBroken("Attempt to access data from empty DataTable.");
        if (rownum >= this->size())
            throw error::LogicBroken("Attempt to access row #"+str(rownum)
                                    +" beyond range [0.."+str(size()-1)+"].");

        string csvLine;
        forEach(TAB::allColumns(),
                [&](auto& col)
                {
                    appendCsvField(csvLine, col.data.at(rownum));
                });
        return csvLine;
    }

    size_t size()  const
    {
        if (0 == columnCnt) return 0;
        size_t rowCnt = std::numeric_limits<size_t>::max();
        forEach(unConst(this)->allColumns(),
                [&](auto& col)
                {
                    rowCnt = std::min(rowCnt, col.data.size());
                });
        return rowCnt;
    }

    bool empty()  const
    {
        return 0 == this->size();
    }

    template<size_t i>
    decltype(auto) getCol()
    {
        return std::get<i>(TAB::allColumns());
    }

    template<size_t i>
    decltype(auto) getStorage()
    {
        return getCol<i>().data;
    }
    template<size_t i>
    string getHeader()
    {
        return getCol<i>().header;
    }
};



} // namespace util
#endif /*TESTRUNNER_UTIL_DATA_HPP_*/
