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

#include "lib/error.hpp"
#include "lib/meta/util.hpp"
#include "lib/meta/size-trait.hpp"

#include <string>
#include <typeinfo>
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
  
  typedef unsigned char uchar;
  
  using boost::enable_if;
  using std::string;

  
  
  
  
  /** 
   * @todo write type comment
   */
  class _Fmt
    : boost::noncopyable
    {
      /** size of an internal implementation Buffer */
      enum{ FORMATTER_SIZE = lib::meta::SizeTrait::BOOST_FORMAT };
      
      
      /** @internal buffer to hold a boost::format */
      mutable char formatter_[FORMATTER_SIZE];
      
      
      /** helper to prepare parameters for inclusion */
      template<typename VAL, class SEL =void>
      struct Converter;
      
      template<typename VAL>
      void pushParameter (VAL const&);
      
      template<typename VAL>
      void pushParameter (const VAL * const);
      
      
    public:
     ~_Fmt ();
      _Fmt (string formatString);
      
      operator string()  const;  ///< get the formatted result 
      
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
  
  /** The percent operator (\c '%' ) is used do feed parameter values
   *  to be included into the formatted result, at the positions marked 
   *  by printf-style placeholders within the format string.   
   * 
   * \par type specific treatment
   * Basic types (numbers, chars, strings) are passed to the implementation
   * (= boost::format) literally. For custom types, we try to use a custom
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
    
    /** 
     * by default we don't allow to 
     * treat any types directly by boost::format.
     * As fallback we rather just produce a type-ID
     */
    template<typename X>
    struct _shall_forward                     { enum{ value = false };};
    
    /* the following definitions enable some basic types
     * to be forwarded to boost::format literally */
    template<> struct _shall_forward<string>  { enum{ value = true }; };
    template<> struct _shall_forward<char>    { enum{ value = true }; };
    template<> struct _shall_forward<uchar>   { enum{ value = true }; };
    template<> struct _shall_forward<int>     { enum{ value = true }; };
    template<> struct _shall_forward<uint>    { enum{ value = true }; };
    template<> struct _shall_forward<short>   { enum{ value = true }; };
    template<> struct _shall_forward<ushort>  { enum{ value = true }; };
    template<> struct _shall_forward<int64_t> { enum{ value = true }; };
    template<> struct _shall_forward<uint64_t>{ enum{ value = true }; };
    template<> struct _shall_forward<float>   { enum{ value = true }; };
    template<> struct _shall_forward<double>  { enum{ value = true }; };
    template<> struct _shall_forward<void*>   { enum{ value = true }; };
    
    
    template<typename X>
    struct _shall_convert_toString
      {
        enum{ value = ! _shall_forward<X>::value
                 &&    lib::meta::can_convertToString<X>::value
                 &&   !lib::meta::is_sameType<X,char*>::value
        };
      };
    
    
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
  
  /** default/fallback: just indicate the (static) type */
  template<typename VAL, class SEL>
  struct _Fmt::Converter
    {
      static string
      prepare (VAL const&)
        {
          return string("«")+typeid(VAL).name()+"»";
        }
    };
  
  /** some custom types explicitly provide string representation */
  template<typename VAL>
  struct _Fmt::Converter<VAL,      typename enable_if< _shall_convert_toString<VAL> >::type>
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
  
  /** some basic types are directly forwarded down to the implementation;
   * @note this requires explicit specialisations in format-string.cpp */
  template<typename VAL>
  struct _Fmt::Converter<VAL,      typename enable_if< _shall_forward<VAL> >::type>
    {
      static VAL const&
      prepare (VAL const& val)
        {
          return val;
        }
    };
  
  template<typename VAL>
  struct _Fmt::Converter<VAL*,     typename enable_if< _shall_forward<VAL> >::type>
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
#endif
