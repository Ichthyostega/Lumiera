/*
 *  csv - parser and encoder
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


/** @file csv.hpp
 ** Encoding and decoding of data into CSV format.
 ** The sequence of values transformed here is part of a data table, with columns
 ** holding data of various primitive value types; persisted CSV data is human readable,
 ** can be checked into Git and loaded into various spreadsheet and statistics applications.
 **
 ** # CSV Format
 ** Even while there is a standard defined in [RFC 4180], a plethora of format variations
 ** can be found _in the wild._ Since the primary purpose of this implementation is _to read
 ** back our own data,_ by deliberate choice only one single form of CSV is accepted.
 ** - first line is a header line and used to verify the number of columns
 ** - one record per line, embedded line breaks prohibited
 ** - fields separated by comma, semicolon tolerated
 ** - fields are trimmed and may be empty
 ** - a field may be double quoted
 ** - only quoted fields may contain whitespace or comma
 ** - no escaping of quotes, i.e. no quotes within quotes
 ** [RFC 4180]: https://datatracker.ietf.org/doc/html/rfc4180
 ** 
 ** @todo WIP as of 9/21
 ** @see util::DataFile used for [Timing statistics](\ref TimingObservation.hpp)
 ** 
 */



#ifndef TESTRUNNER_UTIL_CSV_HPP_
#define TESTRUNNER_UTIL_CSV_HPP_


//#include "util/nocopy.hpp"
#include "util/error.hpp"
#include "util/format.hpp"
#include "util/regex.hpp"
//#include "util/utils.hpp"

//#include <string>
//#include <memory>
//#include <utility>
//#include <vector>


namespace util {

using std::regex;
//using std::vector;
//using util::isnil;

namespace { // Implementation details...

    const string MATCH_SINGLE_TOKEN {R"~(([^,;"\s]*)\s*)~"};
    const string MATCH_QUOTED_TOKEN {R"~("([^"]*)"\s*)~"};
    const string MATCH_DELIMITER    {R"~((?:^|,|;)\s*)~"};

    const regex ACCEPT_FIELD{ MATCH_DELIMITER + "(?:"+ MATCH_QUOTED_TOKEN +"|"+ MATCH_SINGLE_TOKEN +")"
                            , regex::optimize};


    template<typename VAL>
    inline string format4Csv(VAL const& val)
    {
        return util::str(val);
    }
    inline string format4Csv(string const& val)
    {
        return '"'+val+'"';
    }

}//(End)Implementation


/**
 * Parser to split one line of CSV data into fields.
 * @remarks iterator-like throw-away object
 *  - the `bool` evaluation indicates more fields to extract
 *  - dereference to get the field as string
 *  - increment to move to the next field
 * @throws error::Invalid on CSV format violation
 */
class CsvLine
    : util::NonCopyable
    , MatchSeq
{
    string const& line_;
    size_t   field_;
    iterator curr_;
    size_t   pos_;

public:
    CsvLine(string const& line)
        : MatchSeq(line, ACCEPT_FIELD)
        , line_{line}
        , field_{0}
        , curr_{MatchSeq::begin()}
        , pos_{0}
    { }

    explicit operator bool()
    {
        return isValid();
    }

    string operator*()
    {
        if (not isValid()) fail();
        auto& mat = *curr_;
        return mat[2].matched? mat[2]
                             : mat[1];
    }

    void operator++()
    {
        if (not isValid())
            fail();
        pos_ = curr_->position() + curr_->length();
        ++curr_;
        if (pos_ < line_.length() and not isValid())
            fail();
        ++field_;
    }

    size_t getParsedFieldCnt()
    {
        return field_;
    }

    bool isValid()
    {
        return curr_ != end()
           and curr_->position() == pos_
           and not curr_->empty();
    }

    void fail()
    {
        if (curr_ == end())
            if (pos_ >= line_.length())
                throw error::Invalid("Only "+formatVal(field_)+" data fields. Line:"+line_);
            else
                throw error::Invalid("Garbage after last field. Line:"
                                    +line_.substr(0,pos_)+"|↯|"+line_.substr(pos_));
        else
            if (pos_ != curr_->position())
                throw error::Invalid("Garbage before field("+formatVal(field_+1)+"):"
                                    +line_.substr(0,pos_)+"|↯|"+line_.substr(pos_));
            else
                throw error::Invalid("CSV parse floundered. Line:"+line_);
    }
};



/**
 * Format and append a data value to a CSV string representation
 */
template<typename VAL>
inline void appendCsvField(string& csv, VAL const& val)
{
    csv += (0 == csv.length()? "":",")
         + format4Csv(val);
}



} // namespace util
#endif /*TESTRUNNER_UTIL_CSV_HPP_*/
