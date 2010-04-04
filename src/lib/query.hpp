/*
  QUERY.hpp  -  interface for capability queries
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/


#ifndef LUMIERA_QUERY_H
#define LUMIERA_QUERY_H


#include <string>
#include <typeinfo>

#include <boost/format.hpp>

#include "lib/symbol.hpp"


namespace lumiera {
  
  using lib::Symbol;
  using lib::Literal;
  using std::string;
  using boost::format;
  
  /* ==== common definitions for rule based queries ==== */
  
  
  
  /**
   * Generic query interface for retrieving objects matching
   * some capability query
   */
  template<class OBJ>
  class Query : public std::string
    {
    public:
      explicit Query (string const& predicate="") : string(predicate) {}
      explicit Query (format& pattern)            : string(str(pattern)) {}
      
      const string asKey()  const
        {
          return string(typeid(OBJ).name())+": "+*this;
        }
      
      operator string& () { return *this; }      // TODO: needed temporarily by fake-configrules
    };                                          //        for calling removeTerm on the string-ref....
  
  
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
    uint countPraed (const string&);
    
    
    const string extractID (Symbol, const string& termString);
    
    const string removeTerm (Symbol, string& termString);
    
    
}} // namespace lumiera::query
#endif
