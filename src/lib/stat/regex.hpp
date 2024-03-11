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
 ** 
 ** @todo 3/2024 should be with the generic utils, might be a Lumiera Forward Iterator
 */


#ifndef LIB_STAT_REGEX_H
#define LIB_STAT_REGEX_H


#include <regex>
#include <string>
#include <optional>

namespace util {
  
  using std::regex;
  using std::smatch;
  using std::string;
  
  
  
  /** wrapped regex iterator to allow usage in foreach loops */
  struct MatchSeq
    : std::sregex_iterator
    {
      MatchSeq (string const& toParse, regex const& regex)
        : std::sregex_iterator{toParse.begin(), toParse.end(), regex}
        { }
  
      using iterator = std::sregex_iterator;
      iterator begin() const { return *this; }
      iterator end()   const { return iterator(); }
    };
  
} // namespace util
#endif/*LIB_STAT_REGEX_H*/
