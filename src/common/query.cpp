/*
  Query  -  interface for capability queries
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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

using boost::algorithm::is_upper;
using boost::algorithm::is_alpha;


namespace cinelerra
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
  
  } // namespace query
    
  
  /** */


} // namespace cinelerra
