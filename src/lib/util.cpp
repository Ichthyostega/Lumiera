/*
  util.cpp  -  helper functions implementation

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file util.cpp
 ** Implementation of widely used helper functions.
 ** The helpers implemented here are typically accessed by including util.hpp,
 ** so the reason of pushing down the implementation into a separate translation unit
 ** (this one here) is to avoid more heavyweight includes (e.g. boost).
 */


#include "error.hpp"
#include "lib/util.hpp"
#include "lib/format-string.hpp"

#include <boost/algorithm/string.hpp>
#include <functional>
#include <boost/bind.hpp>          // we need operator! for bind-expressions

using boost::algorithm::trim_right_copy_if;
using boost::algorithm::is_any_of;
using boost::algorithm::is_alnum;
using boost::algorithm::is_space;

#include <regex>

using std::regex;
using std::regex_match;

using std::function;
using util::_Fmt;

namespace util {

  
  using ChPredicate = function<bool(string::value_type)>;
  ChPredicate operator! (ChPredicate p) { return not bind(p,_1); }

  // character classes used for sanitising a string
  ChPredicate isValid (is_alnum() or is_any_of("-_.+$()@"));                ///< characters to be retained
  ChPredicate isPunct (is_space() or is_any_of(",;:#*~´`?\\=/&%![]{}<>")); ///<  punctuation to be replaced by '_'

  
  string
  sanitise (string const& org)
  {
    string res (trim_right_copy_if(org, !isValid ));
    string::iterator       j = res.begin();
    string::const_iterator i = org.begin();
    string::const_iterator e = i + (res.length());
    while ( i != e )
      {
        while ( i != e && !isValid (*i) )   ++i;
        while ( i != e && isValid  (*i) )   *(j++) = *(i++);
        if    ( i != e && isPunct  (*i) )
          {
            *j++ = '_';
            do  ++i;
            while ( i != e && isPunct (*i));
          }
      }
    res.erase(j,res.end());
    return res;
  }
  
  
  /**
   * @remarks this function just forwards to boost::algorithm::trim_copy.
   *          Use this call when boost header inclusion is an issue, otherwise
   *          a direct invocation is likely to perform better, due to inlining.
   */
  string
  trim (string const& org)
  {
    return boost::algorithm::trim_copy (org);
  }
  
  
  
  
  namespace {
    regex trueTokens { "\\s*(true|yes|on|1|\\+)\\s*",  regex::icase | regex::optimize };
    regex falseTokens{ "\\s*(false|no|off|0|\\-)\\s*", regex::icase | regex::optimize };
  }
  
  bool
  boolVal (string const& textForm)
  {
    if (regex_match (textForm, trueTokens))  return true;
    if (regex_match (textForm, falseTokens)) return false;
    throw lumiera::error::Invalid(_Fmt{"String '%s' can not be interpreted as bool value"} % textForm);
  }
  
  
  bool
  isYes (string const& textForm) noexcept
  {
    return regex_match (textForm, trueTokens);
  }
  
  
  bool
  isNo (string const& textForm) noexcept
  {
    return isnil (textForm)
        or regex_match (textForm, falseTokens);
  }
  

  
} // namespace util

