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
#include "lib/null-value.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/format-string.hpp"
#include "lib/format-util.hpp"
#include "lib/regex.hpp"

#include <limits>
#include <string>
#include <vector>

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
    
    const regex FIND_DELIMITER_TOKEN{"[,;]"};
    
    const regex ACCEPT_FIELD{ MATCH_DELIMITER + "(?:"+ MATCH_QUOTED_TOKEN +"|"+ MATCH_SINGLE_TOKEN +")"
                            , regex::optimize};
    
    template<typename VAL>
    inline string
    format4Csv (VAL const& val)
    {
      if constexpr (std::is_floating_point_v<VAL>)
        return util::showDecimal (val);
                  //   standard textual rendering
      auto res = util::toString (val);
      if constexpr (std::is_arithmetic_v<VAL>)
        return res;   // includes bool
      else
        return '"'+res+'"';
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
   * A string with the ability to construct
   * or append the CSV-rendering of data fields
   */
  struct CSVLine
    : std::string
    {
      using value_type = string;
      
      template<typename...ELMS,                 typename = meta::disable_if_self<CSVLine,ELMS...>>
      CSVLine (ELMS&& ...items)
        {
          meta::forEach (std::make_tuple (items...)
                        ,[this](auto const& it){ *this += it; }
                        );
        }
      // Standard copy acceptable
      
      
      template<typename X>
      CSVLine&
      operator+= (X const& x)
      {
        stat::appendCsvField (*this, x);
        return *this;
      }
    };
  
  /**
   * Wrapper to simplify notation in tests.
   * Accepts data suitable for representation as CSV
   * - either as an std::initializer_list<string> for pre-formatted rows
   * - or as a sequence of strings (words) to form a single header line
   * - or a list of strings for the header, and then a list of data tuples,
   *   which will be rendered into data rows in CSV format
   * Since this wrapper is-a `vector<string>`, the rows can be retrieved
   * directly and then rendered, or the \ref operator string() can be used
   * to retrieve the complete data set in a single string of data lines.
   */
  struct CSVData
    : std::vector<CSVLine>
    {
      using VecCSV = std::vector<CSVLine>;
      
      CSVData (std::initializer_list<string> lines)
        : VecCSV(detectHeader(lines))
        { }
      
      CSVData (std::initializer_list<string> header
              ,std::initializer_list<CSVLine> data)
        {
          reserve (data.size()+1);
          appendHeaderLine(*this, header);
          for (CSVLine const& line : data)
            emplace_back (line);
        }
      
      // standard copy operations acceptable
      
      
      operator string()  const
        {
          return util::join (*this, "\n");
        }
      
      
    private:
      static bool
      containsCSV (string const& line)
        {
          return std::regex_search (line, FIND_DELIMITER_TOKEN);
        }
      
      static void
      appendHeaderLine (VecCSV& data, std::initializer_list<string> const& input)
        {
          CSVLine header;
          for (string const& s : input)
            header += s;
          data.emplace_back (move(header));
        }
      
      static VecCSV
      detectHeader (std::initializer_list<string> input)
        {
          VecCSV csv;
          if (input.size() > 0 and containsCSV(*input.begin()))
            {// the first line is a header => slurp in all as lines
              csv.reserve (input.size());
              for (string const& s : input)
                csv.emplace_back (s);
            }
          else // combine all strings into a single header line
            appendHeaderLine (csv, input);
          return csv;
        }
    };
  
  
  
  /** parse string representation into typed value */
  template<typename TAR>
  inline TAR
  parseAs (string const& encodedVal)
  {
      std::istringstream converter{encodedVal};
      TAR value;
      converter >> value;
      if (converter.fail())
          throw error::Invalid{_Fmt{"unable to parse \"%s\""} % encodedVal};
      return value;
  }
  
  template<>
  inline bool
  parseAs (string const& encodedBool)
  {
      return util::boolVal(encodedBool);
  }
  template<>
  inline string
  parseAs (string const& string)
  {
      return string; // pass-through (even if empty)
  }
  
  
  
  
  /**
   * Parser to split one line of CSV data into fields.
   * @remarks iterator-like throw-away object
   *  - the `bool` evaluation indicates more fields to extract
   *  - dereference to get the field as string
   *  - increment to move to the next field
   * @throws error::Invalid on CSV format violation
   */
  class CsvParser
    : public util::RegexSearchIter
    {
      string const& line_{};
      size_t   field_{0};
      size_t   pos_{0};
      
      util::RegexSearchIter const& curr() const { return *this; }
      util::RegexSearchIter        end()  const { return util::RegexSearchIter{}; }

    public:
      CsvParser()
        : line_{lib::NullValue<string>::get()}
        { }
      
      CsvParser (string& line) // NOTE: string and reg-exp must exist elsewhere
        : RegexSearchIter(line, ACCEPT_FIELD)
        , line_{line}
        { }
      
      explicit operator bool()  const
        {
          return isValid();
        }

      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (CsvParser);
      
      
      string operator*()  const
        {
          if (not isValid()) fail();
          auto& mat = *curr();
          return mat[2].matched? mat[2]
                               : mat[1];
        }
      
      void
      operator++()
        {
          if (not isValid())
            fail();
          pos_ = curr()->position() + curr()->length();
          util::RegexSearchIter::operator ++();
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
          return curr() != end()
             and pos_ == size_t(curr()->position())
             and not curr()->empty();
        }
      
      bool
      isParseFail()  const
        {
          return curr() != end()
             and not isValid();
        }
      
      void
      fail()  const
        {
          if (curr() == end())
              if (pos_ >= line_.length())
                  throw error::Invalid{_Fmt{"Only %d data fields. Line:%s"}
                                           % field_ % line_};
              else
                  throw error::Invalid{_Fmt{"Garbage after last field. Line:%s|↯|%s"}
                                           % line_.substr(0,pos_) % line_.substr(pos_)};
          else
          if (pos_ != size_t(curr()->position()))
                  throw error::Invalid{_Fmt{"Garbage before field(%d):%s|↯|%s"}
                                           % (field_+1)
                                           % line_.substr(0,pos_) % line_.substr(pos_)};
          
          throw error::Invalid{_Fmt{"CSV parse floundered. Line:%s"} % line_};
        }
    };
  
}} // namespace lib::stat
#endif /*LIB_STAT_CSV_H*/
