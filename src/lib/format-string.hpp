/*
  FORMAT-STRING.hpp  -  string template formatting based on boost::format

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file format-string.hpp
 ** Front-end for printf-style string template interpolation.
 ** While the actual implementation just delegates to boost::format, this front-end
 ** hides the direct dependency, additionally invokes a custom toSting conversion
 ** whenever possible, provides a direct conversion to string and catches any exceptions.
 ** This front-end is used pervasively for diagnostics and logging, so keeping down the
 ** compilation and object size cost and reliably handling any error is more important
 ** than the (small) performance gain of directly invoking boost::format (which is
 ** known to be 10 times slower than printf anyway).
 ** 
 ** @remarks The implementation is invoked through a set of explicit specialisations.
 **          For custom types, we prefer to invoke operator string(), which is determined
 **          by a directly coded metaprogramming test. The compile time and object size
 **          overhead incurred by using this header was verified to be negligible. 
 ** 
 ** @see FormatString_test
 ** @see format-helper
 ** 
 */


#ifndef UTIL_FORMAT_STRING_H
#define UTIL_FORMAT_STRING_H

//#include "lib/meta/trait.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"
#include "lib/error.hpp"

#include <string>
//#include <cstring>
#include <typeinfo>
//#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include <boost/utility/enable_if.hpp>



namespace std { // forward declaration to avoid including <iostream>
  
  template<typename C>
  class char_traits;
  
  template<typename C, class _TRAITS>
  class basic_ostream;
  
  typedef basic_ostream<char, char_traits<char> > ostream;
}



namespace util {
  
  using boost::enable_if;
//using util::isnil;
  using std::string;
  
  
  
  
  
  
  /** 
   * @todo write type comment
   */
  class _Fmt
    : boost::noncopyable
    {
      enum{ FORMATTER_SIZE = 100 };

      
      
      char formatter_[FORMATTER_SIZE];
      
      template<typename VAL, class SEL =void>
      struct Converter;
      
      template<typename VAL>
      void pushParameter (VAL const&);
      template<typename VAL>
      void pushParameter (const VAL * const);
      
      
    public:
      _Fmt (string formatString);
      
      operator string()  const;
      
      template<typename VAL>
      _Fmt&
      operator% (VAL const&);
      
      
      friend std::ostream&
      operator<< (std::ostream& os, _Fmt const&);
      
      friend bool operator== (_Fmt const&,        _Fmt const&);
      friend bool operator== (_Fmt const&,      string const&);
      friend bool operator== (_Fmt const&, const char * const);
      friend bool operator== (string const&     , _Fmt const&);
      friend bool operator== (const char * const, _Fmt const&);
      
      template<typename X>
      friend bool operator != (_Fmt const& fmt, X const& x) { return !(fmt == x); }
      template<typename X>
      friend bool operator != (X const& x, _Fmt const& fmt) { return !(x == fmt); }
    };
  
  
  
  
  
  /* ===== forwarding into the implementation ====== */
  
  /** The percent operator (\c '%' ) is used do feed 
   *  additional parameter values to be included into 
   *  the formatted result, at the positions marked 
   *  within the format string.   
   * 
   * \par type specific treatment
   * Basic types (numbers, chars, strings) are passed to the implementation
   * (= boost::format) literally. For custom types, we try to use an custom
   * string conversion, if applicable. Any other type gets just translated
   * into a type-ID (using the mangled RTTI info). In case of errors during
   * the conversion, a string representation of the error is returned
   * @param val arbitrary value or pointer to be included into the result
   * @warning you need to provide exactly the right number of parameters,
   *          i.e. matching the number of fields in the format string.
   */
  template<typename VAL>
  inline _Fmt&
  _Fmt::operator% (VAL const& val)
  {
    pushParameter (Converter<VAL>::prepare (val));
    return *this;
  }
  
  
  
  namespace { // helpers to pick a suitable specialisation....
    
    template<typename X>
    struct _can_convertToString
      {
        enum{ value = false };
      };
    
    
    /** 
     * by default we don't allow to 
     * treat any types directly by boost::format.
     * As fallback we rather just produce a type-ID
     */
    template<typename X>
    struct _can_forward                     { enum{ value = false };};
    
    /* the following definitions enable some basic types
     * to be forwarded to boost::format literally */
    template<> struct _can_forward<string>  { enum{ value = true }; };
    template<> struct _can_forward<int>     { enum{ value = true }; };
    template<> struct _can_forward<uint>    { enum{ value = true }; };
    
    
    
    inline string
    _log_and_stringify (std::exception const& ex)
    {
      WARN (progress, "Error while invoking custom string conversion: %s", ex.what());
      return ex.what();
    }
    
    inline string
    _log_unknown_exception()
    {
      const char* errID = lumiera_error();
      if (errID)
        {
          ERROR (progress, "Unknown error while invoking custom string conversion. Lumiera error flag = %s", errID);
          return string("Unknown error in string conversion. FLAG=")+errID;
        }
      else
        {
          ERROR (progress, "Unknown error while invoking custom string conversion. No Lumiera error flag set.");
          return "Unknown error in string conversion";
        }
    }
    
  }//(End) guards/helpers
  
  
  
  /* === explicit specialisations to control the kind of conversion === */
  
  template<typename VAL, class SEL>
  struct _Fmt::Converter
    {
      static string
      prepare (VAL const&)
        {
          UNIMPLEMENTED ("get type string");
        }
    };
  
  template<typename VAL>
  struct _Fmt::Converter<VAL,      typename enable_if< _can_convertToString<VAL> >::type>
    {
      static string
      prepare (VAL const& val)
        try {
            return string(val); 
          }
        catch(std::exception const& ex)
          {
            return _log_and_stringify(ex);
          }
        catch(...)
          {
            return _log_unknown_exception();
          }
    };
  
  template<typename VAL>
  struct _Fmt::Converter<VAL,      typename enable_if< _can_forward<VAL> >::type>
    {
      static VAL const&
      prepare (VAL const& val)
        {
          return val;
        }
    };
  
  template<typename VAL>
  struct _Fmt::Converter<VAL*,     typename enable_if< _can_forward<VAL> >::type>
    {
      static const VAL *
      prepare (const VAL * const pVal)
        {
          return pVal;
        }
    };
  
  
  
  /* === comparison of formatter objects === */
  
  inline bool
  operator== (_Fmt const& left, _Fmt const& right)
  {
    return string(left) == string(right);
  }
  
  inline bool
  operator== (_Fmt const& fmt, string const& str)
  {
    return string(fmt) == str;
  }
  
  inline bool
  operator== (_Fmt const& fmt, const char * const cString)
  {
    return string(fmt) == string(cString);
  }
  
  inline bool
  operator== (string const& str, _Fmt const& fmt)
  {
    return fmt == str;
  }
  
  inline bool
  operator== (const char * const cString, _Fmt const& fmt)
  {
    return fmt == cString;
  }
  
  
  
} // namespace util

#endif /*UTIL_FORMAT_STRING_H*/
