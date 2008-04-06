/*
  QUERYDIAGNOSTICS.hpp  -  herlpers for writing tests covering config queries
 
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


#ifndef LUMIERA_TEST_QUERY_QUERYDIAGNOSTICS_H
#define LUMIERA_TEST_QUERY_QUERYDIAGNOSTICS_H


//#include "common/factory.hpp"
//#include "common/util.hpp"

//#include <boost/algorithm/string.hpp>
//#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
//#include <iostream>
#include <cstdlib>

//using boost::algorithm::join;
//using boost::lexical_cast;
using boost::format;
using std::string;
//using std::cout;
using std::rand;



namespace lumiera
  {
  namespace query
    {
    namespace test
      {
      
      namespace // implementation constants
        {
        
        format predicatePatt ("%s_%2i( %s )");
        const string garbage ("asdfghjklqwertzuiop");
  
        const uint MAX_DEGREE_RAND = 9;
        
      }
      
      
      
      inline string
      garbage_term ()         ///< yields a random string of 3 letters
      {
        return str (predicatePatt 
                   % char ('a'+ rand() % 26)
                   % (rand() % 100)
                   % garbage.substr(rand() % 17 , 3)
                   );
      }
      
      inline string
      garbage_query (int degree=0)    ///< fabricating (random) query strings
      {
        string fake;
        if (!degree) 
          degree = 1 + rand() % MAX_DEGREE_RAND;
        while (0 < --degree)
          fake += garbage_term() + ", ";
        fake += garbage_term() + ".";
        return fake;
      }
      
      
      
      
    } // namespace test
    
  } // namespace query

} // namespace lumiera
#endif
