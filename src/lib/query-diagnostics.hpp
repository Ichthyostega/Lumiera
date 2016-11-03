/*
  QUERY-DIAGNOSTICS.hpp  -  helpers for writing tests covering config queries

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file query-diagnostics.hpp
 ** TODO query-diagnostics.hpp
 */


#ifndef LIB_QUERY_DIAGNOSTICS_H
#define LIB_QUERY_DIAGNOSTICS_H


#include "lib/format-string.hpp"

using util::_Fmt;
using std::string;
using std::rand;



namespace lib  {
namespace query{
namespace test {
  
  namespace {// implementation constants
    
    _Fmt predicatePattern ("%s_%02i( %s )");
    const string garbage ("asanisimasabibeddiboom");
    
    const uint MAX_DEGREE_RAND = 9;
    
  }
  
  
  
  inline string
  garbage_term ()         ///< yields a random string of 3 letters
  {
    return predicatePattern
         % char ('a'+ rand() % 26)
         % (rand() % 100)
         % garbage.substr(rand() % 19 , 3);
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
  
  
  
  
}}} // namespace lib::query::test
#endif
