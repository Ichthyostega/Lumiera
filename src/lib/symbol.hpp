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
 ** WIP placeholder definition for a planned Symbol datatype.
 ** 
 ** @todo for the (currently just planned as of 11/08) rules based configuration
 ** in the Proc-Layer a explicit Symbol datatype will probably very helpful.
 ** For now just a typedef is sufficient. A real Symbol datatype should 
 ** - be definable by string constant
 ** - integrate smoothly with std::string
 ** - provide a unique numeric index for each distinct Symbol
 ** - automatically maintain a symbol table at runtime to support this
 ** - provide some magic (macros) allowing to build distinct types based on symbols.
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
   *  This is a marker type to handle literally given C-Strings.
   */
  class Literal
    {
       const char * str_;
       
    public:
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
       /** Assignment prohibited */
       Literal& operator= (const char* newStr)
         {
           str_ = newStr;
           return *this;
         }
    };
  
  
  /** Token or Atom with distinct identity
   * @note same literal string==same pointer representation
   */
  class Symbol
    : public Literal
    {
    public:
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
