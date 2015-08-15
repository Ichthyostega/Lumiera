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
 ** The design restrictions were chosen deliberately, since a variant type might
 ** promote "probe and switch on type" style programming, which is known to be fragile.
 ** Likewise, we do not want to support mutations of the variant type at runtime. Basically,
 ** using a variant record is recommended only if either the receiving context has structural
 ** knowledge about the type to expect, or when a visitor implementation can supply a sensible
 ** handling for \em all the possible types. As an alternative, you might consider the
 ** lib::PolymorphicValue to hold types implementing a common interface.
 ** 
 ** \par implementation notes
 ** We use a "double capsule" implementation technique similar to lib::OpaqueHolder.
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
 ** To support copying and assignment of variant instances, but limit these operations
 ** to variants holding the same type, we use a virtual assignment function. In case the
 ** concrete type does not support assignment or copy construction, the respective access
 ** function is replaced by an implementation raising a runtime error.
 ** 
 ** @see Veriant_test
 ** @see lib::diff::GenNode
 ** @see virtual-copy-support.hpp
 ** 
 */


#ifndef LIB_VARIANT_H
#define LIB_VARIANT_H


#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/generator.hpp"
#include "lib/meta/virtual-copy-support.hpp"

#include <type_traits>
#include <utility>
#include <string>


namespace lib {
  
  using std::string;
  
  using std::move;
  using std::forward;
  using util::unConst;
  
  namespace error = lumiera::error;
  
  
  namespace { // implementation helpers
    
    using std::remove_reference;
    using meta::NullType;
    using meta::Node;
    
    
    template<typename X, typename TYPES>
    struct CanBuildFrom
      : CanBuildFrom<typename remove_reference<X>::type
                    ,typename TYPES::List
                    >
      { };
    
    template<typename X, typename TYPES>
    struct CanBuildFrom<X, Node<X, TYPES>>
      {
        using Type = X;
      };
    
    template<typename X, typename TYPES>
    struct CanBuildFrom<const X, Node<X, TYPES>>
      {
        using Type = X;
      };
    
    template<typename X, typename T,typename TYPES>
    struct CanBuildFrom<X, Node<T, TYPES>>
      {
        using Type = typename CanBuildFrom<X,TYPES>::Type;
      };
    
    template<typename X>
    struct CanBuildFrom<X, NullType>
      {
        static_assert (0 > sizeof(X), "No type in Typelist can be built from the given argument");
      };
    
    
    
    template<class VAL>
    struct ValueAcceptInterface
      {
        virtual void handle(VAL&) { /* NOP */ };
      };
    
    template<typename TYPES>
    using VisitorInterface
        = meta::InstantiateForEach<typename TYPES::List, ValueAcceptInterface>;
    
    
  }//(End) implementation helpers
  
  
  
  
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
   * @warning not threadsafe
   * @todo we need to define all copy operations explicitly, due to the
   *       templated one-arg ctor to wrap the actual values.
   *       There might be a generic solution for that     ////////////////////////TICKET #963  Forwarding shadows copy operations -- generic solution??
   *       But -- Beware of unverifiable generic solutions! 
   */
  template<typename TYPES>
  class Variant
    {
    public:
      enum { SIZ = meta::maxSize<typename TYPES::List>::value };
      
      class Visitor
        : public VisitorInterface<TYPES>
        {
        public:
          virtual ~Visitor() { } ///< this is an interface
        };
      
      
    private:
      /** Inner capsule managing the contained object (interface) */
      struct Buffer
        : meta::VirtualCopySupportInterface<Buffer>
        {
          char content_[SIZ];
          
          void* ptr() { return &content_; }
          
          
          virtual ~Buffer() {}           ///< this is an ABC with VTable
          
          virtual void dispatch (Visitor&)  =0;
          virtual operator string()  const  =0;
        };
      
      
      /** concrete inner capsule specialised for a given type */
      template<typename TY>
      struct Buff
        : meta::CopySupport<TY>::template Policy<Buffer,Buff<TY>>
        {
          static_assert (SIZ >= sizeof(TY), "Variant record: insufficient embedded Buffer size");
          
          TY&
          access()  const  ///< core operation: target is contained within the inline buffer
            {
              return *reinterpret_cast<TY*> (unConst(this)->ptr());
            }
          
         ~Buff()
            {
              access().~TY();
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
              new(Buffer::ptr()) TY(oBuff.access());
            }
          
          Buff (Buff && rBuff)
            {
              new(Buffer::ptr()) TY(move (rBuff.access()));
            }
          
          void
          operator= (Buff const& buff)
            {
              *this = buff.access();
            }
          
          void
          operator= (Buff&& rref)
            {
              *this = move (rref.access());
            }
          
          void
          operator= (TY const& ob)
            {
              if (&ob != Buffer::ptr())
                this->access() = ob;
            }
          
          void
          operator= (TY && rob)
            {
              this->access() = move(rob);
            }
          
          
          
          static Buff&
          downcast (Buffer& b)
            {
              Buff* buff = dynamic_cast<Buff*> (&b);
              
              if (!buff)
                throw error::Logic("Variant type mismatch: "
                                   "the given variant record does not hold "
                                   "a value of the type requested here"
                                  ,error::LUMIERA_ERROR_WRONG_TYPE);
              else
               return *buff;
            }
          
          void
          dispatch (Visitor& visitor)
            {
              ValueAcceptInterface<TY>& typeDispatcher = visitor;
              typeDispatcher.handle (this->access());
            }
          
          /** diagnostic helper */
          operator string()  const;
        };
      
      enum{ BUFFSIZE = sizeof(Buffer) };
      
      /** embedded buffer actually holding the concrete Buff object,
       *  which in turn holds and manages the target object.
       *  @note Invariant: always contains a valid Buffer subclass */
      char storage_[BUFFSIZE];
      
      
      
      
      
    protected: /* === internal interface for managing the storage === */
      
      Buffer&
      buffer()
        {
          return *reinterpret_cast<Buffer*> (&storage_);
        }
      Buffer const&
      buffer()  const
        {
          return *reinterpret_cast<const Buffer*> (&storage_);
        }
      
      template<typename X>
      Buff<X>&
      buff()
        {
          return Buff<X>::downcast(this->buffer());
        }
      
      
    public:
     ~Variant()
        {
          buffer().~Buffer();
        }
      
      Variant()
        {
          using DefaultType = typename TYPES::List::Head;
          
          new(storage_) Buff<DefaultType> (DefaultType());
        }
      
      template<typename X>
      Variant(X&& x)
        {
          using StorageType = typename CanBuildFrom<X, TYPES>::Type;
          
          new(storage_) Buff<StorageType> (forward<X>(x));
        }
      
      Variant (Variant& ref)
        {
          ref.buffer().copyInto (&storage_);
        }
      
      Variant (Variant const& ref)
        {
          ref.buffer().copyInto (&storage_);
        }
      
      Variant (Variant&& rref)
        {
          rref.buffer().moveInto (&storage_);
        }
      
      template<typename X>
      Variant&
      operator= (X x)
        {
          using RawType = typename remove_reference<X>::type;
          static_assert (meta::isInList<RawType, typename TYPES::List>(),
                         "Type error: the given variant could never hold the required type");
          static_assert (meta::can_use_assignment<RawType>::value, "target type does not support assignment");
          
          buff<RawType>() = forward<X>(x);
          return *this;
        }
      
      Variant&
      operator= (Variant& ovar)
        {
          ovar.buffer().copyInto (this->buffer());
          return *this;
        }
      
      Variant&
      operator= (Variant const& ovar)
        {
          ovar.buffer().copyInto (this->buffer());
          return *this;
        }
      
      Variant&
      operator= (Variant&& rvar)
        {
          rvar.buffer().moveInto (this->buffer());
          return *this;
        }
      
      
      /** diagnostic helper */
      operator string()  const;
      
      
      /* === Access === */
      
      template<typename X>
      X&
      get()
        {
          static_assert (meta::isInList<X, typename TYPES::List>(),
                         "Type error: the given variant could never hold the required type");
          
          return buff<X>().access();
        }
      
      template<typename X>
      X const&
      get()  const
        {
          return unConst(this)->template get<X>();
        }
      
      void
      accept (Visitor& visitor)
        {
          buffer().dispatch (visitor);
        }
    };
  
  
  
  /* == diagnostic helper == */

  
  template<typename TYPES>
  Variant<TYPES>::operator string()  const
  {
    return string(buffer());
  }
  
  template<typename TYPES>
  template<typename TY>
  Variant<TYPES>::Buff<TY>::operator string()  const
  {
    return "Variant|"
#ifdef LIB_FORMAT_UTIL_H
        +  util::str (this->access(),
                     (util::tyStr<TY>()+"|").c_str())
#endif
                     ;
  }
  
  
  
}// namespace lib
#endif /*LIB_VARIANT_H*/
