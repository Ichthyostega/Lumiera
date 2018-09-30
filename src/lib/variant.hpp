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
 ** once created, a variant _must hold a valid element_ and always an element
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
 ** handling for _all the possible types._ As an alternative, you might consider the
 ** lib::PolymorphicValue to hold types implementing a common interface.
 ** 
 ** ## implementation notes
 ** 
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
 ** @note we use a Visitor interface generated through metaprogramming.
 **       This may generate a lot of warnings "-Woverloaded-virtual",
 **       since one `handle(TX)` function may shadow other `handle(..)` functions
 **       from the inherited (generated) Visitor interface. These warnings are besides
 **       the point, since not the _client_ uses these functions, but the Variant does,
 **       after upcasting to the interface. Make sure you define your specialisations with
 **       the override modifier; when done so, it is safe to disable this warning here.
 ** 
 ** @see Veriant_test
 ** @see lib::diff::GenNode
 ** @see virtual-copy-support.hpp
 ** 
 */


#ifndef LIB_VARIANT_H
#define LIB_VARIANT_H


#include "lib/error.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/generator.hpp"
#include "lib/meta/virtual-copy-support.hpp"
#include "lib/format-obj.hpp"
#include "lib/util.hpp"

#include <type_traits>
#include <utility>
#include <string>


namespace lib {
  
  using std::string;
  
  using std::move;
  using std::forward;
  using util::unConst;
  
  namespace error = lumiera::error;
  
  
  namespace variant { // implementation metaprogramming helpers
    
    using std::remove_reference;
    using meta::NullType;
    using meta::Types;
    using meta::Node;
    
    
    template<typename X, typename TYPES>
    struct CanBuildFrom
      : CanBuildFrom<typename remove_reference<X>::type
                    ,typename TYPES::List
                    >
      { };
    
    template<typename X, typename TYPES>
    struct CanBuildFrom<X, Node<X, TYPES>>
      : std::true_type
      {
        using Type = X;
      };
    
    template<typename X, typename TYPES>
    struct CanBuildFrom<const X, Node<X, TYPES>>
      : std::true_type
      {
        using Type = X;
      };
    
    template<typename TYPES, size_t len>
    struct CanBuildFrom<const char [len], Node<string, TYPES>> ///< esp. allow to build string from char literal
      : std::true_type
      {
        using Type = string;
      };
    
    template<typename X, typename T,typename TYPES>
    struct CanBuildFrom<X, Node<T, TYPES>>
      : CanBuildFrom<X,TYPES>
      { };
    
    template<typename X>
    struct CanBuildFrom<X, NullType>
      : std::false_type
      { };
    
    
    template<typename T>
    struct Identity { using Type = T; };
    
    /**
     * Helper to pick the first type from a type sequence,
     * which fulfils the predicate (meta function) given as template
     * @tparam TYPES a type sequence or type list
     * @tparam a predicate template or type trait
     * @note result as embedded typedef `Type`
     */
    template<class TYPES, template<class> class _P_>
    struct FirstMatchingType
      {
        static_assert(not sizeof(TYPES), "None of the possible Types fulfils the condition");
      };
    
    template<class...TYPES, template<class> class _P_>
    struct FirstMatchingType<Types<TYPES...>, _P_>
      : FirstMatchingType<typename Types<TYPES...>::List, _P_>
      { };
    
    template<class T, class TYPES, template<class> class _P_>
    struct FirstMatchingType<Node<T,TYPES>, _P_>
      : std::conditional_t<_P_<T>::value, Identity<T>, FirstMatchingType<TYPES, _P_>>
      {  };
    
    
    
    template<typename RET>
    struct VFunc
      {
        /** how to treat one single type in visitation */
        template<class VAL>
        struct ValueAcceptInterface
          {
            virtual RET handle(VAL&) { /* do nothing */ return RET(); };
          };
        
        
        /** build a generic visitor interface for all types in list */
        template<typename TYPES>
        using VisitorInterface
            = meta::InstantiateForEach<typename TYPES::List, ValueAcceptInterface>;
        
      };
    
    
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
      
      template<typename RET>
      using VisitorFunc      = typename variant::VFunc<RET>::template VisitorInterface<TYPES>;
      template<typename RET>
      using VisitorConstFunc = typename variant::VFunc<RET>::template VisitorInterface<meta::ConstAll<typename TYPES::List>>;
      
      /**
       * to be implemented by the client for visitation
       * @see #accept(Visitor&)
       */
      class Visitor
        : public VisitorFunc<void>
        {
        public:
          virtual ~Visitor() { } ///< this is an interface
        };
      
      class Predicate
        : public VisitorConstFunc<bool>
        {
        public:
          virtual ~Predicate() { } ///< this is an interface
        };
      
      /**
       * Metafunction to pick the first of the variant's types,
       * which satisfies the given trait or predicate template
       * @note result is the embedded typedef `FirstMatching<P>::Type`
       */
      template<template<class> class _P_>
      using FirstMatching = variant::FirstMatchingType<TYPES, _P_>;
      
      
    private:
      /** Inner capsule managing the contained object (interface) */
      struct Buffer
        : meta::VirtualCopySupportInterface<Buffer>
        {
          char content_[SIZ];
          
          void* ptr() { return &content_; }
          
          
          virtual ~Buffer() {}           ///< this is an ABC with VTable
          
          virtual void dispatch (Visitor&)         =0;
          virtual bool dispatch (Predicate&) const =0;
          virtual operator string()          const =0;
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
              if (&rob != Buffer::ptr())
                this->access() = move(rob);
            }
          
          
          
          static string indicateTypeMismatch (Buffer&);
          
          
          static Buff&
          downcast (Buffer& b)
            {
              Buff* buff = dynamic_cast<Buff*> (&b);
              
              if (!buff)
                throw error::Logic(indicateTypeMismatch(b)
                                  ,error::LERR_(WRONG_TYPE));
              else
               return *buff;
            }
          
          void
          dispatch (Visitor& visitor)
            {
              using Dispatcher = variant::VFunc<void>::template ValueAcceptInterface<TY>;
              
              Dispatcher& typeDispatcher = visitor;
              typeDispatcher.handle (this->access());
            }
          
          bool
          dispatch (Predicate& visitor)  const
            {
              using Dispatcher = variant::VFunc<bool>::template ValueAcceptInterface<const TY>;
              
              Dispatcher& typeDispatcher = visitor;
              return typeDispatcher.handle (this->access());
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
      
      /** @internal for derived classes to implement custom access logic */
      template<typename X>
      X*
      maybeGet()
        {
          Buff<X>* buff = dynamic_cast<Buff<X>*> (& this->buffer());
          if (buff)
            return & buff->access();
          else
            return nullptr;
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
          static_assert (variant::CanBuildFrom<X, TYPES>(), "No type in Typelist can be built from the given argument");
          
          using StorageType = typename variant::CanBuildFrom<X, TYPES>::Type;
          
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
          using RawType = typename std::remove_reference<X>::type;
          static_assert (meta::isInList<RawType, typename TYPES::List>(),
                         "Type error: the given variant could never hold the required type");
          static_assert (std::is_copy_assignable<RawType>::value, "target type does not support assignment");
          
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
      
      //note: NOT defining a swap operation, because swapping inline storage is pointless!
      
      
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
      
      bool
      accept (Predicate& visitor)  const
        {
          return buffer().dispatch (visitor);
        }
    };
  
  
  
  /* == diagnostic helper == */

  
  template<typename TYPES>
  Variant<TYPES>::operator string()  const
  {
    return "Variant|" + string(buffer());
  }
  
  template<typename TYPES>
  template<typename TY>
  Variant<TYPES>::Buff<TY>::operator string()  const
  {
    return util::typedString (this->access());
  }
  
  /**
   * error message when accessing the variant content with wrong type assumptions.
   * @remark while this diagnostics can be crucial for finding bugs, we avoid
   *  including \ref format-string.hpp, since lib::Variant is used pervasively
   *  as part of lib::diff::GenNode. Especially in development builds, we observed
   *  a tangible leverage on executable size. Thus we implement the protection
   *  against follow-up exceptions explicitly here.
   */
  template<typename TYPES>
  template<typename TY>
  inline string
  Variant<TYPES>::Buff<TY>::indicateTypeMismatch(Variant<TYPES>::Buffer& target)
  {
    try {
        return "Variant type mismatch: expected value of type «"
             + lib::meta::typeStr<TY>()+"», "
             + "however the given variant record is "
             + string{target};
      }
    catch(...) { return lib::meta::FAILURE_INDICATOR; }
  }
  
  
}// namespace lib
#endif /*LIB_VARIANT_H*/
