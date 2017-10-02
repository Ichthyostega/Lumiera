/*
  SYMBOL.hpp  -  symbolic constant datatype 

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

/** @file symbol.hpp
 ** Marker types to indicate a literal string and a Symbol.
 ** Instead of working just with pointers, which could represent pretty much anything,
 ** it is prudent to express the meaning at interfaces and for variables and members explicitly.
 ** 
 ** On concept level, while a string is just some sequence of characters and nothing can be said
 ** about mutability or lifetime, a Literal on the contrary is meant to be _static._ It is fixed
 ** and assumed to exist literally as is during the whole lifetime of the execution. The concept
 ** of a Symbol is related, yet slightly different: it is meant to be a distinguishable fixed,
 ** unique token. _Identical sequence_ of characters means we have exactly the _same Symbol._
 ** 
 ** These concepts can be fused by treating Symbol as a specialisation of Literal, additionally
 ** maintaining an automatically populated, static [symbol table](\ref symbol-table.hpp), and
 ** we close the circle by allowing Symbol instances to be created from strings at runtime.
 ** 
 ** @remark this started on occasion 11/2008, just with a typedef to mark assumption on interfaces
 **   for rules based configuration in the Proc-Layer. Over time, conversions, comparison and
 **   hashcode implementation were added. It turned out that the most smooth integration in
 **   coding practice is achieved when allowing transparent conversion for Literal, but not
 **   for Symbol or std::string.
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


namespace lib {
  
  /** inline string literal
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
       const char * str_;
       
    public:
       Literal(); ///< empty string by default
       
       Literal (const char* literal)
         : str_(literal)
         { }
       
       Literal (Literal const& o)
         : str_(o.str_)
         { }
       
       operator const char* ()  const { return str_; }
       const char* c()          const { return str_; }
       
       bool
       empty()  const
         {
           return !str_ || 0 == std::strlen(str_);
         }
       
       bool operator== (const char* cString)  const;

    protected:
       /** Assignment generally prohibited */
       Literal& operator= (const char* newStr)
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
      
      Symbol (const char* lit =NULL)
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
      
      Symbol (Literal const& base, const char* ext)
        : Symbol{base, std::string(ext)}
        { }
       
       Symbol (Symbol const&) = default;
       Symbol (Symbol &&)     = default;
       
       Symbol& operator= (Symbol const&) = default;
       Symbol& operator= (Symbol &&)     = default;
       
       explicit operator bool()  const { return not empty(); }
       bool empty()              const { return *this == BOTTOM; }
       
       size_t
       length()  const
         {
           return std::strlen(c());
         }
    };
  
  
  /** safety guard: maximum number of chars to process.
   *  For comparisons, hash calculations etc., when dealing
   *  with raw char ptrs (typically literal values) */
  extern const size_t STRING_MAX_RELEVANT;
  
  /** @note storage guaranteed to exist */
  inline Literal::Literal() : str_(Symbol::EMPTY) { }
  
  
  /* ===== to be picked up by ADL ===== */
  
  size_t hash_value (Literal);
  size_t hash_value (Symbol);
  

  /* === equality comparisons === */

  inline bool operator== (Literal const& s1, Literal const& s2) { return s1.operator== (s2.c()); }
  inline bool operator== (Symbol const& s1,  Symbol const& s2)  { return s1.c() == s2.c(); } ///< @note comparison of symbol table entries
  
  /* === mixed comparisons === */
  
  inline bool operator== (const char* s1,    Literal s2)        { return s2.operator== (s1); }
  inline bool operator== (Symbol s1,         const char* s2)    { return s1.operator== (s2); }
  inline bool operator== (const char* s1,    Symbol      s2)    { return s2.operator== (s1); }
  inline bool operator== (Literal s1,        Symbol  s2)        { return s1.operator== (s2.c()); }
  inline bool operator== (Symbol s1,         Literal s2)        { return s2.operator== (s1.c()); }
  inline bool operator== (Literal s1,        std::string s2)    { return s1.operator== (s2.c_str()); }
  inline bool operator== (std::string s1,    Literal     s2)    { return s2.operator== (s1.c_str()); }
  inline bool operator== (Symbol s1,         std::string s2)    { return s1.operator== (s2.c_str()); }
  inline bool operator== (std::string s1,    Symbol      s2)    { return s2.operator== (s1.c_str()); }
  
  /* === negations === */
  
  inline bool operator!= (Literal const& s1, Literal const& s2) { return not s1.operator== (s2.c()); }
  inline bool operator!= (Symbol const& s1,  Symbol const& s2)  { return not (s1.c() == s2.c()); }
  inline bool operator!= (Literal s1,        const char* s2)    { return not s1.operator== (s2); }
  inline bool operator!= (const char* s1,    Literal s2)        { return not s2.operator== (s1); }
  inline bool operator!= (Symbol s1,         const char* s2)    { return not s1.operator== (s2); }
  inline bool operator!= (const char* s1,    Symbol      s2)    { return not s2.operator== (s1); }
  inline bool operator!= (Literal s1,        Symbol  s2)        { return not s1.operator== (s2.c()); }
  inline bool operator!= (Symbol s1,         Literal s2)        { return not s2.operator== (s1.c()); }
  inline bool operator!= (Literal s1,        std::string s2)    { return not s1.operator== (s2.c_str()); }
  inline bool operator!= (std::string s1,    Literal     s2)    { return not s2.operator== (s1.c_str()); }
  inline bool operator!= (Symbol s1,         std::string s2)    { return not s1.operator== (s2.c_str()); }
  inline bool operator!= (std::string s1,    Symbol      s2)    { return not s2.operator== (s1.c_str()); }
  
  
  
  /// string concatenation
  inline std::string
  operator+ (std::string str, Literal const& sym)
  {
    const char* symP (sym);
    return str + symP;
  }
  
  inline std::string
  operator+ (Literal const& sym, std::string str)
  {
    const char* symP (sym);
    return symP + str;
  }
  
  
  
} // namespace lib
#endif /*LIB_SYMBOL_H*/
