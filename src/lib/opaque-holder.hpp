/*
  OPAQUE-HOLDER.hpp  -  buffer holding an object inline while hiding the concrete type 
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/

/** @file opaque-holder.hpp
 ** Helper allowing type erasure while holding the actual object inline.
 ** Controlling the actual storage of objects usually binds us to commit
 ** to a specific type, thus ruling out polymorphism. But sometimes, when
 ** we are able to control the maximum storage for a family of classes, we
 ** can escape this dilemma by using the type erasure pattern combined with
 ** an inline buffer holding an object of the concrete subclass. Typically,
 ** this situation arises when dealing with functor objects.
 ** 
 ** This template helps building custom objects and wrappers based on this
 ** pattern: it provides an buffer for the target objects and controls access
 ** through a two-layer capsule; while the outer container exposes a neutral
 ** interface, the inner container keeps track of the actual type by means
 ** of a vtable. OpaqueHolder can be empty; but re-accessing the concrete
 ** object requires knowledge of the actual type, similar to boost::any
 ** (but the latter uses heap storage). 
 ** 
 ** Using this approach is bound to specific stipulations regarding the
 ** properties of the contained object and the kind of access needed.
 ** When, to the contrary, the contained types are \em not related
 ** and you need to re-discover their concrete type, then maybe
 ** a visitor or variant record might be a better solution.
 ** 
 ** @see opaque-holder-test.cpp
 ** @see function-erasure.hpp usage example
 ** @see variant.hpp 
 */


#ifndef LIB_OPAQUE_HOLDER_H
#define LIB_OPAQUE_HOLDER_H


#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"
#include "lib/access-casted.hpp"
#include "lib/util.hpp"


namespace lib {
  
  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  using util::isSameObject;
  using util::AccessCasted;
  using util::unConst;
  
  
  namespace { // implementation helpers...
    
    using boost::disable_if;
    using boost::is_convertible;
    
    bool
    validitySelfCheck (bool boolConvertible)
      {
        return boolConvertible;
      }
    
    template<typename X>
        typename disable_if< is_convertible<X,bool>, 
    bool >::type
    validitySelfCheck (X const&)
      {
        return true; // just pass if this type doesn't provide a validity check...
      }
    
  }
  
  
  
  
  /**
   * Inline buffer holding and owning an object while concealing the
   * concrete type. Access to the contained object is similar to a
   * smart-pointer, but the object isn't heap allocated. OpaqueHolder
   * may be created empty, which can be checked by a bool test.
   * The whole compound is copyable if and only if the contained object
   * is copyable.
   * 
   * For using OpaqueHolder, several \b assumptions need to be fulfilled
   * - any instance placed into OpaqueHolder is below the specified maximum size
   * - the caller cares for thread safety. No concurrent get calls while in mutation!
   */
  template
    < class BA                   ///< the nominal Base/Interface class for a family of types
    , size_t siz = sizeof(BA)    ///< maximum storage required for the targets to be held inline
    >
  class OpaqueHolder
    : public BoolCheckable<OpaqueHolder<BA,siz> >
    {
      
      /** Inner capsule managing the contained object (interface) */
      struct Buffer
        {
          char content_[siz];
          void* ptr() { return &content_; }
          
          virtual ~Buffer() {}
          virtual bool isValid()  const { return false; }
          virtual bool empty()    const { return true; }
          
          virtual void
          clone (void* targetStorage)  const
            {
              new(targetStorage) Buffer();
            }
          
          virtual BA&
          get()  const
            {
              throw lumiera::error::Logic ("get() called on empty Buffer");
            }
        };
      
      
      /** concrete subclass managing a specific kind of contained object.
       *  @note invariant: content_ always contains a valid SUB object */
      template<typename SUB>
      struct Buff : Buffer
        {
          SUB&
          get()  const  ///< core operation: target is contained within the inline buffer
            {
              return *reinterpret_cast<SUB*> (unConst(this)->ptr());
            }
          
          ~Buff()
            {
              get().~SUB();
            }
          
          explicit
          Buff (SUB const& obj)
            {
              REQUIRE (siz >= sizeof(SUB));
              new(Buffer::ptr()) SUB (obj);
            }
          
          Buff (Buff const& oBuff)
            {
              new(Buffer::ptr()) SUB (oBuff.get());
            }
      
          Buff&
          operator= (Buff const& ref) ///< not used currently 
            {
              if (&ref != this)
                get() = ref.get();
              return *this;
            }
          
          
          void
          clone (void* targetStorage)  const
            {
              new(targetStorage) Buff(this->get());
            }
          
          bool
          empty()  const
            {
              return false;
            }
          
          bool
          isValid()  const
            {
              return validitySelfCheck (get());
            }
        };
      
      
      enum{ BUFFSIZE = sizeof(Buffer) };
      
      /** embedded buffer actually holding the concrete Buff object,
       *  which in turn holds and manages the target object.
       *  @note Invariant: always contains a valid Buffer subclass */
      char storage_[BUFFSIZE];
      
      
      
      
      Buffer&
      buff()
        {
          return *reinterpret_cast<Buffer*> (&storage_);
        }
      const Buffer&
      buff()  const
        {
          return *reinterpret_cast<const Buffer *> (&storage_);
        }
      
      void killBuffer()                          
        { 
          buff().~Buffer();
        }
      
      void make_emptyBuff()
        {
          new(&storage_) Buffer();
        }
      
      template<class SUB>
      void place_inBuff (SUB const& obj)
        {
          new(&storage_) Buff<SUB> (obj);
        }
      
      void clone_inBuff (OpaqueHolder const& ref)
        {
          ref.buff().clone (storage_);
        }
    
      
    public:
      ~OpaqueHolder()
        {
          killBuffer();
        }
      
      void
      clear ()
        {
          killBuffer();
          make_emptyBuff();
        }
      
      
      OpaqueHolder()
        { 
          make_emptyBuff();
        }
      
      template<class SUB>
      OpaqueHolder(SUB const& obj)
        { 
          place_inBuff (obj);
        }
      
      OpaqueHolder (OpaqueHolder const& ref)
        {
          clone_inBuff (ref);
        }
      
      OpaqueHolder&
      operator= (OpaqueHolder const& ref)
        {
          if (!isSameObject (*this, ref))
            {
              killBuffer();
              try
                {
                  clone_inBuff (ref);
                }
              catch (...)
                {
                  make_emptyBuff();
                  throw;
                }
            }
          return *this;
        }
      
      template<class SUB>
      OpaqueHolder&
      operator= (SUB const& newContent)
        {
          if (  !empty() 
             && !isSameObject (buff().get(), newContent)
             )
            {
              killBuffer();
              try
                {
                  place_inBuff (newContent);
                }
              catch (...)
                {
                  make_emptyBuff();
                  throw;
                }
            }
          return *this;
        }
      
      
      /* === smart-ptr style access === */
      
      BA&
      operator* ()  const
        {
          ASSERT (!empty());
          return buff().get();
        }
      
      BA* 
      operator-> ()  const
        {
          ASSERT (!empty());
          return &(buff().get());
        }
      
      template<class SUB>
      SUB& get()  const
        {
          typedef const Buffer* Iface;
          typedef const Buff<SUB> * Actual;
          Iface interface = &buff();
          Actual actual = dynamic_cast<Actual> (interface);
          if (actual)
            return actual->get();
          
          // second try: maybe we can perform a
          // dynamic upcast or direct conversion on the
          // concrete target object. But we need to exclude a
          // brute force static cast (which might slice or reinterpret)
          if (!util::use_static_downcast<BA*,SUB*>::value)
            {
              BA*  asBase  = &(buff().get());
              SUB* content = AccessCasted<SUB*>::access (asBase);
              if (content) 
                return *content;
            }
          
          throw lumiera::error::Logic ("Attempt to access OpaqueHolder's contents "
                                       "specifying incompatible target type"
                                      , LUMIERA_ERROR_WRONG_TYPE
                                      );
        }
      
      
      bool
      empty() const
        {
          return buff().empty();
        }
      
      
      bool
      isValid() const
        {
          return buff().isValid();
        }
    };
  
  
  
  
} // namespace lib
#endif
