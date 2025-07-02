/*
  SYMBOL.hpp  -  symbolic constant datatype

   Copyright (C)
     2008,2017        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file symbol.hpp
 ** Marker types to indicate a literal string and a Symbol.
 ** Instead of working just with pointers, which could represent pretty much anything,
 ** it is prudent to express the meaning at interfaces and for variables and members explicitly.
 ** 
 ** On conceptual level, while a **string** is just some sequence of characters and nothing can be
 ** said about its mutability or lifetime, a **Literal** is explicitly meant to be _static._ It is
 ** a fixed sequence of characters placed in a stable memory location and assumed to exist during
 ** the whole lifetime of the execution. The concept of a **Symbol** is slightly different: it is
 ** meant to be a distinguishable, fixed, unique token. An _Identical sequence_ of characters means
 ** we have _exactly the same Symbol_.
 ** 
 ** These concepts can be fused by treating Symbol as a specialisation of Literal, additionally
 ** maintaining an automatically populated, static [symbol table](\ref symbol-table.hpp), and
 ** we close the circle by allowing Symbol instances to be created from strings at runtime.
 ** 
 ** @remark this abstraction was first used occasionally 11/2008, at that time just as a typedef
 **   to mark assumption on the new interfaces for rules based configuration in the Steam-Layer.
 **   Over time, conversions, comparison and hashcode implementation were added. It turned out
 **   that the most smooth integration in coding practice is achieved when allowing transparent
 **   conversion between Literal ⟷ CStr, but not for Symbol or std::string.
 ** @todo 9/2017 consider this mostly as settled, but might require some finishing touches
 ** - maybe improve interoperation of Symbol and std::string
 ** - investigate performance of the automatic symbol table
 ** - improve Lifecycle in startup and shutdown phase
 ** - maybe some metaprogramming magic to build distinct types based on symbols.
 ** 
 ** @see symbol-impl.cpp
 ** @see configrules.hpp
 ** @see query.hpp
 */


#ifndef LIB_SYMBOL_H
#define LIB_SYMBOL_H

#include "lib/hash-standard.hpp"

#include <string>
#include <cstring>

using CStr = const char*;

/** convenience shortcut: forced conversion to c-String via string.
 *  usable for printf with objects providing to-string conversion. */
inline CStr
cStr (std::string const& rendered)
{
  return rendered.c_str();
}


namespace lib {
  
  /** Inline string literal.
   *  This is a _marker type_ to indicate that
   *  - the string was given literally
   *  - storage is _somewhere_, not managed by Literal,
   *    yet guaranteed to exist during the whole lifetime
   *  - it is transparently convertible to/from C-string
   *  - defaults to the empty string
   *  - can not be altered
   */
  class Literal
    {
      CStr str_;
      
    public:
      /** empty string by default */
      constexpr Literal()  noexcept;
      
      constexpr Literal (CStr literal) noexcept
        : str_(literal)
        { }
      
      constexpr Literal (Literal const&)            noexcept = default;
      constexpr Literal& operator= (Literal const&) noexcept = default;
      
      constexpr operator CStr()  const { return str_; }
      constexpr const char* c()  const { return str_; }
      
      constexpr bool
      empty()  const
        {
          return not str_ or 0 == std::strlen(str_);
        }
      
      constexpr bool operator== (CStr cString)  const;
      constexpr size_t length()  const;

    protected:
      /** Assignment generally prohibited */
      Literal& operator= (CStr newStr) noexcept
        {
          str_ = newStr;
          return *this;
        }
    };
  
  
  /** Token or Atom with distinct identity.
   *  It can be created from arbitrary strings, yet not altered
   * @note same literal string==same pointer representation
   */
  class Symbol
    : public Literal
    {
    public:
      static Symbol ANY;
      static Symbol EMPTY;
      static Symbol BOTTOM;
      static Symbol FAILURE;
      
      Symbol (CStr lit =NULL)
        : Symbol{std::string(lit? lit : BOTTOM.c())}
        { }
      
      explicit
      Symbol (std::string&& definition);
      
      Symbol (std::string const& str)
        : Symbol{std::string(str)}
        { }
      
      Symbol (Literal const& base, std::string const& ext)
        : Symbol{std::string(base)+"."+ext}
        { }
      
      Symbol (Literal const& base, CStr ext)
        : Symbol{base, std::string(ext)}
        { }
      
      Symbol (Symbol const&) = default;
      Symbol (Symbol &&)     = default;
      
      Symbol& operator= (Symbol const&) = default;
      Symbol& operator= (Symbol &&)     = default;
      
      explicit operator bool()  const { return not empty(); }
      bool empty()              const { return *this == BOTTOM.c() or *this == EMPTY.c(); }
    };
  
  
  /** @note storage guaranteed to exist */
  constexpr inline Literal::Literal() noexcept : str_(Symbol::EMPTY) { }

  namespace{
    constexpr inline int
    strNcmp (CStr a, CStr b, size_t len)
    {
      return a == b ? 0 : std::strncmp (a?a:"", b?b:"", len);
    }
  }
  
  /** safety guard: maximum number of chars to process.
   *  For comparisons, hash calculations etc., when dealing
   *  with raw char ptrs (typically literal values) */
  extern const size_t STRING_MAX_RELEVANT;
  
  /** equality on Literal and Symbol values is defined
   *  based on the content, not the address. */
  constexpr inline bool
  Literal::operator== (CStr charPtr)  const
  {
    return 0 == strNcmp (this->str_, charPtr, STRING_MAX_RELEVANT);
  }
  
  constexpr inline size_t
  Literal::length()  const
  {
    return std::strlen(c());
  }
  
  
  
  
  
  /* ===== to be picked up by ADL ===== */
  
  HashVal hash_value (Literal);
  HashVal hash_value (Symbol);
  

  /* === equality comparisons === */

  constexpr inline bool operator== (Literal const& s1, Literal const& s2) { return s1.operator== (s2.c()); }
  constexpr inline bool operator== (Symbol const& s1,  Symbol const& s2)  { return s1.c() == s2.c(); } ///< @note comparison of symbol table entries
  
  /* === mixed comparisons === */
  
  constexpr inline bool operator== (CStr    s1,            Literal s2)    { return s2.operator== (s1); }
  constexpr inline bool operator== (Symbol  s1,               CStr s2)    { return s1.operator== (s2); }
  constexpr inline bool operator== (Literal s1,             Symbol s2)    { return s1.operator== (s2.c()); }
  constexpr inline bool operator== (Literal s1, std::string const& s2)    { return s1.operator== (s2.c_str()); }
  constexpr inline bool operator== (Symbol  s1, std::string const& s2)    { return s1.operator== (s2.c_str()); }
  
  
  
  /// string concatenation
  inline std::string
  operator+ (std::string str, Literal const& sym)
  {
    CStr symP (sym);
    return str + symP;
  }
  
  inline std::string
  operator+ (Literal const& sym, std::string str)
  {
    CStr symP (sym);
    return symP + str;
  }
  
  
  
} // namespace lib
#endif /*LIB_SYMBOL_H*/
