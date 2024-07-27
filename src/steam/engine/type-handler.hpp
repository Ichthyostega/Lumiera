/*
  TYPE-HANDLER.hpp  -  a functor pair for setup and destruction

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

/** @file type-handler.hpp
 ** Helper holding a pair of type-build-up and destruction functors.
 ** Basically these two functors embody all type specific knowledge required
 ** to place an object into some buffer space and to clean up later. They may even
 ** be used in a more unspecific way, e.g. just to "prepare" a buffer or frame and to
 ** "clean up" after usage.
 ** 
 ** Within the Lumiera Engine, the BufferProvider default implementation utilises instances
 ** of TypeHandler to _describe specific buffer types_ capable of managing an attached object,
 ** or requiring some other kind of special treatment of the memory area used for the buffer.
 ** This BuffDescr is embodied into the BufferMetadata::Key and used later on to invoke
 ** the contained ctor / dtor functors, passing a concrete buffer (memory area).
 ** 
 ** @see buffer-metadata.hpp
 ** @see buffer-provider.hpp
 ** @see BufferMetadataKey_test#verifyTypeHandler unit-test
 */

#ifndef STEAM_ENGINE_TYPE_HANDLER_H
#define STEAM_ENGINE_TYPE_HANDLER_H


#include "lib/error.hpp"
#include "lib/hash-value.h"

#include <utility>
#include <functional>
#include <boost/functional/hash.hpp>


namespace steam {
namespace engine {
  
  using lib::HashVal;
  using std::bind;
  using std::forward;
  using std::function;
  using std::placeholders::_1;
  
  namespace error = lumiera::error;
  
  
  namespace { // (optional) helpers to build an object embedded into a buffer...
    
    template<class X, typename...ARGS>
    inline void
    buildIntoBuffer (void* storageBuffer, ARGS&& ...args)
    {
      new(storageBuffer) X(forward<ARGS> (args)...);
    }
    
    template<class X>
    inline void
    destroyInBuffer (void* storageBuffer)
    {
      X* embedded = static_cast<X*> (storageBuffer);
      embedded->~X();
    }
    
    template<typename CTOR, typename DTOR>
    inline HashVal
    deriveCombinedTypeIdenity()
    {
      HashVal hash{0};
      boost::hash_combine (hash, typeid(CTOR).hash_code());
      boost::hash_combine (hash, typeid(DTOR).hash_code());
      return hash;
    }
  }//(End)placement-new helpers
  
  
  
  /**
   * A pair of functors to maintain a datastructure within a buffer.
   * TypeHandler describes how to outfit the buffer in a specific way.
   * Special convenience builder function(s) are provided to create a
   * TypeHandler performing placement-new into a buffer given on invocation.
   * @note engine::BufferMetadata uses a TypeHandler to represent any
   *       special treatment of a buffer space. When defined, the buffer
   *       will be prepared on locking and cleanup will be invoked
   *       automatically when releasing. 
   * @warning comparison and hash values are based merely on the type
   *       of the Ctor and Dtor functions -- so all type handlers bound
   *       to the same functor type count as equivalent. This might not
   *       be what you'd expect, however, there is no sane way to test
   *       for equivalence of functors anyway. In the typical usage,
   *       a TypeHandler will be created by TypeHandler::create<TY>(),
   *       and thus will be dedicated to a given type to be placed
   *       into the storage buffer.
   */
  struct TypeHandler
    {
      typedef function<void(void*)> DoInBuffer;
      
      DoInBuffer createAttached;
      DoInBuffer destroyAttached;
      HashVal identity;
      
      /** Marker for the default case: raw buffer without type handling */
      static const TypeHandler RAW;
      
      /** build an invalid NIL TypeHandler */
      TypeHandler()
        : createAttached()
        , destroyAttached()
        , identity{0}
        { }
      
      /** build a TypeHandler
       *  binding to arbitrary constructor and destructor functions.
       *  On invocation, these functions get a void* to the buffer.
       * @note the functor objects created from these operations
       *       might be shared for handling multiple buffers.
       *       Be careful with any state or arguments.
       */
      template<typename CTOR, typename DTOR>
      TypeHandler(CTOR ctor, DTOR dtor)
        : createAttached (ctor)
        , destroyAttached (dtor)
        , identity{deriveCombinedTypeIdenity<CTOR,DTOR>()}
        { }
      
      /** builder function defining a TypeHandler
       *  to place an object into the buffer,
       *  possibly with given ctor arguments. */
      template<class X, typename...ARGS>
      static TypeHandler
      create (ARGS&& ...args)
        {
          return TypeHandler ( bind (buildIntoBuffer<X,ARGS...>, _1, forward<ARGS> (args)...)
                             , destroyInBuffer<X>);
        }
      
      bool
      isValid()  const
        {
          return bool(createAttached)
             and bool(destroyAttached);
        }
      
      friend HashVal
      hash_value (TypeHandler const& handler)
      {
        return handler.identity;
      }
      
      friend bool
      operator== (TypeHandler const& left, TypeHandler const& right)
      {
        return (not left.isValid() and not right.isValid())
            || (left.identity == right.identity);
      }
      friend bool
      operator!= (TypeHandler const& left, TypeHandler const& right)
      {
        return not (left == right);
      }
    };
  
  
  
}} // namespace steam::engine
#endif
