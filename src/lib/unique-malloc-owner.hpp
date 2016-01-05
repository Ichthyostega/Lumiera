/*
  UNIQUE-MALLOC-OWNER.hpp  -  automatic management of C-malloced memory

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file unique-malloc-owner.hpp
 ** Helper to deal with C-MALLOCed memory automatically.
 ** This solution is a simple dress-up of std::unique_ptr,
 ** mostly for the purpose of documenting the issue at the
 ** usage site.
 ** 
 ** @see lib::meta::demangleCxx
 ** @see UniqueMallocOwner_test
 ** 
 */


#ifndef LIB_UNIQUE_MALLOC_OWNER_H
#define LIB_UNIQUE_MALLOC_OWNER_H


#include "lib/error.hpp"

#include <memory>


namespace lib {
  
  
  /**
   * Ownership token for a piece of heap memory allocated in plain-C style.
   * This smart ptr takes ownership of the memory given at construction,
   * to make sure it is deallocated properly with \c free(void*).
   * Ownership can be transferred by move semantics.
   * @remarks \c std::free is installed as deleter function,
   *          which means we're carrying along an additional pointer
   */
  template<class X =void>
  class UniqueMallocOwner
    : public std::unique_ptr<X, void(*)(void*)>
    {
      using _Parent = std::unique_ptr<X, void(*)(void*)>;
      
    public:
      explicit
      UniqueMallocOwner(void* memory =nullptr)
        : _Parent(static_cast<X*>(memory), std::free)
        { }
      
      explicit
      UniqueMallocOwner(X* alloc)
        : _Parent(alloc, std::free)
        { }
      
      
      bool
      empty()  const
        {
          return ! bool(*this);
        }
    };
  
  
  
  
  
} // namespace lib  
#endif /*LIB_UNIQUE_MALLOC_OWNER_H*/
