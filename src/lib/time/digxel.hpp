/*
  DIGXEL.hpp  -  grid aligned and fixed format time specifications

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file digxel.hpp
 ** A self-contained numeric element for building structured numeric displays.
 ** Within the GUI of an editing application, we got to build several display widgets
 ** to show numeric values in a structured fashion, like Colours or Timecode. While the
 ** actual formats of such display elements vary largely, the common property is that
 ** they use an underlying \em format to build the numbers out of individual numeric elements.
 ** For a timecode display these are for example the sexagesimal (base 60) "digits" of the
 ** common human readable time representation. A "Digxel" is an abstract element with specific
 ** properties to support building such display widgets. It doesn't contain any GUI code, but
 ** can be wrapped up to build a custom widget.
 ** 
 ** # properties of a "Digxel"
 ** 
 ** Semantically, it's a number or number component. It holds an internal numeric representation
 ** and is implicitly convertible back to the underlying numeric type (usually int or double).
 ** 
 ** But at the same time, a Digxel has a definite textual format and the ability to present
 ** its numeric value formatted accordingly. To this end, the contract \em requires that
 ** numeric data pushed to the Digxel be kept within such limits to prevent exceeding the
 ** embedded formatting buffer. There is an assertion in debug mode, and a range check,
 ** but the result will be just truncated, so passing only sane values is clearly the
 ** caller's responsibility. Digxel might be considered an implementation support class,
 ** and performance is important to some limited degree;
 ** especially, formatted values will be \em cached.
 ** 
 ** To support in-place modification, the digxel stores a mutation signal (functor). This
 ** functor will be invoked, whenever a new value gets assigned. The actual functor is free
 ** to cause side effects; the value returned from this functor will be the new value to set.
 ** If not configured, the default implementation just accepts the given value unaltered. Usually
 ** this mutation functor should invoke some internal recalculations, maybe resulting in a new
 ** value being pushed to the Digxel for display.
 ** 
 ** # configuration
 ** 
 ** the Digxel template can be configured to some degree to adjust the stored numeric data
 ** and the actual format to be applied
 ** 
 ** @see timecode.hpp typical usage example
 **
 */

#ifndef LIB_TIME_DIGXEL_H
#define LIB_TIME_DIGXEL_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <boost/operators.hpp>
#include <boost/lexical_cast.hpp>
#include <functional>
#include <string>
#include <cstdio>
#include <cmath>

using std::string;


namespace lib {
namespace time {
  
  namespace digxel {
    
    using util::cStr;
    using lib::Literal;
    using boost::lexical_cast;
    
    typedef const char* CBuf;
    
    
    /**
     * Default / base implementation for Digxel formatting.
     * This formatter holds an inline buffer of limited size,
     * receiving and caching the textual representation
     */
    template<typename NUM, size_t len>
    class PrintfFormatter
      {
        enum{ bufsiz = len+1 };
        
        char printbuffer_[bufsiz];
        Literal formatSpec_;
        
      public:
        PrintfFormatter (Literal fmt)
          : printbuffer_()
          , formatSpec_(fmt)
          {
            clear();
          }
        
        void clear() { printbuffer_[0] = '\0'; }
        bool empty() { return '\0' == *printbuffer_; }
        
        size_t
        maxlen()  const
          {
            return len;
          } 
        
        CBuf
        show (NUM val)
          {
            if (empty())
              {
                size_t space = std::snprintf (printbuffer_, bufsiz, formatSpec_, val);
                REQUIRE (space < bufsiz, "Digxel value exceeded available buffer size. "
                                         "For showing %s, %zu+1 chars instead of just %zu+1 would be required."
                                       , cStr(lexical_cast<string>(val)), space, len);               ///////////TICKET #197
              }
            ENSURE (!empty());
            return printbuffer_;
          }
      };
    
    
    /** 
     * default configured Formatter implementations
     * for some of the basic numeric types 
     */
    template<typename NUM>
    struct Formatter;
    
    template<>
    struct Formatter<int>
      : PrintfFormatter<int, 9>
      {
        Formatter() : PrintfFormatter<int,9>("%3d") { }
      };
    
    template<>
    struct Formatter<double>
      : PrintfFormatter<double, 7>
      {
        Formatter() : PrintfFormatter<double,7>("%06.3f") { }
      };
    
    /* == other specialised Formatters == */
    struct SexaFormatter
      : PrintfFormatter<int, 4>
      {
        SexaFormatter() : PrintfFormatter<int,4>("%02d") { }
      };
    
    struct HexaFormatter
      : PrintfFormatter<uint, 2>
      {
        HexaFormatter() : PrintfFormatter<uint,2>("%02X") { }
      };
    
    struct CountFormatter
      : PrintfFormatter<long, 20>
      {
        CountFormatter() : PrintfFormatter<long,20>("%04ld") { }
      };
    
    struct HourFormatter
      : PrintfFormatter<int, 9>
      {
        HourFormatter() : PrintfFormatter<int,9>("%2d") { }
      };
    
      
    struct SignFormatter
      {
        void clear()          {  }
        size_t maxlen() const { return 1; }        
        CBuf show (int val)   { return val<0? "-":" "; }
      };
    
  } //(End) digxel configuration namespace
  
  
  
  
  
  using std::bind;
  using std::function;
  using std::placeholders::_1;
  
  
  /**
   * A number element for building structured numeric displays.
   * The purpose is to represent parts of a numeric format, like
   * e.g. the sexagesimal "digits" of a timecode display. A Digxel
   * - is customised by template parameters to a specific number format
   * - requires that any given number must not overflow the format buffer
   * - can receive new numbers by assignment
   * - stores and these given value numerically
   * - will then format these numbers and cache the formatted representation
   * - can store and invoke a mutation functor to pre-process values on setting
   * 
   * @note comparisons are assumed to be not performance relevant
   * @param NUM numeric type to be used for the value
   * @param FMT a formatter and buffer holder type
   * @see digxel::Formatter default printf based formatter
   * @see lib::time::TCode
   * @see Digxel_test
   * 
   */
    template< typename NUM
            , class FMT  = digxel::Formatter<NUM>
            >
  class Digxel
    : public boost::totally_ordered<Digxel<NUM,FMT>>
    {
      mutable
      FMT buffer_;
      NUM value_;
      
      typedef Digxel<NUM,FMT>      _Digxel;
      typedef function<void(NUM)> _Mutator;
      
      
      _Mutator mutator; ///< Functor for setting a new digxel value

    public:
      /** install an external functor to be applied on any new digxel value.
       * This allows individual instances to limit the possible digxel values,
       * or to update an compound value (e.g. a time comprised of hour, minute
       * and second digxel elements). The installed functor needs to accept
       * a "this" pointer and actually perform any desired state change
       * as sideeffect. The default is to accept any value as-is.
       * @warning using a mutator creates significant overhead;
       *          measurements indicate a factor of 4
       * @see Digxel_test
       */
      template<typename FUN, class THIS>
      void
      installMutator (FUN mutate, THIS& self)
        {
          mutator = bind (mutate, &self, _1 );
        }
      
      
      Digxel ()
        : buffer_()
        , value_ ()
        , mutator()
        { }
      
      // using the standard copy operations
      
      operator NUM()    const { return value_; }
      operator string() const { return show(); }
      
      size_t maxlen()   const { return buffer_.maxlen(); }
      
      
      digxel::CBuf
      show()  const
        {
          return buffer_.show (value_);
        }
      
      
      void
      operator= (NUM n)
        {
          if (n == value_) return;
          if (mutator)
            mutator (n);
          else
            setValueRaw (n);
        }
      
      void
      setValueRaw (NUM newVal)
        {
          if (newVal != value_)
            {
              value_ = newVal;
              buffer_.clear();
            }
        }
      
      
      
      //---Supporting-increments--------------
      Digxel& operator+=  (NUM inc)  { *this = value_ + inc; return *this; }
      Digxel& operator-=  (NUM dec)  { *this = value_ - dec; return *this; }
      Digxel& operator++  ()         { *this = value_ + 1;   return *this; }
      Digxel& operator--  ()         { *this = value_ - 1;   return *this; }
      NUM     operator++  (int)      { NUM p(value_); *this =p+1; return p;}
      NUM     operator--  (int)      { NUM p(value_); *this =p-1; return p;}
      
      //---Supporting-totally_ordered---------
      bool operator<  (Digxel const& o)  const { return value_ <  NUM(o); }
      bool operator== (Digxel const& o)  const { return value_ == NUM(o); }
    };
  
  
  
  /* == predefined Digxel configurations == */
  typedef Digxel< int, digxel::SexaFormatter> SexaDigit;  ///< for displaying time components (sexagesimal)
  typedef Digxel<uint, digxel::HexaFormatter> HexaDigit;  ///< for displaying a hex byte
  typedef Digxel< int, digxel::HourFormatter> HourDigit;  ///< for displaying hours in H:M.S
  
  typedef int64_t FrameCnt;
  typedef Digxel<FrameCnt, digxel::CountFormatter> CountVal;  ///< for displaying a counter
  
  
  /** special Digxel to show a sign.
   * @note values limited to +1 and -1 */
  class Signum
    : public Digxel<int,digxel::SignFormatter>
    {
      typedef Digxel<int, digxel::SignFormatter> _Par;
      
      void
      storeSign (int val)
        {
          setValueRaw (val<0? -1:+1);
        } 
      
    public:
      Signum()
        {
          setValueRaw(1);
          installMutator (&Signum::storeSign, *this);
        }
      
      using _Par::operator=;
      
      friend int operator*= (Signum& s, int c) { s = c*s; return s; }
    };
  
  
}} // lib::time
#endif
