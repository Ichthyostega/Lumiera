/*
  UTIL.hpp  -  collection of small helper functions used "everywhere"

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


#ifndef LIB_UTIL_H
#define LIB_UTIL_H

#include "include/limits.h"
#include "lib/hash-standard.hpp"

#include <set>
#include <string>
#include <algorithm>



namespace util {
  
  using std::string;
  
  
  template <class NUM>
  inline int
  sgn (NUM n)
  {
    return (n==0)? 0 :((n<0)? -1:+1 );
  }
  
  template <class N1, class N2>
  inline N1
  min (N1 n1, N2 n2)
  {
    return n2 < n1? N1(n2) : n1;
  }
  
  template <class N1, class N2>
  inline N1
  max (N1 n1, N2 n2)
  {
    return n1 < n2? N1(n2) : n1;
  }
  
  /** cut a numeric value to be >=0 */
  template <typename NUM>
  inline NUM 
  noneg (NUM val)
  {
    return (0<val? val : 0);
  }
  
  /** force a numeric to be within bounds, inclusively */
  template <typename NUM, typename NB>
  inline NUM 
  limited (NB lowerBound, NUM val, NB upperBound)
  {
    return min ( max (val, lowerBound)
               , upperBound);
  }
  
  /** positive integral number from textual representation
   * @return always a number, 0 in case of unparseable text,  
   *         limited to 0 <= num <= LUMIERA_MAX_ORDINAL_NUMBER */
  inline uint
  uNum (const char* pCStr)
  {
    if (!pCStr) return 0;
    int parsedNumber(std::atoi (pCStr));
    return limited (0, parsedNumber, LUMIERA_MAX_ORDINAL_NUMBER);
  }
  
  inline int
  sNum (const char* pCStr)
  {
    if (!pCStr) return 0;
    int parsedNumber(std::atoi (pCStr));
    return limited (-LUMIERA_MAX_ORDINAL_NUMBER, parsedNumber, LUMIERA_MAX_ORDINAL_NUMBER);
  }
  
  inline uint
  uNum (string const& spec)
  {
    return uNum (spec.c_str());
  }
  
  inline int
  sNum (string const& spec)
  {
    return sNum (spec.c_str());
  }
  
  
  
  /*  ======== generic empty check =========  */
  
  /** a family of util functions providing a "no value whatsoever" test.
      Works on strings and all STL containers, includes NULL test for pointers */
  template <class CONT>
  inline bool
  isnil (const CONT& container)
  {
    return container.empty();
  }
  
  template <class CONT>
  inline bool
  isnil (const CONT* pContainer)
  {
    return !pContainer || pContainer->empty();
  }
  
  template <class CONT>
  inline bool
  isnil (CONT* pContainer)
  {
    return !pContainer || pContainer->empty();
  }
  
  inline bool
  isnil (const char* pCStr)
  {
    return !pCStr || !(*pCStr);
  }
  
  
  
  /** check if string starts with a given prefix */
  inline bool
  startsWith (string const& str, string const& prefix)
  {
    return 0 == str.rfind(prefix, 0);
  }
  
  inline bool
  startsWith (string const& str, const char* prefix)
  {
    return 0 == str.rfind(prefix, 0);
  }
  
  
  /** shortcut for containment test on a map */
  template <typename MAP>
  inline bool 
  contains (MAP& map, typename MAP::key_type const& key)
  {
    return map.find(key) != map.end();
  }
  
  /** shortcut for set value containment test */
  template <typename T>
  inline bool 
  contains (std::set<T>& set, T const& val)
  {
    return set.end() != set.find (val);
  }
  
  /** shortcut for string value containment test */
  template <typename T>
  inline bool 
  contains (std::string const& str, const T& val)
  {
    return str.find (val) != std::string::npos;
  }
  
  /** shortcut for brute-force containment test
   *  in any sequential container */
  template <typename SEQ>
  inline bool 
  contains (SEQ const& cont, typename SEQ::const_reference val)
  {
    typename SEQ::const_iterator begin = cont.begin();
    typename SEQ::const_iterator end   = cont.end();
    
    return end != std::find(begin,end, val);
  }
  
  /** fetch value from a Map, or return a default if not found */
  template <typename MAP>
  inline typename MAP::mapped_type
  getValue_or_default (MAP& map, typename MAP::key_type const& key
                               , typename MAP::mapped_type defaultVal)
  {
    typename MAP::const_iterator pos = map.find (key);
    if (pos != map.end())
      return pos->second;
    else
      return defaultVal;
  }
  
  /** expose a reference to a map entry, with fall-back to some default object
   * @note exposing a const reference; especially the default needs to reside
   *       somewhere at a safe storage location.
   * @see lib::NullValue
   */
  template <typename MAP>
  inline typename MAP::mapped_type const &
  access_or_default (MAP& map, typename MAP::key_type const& key
                             , typename MAP::mapped_type const&  refDefault)
  {
    typename MAP::const_iterator pos = map.find (key);
    if (pos != map.end())
      return pos->second;
    else
      return refDefault;
  }
  
  /** shortcut for removing all copies of an Element
   *  in any sequential collection */
  template <typename SEQ>
  inline typename SEQ::iterator 
  removeall (SEQ& coll, typename SEQ::value_type& val)
  {
    typename SEQ::iterator collEnd = coll.end();
    return coll.erase (std::remove (coll.begin(), collEnd, val),
                       collEnd
                      );
  }
  
  
  /** remove all elements fulfilling a given predicate
   *  from a (sorted) set.
   *  @return true if anything has been removed. */
  template<class SET, typename PRD>
  bool remove_if (SET& set, PRD test)
    {
      typedef typename SET::iterator Itor;
      bool found = false;
      Itor   end = set.end();
      Itor begin = set.begin();
      Itor   pos = begin;
      while (pos!=end)
        {
          if (!test (*pos)) ++pos;
          else
            {
              found = true;
              if (pos==begin) 
                {
                  set.erase(pos);
                  pos = begin = set.begin();
                }
              else
                {
                  set.erase(pos--);
                  ++pos;
                }
              end = set.end();
        }   }
      return found;
    }
  
  
  /** shortcut to save some typing when having to define
   *  const and non-const variants of member functions
   */
  template<class OBJ>
  OBJ*
  unConst (const OBJ* o)
  {
    return const_cast<OBJ*> (o);
  }
  
  template<class OBJ>
  OBJ&
  unConst (OBJ const& ro)
  {
    return const_cast<OBJ&> (ro);
  }
  
  
  /** compare plain object identity,
   *  bypassing any custom comparison operators.
   */
  template<class A, class B>
  inline bool
  isSameObject (A const& a, B const& b)
  {
    return static_cast<const void*> (&a)
        == static_cast<const void*> (&b);
  }
  
  
  
  
  /** produce an identifier based on the given string.
   *  remove non-standard-chars, reduce sequences of punctuation 
   *  and whitespace to single underscores. The sanitised string
   *  will start with an alphanumeric character.
   * 
   * @par Example Conversions
\verbatim
   "Word"                             --> 'Word'
   "a Sentence"                       --> 'a_Sentence'
   "trailing Withespace  \t \n"       --> 'trailing_Withespace'
   "with    a   lot  \nof Whitespace" --> 'with_a_lot_of_Whitespace'
   "with\"much (punctuation)[]!"      --> 'withmuch_(punctuation)'
   "§&Ω%€  leading garbage"           --> 'leading_garbage'
   "mixed    Ω   garbage"             --> 'mixed_garbage'
   "Bääääh!!"                         --> 'Bh'
\endverbatim
   * @see sanitised-identifier-test.cpp
   */
  string sanitise (string const& org);
  
  
  
  
  /** convenience shortcut: conversion to c-String via string.
   *  usable for printf with objects providing to-string conversion.
   */
  inline const char* 
  cStr (string const& org)
  {
    return org.c_str();
  }
  
  
} // namespace util



 /* === some common macro definitions === */

/** suppress "warning: unused variable" on vars, which are
 *  introduced into a scope because of some sideeffect, i.e. Locking
 */
#define SIDEEFFECT __attribute__ ((unused));

/** this macro wraps its parameter into a cstring literal */
#define STRINGIFY(TOKEN) __STRNGFY(TOKEN)
#define __STRNGFY(TOKEN) #TOKEN

/** shortcut for subclass test, intended for assertions only.
 *  @note it is considered bad style to use such in non-assertion code,
 *        and we probably will enforce this design rule in future. */
#define INSTANCEOF(CLASS, EXPR) (dynamic_cast<const CLASS*> (EXPR))

/** the inevitable MAX macro, sometimes still necessary in template code */
#ifndef MAX   // this is present to make this definition play nice with GLib
#define MAX(A,B) ((A < B) ?  (B) : (A))
#endif

#endif /*UTIL_HPP_*/
