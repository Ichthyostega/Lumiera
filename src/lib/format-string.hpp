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
 ** whenever possible, provides a direct automatic conversion to the formatted result
 ** to string and catches any exceptions.
 ** 
 ** This front-end is used pervasively for diagnostics and logging, so keeping down the
 ** compilation and object size cost and reliably handling any error is more important
 ** than the (small) performance gain of directly invoking boost::format (which is
 ** known to be 10 times slower than printf anyway).
 ** 
 ** # Implementation notes
 ** To perform the formatting, usually a \c _Fmt object is created as an anonymous
 ** temporary, but it may as well be stored into a variable. Copying is not permitted.
 ** Individual parameters are then fed for formatting through the \c '%' operator.
 ** Each instance of _Fmt uses its own embedded boost::format object for implementation,
 ** but this formatter resides within an opaque buffer embedded into the frontend object.
 ** The rationale for this admittedly tricky approach is to confine any usage of boost::format
 ** to the implementation translation unit (format-string.cpp).
 ** 
 ** The implementation is invoked by the frontend through a set of explicit specialisations
 ** for all the relevant \em primitive data types. For custom types, we prefer to invoke
 ** operator string() if possible, which is determined by a simple metaprogramming test,
 ** which is defined in lib/meta/util.pp, without relying on boost. As a fallback, for
 ** all other types without built-in or custom string conversion, we use the mangled
 ** type string produced by RTTI.
 ** 
 ** The compile time and object size overhead incurred by using this header was verified
 ** to be negligible, in comparison to using boost::format. When compiling a demo example
 ** on x86_64, the following executable sizes could be observed:
 ** 
 **                                          |debug | stripped
 ** ----------------------------------------:|----: | ---:
 ** just string concatenation ...            |  42k | 8.8k
 ** including and using format-string.hpp ...|  50k | 9.4k
 ** including and using boost::format ...    | 420k | 140k
 ** 
 ** In addition, we need to take the implementation translation unit (format-string.cpp)
 ** into account, which is required once per application and contains the specialisations
 ** for all primitive types. In the test showed above, the corresponding object file
 ** had a size of 1300k (with debug information) resp. 290k (stripped).
 ** 
 ** # Usage
 ** The syntax of the format string is defined by boost::format and closely mimics
 ** the printf formatting directives. The notable difference is that boost::format
 ** uses the C++ stream output framework, and thus avoids the perils of printf.
 ** The individual formatting placeholders just set the corresponding flags on
 ** an embedded string stream, thus the actual parameter types cause the
 ** selection of a suitable format, not the definitions within the
 ** format string.
 ** 
 ** An illegal format string will raise an error::Fatal. Any other error during usage of
 ** the formatter is caught, logged and suppressed, inserting an error indicator into
 ** the formatted result instead
 ** 
 ** A formatter is usually created as an anonymous object, at places where a string
 ** is expected. An arbitrary number of parameters is then supplied using the \c '%' operator.
 ** The result can be obtained
 ** - by string conversion
 ** - by feeding into an output stream.
 ** 
 ** Code example:
 ** \code
 ** double total = 22.9499;
 ** const char * currency = "€";
 ** cout << _Fmt("price %+5.2f %s") % total % currency << endl;
 ** \endcode
 ** 
 ** @remarks See the unit-test for extensive usage examples and corner cases.
 **          The header format-obj.hpp provides an alternative string conversion,
 **          using a bit of type traits and boost lexical_cast, but no boost::format.
 ** @warning not suited for performance critical code. About 10 times slower than printf.
 ** 
 ** TICKET #1204 : proper alignment verified 10/2019
 ** 
 ** @see FormatString_test
 ** @see format-util.hpp
 ** @see format-obj.hpp
 ** 
 */


#ifndef UTIL_FORMAT_STRING_H
#define UTIL_FORMAT_STRING_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/meta/util.hpp"
#include "lib/meta/size-trait.hpp"

#include <string>



namespace std { // forward declaration to avoid including <iostream>
  
  template<typename C>
  struct char_traits;
  
  template<typename C, class _TRAITS>
  class basic_ostream;
  
  using ostream = basic_ostream<char, char_traits<char>>;
}


namespace lib {
  class Literal;
  class Symbol;
}


namespace util {
  
  using std::string;
  
  typedef unsigned char uchar;

  LUMIERA_ERROR_DECLARE (FORMAT_SYNTAX); ///< "Syntax error in format string for boost::format"
  
  
  
  /**
   * A front-end for using printf-style formatting.
   * Values to be formatted can be supplied through the
   * operator%. Custom defined string conversions on objects
   * will be used, any errors while invoking the format operation
   * will be suppressed. The implementation is based on boost::format,
   * but kept opaque to keep code size and compilation times down.
   * @see FormatString_test
   */
  class _Fmt
    : util::NonCopyable
    {
      /** size of an opaque implementation Buffer */
      enum{ FORMATTER_SIZE = lib::meta::SizeTrait::BOOST_FORMAT };
      
      typedef char Implementation[FORMATTER_SIZE];
      
      
      /** @internal buffer to hold a boost::format */
      mutable Implementation formatter_;
      
      
      /** call into the opaque implementation */
      template<typename VAL>
      static void format (const VAL, Implementation&);
      
      /** helper to prepare parameters for inclusion */
      template<typename VAL, class SEL =void>
      struct Converter;
      
      
      
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
      friend bool operator != (_Fmt const& fmt, X const& x) { return not (fmt == x); }
      template<typename X>
      friend bool operator != (X const& x, _Fmt const& fmt) { return not (x == fmt); }
    };
  
  
  
  
  
  /* ===== forwarding into the implementation ====== */
  
  /** The percent operator (\c '%' ) is used do feed parameter values
   *  to be included into the formatted result, at the positions marked
   *  by printf-style placeholders within the format string.
   * 
   * \par type specific treatment
   * Basic types (numbers, chars, strings) are passed to the implementation
   * (= boost::format) literally. For custom types, we try to use a custom
   * string conversion, if applicable. Non-NULL pointers will be dereferenced,
   * with the exception of C-Strings and \c void*. Any other type gets just
   * translated  into a type-ID (using the mangled RTTI info). In case of errors
   * during the conversion, a string representation of the error is returned
   * @param val arbitrary value or pointer to be included into the result
   * @warning you need to provide exactly the right number of parameters,
   *          i.e. matching the number of fields in the format string.
   * @note EX_FREE
   */
  template<typename VAL>
  inline _Fmt&
  _Fmt::operator% (VAL const& val)
  {
    Converter<VAL>::dump (val, formatter_);
    return *this;
  }
  
  
  
  namespace { // helpers to pick a suitable specialisation....
    
    using std::__and_;
    using std::__not_;
    
    /**
     * by default we don't allow to
     * treat any types directly by boost::format.
     * As fallback we rather just produce a type-ID
     */
    template<typename X>
    struct _allow_call : std::false_type {};
    
    /* the following definitions enable some primitive types
     * to be handed over to the boost::format implementation */
    template<> struct _allow_call<string>  : std::true_type { };
    template<> struct _allow_call<char>    : std::true_type { };
    template<> struct _allow_call<uchar>   : std::true_type { };
    template<> struct _allow_call<int16_t> : std::true_type { };
    template<> struct _allow_call<uint16_t>: std::true_type { };
    template<> struct _allow_call<int32_t> : std::true_type { };
    template<> struct _allow_call<uint32_t>: std::true_type { };
    template<> struct _allow_call<int64_t> : std::true_type { };
    template<> struct _allow_call<uint64_t>: std::true_type { };
    template<> struct _allow_call<float>   : std::true_type { };
    template<> struct _allow_call<double>  : std::true_type { };
#ifndef __x86_64__
    template<> struct _allow_call<long>    : std::true_type { };
    template<> struct _allow_call<ulong>   : std::true_type { };
#endif
    
    template<typename X>
    struct _shall_format_directly
      : _allow_call<std::remove_cv_t<X>>
      { };
    
    template<typename X>
    struct _shall_convert_toString
      : __and_<__not_<_shall_format_directly<X>>
              , std::bool_constant<lib::meta::can_convertToString<X>::value>
              >
      { };
    
    
    template<typename SP>
    struct _is_smart_wrapper
      : std::false_type
      { };
    template<typename T>
    struct _is_smart_wrapper<std::shared_ptr<T>>
      : std::true_type
      { };
    template <typename T, typename D>
    struct _is_smart_wrapper<std::unique_ptr<T,D>>
      : std::true_type
      { };
    
    
    
    template<typename SP>
    struct _shall_show_smartWrapper
      : __and_<__not_<_shall_convert_toString<SP>>
              ,_is_smart_wrapper<std::remove_reference_t<std::remove_cv_t<SP>>>
              >
      { };
    
    
    
    
    
    inline void
    _clear_errorflag()
    {
      const char* errID = lumiera_error();
      TRACE_IF (errID, progress, "Lumiera errorstate '%s' cleared.", errID);
    }
    
    inline string
    _log_and_stringify (std::exception const& ex)
    {
      _clear_errorflag();
      WARN (progress, "Error while invoking custom string conversion: %s", ex.what());
      try {
          return string("<string conversion failed: ")+ex.what()+">";
        }
      catch(...) { /* secondary errors ignored */ }
      return "(formatting failure)";
    }
    
    inline string
    _log_unknown_exception()
    {
      const char* errID = lumiera_error();
      if (errID)
        ERROR (progress, "Unknown error while invoking custom string conversion. Lumiera error flag = %s", errID);
      else
        ERROR (progress, "Unknown error while invoking custom string conversion. No Lumiera error flag set.");
      return "<Unknown error in string conversion>";
    }
    
  }//(End) guards/helpers
  
  
  
  
  /* === explicit specialisations to control the kind of conversion === */
  
  /** default/fallback: just indicate the (static) type */
  template<typename VAL, class SEL>
  struct _Fmt::Converter
    {
      static void
      dump (VAL const&, Implementation& impl)
        {
          format ("«"+typeStr<VAL>()+"»", impl);
        }
    };
  
  template<typename VAL>
  struct _Fmt::Converter<VAL*>
    {
      static void
      dump (const VAL *pVal, Implementation& impl)
        {
          if (pVal)
            Converter<VAL>::dump(*pVal, impl);
          else
            format (BOTTOM_INDICATOR, impl);
        }
    };
  
  template<>
  struct _Fmt::Converter<void *>
    {
      static void
      dump (const void* address, Implementation& impl)
        {
          format (address, impl);
        }
    };
  
  template<>
  struct _Fmt::Converter<const char *>
    {
      static void
      dump (const char* cString, Implementation& impl)
        {
          format (cString? cString : BOTTOM_INDICATOR, impl);
        }
    };
  
  template<>
  struct _Fmt::Converter<bool>
    {
      static void
      dump (bool yes, Implementation& impl)
        {
          format (yes? "true":"false", impl);
        }
    };
  
  template<>
  struct _Fmt::Converter<lib::Literal>
    {
      static void
      dump (lib::Literal const& literal, Implementation& impl)
        {
          format (literal.empty()? "" : literal.c(), impl);
        }
    };
  
  template<>
  struct _Fmt::Converter<lib::Symbol>
    {
      static void
      dump (lib::Symbol const& symbol, Implementation& impl)
        {
          format (symbol.c(), impl);
        }
    };
  
  /** some custom types explicitly provide a string representation */
  template<typename VAL>
  struct _Fmt::Converter<VAL,      lib::meta::enable_if<_shall_convert_toString<VAL>> >
    {
      static void
      dump (VAL const& val, Implementation& impl)
        try {
            format (string(val), impl);
          }
        catch(std::exception const& ex)
          {
            format (_log_and_stringify(ex), impl);
          }
        catch(...)
          {
            format (_log_unknown_exception(), impl);
          }
    };
  
  template<typename SP>
  struct _Fmt::Converter<SP,       lib::meta::enable_if<_shall_show_smartWrapper<SP>> >
    {
      static void
      dump (SP const& smP, Implementation& impl)
        try {
            format (showSmartPtr (smP, lib::meta::typeSymbol(smP)), impl);
          }
        catch(std::exception const& ex)
          {
            format (_log_and_stringify(ex), impl);
          }
        catch(...)
          {
            format (_log_unknown_exception(), impl);
          }
    };
  
  /** some basic types are directly forwarded down to the implementation;
   * @note this requires explicit specialisations in format-string.cpp */
  template<typename VAL>
  struct _Fmt::Converter<VAL,      lib::meta::enable_if<_shall_format_directly<VAL>> >
    {
      static void
      dump (const VAL val, Implementation& impl)
        {
          format (val, impl);
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
