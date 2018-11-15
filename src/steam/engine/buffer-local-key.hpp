/*
  BUFFER-LOCAL-KEY.hpp  -  opaque data for BufferProvider implementation

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


/** @file buffer-local-key.hpp
 ** A marker data type used in metadata / buffer management of the render engine.
 */


#ifndef STEAM_ENGINE_BUFFR_LOCAL_KEY_H
#define STEAM_ENGINE_BUFFR_LOCAL_KEY_H


#include "lib/error.hpp"
#include "lib/hash-value.h"

#include <boost/functional/hash.hpp>


namespace steam {
namespace engine {
  
  namespace metadata {
    class Key;
    class Entry;
  }
  class BufferMetadata;
  
  using lib::HashVal;
  
  
  
  /**
   * an opaque ID to be used by the BufferProvider implementation.
   * Typically this will be used, to set apart some pre-registered
   * kinds of buffers. It is treated as being part of the buffer type.
   * LocalKey objects may be copied but not re-assigned or changed.
   */
  class LocalKey
    {
      union OpaqueData
        {
          uint64_t _as_number;
          void*    _as_pointer;
        };
      
      OpaqueData privateID_;
      
    public:
      explicit
      LocalKey (uint64_t opaqueValue=0)
        { 
          privateID_._as_number = opaqueValue;
        }
      
      LocalKey (void* impl_related_ptr)
        { 
          privateID_._as_number  = 0;          
          privateID_._as_pointer = impl_related_ptr;
        }
      
      
      operator uint64_t()  const
        {
          return privateID_._as_number;
        }
      
      operator void*()  const
        {
          return privateID_._as_pointer;
        }
      
      bool
      isDefined()  const
        {
          return bool(privateID_._as_number); 
        }
      
      friend size_t
      hash_value (LocalKey const& lkey)
      {
        boost::hash<uint64_t> hashFunction;
        return hashFunction(lkey.privateID_._as_number);
      }
      
      friend bool
      operator== (LocalKey const& left, LocalKey const& right)
      {
        return uint64_t(left) == uint64_t(right);
      }
      friend bool
      operator!= (LocalKey const& left, LocalKey const& right)
      {
        return uint64_t(left) != uint64_t(right);
      }
      
      
    private:
      /** assignment usually prohibited */
      LocalKey& operator= (LocalKey const& o)
        {
          privateID_ = o.privateID_;
          return *this;
        }
      
      /** but Key assignments are acceptable */
      friend class metadata::Key;
    };
  
  
}} // namespace steam::engine
#endif
