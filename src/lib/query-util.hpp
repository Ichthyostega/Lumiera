/*
  QUERY-UTIL.hpp  -  support for working with terms and queries

  Copyright (C)         Lumiera.org
    2008, 2012          Hermann Vosseler <Ichthyostega@web.de>

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


#ifndef LIB_QUERY_UTIL_H
#define LIB_QUERY_UTIL_H


#include "lib/symbol.hpp"

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
    
    
    const string extractID (Symbol, const string& termString);
    
    const string removeTerm (Symbol, string& termString);
    
    
    template<typename TY>
    const string
    buildTypeID()
    {
      string typeID (typeid(TY).name());
      normaliseID (typeID);
      return typeID;
    }
    
    
    
  }} // namespace lib::query
#endif
