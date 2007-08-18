/*
  TIME.hpp  -  unified representation of a time point, including conversion functions
 
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

} // namespace util
#endif /*UTIL_HPP_*/
