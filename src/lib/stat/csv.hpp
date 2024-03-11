/*
  CSV.hpp  -  Parser and Encoder for CSV data

  Copyright (C)         Lumiera.org
    2022,               Hermann Vosseler <Ichthyostega@web.de>

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

*/


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
 ** @see lib::stat::DataFile
 **
 */


#ifndef LIB_STAT_CSV_H
#define LIB_STAT_CSV_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/format-string.hpp"
#include "lib/format-obj.hpp"
#include "lib/stat/regex.hpp"

#include <limits>
#include <string>

namespace lib {
namespace stat {
  
  namespace error = lumiera::error;

  using util::_Fmt;
  using util::toString;
  using std::string;
  using std::regex;
  
  
  namespace { // Implementation details...
  
    const string MATCH_SINGLE_TOKEN { R"~(([^,;"\s]*)\s*)~"};
    const string MATCH_QUOTED_TOKEN { R"~("([^"]*)"\s*)~" };
    const string MATCH_DELIMITER    { R"~((?:^|,|;)\s*)~" };
    
    const regex ACCEPT_FIELD{ MATCH_DELIMITER + "(?:"+ MATCH_QUOTED_TOKEN +"|"+ MATCH_SINGLE_TOKEN +")"
                            , regex::optimize};
    
    template<typename VAL>
    inline string
    format4Csv (VAL const& val)
    {
      std::ostringstream oss;
      oss.precision (std::numeric_limits<VAL>::digits10);  /////////////////////////////OOO herausfinden ob hier lexical_cast genügt ==> dann toString()
      oss << val;
      return oss.str();
    }
    
    inline string
    format4Csv (string const& val)
    {
      return '"'+val+'"';
    }
    
    inline string
    format4Csv (bool boo)
    {
      return util::showBool(boo);  ///////////////////////OOO würde toSting() das korrekt hinbekommen
    }
  }//(End)Implementation
  
  
  /**
   * Format and append a data value to a CSV string representation
   */
  template<typename VAL>
  inline void
  appendCsvField (string& csv, VAL const& val)
  {
    csv += (0 == csv.length()? "":",")
         + format4Csv(val);
  }
  
  
  /**
   * Parser to split one line of CSV data into fields.
   * @remarks iterator-like throw-away object
   *  - the `bool` evaluation indicates more fields to extract
   *  - dereference to get the field as string
   *  - increment to move to the next field
   * @throws error::Invalid on CSV format violation
   * @todo 3/24 should be rewritten as Lumiera Forward Iterator
   */
  class CsvLine
    : util::NonCopyable
    , util::MatchSeq
    {
      string const& line_;
      size_t   field_;
      iterator curr_;
      size_t   pos_;

    public:
      CsvLine (string const& line)
        : MatchSeq(line, ACCEPT_FIELD)
        , line_{line}
        , field_{0}
        , curr_{MatchSeq::begin()}
        , pos_{0}
        { }
      
      explicit operator bool()  const
        {
          return isValid();
        }
      
      string operator*()  const
        {
          if (not isValid()) fail();
          auto& mat = *curr_;
          return mat[2].matched? mat[2]
                               : mat[1];
        }
      
      void
      operator++()
        {
          if (not isValid())
            fail();
          pos_ = curr_->position() + curr_->length();
          ++curr_;
          if (pos_ < line_.length() and not isValid())
            fail ();
          ++field_;
        }
      
      size_t
      getParsedFieldCnt()
        {
          return field_;
        }
      
      bool
      isValid()  const
        {
          return curr_ != end()
             and pos_ == size_t(curr_->position())
             and not curr_->empty();
        }
      
      bool
      isParseFail()  const
        {
          return curr_ != end()
             and not isValid();
        }
      
      void
      fail()  const
        {
          if (curr_ == end())
              if (pos_ >= line_.length())
                  throw error::Invalid{_Fmt{"Only %d data fields. Line:%s"}
                                           % field_ % line_};
              else
                  throw error::Invalid{_Fmt{"Garbage after last field. Line:%s|↯|%s"}
                                           % line_.substr(0,pos_) % line_.substr(pos_)};
          else
          if (pos_ != size_t(curr_->position()))
                  throw error::Invalid{_Fmt{"Garbage before field(%d):%s|↯|%s"}
                                           % (field_+1)
                                           % line_.substr(0,pos_) % line_.substr(pos_)};
          
          throw error::Invalid{_Fmt{"CSV parse floundered. Line:%s"} % line_};
        }
    };
  
}} // namespace lib::stat
#endif /*LIB_STAT_CSV_H*/
