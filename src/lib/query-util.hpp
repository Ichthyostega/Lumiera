/*
  QUERY-UTIL.hpp  -  support for working with terms and queries

   Copyright (C)
     2008, 2012       Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file query-util.hpp
 ** Utilities to support working with predicate queries
 */


#ifndef LIB_QUERY_UTIL_H
#define LIB_QUERY_UTIL_H


#include "lib/symbol.hpp"
#include "lib/meta/util.hpp"

#include <typeinfo>
#include <string>


namespace lib {
  
  using lib::Symbol;
  using lib::Literal;
  using std::string;
  
  
  
  
  
  
  namespace query {
    
    /** ensure standard format for a given id string.
     *  Trim, sanitise and ensure the first letter is lower case.
     *  @note modifies the given string ref in place
     */
    void normaliseID (string& id);
    
    
    /** count the top-level predicates in the query string.
     *  usable for ordering queries, as more predicates usually
     *  mean more conditions, i.e. more constriction
     */
    uint countPred (const string&);
    
    
    string extractID (Symbol, string const& termString);
    
    string removeTerm (Symbol, string& queryString);
    bool hasTerm (Symbol sym, string const& queryString);
    
    string appendTerms (string const& pred1, string const& pred2);
    
    
    template<typename TY>
    const string
    buildTypeID()
    {
      string typeID {util::typeStr<TY>()};
      normaliseID (typeID);
      return typeID;
    }
    
    
    
  }} // namespace lib::query
#endif
