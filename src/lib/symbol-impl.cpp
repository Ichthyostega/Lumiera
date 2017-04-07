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

/** @file symbol-impl.hpp
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
#include "include/limits.h"
extern "C" {
#include "lib/safeclib.h"
}

#include <boost/functional/hash.hpp>
#include <unordered_set>
#include <cstddef>
#include <utility>
#include <string>

using std::size_t;
using std::string;
using std::forward;
using boost::hash_combine;




namespace lib {
  
  const size_t STRING_MAX_RELEVANT = LUMIERA_IDSTRING_MAX_RELEVANT;
  
  
  /** equality on Literal and Symbol values is defined
   *  based on the content, not the address. */
  bool
  Literal::operator== (const char* cString)  const
  {
    return !lumiera_strncmp (this->str_, cString, STRING_MAX_RELEVANT);
  }
  
  
  /** generating a hash value, e.g. for hashtables.
      This function is intended to be picked up by ADL,
      and should be usable both with  \c std::tr1 and 
      \c <boost/functional/hash.hpp> . It is implemented
      similar as the boost::hash specialisation for std::string */
  size_t
  hash_value (Literal sym)
  {
    size_t hash=0;
    if (sym)
      {
        const char *pos = sym;
        size_t maxpos = STRING_MAX_RELEVANT;
        for ( ; *pos && --maxpos; ++pos)
          hash_combine(hash, *pos);
      }
    
    return hash;
  }
  
  
  
  namespace { // quick-n-dirty symbol table implementation
    
    /** @warning grows eternally, never shrinks */
    std::unordered_set<string> symbolTable;                  ////////////////////////////////TICKET #158 replace by symbol table class
  }
  
  /** create Symbol by symbol table lookup.
   * @note identical strings will be mapped to
   *       the same Symbol (embedded pointer)
   */
  Symbol::Symbol (string&& definition)
   : Literal{symbolTable.insert(forward<string> (definition)).first->c_str()}
   { }
  
  
  
} // namespace lib
