/*
  UTIL.hpp  -  collection of small helper functions used "everywhere"
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <string>
#include <cstring>


namespace util
  {
  using std::string;


  /** a family of util functions providing a "no value whatsoever" test */
  inline bool isnil(const string& val)
    {
      return 0 == val.length();
    }
  
  inline bool isnil(const string* pval)
    {
      return !pval || 0 == pval->length();
    }
  
  inline bool isnil(const char* pval)
    {
      return !pval || 0 == std::strlen(pval);
    }

  
  /** cut a numeric value to be >=0 */
  template <typename NUM>
  inline NUM noneg (NUM val)
    {
      return (0<val? val : 0);
    }
  
  /** shortcut for containment test on a map */
  template <typename MAP>
  inline bool contains (MAP& map, typename MAP::key_type& key)
    {
      return map.find(key) != map.end();
    }
  
  /** shortcut for operating on all elements of a container.
   *  Isn't this already defined somewhere? It's so obvious..
   */
  template <typename Container, typename Oper>
  inline Oper
  for_each (Container& c, Oper& doIt)
    {
      return std::for_each (c.begin(),c.end(), doIt);
    }
   
  
} // namespace util

 /* some common macro definitions */

/** this macro wraps its parameter into a cstring literal */
#define STRINGIFY(TOKEN) __STRNGFY(TOKEN)
#define __STRNGFY(TOKEN) #TOKEN


#endif /*UTIL_HPP_*/
