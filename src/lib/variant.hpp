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
#include "lib/meta/generator.hpp"
//#include "lib/util.hpp"

#include <type_traits>
#include <typeindex>
//#include <utility>
//#include <string>
//#include <array>


namespace lib {
  
  using std::string;
  
  using std::move;
  using std::forward;
  using util::unConst;
  
  namespace error = lumiera::error;
  
  template<typename TYPES>
  class Variant;

  
  namespace { // implementation helpers
    
    using std::is_constructible;
    using std::is_move_constructible;
    using std::is_copy_constructible;
    using std::is_copy_assignable;
    using std::remove_reference;
    using std::remove_const;
    using std::is_same;
    using std::enable_if;
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
    
    
    template<typename T>
    struct Bare
      {
        using Type = typename remove_const<
                     typename remove_reference<T>::type>::type;
      };
    
    template<class T>
    struct is_Variant
      {
        static constexpr bool value = false;
      };
    
    template<class TYPES>
    struct is_Variant<Variant<TYPES>>
      {
        static constexpr bool value = true;
      };
    
    template<class V>
    struct use_if_is_Variant
      : enable_if<is_Variant<typename Bare<V>::Type>::value, V>
      { };
    
    
    
    using EmptyBase = struct{};
    
    template<class IFA, class BASE = EmptyBase>
    class VirtualCopySupportInterface
      : public BASE
      {
      public:
        virtual void  copyInto (void* targetStorage)  const =0;
        virtual void  moveInto (void* targetStorage)        =0;
        virtual void  copyInto (IFA&         target)  const =0;
        virtual void  moveInto (IFA&         target)        =0;
      };
    
    
    
    template<class B, class D>
    class NoCopyMoveSupport
      : public B
      {
        virtual void
        copyInto (void*)  const override
          {
            throw error::Logic("Copy construction invoked but target is noncopyable");
          }
        
        virtual void
        moveInto (void*)  override
          {
            throw error::Logic("Move construction invoked but target is noncopyable");
          }
        
        virtual void
        copyInto (B&)  const override
          {
            throw error::Logic("Assignment invoked but target is not assignable");
          }
        
        virtual void
        moveInto (B&)  override
          {
            throw error::Logic("Assignment invoked but target is not assignable");
          }
      };
    
    
    template<class B, class D>
    class MoveSupport
      : NoCopyMoveSupport<B, D>
      {
        virtual void
        copyInto (void*)  const override
          {
            throw error::Logic("Copy construction invoked but target allows only move construction");
          }
        
        virtual void
        moveInto (void* targetStorage)  override
          {
            D& src = static_cast<D&> (*this);
            new(targetStorage) D(move(src));
          }
      };
    
    
    template<class B, class D>
    class CloneSupport
      : public MoveSupport<B,D>
      {
        virtual void
        copyInto (void* targetStorage)  const override
          {
            D const& src = static_cast<D const&> (*this);
            new(targetStorage) D(src);
          }
      };
    
    
    template<class B, class D>
    class FullCopySupport
      : public CloneSupport<B,D>
      {
        virtual void
        copyInto (B& target)  const override
          {
            D&       t = D::downcast(target);
            D const& s = static_cast<D const&> (*this);
            t = s;
          }
        
        virtual void
        moveInto (B& target)  override
          {
            D& t = D::downcast(target);
            D& s = static_cast<D&> (*this);
            t = move(s);
          }
      };
    
    
    
    
    /** workaround for GCC 4.7: need to exclude some types,
     *  since they raise private access violation during probing.
     *  Actually, in C++11 such a case should trigger substitution
     *  failure, not an compilation error */
    template<class X>
    struct can_use_assignment
      : is_copy_assignable<X>
      { };
    
    template<>
    struct can_use_assignment<lib::time::Time>
      { static constexpr bool value = false; };
    
    
    
    template<class X>
    struct use_if_supports_only_move
      : enable_if<    is_move_constructible<X>::value
                  && !is_copy_constructible<X>::value
                  && !can_use_assignment<X>::value
                 ,X
                 >
      { };
      
    template<class X>
    struct use_if_supports_cloning
      : enable_if<    is_move_constructible<X>::value
                  &&  is_copy_constructible<X>::value
                  && !can_use_assignment<X>::value
                 ,X
                 >
      { };
      
    template<class X>
    struct use_if_supports_copy_and_assignment
      : enable_if<    is_move_constructible<X>::value
                  &&  is_copy_constructible<X>::value
                  &&  can_use_assignment<X>::value
                 ,X
                 >
      { };
    
    
    
    
    
    template<class X, class SEL=void>
    struct CopySupport
      {
        template<class B, class D>
        using Policy = NoCopyMoveSupport<B,D>;
      };
    
    template<class X>
    struct CopySupport<X,            typename use_if_supports_only_move<X>::type>
      {
        template<class B, class D>
        using Policy = MoveSupport<B,D>;
      };
    
    template<class X>
    struct CopySupport<X,            typename use_if_supports_cloning<X>::type>
      {
        template<class B, class D>
        using Policy = CloneSupport<B,D>;
      };
    
    template<class X>
    struct CopySupport<X,            typename use_if_supports_copy_and_assignment<X>::type>
      {
        template<class B, class D>
        using Policy = FullCopySupport<B,D>;
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
        : VirtualCopySupportInterface<Buffer>
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
        : CopySupport<TY>::template Policy<Buffer,Buff<TY>>
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
      
      
      void
      accept (Visitor& visitor)
        {
          buffer().dispatch (visitor);
        }
    };
  
  
  
} // namespace lib



  /* == diagnostic helper == */
  
#ifdef LIB_FORMAT_UTIL_H
namespace lib {
  
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
        +  util::str (this->access(),
                     (util::tyStr<TY>()+"|").c_str()
                     );
  }
} // namespace lib

#endif
#endif /*LIB_VARIANT_H*/
