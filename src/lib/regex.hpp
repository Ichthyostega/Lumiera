/*
  REGEX.hpp  -  helpers for working with regular expressions

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


/** @file regex.hpp
 ** Convenience wrappers and helpers for dealing with regular expressions.
 */


#ifndef LIB_REGEX_H
#define LIB_REGEX_H


#include "lib/iter-adapter.hpp"

#include <regex>
#include <string>
#include <optional>

namespace util {
  
  using std::regex;
  using std::smatch;
  using std::string;
  using std::string_view;
  
  
  
  /** wrapped regex iterator to allow usage in foreach loops */
  struct RegexSearchIter
    : std::sregex_iterator
    {
      RegexSearchIter()  = default;
      
      RegexSearchIter (string const& toParse, regex const& regex)
        : std::sregex_iterator{toParse.begin(), toParse.end(), regex}
        { }
      RegexSearchIter (string_view toParse, regex const& regex)
        : std::sregex_iterator{string::const_iterator{toParse.begin()}
                              ,string::const_iterator{toParse.end()}, regex}
        { }
      
      operator bool()  const { return isValid(); }
      
      bool isValid ()  const { return (*this)->ready() and not (*this)->empty(); }
      bool empty ()    const { return not isValid(); }

      LIFT_PARENT_INCREMENT_OPERATOR (std::sregex_iterator);
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (RegexSearchIter);
    };
  
}// namespace util

namespace lib {
  using std::regex;
  using std::smatch;
  using std::string;
}// namespace lib
#endif/*LIB_STAT_REGEX_H*/
