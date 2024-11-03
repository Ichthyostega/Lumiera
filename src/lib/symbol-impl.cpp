/*
  Symbol(impl)  -  helpers for working with literal string IDs 

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

* *****************************************************/

/** @file symbol-impl.cpp
 ** Collection of helpers for working with the lib::Symbol.
 ** 
 ** @todo currently as of 9/09 this is more of a placeholder.
 ** And maybe a location for collecting small bits of implementation,
 ** which could be usable later for real Symbol and Literal datatypes.
 ** 
 ** lib::Symbol
 ** control::CommandRegistry for usage example of the hash function.
 ** 
 */



#include "lib/symbol.hpp"
#include "lib/symbol-table.hpp"

#include <boost/functional/hash.hpp>
#include <cstddef>
#include <cstring>
#include <string>

using std::size_t;
using std::string;
using std::forward;
using boost::hash_combine;




namespace lib {
  
  const size_t STRING_MAX_RELEVANT = LUMIERA_IDSTRING_MAX_RELEVANT;
  
  
  namespace { // global symbol table
    
    SymbolTable&
    symbolTable()
    {
      static SymbolTable theSymbolTable;
      return theSymbolTable;  // Meyer's Singleton
    }
    
    inline int
    strNcmp (CStr a, CStr b, size_t len)
    {
      return a == b ? 0 : std::strncmp (a?a:"", b?b:"", len);
    }
  }
  
  
  /**
   * create Symbol by symbol table lookup.
   * @note identical strings will be mapped to the same Symbol (embedded pointer)
   * @warning potential lock contention, since each ctor call needs to do a lookup
   * @remark since lumiera::LifecycleHook entries use a Symbol, the symbol table is
   *         implemented as Meyer's singleton and pulled up early, way before main()
   */
  Symbol::Symbol (string&& definition)
    : Literal{symbolTable().internedString (forward<string> (definition))}
    { }
  
  
  /* == predefined marker Symbols == */
  Symbol Symbol::ANY     = "*";
  Symbol Symbol::EMPTY   = "";
  Symbol Symbol::BOTTOM  = "⟂";
  Symbol Symbol::FAILURE = "↯";
  
  // see also: lib/format-obj.cpp
  // We can not share these definitions due to undefined static init order
  
  
  
  
  /** equality on Literal and Symbol values is defined
   *  based on the content, not the address. */
  bool
  Literal::operator== (CStr charPtr)  const
  {
    return 0 == strNcmp (this->str_, charPtr, STRING_MAX_RELEVANT);
  }
  
  
  /** generate hash value based on the Literal's contents.
   *  This function is intended to be picked up by ADL, and should be usable
   *  both with  `std::hash` and `<boost/functional/hash.hpp>`. It is implemented
   *  similar as the boost::hash specialisation for std::string */
  HashVal
  hash_value (Literal literal)
  {
    size_t hash=0;
    if (literal)
      {
        size_t cnt = 1;
        const char *pos = literal;
        for ( ; cnt <= STRING_MAX_RELEVANT and *pos ; ++cnt, ++pos )
          hash_combine (hash, *pos);
      }
    
    return hash;
  }
  
  /** hash value for Symbols is directly based on the symbol table entry */
  HashVal
  hash_value (Symbol sym)
  {
    return sym? boost::hash_value (sym.c())
              : 0;
  }
  
  
  
} // namespace lib
