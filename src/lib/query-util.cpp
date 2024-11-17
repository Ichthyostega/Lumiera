/*
  QueryUtil  -  support for working with terms and queries

   Copyright (C)
     2008, 2012       Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file query-util.cpp
 ** Implementation of helpers for working with predicate queries.
 */


#include "lib/error.hpp"
#include "lib/query-util.hpp"
#include "lib/util.hpp"

#include <boost/algorithm/string.hpp>
#include <functional>
#include <regex>
#include <map>

using std::map;
using std::regex;
using std::smatch;
using std::regex_search;
using std::sregex_iterator;

using util::contains;
using util::isnil;


namespace lib {
  namespace query {
    
    namespace { // local definitions
      
      using ChPredicate = std::function<bool(string::value_type)> ;
      
      ChPredicate is_alpha = boost::algorithm::is_alpha();    
      ChPredicate is_upper = boost::algorithm::is_upper();    
    } // local defs
    
    
    void
    normaliseID (string& id)
    {
      id = util::sanitise(id);
      if (isnil(id) || !is_alpha (id[0]))
        id.insert(0, "o");
      
      
      REQUIRE (!isnil(id));
      REQUIRE (is_alpha (id[0]));
      
      char first = id[0];
      if (is_upper (first))
        id[0] = std::tolower (first);
    }
    
    
    
                                                                          //////////////////////TICKET #613 : centralise generally useful RegExps
    namespace{  // Implementation details
      
      map<Symbol, regex> regexTable;
      
      Literal MATCH_ARGUMENT = R"~(\(\s*([\w_\.\-]+)\s*\),?\s*)~";
      const regex FIND_PREDICATE{string{"(\\w+)"} + MATCH_ARGUMENT};
      
      inline regex&
      getTermRegex (Symbol sym)
      {
        if (!contains (regexTable, sym))
          regexTable[sym] = regex (string(sym)+MATCH_ARGUMENT);
        return regexTable[sym];
      }
    }
    
    /** (preliminary) helper: instead of really parsing and evaluating the terms,
     *  just do a regular expression match to extract the literal argument 
     *  behind the given predicate symbol. e.g calling
     *  `extractID ("stream", "id(abc), stream(mpeg)")` yields \c "mpeg"
     */
    string
    extractID (Symbol sym, const string& termString)
    {
      smatch match;
      if (regex_search (termString, match, getTermRegex (sym)))
        return (match[1]);
      else
        return "";
    } 
    
    
    /** (preliminary) helper: cut a term with the given symbol. 
     *  The term is matched, removed from the original string and returned
     *  @note parameter termString will be modified!
     *  @todo as it seems we're not using the extracted term anymore,
     *        we could save the effort of rebuilding that term.
     */
    string
    removeTerm (Symbol sym, string& queryString)
    {
      smatch match;
      if (regex_search (queryString, match, getTermRegex (sym)))
        {
          string res (sym); res += "("+match[1]+")";
          queryString.erase (match.position(), match[0].length());
          return res;
        }
      else
        return "";
    }
    
    
    bool
    hasTerm (Symbol sym, string const& queryString)
    {
      smatch match;
      return regex_search (queryString, match, getTermRegex (sym));
    }
  
    
    /** @note this is a very hackish preliminary implementation. 
     *  The regex used will flounder when applied to nested terms. 
     *  We need a real parser for predicate logic terms (which we
     *  probably get for free when we embed a prolog system)...
     */
    uint 
    countPred (const string& q)
    {
      uint cnt (0);
      sregex_iterator end;
      for (sregex_iterator i (q.begin(),q.end(), FIND_PREDICATE); 
           i != end; ++i)
        ++cnt;
      return cnt;
    }
    
    
    /** @note preliminary implementation without any syntax checks
     * @return a conjunction of the predicates
     */
    string
    appendTerms (string const& pred1, string const& pred2)
    {
      return isnil(pred1)? pred2
           : isnil(pred2)? pred1
                         : pred1 + ", " + pred2;
    }
    
    
  } // namespace query
  
} // namespace lib
