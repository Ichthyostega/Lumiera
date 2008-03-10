/*
  Query  -  interface for capability queries
 
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
 
* *****************************************************/


#include "common/query.hpp"
#include "common/util.hpp"
#include "nobugcfg.h"

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <map>

using std::map;
using boost::regex;
using boost::smatch;
using boost::regex_search;
using boost::algorithm::is_upper;
using boost::algorithm::is_alpha;

using util::contains;


namespace lumiera
  {
  
  namespace query
    {
    
    void
    normalizeID (string& id)
    {
      id = util::sanitize(id);
      REQUIRE (!util::isnil(id));
      REQUIRE (is_alpha() (id[0]));
      
      char first = id[0];
      if (is_upper() (first))
        id[0] = std::tolower (first);
    }

    
    
    
    namespace // Implementation details
      {
      map<Symbol, regex> regexTable;
    }
    
    /** (preliminary) helper: instead of really parsing and evaluating the terms,
     *  just do a regular expression match to extract the literal argument 
     *  behind the given predicate symbol. e.g calling
     *  queryID ("stream", "id(abc), stream(mpeg)") 
     *  yields "mpeg"
     */
    const string
    extractID (Symbol sym, const string& termString)
    {
      
      if (!contains (regexTable, sym))
        regexTable[sym] = regex (string(sym)+="\\(\\s*([\\w_\\.\\-]+)\\s*\\)"); 
      
      smatch match;
      if (regex_search (termString, match, regexTable[sym]))
        return string (match[1]);
      else
        return "";
    
  } 
  
  } // namespace query
    
  
  /** */


} // namespace lumiera
