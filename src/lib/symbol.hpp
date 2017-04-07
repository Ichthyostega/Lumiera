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
#include "include/logging.h" /////////////TODO only temporarily

#include <string>
#include <cstring>


namespace lib {
  
  /** inline string literal
   *  @todo improve interaction with Symbol
   *  @todo make it non-nullable 
   *  @todo maybe use boost/operators  Ticket #417
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
       explicit
       Symbol (std::string&& definition);
       
       Symbol()
         : Symbol{""}
         { }
       
       Symbol (const char* lit)
         : Symbol{std::string(lit)}
         { }
      
       Symbol (Literal const& base, std::string ext)
         : Symbol{std::string(base)+"."+ext}
         { }
      
       Symbol (Literal const& base, const char* ext)
         : Symbol{base, std::string(ext)}
         { }
      
       Symbol (Symbol const&) = default;
       Symbol (Symbol &&)     = default;
       
       Symbol& operator= (Symbol const&) = default;
       Symbol& operator= (Symbol &&)     = default;
    };
  
  
  /** safety guard: maximum number of chars to process.
   *  For comparisons, hash calculations etc., when dealing
   *  with raw char ptrs (typically literal values) */
  extern const size_t STRING_MAX_RELEVANT;
  
  
  /* ===== to be picked up by ADL ===== */
  
  size_t hash_value (Literal);
  
  bool operator== (Literal sy1, Literal sy2);
  
  inline bool
  operator!= (Literal sy1, Literal sy2)
  {
    return not (sy1 == sy2); 
  }
  
  /** @note two symbols are equal iff
   *        they use the same symbol table entry
   */
  inline bool
  operator== (Symbol sy1, Symbol sy2)
  {
    return sy1.c() == sy2.c(); 
  }
  
  inline bool
  operator!= (Symbol sy1, Symbol sy2)
  {
    return not (sy1 == sy2); 
  }
  
  
  /// mixed comparison based on string equality
  inline bool
  operator== (Symbol sy1, Literal sy2)
  {
    return Literal(sy1) == sy2;
  }
  
  inline bool
  operator== (Literal sy1, Symbol sy2)
  {
    return sy1 == Literal(sy2);
  }
  
  inline bool
  operator!= (Symbol sy1, Literal sy2)
  {
    return not (sy1 == sy2);
  }
  
  inline bool
  operator!= (Literal sy1, Symbol sy2)
  {
    return not (sy1 == sy2);
  }
  
  
  /// comparison with c-strings                     //////TICKET #417
  inline bool
  operator== (Literal sy1, const char* sy2)
  {
    return   (sy1 == Literal(sy2));
  }
  
  inline bool
  operator== (const char* sy1, Literal sy2)
  {
    return   (Literal(sy1) == sy2);
  }
  
  inline bool
  operator!= (Literal sy1, const char* sy2)
  {
    return ! (sy1 == Literal(sy2));
  }
  
  inline bool
  operator!= (const char* sy1, Literal sy2)
  {
    return ! (Literal(sy1) == sy2);
  }
  
  inline bool
  operator== (Symbol sy1, const char* sy2)
  {
    return   (Literal(sy1) == Literal(sy2));
  }
  
  inline bool
  operator== (const char* sy1, Symbol sy2)
  {
    return   (Literal(sy1) == Literal(sy2));
  }
  
  inline bool
  operator!= (Symbol sy1, const char* sy2)
  {
    return ! (Literal(sy1) == Literal(sy2));
  }
  
  inline bool
  operator!= (const char* sy1, Symbol sy2)
  {
    return ! (Literal(sy1) == Literal(sy2));
  }
  
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
