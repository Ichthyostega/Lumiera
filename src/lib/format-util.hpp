/*
  FORMAT-UTIL.hpp  -  helpers for formatting and diagnostics

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file format-util.hpp
 ** Collection of small helpers and convenience shortcuts for diagnostics & formatting.
 ** - util::str() performs a failsafe to-String conversion, thereby preferring a
 **         built-in conversion operator, falling back to just a mangled type string.
 ** - util::join() generates an enumerating string from elements
 **         of an arbitrary sequence or iterable. Elements will be passed
 **         through our [generic string conversion](\ref util::toString)
 ** 
 ** @see FormatHelper_test
 ** @see [frontend for boost::format, printf-style](\ref format-string.hpp)
 ** 
 */


#ifndef LIB_FORMAT_UTIL_H
#define LIB_FORMAT_UTIL_H

#include "lib/meta/trait.hpp"
#include "lib/format-obj.hpp"
#include "lib/itertools.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <typeinfo>



namespace util {
  
  using lib::meta::can_IterForEach;
  using std::string;
  using std::forward;
  using std::move;
  
  
  
  namespace { // helper to convert arbitrary elements toString
    
    template<class CON>
    inline void
    do_stringify(CON&)
    { /* do nothing */ }
    
    template<class CON, typename X, typename...ELMS>
    inline void
    do_stringify(CON& container, X const& elm, ELMS const& ...args)
    {
      container += util::toString (elm);
      do_stringify (container, args...);
    }
    
    
    template<class CON, typename...ELMS>
    struct SeqContainer
      : CON
      {
        void
        operator+= (string&& s)
          {
            CON::push_back (move(s));
          }
      };
    
    // most common case: use a vector container...
    using std::vector;
    
    template<typename X, typename...ELMS>
    struct SeqContainer<vector<X>, ELMS...>
      :vector<X>
      {
        SeqContainer()
          {
            this->reserve(sizeof...(ELMS));
          }
        
        void
        operator+= (string&& s)
          {
            this->emplace_back (move(s));
          }
      };
  }//(end) stringify helper
  
  
  /** convert a sequence of elements to string
   * @param elms sequence of arbitrary elements
   * @tparam CON the container type to collect the results
   * @return a collection of type CON, initialised by the
   *         string representation of the given elements
   */
  template<class CON, typename...ELMS>
  inline CON
  collectStr(ELMS const& ...elms)
  {
    SeqContainer<CON,ELMS...> storage;
    do_stringify (storage, elms...);
    return CON {move(storage)};
  }
  
  /** standard setup: convert to string into a vector */
  template<typename...ELMS>
  inline vector<string>
  stringify (ELMS const& ...elms)
  {
    return collectStr<vector<string>> (elms...);
  }
  
  /** convert to string as transforming step in a pipeline
   * @param src a "Lumiera Forward Iterator" with arbitrary result type
   * @return a "Lumiera Forward Iterator" with string elements
   * @see FormatHelper_test::checkStringify()
   */
  template<class IT>
  inline auto
  stringify (IT&& src)
  {
    using Val = typename lib::meta::ValueTypeBinding<IT>::value_type;
    
    return lib::transformIterator(forward<IT>(src), util::toString<Val>);
  }
  
  
  
  namespace { // helper to build range iterator on demand
    template<class CON, typename TOGGLE = void>
    struct _RangeIter
      {
        using StlIter = typename CON::const_iterator;
        
        lib::RangeIter<StlIter> iter;
        
        _RangeIter(CON const& collection)
          : iter(begin(collection), end(collection))
          { }
      };
    
    template<class IT>
    struct _RangeIter<IT,   lib::meta::enable_if< can_IterForEach<IT>> >
      {
        IT iter;
        
        _RangeIter(IT&& srcIter)
          : iter(std::forward<IT>(srcIter))
          { }
        _RangeIter(IT const& srcIter)
          : iter(srcIter)
          { }
        
      };
  }//(end) join helper
  
  
  /**
   * enumerate a collection's contents, separated by delimiter.
   * @param coll something that is standard-iterable
   * @return all contents converted to string and joined into
   *         a single string, with separators interspersed.
   * @remarks based `ostringstream`; additionally, we use our
   *          [failsafe string conversion](\ref util::str),
   *          which in turn invokes custom string conversion,
   *          or lexical_cast as appropriate.
   * @remarks alternatively, the `boost::join` library function
   *          could be used, which works on _arbitrary sequences_,
   *          which incurs some additional weight (both in terms
   *          of header include and debug code size). And failures
   *          on template substitution tend to be hard to understand,
   *          since this _generic sequence_ concept is just so danm
   *          absolutely generic (In fact that was the reason why I
   *          gave up and just rolled our own `join` utility)
   */
  template<class CON>
  inline string
  join (CON&& coll, string const& delim =", ")
  {
    using Coll = typename lib::meta::Strip<CON>::TypePlain;
    _RangeIter<Coll> range(std::forward<CON>(coll));
    
    auto strings = stringify (std::move (range.iter));
    if (!strings) return "";
    
    std::ostringstream buffer;
    for ( ; strings; ++strings)
      buffer << *strings << delim;
    
    // chop off last delimiter
    size_t len = buffer.str().length();
    ASSERT (len >= delim.length());
    return buffer.str().substr(0, len - delim.length());
  }
  
  template<class X>
  inline string
  join (std::initializer_list<X> const&& ili, string const& delim =", ")
  {
    return join (ili, delim);
  }
  
  // Note: offering a variant of join with var-args would create lots of ambiguities
  
  /** shortcut: List in parentheses, separated by comma, using temporary vector */
  template<typename...ARGS>
  inline string
  joinArgList (ARGS const& ...args)
  {
    return "("+join (stringify (args...))+")";
  }
  
  /** shortcut: join directly with dashes */
  template<typename...ARGS>
  inline string
  joinDash (ARGS const& ...args)
  {
    return join (stringify (args...), "-");
  }
  
  /** shortcut: join directly with dots */
  template<typename...ARGS>
  inline string
  joinDot (ARGS const& ...args)
  {
    return join (stringify (args...), ".");
  }
  
  
} // namespace util
#endif /*LIB_FORMAT_UTIL_H*/
