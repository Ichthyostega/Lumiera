/*
  QueryText  -  syntactical standard representation for queries

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


//#include "lib/util.hpp"
//#include "lib/symbol.hpp"
//#include "include/logging.h"
#include "lib/query-text.hpp"

//#include <boost/noncopyable.hpp>
#include <boost/functional/hash.hpp>
#include <string>
//#include <map>

//using std::map;
using std::string;

//using util::contains;
//using util::isnil;

namespace lib {
  
  namespace { // internal details
  
  } // internal details
  
  
  /** support using queries in hashtables. 
   * @warning right now (2012) the dummy implementation of QueryText
   *          doesn't normalise the query in any way, which makes the
   *          generated hash value dependent on the actual textual
   *          form used to build the QueryText. This is not how
   *          it's intended to work, it should rely on a normalised
   *          form after parsing the query definition.
   */
  HashVal
  hash_value (QueryText const& entry)
  {
    return boost::hash_value (entry.definition_);
  }
  

} // namespace lib
