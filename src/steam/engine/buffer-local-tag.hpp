/*
  BUFFER-LOCAL-TAG.hpp  -  opaque data for BufferProvider implementation

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


/** @file buffer-local-tag.hpp
 ** A marker data type used in metadata / buffer management of the render engine.
 */


#ifndef STEAM_ENGINE_BUFFR_LOCAL_TAG_H
#define STEAM_ENGINE_BUFFR_LOCAL_TAG_H


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
   * an opaque mark to be used by the BufferProvider implementation.
   * Typically this will be used, to set apart some pre-registered
   * kinds of buffers. It is treated as being part of the buffer type.
   * LocalTag objects may be copied but not re-assigned or changed.
   */
  class LocalTag
    {
      union OpaqueData
        {
          uint64_t _as_number;
          void*    _as_pointer;
        };
      
      OpaqueData privateID_;
      
    public:
      explicit
      LocalTag (uint64_t opaqueValue=0)
        { 
          privateID_._as_number = opaqueValue;
        }
      
      LocalTag (void* impl_related_ptr)
        { 
          privateID_._as_number  = 0;          
          privateID_._as_pointer = impl_related_ptr;
        }
      
      /** Marker when no distinct local key is given */
      static const LocalTag UNKNOWN;
      
      operator uint64_t()  const
        {
          return privateID_._as_number;
        }
      
      operator void*()  const
        {
          return privateID_._as_pointer;
        }
      
      explicit
      operator bool()  const
        {
          return bool(privateID_._as_number); 
        }
      
      friend size_t
      hash_value (LocalTag const& lkey)
      {
        boost::hash<uint64_t> hashFunction;
        return hashFunction(lkey.privateID_._as_number);
      }
      
      friend bool
      operator== (LocalTag const& left, LocalTag const& right)
      {
        return uint64_t(left) == uint64_t(right);
      }
      friend bool
      operator!= (LocalTag const& left, LocalTag const& right)
      {
        return uint64_t(left) != uint64_t(right);
      }
      
      
    private:
      /** assignment usually prohibited */
      LocalTag& operator= (LocalTag const& o)
        {
          privateID_ = o.privateID_;
          return *this;
        }
      
      /** but Key assignments are acceptable */
      friend class metadata::Key;
    };
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_BUFFR_LOCAL_TAG_H*/
