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
 ** - util::tyStr() generates a string corresponding to the type of the given object.
 **         Currently just implemented through the mangled RTTI type string  
 ** 
 ** @see FormatHelper_test
 ** @see format-string.hpp frontend for boost::format, printf-style
 ** 
 */


#ifndef LIB_FORMAT_UTIL_H
#define LIB_FORMAT_UTIL_H

#include "lib/hash-standard.hpp"
#include "lib/meta/trait.hpp"
#include "lib/itertools.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <string>
#include <sstream>
#include <cstring>
#include <utility>
#include <typeinfo>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>


namespace lib {
namespace test{ // see test-helper.cpp
    std::string demangleCxx (lib::Literal rawName);
}}


namespace util {
  
  using boost::enable_if;
  using lib::meta::can_ToString;
  using lib::meta::can_lexical2string;
  using lib::meta::can_IterForEach;
  using lib::Symbol;
  using util::isnil;
  using std::string;
  using std::move;
  
  
  namespace { // we need to guard the string conversion
             //  to avoid a compiler error in case the type isn't convertible....
    
    // precision for rendering of double values
    const auto DIAGNOSTICS_DOUBLE_PRECISION = 8;
    const auto DIAGNOSTICS_FLOAT_PRECISION  = 5;
    
    
    template<typename X>
    struct use_StringConversion : can_ToString<X> { };
    
    template<typename X>
    struct use_LexicalConversion
      {
        enum { value = can_lexical2string<X>::value
                  &&  !can_ToString<X>::value
             };
      };
    
    
    /** helper: reliably get some string representation for type X */
    template<typename X, typename COND =void>
    struct _InvokeFailsafe
      {
        static string toString (X const&) { return ""; }
      };
    
    template<typename X>
    struct _InvokeFailsafe<X,     typename enable_if< use_StringConversion<X> >::type>
      {
        static string
        toString (X const& val)
          try        { return string(val); }
          catch(...) { return ""; }
      };
    
    template<typename X>
    struct _InvokeFailsafe<X,     typename enable_if< use_LexicalConversion<X> >::type>
      {
        static string
        toString (X const& val)
          try        { return boost::lexical_cast<string> (val); }
          catch(...) { return ""; }
      };
    
    /** explicit specialisation to control precision of double values.
     * @note we set an explicit precision, since this is a diagnostic facility
     *       and we typically do not want to see all digits, but, for test code,
     *       we do want a predictable string representation of simple fractional
     *       values like `0.1` (which can not be represented as binary floats)
     */
    template<>
    struct _InvokeFailsafe<double>
      {
        static string
        toString (double const& val)
          try {
              std::ostringstream buffer;
              buffer.precision(DIAGNOSTICS_DOUBLE_PRECISION);
              buffer << val;
              return buffer.str();
            }
          catch(...) { return ""; }
      };
    template<>
    struct _InvokeFailsafe<float>
      {
        static string
        toString (float const& val)
          try {
              std::ostringstream buffer;
              buffer.precision(DIAGNOSTICS_FLOAT_PRECISION);
              buffer << val;
              return buffer.str();
            }
          catch(...) { return ""; }
      };
  }//(End) guards/helpers
  
  
  /** @return a string denoting the type. */
  template<typename TY>
  inline string
  tyStr (const TY* =0)
  {
    return "«"+ lib::test::demangleCxx (typeid(TY).name())+"»";
  }
  
  template<typename TY>
  inline string
  tyStr (TY const& ref)
  { return tyStr(&ref); }
  

  /** try to get an object converted to string.
   *  A custom/standard conversion to string is used,
   *  if applicable; otherwise, some standard types can be
   *  converted by a lexical_cast (based on operator<< ).
   *  Otherwise, either the fallback string is used, or just
   *  a string based on the (mangled) type.
   */
  template<typename TY>
  inline string
  str ( TY const& val
      , Symbol prefix=""      ///< prefix to prepend in case conversion is possible
      , Symbol fallback =0   /// < replacement text to show if string conversion fails
      )
  {
    string res = _InvokeFailsafe<TY>::toString(val);
    if (!isnil (res))
      return string(prefix) + res;
    else
      return fallback? string(fallback)
                     : tyStr(val);
  }
  
  
  
  namespace { // helper to convert arbitrary elements toString
    
    template<class CON>
    inline void
    do_stringify(CON&)
    { /* do nothing */ }
    
    template<class CON, typename X, typename...ELMS>
    inline void
    do_stringify(CON& container, X const& elm, ELMS const& ...args)
    {
      container += util::str(elm);
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
  }
  
  
  /** convert a sequence of elements to string
   * @param elms sequence of arbitrary elements
   * @return a collection of type CON, initialised by the
   *         string representation of the given elements
   */
  template<class CON, typename...ELMS>
  inline CON
  stringify(ELMS const& ...elms)
  {
    SeqContainer<CON,ELMS...> storage;
    do_stringify (storage, elms...);
    return CON {move(storage)};
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
    struct _RangeIter<IT,   typename enable_if< can_IterForEach<IT> >::type>
      {
        IT iter;
        
        _RangeIter(IT&& srcIter)
          : iter(std::forward<IT>(srcIter))
          { }
        _RangeIter(IT const& srcIter)
          : iter(srcIter)
          { }
        
      };
  }
  
  /**
   * enumerate a collection's contents, separated by delimiter.
   * @param coll something that is standard-iterable
   * @return all contents converted to string and joined into
   *         a single string, with separators interspersed.
   * @remarks based on the \c boost::join library function,
   *          which in turn is based on
   *          additionally, we use our
   *          \link #str failsafe string conversion \endlink
   *          which in turn invokes custom string conversion,
   *          or lexical_cast as appropriate.
   */
  template<class CON>
  inline string
  join (CON&& coll, string const& delim =", ")
  {
    using Coll = typename lib::meta::Strip<CON>::Type;
    using Val =  typename Coll::value_type;
    
    std::function<string(Val const&)> toString = [] (Val const& val) { return str(val); };
    
    _RangeIter<Coll> range(std::forward<CON>(coll));
    auto strings = lib::transformIterator(range.iter, toString);
    
    if (!strings) return "";
    
    std::ostringstream buffer;
    for (string const& elm : strings)
        buffer << elm << delim;
    
    // chop off last delimiter
    size_t len = buffer.str().length();
    ASSERT (len > delim.length());
    return buffer.str().substr(0, len - delim.length());
  }
  
  
} // namespace util
#endif /*LIB_FORMAT_UTIL_H*/
