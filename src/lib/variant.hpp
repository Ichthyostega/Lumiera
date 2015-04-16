/*
  VARIANT.hpp  -  lightweight typesafe union record

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file variant.hpp
 ** A typesafe union record to carry embedded values of unrelated type.
 ** This file defines a simple alternative to boost::variant. It pulls in
 ** fewer headers, has a shorter code path and is hopefully more readable,
 ** but also doesn't deal with alignment issues and is <b>not threadsafe</b>.
 **  
 ** Deliberately, the design rules out re-binding of the contained type. Thus,
 ** once created, a variant \em must hold a valid element and always an element
 ** of the same type. Beyond that, variant elements are copyable and mutable.
 ** Direct access requires knowledge of the embedded type (no switch-on type).
 ** Type mismatch is checked at runtime. As a fallback, we provide a visitor
 ** scheme for generic access.
 ** 
 ** \par implementation notes
 ** We use a similar "double capsule" implementation technique as for lib::OpaqueHolder.
 ** In fact, Variant is almost identical to the latter, just omitting unnecessary flexibility.
 ** The outer capsule exposes the public handling interface, while the inner, private capsule
 ** is a polymorphic value holder. Since C++ as such does not support polymorphic values,
 ** the inner capsule is placed "piggyback" into a char buffer. The actual value is carried
 ** within yet another, nested char buffer. Thus, effectively the first "slot" of the storage
 ** will hold the VTable pointer, thereby encoding the actual type information -- leading to
 ** a storage requirement of MAX<TYPES...> plus one "slot" for the VTable. (with "slot" we
 ** denote the smallest disposable storage size for the given platform after alignment,
 ** typically the size of a size_t).
 ** 
 ** @see Veriant_test
 ** @see lib::diff::GenNode
 ** 
 */


#ifndef LIB_VARIANT_H
#define LIB_VARIANT_H


#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"
//#include "lib/util.hpp"

#include <type_traits>
//#include <utility>
//#include <string>
//#include <array>


namespace lib {
  
  using std::move;
  using std::string;
  using util::unConst;
  
  namespace error = lumiera::error;
  using error::LUMIERA_ERROR_WRONG_TYPE;
  
  
  /**
   * Typesafe union record.
   * A Variant element may carry an embedded value of any of the predefined types.
   * The type may not be rebound: It must be created holding some value and each
   * instance is fixed to the specific type used at construction time.
   * Yet within the same type, variant elements are copyable and assignable.
   * The embedded type is erased on the signature, but knowledge about the
   * actual type is retained, encoded into the embedded VTable. Thus,
   * any access to the variant's value requires knowledge of the type
   * in question, but type mismatch will provoke an exception at runtime.
   * Generic access is possible using a visitor.
   */
  template<typename TYPES>
  class Variant
    {
      enum { SIZ = meta::maxSize<typename TYPES::List>::value };
      
      /** Inner capsule managing the contained object (interface) */
      struct Buffer
        {
          char content_[SIZ];
          
          void* ptr() { return &content_; }
          
          
          virtual ~Buffer() {}           ///< this is an ABC with VTable
          
          virtual void  copyInto (void* targetStorage)  const =0;
        };
      
      
      /** concrete inner capsule specialised for a given type */
      template<typename TY>
      struct Buff
        : Buffer
        {
          static_assert (SIZ >= sizeof(TY), "Variant record: insufficient embedded Buffer size");
          
          TY&
          get()  const  ///< core operation: target is contained within the inline buffer
            {
              return *reinterpret_cast<TY*> (unConst(this)->ptr());
            }
          
         ~Buff()
            {
              get().~TY();
            }
          
          Buff (TY const& obj)
            {
              new(Buffer::ptr()) TY(obj);
            }
          
          Buff (TY && robj)
            {
              new(Buffer::ptr()) TY(move(robj));
            }
          
          Buff (Buff const& oBuff)
            {
              new(Buffer::ptr()) TY(oBuff.get());
            }
          
          Buff (Buff && rBuff)
            {
              new(Buffer::ptr()) TY(move (rBuff.get()));
            }
          
          Buff&
          operator= (TY const& obj)
            {
              if (&obj != Buffer::ptr())
                get() = obj;
              return *this;
            }
          
          Buff&
          operator= (TY && robj)
            {
              get() = move(robj);
              return *this;
            }
          
          Buff&
          operator= (Buff const& ref)
            {
              if (&ref != this)
                get() = ref.get();
              return *this;
            }
          
          Buff&
          operator= (Buff && rref)
            {
              get() = move(rref.get());
              return *this;
            }
          
          
          /* == virtual access functions == */
          
          virtual void
          copyInto (void* targetStorage)  const override
            {
              new(targetStorage) Buff(get());
            }
        };
      
      enum{ BUFFSIZE = sizeof(Buffer) };
      
      /** embedded buffer actually holding the concrete Buff object,
       *  which in turn holds and manages the target object.
       *  @note Invariant: always contains a valid Buffer subclass */
      char storage_[BUFFSIZE];
      
    public:
      Variant()
        {
          UNIMPLEMENTED("default constructed element of first type");
        }
      
      template<typename X>
      Variant(X const& x)
        {
          UNIMPLEMENTED("place buffer to hold element of type X");
        }
      
      template<typename X>
      Variant(X && x)
        {
          UNIMPLEMENTED("place buffer and move element of type X");
        }
      
      
#ifdef LIB_TEST_TEST_HELPER_H
      /* == diagnostic helper == */
      
      operator string()  const
        {
          UNIMPLEMENTED("diagnostic string conversion");
        }
#endif      
      
      /* === Access === */
      
      template<typename X>
      X&
      get()
        {
          UNIMPLEMENTED("value access");
        }
      
      
      class Visitor
        {
        public:
          virtual ~Visitor() { } ///< this is an interface
        };
      
      void
      accept (Visitor& visitor)
        {
          UNIMPLEMENTED("visitor style value access");
        }
    };
  
  
  
} // namespace lib  
#endif /*LIB_VARIANT_H*/
