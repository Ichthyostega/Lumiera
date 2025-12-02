/*
  REGEX.hpp  -  helpers for working with regular expressions

   Copyright (C)
     2022,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
  
  
  /**
   * Helper algorithm to perform a search but require the match to start
   * at the beginning of the string or sequence, while accepting trailing content.
   * @param toParse a string, string-view or something that can be converted to string.
   * @return a std::optional with the match result (`std::smatch`).
   */
  template<typename STR>
  std::optional<smatch>
  matchAtStart (STR&& toParse, regex const& regex)
  {
    auto search = RegexSearchIter{std::forward<STR> (toParse), regex};
    if (search and 0 == search->position(0))
      return *search;
    else
      return std::nullopt;
  }
  
  /** @return number of leading whitespace characters */
  template<typename STR>
  size_t
  leadingWhitespace (STR&& toParse)
  {
    static const regex LEADING_WHITESPACE{"^\\s*", regex::optimize};
    auto search = RegexSearchIter{std::forward<STR> (toParse), LEADING_WHITESPACE};
    return search? search->length() : 0;
  }
  
}// namespace util

namespace lib {
  using std::regex;
  using std::smatch;
  using std::string;
}// namespace lib
#endif/*LIB_REGEX_H*/
