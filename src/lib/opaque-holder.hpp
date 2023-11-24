/*
  OPAQUE-HOLDER.hpp  -  buffer holding an object inline while hiding the concrete type

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
 ** # Managed opaque placement buffer
 ** 
 ** These templates help with building custom objects and wrappers based on
 ** this pattern: lib::InPlaceAnyHolder provides a buffer for target objects
 ** and controls access through a two-layer capsule; while the outer container
 ** exposes a neutral interface, the inner container keeps track of the actual
 ** type by means of a vtable. OpaqueHolder is built on top of InPlaceAnyHolder
 ** additionally to support a "common base interface" and re-access of the
 ** embedded object through this interface. For this to work, all of the
 ** stored types need to be derived from this common base interface.
 ** OpaqueHolder then may be even used like a smart-ptr, exposing this
 ** base interface. To the contrary, InPlaceAnyHolder has lesser requirements
 ** on the types to be stored within. It can be configured with policy classes
 ** to control the re-access; when using InPlaceAnyHolder_unrelatedTypes
 ** the individual types to be stored need not be related in any way, but
 ** of course this rules out anything beyond re-accessing the embedded object
 ** by knowing it's exact type. Generally speaking, re-accessing the concrete
 ** object requires knowledge of the actual type, similar to boost::any
 ** (but contrary to OpaqueHolder the latter uses heap storage).
 ** 
 ** # Lightweight passively managed opaque holder buffer
 ** 
 ** As a supplement, a more lightweight implementation is provided as
 ** lib::InPlaceBuffer, requiring just the object storage and lacking the
 ** ability to track the actual type of the embedded object, and the buffer
 ** can not be empty with this model -- which turns out to be adequate in
 ** most usage scenarios. This kind of lightweight "inline buffer" can even
 ** be exposed on API through a lib::PlantingHandle, allowing an arbitrary
 ** client to plant an likewise opaque implementation subclass into the
 ** buffer, as long as the storage size constraint is observed.
 ** 
 ** Using this approach is bound to specific stipulations regarding the
 ** properties of the contained object and the kind of access needed.
 ** When, to the contrary, the contained types are \em not related
 ** and you need to re-discover their concrete type, then maybe
 ** a visitor or variant record might be a better solution.
 ** 
 ** TICKET #1204 : proper alignment verified 10/2019
 ** 
 ** @see opaque-holder-test.cpp
 ** @see function-erasure.hpp usage example
 ** @see variant.hpp
 */


#ifndef LIB_OPAQUE_HOLDER_H
#define LIB_OPAQUE_HOLDER_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/access-casted.hpp"
#include "lib/meta/util.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>

#include <type_traits>
#include <utility>


namespace lib {
  
  namespace error = lumiera::error;
  using error::LERR_(BOTTOM_VALUE);
  using error::LERR_(WRONG_TYPE);
  
  using util::isSameObject;
  using util::unConst;
  
  
  namespace { // implementation helpers...
    
    using lib::meta::enable_if;
    using lib::meta::disable_if;
    using std::is_constructible;
    
    template<typename X>
                       enable_if< is_constructible<bool,X>,
    bool              >
    validitySelfCheck (X const& boolConvertible)
      {
        return bool(boolConvertible);
      }
    
    template<typename X>
                       disable_if< is_constructible<bool,X>,
    bool              >
    validitySelfCheck (X const&)
      {
        return true; // just pass if this type doesn't provide a validity check...
      }
    
  }
  
  
  
  /* ==== Policy classes controlling re-Access ==== */
  
  /**
   * Standard policy for accessing the contents via
   * a common base class interface. Using this policy
   * causes static or dynamic casts or direct conversion
   * to be employed as appropriate.
   */
  template<class BA>
  struct InPlaceAnyHolder_useCommonBase
    {
      typedef BA Base;
      
      template<class SUB>
      static Base*
      convert2base (SUB& obj)
        {
          SUB* oPtr = &obj;
          BA* asBase = util::AccessCasted<BA*>::access (oPtr);
          if (asBase)
            return asBase;
          
          throw error::Logic ("Unable to convert concrete object to Base interface"
                             , LERR_(WRONG_TYPE)
                             );
        }
    };
  
  /**
   * Alternative policy for accessing the contents without
   * a common interface; use this policy if the intention is
   * to use OpaqueHolder with a family of similar classes,
   * _without requiring all of them_ to be derived from
   * a _common base_ class. (E.g. std::function objects).
   * In this case, the "Base" type will be defined to void*
   * As a consequence, we loose all type information and
   * no conversions are possible on re-access. You need
   * to know the _exact_ type to get back at the object.
   */
  struct InPlaceAnyHolder_unrelatedTypes
    {
      typedef void Base;
      
      template<class SUB>
      static void*
      convert2base (SUB& obj)
        {
          return static_cast<void*> (&obj);
        }
    };
  
  
  
  
  
  
  
  /**
   * Inline buffer to hold and own an object while concealing the
   * concrete type. The object is given either as ctor parameter or
   * by direct assignment; it is copy-constructed into the buffer.
   * It is necessary to specify the required buffer storage space
   * as a template parameter. InPlaceAnyHolder may be created empty
   * or cleared afterwards, and this #empty() state may be detected
   * at runtime. In a similar vein, when the stored object has a
   * `bool` validity check, this can be accessed though #isValid().
   * Moreover `!empty() && isValid()` may be tested as by `bool`
   * conversion of the Holder object. The whole compound
   * is copyable if and only if the contained object is copyable.
   * 
   * @note assertion failure when trying to place an instance not
   *       fitting into given size.
   * @note \em not threadsafe!
   * @todo add support for moving of rvalue refs
   */
  template
    < size_t siz           ///< maximum storage required for the targets to be held inline
    , class AccessPolicy = InPlaceAnyHolder_unrelatedTypes
                           ///< how to access the contents via a common interface?
    >
  class InPlaceAnyHolder
    {
      typedef typename AccessPolicy::Base * BaseP;
      
      /** Inner capsule managing the contained object (interface) */
      struct Buffer
        {
          char content_[siz];
          void* ptr() { return &content_; }
          
          virtual ~Buffer() {}           ///< this is an ABC with VTable
          virtual bool  isValid()  const =0;
          virtual bool  empty()    const =0;
          virtual BaseP getBase()  const =0;
          
          virtual void  clone (void* targetStorage)  const =0;
        };
      
      /** special case: no stored object */
      struct EmptyBuff : Buffer
        {
          virtual bool isValid()  const { return false; }
          virtual bool empty()    const { return true; }
          
          BaseP
          getBase()  const
            {
              throw error::Invalid("accessing empty holder"
                                  , LERR_(BOTTOM_VALUE));
            }
          
          virtual void
          clone (void* targetStorage)  const
            {
              new(targetStorage) EmptyBuff();
            }
        };
      
      
      /** concrete subclass to manage a specific kind of contained object.
       *  @note invariant: #content_ always contains a valid SUB object */
      template<typename SUB>
      struct Buff : Buffer
        {
          static_assert (siz >= sizeof(SUB), "InPlaceAnyHolder: insufficient Buffer size");
          
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
              new(Buffer::ptr()) SUB (obj);
            }
          
          Buff (Buff const& oBuff)
            {
              new(Buffer::ptr()) SUB (oBuff.get());
            }
          
          Buff&
          operator= (Buff const& ref) ///< currently not used
            {
              if (&ref != this)
                get() = ref.get();
              return *this;
            }
          
          /* == virtual access functions == */
          
          virtual void
          clone (void* targetStorage)  const
            {
              new(targetStorage) Buff(get());
            }
          
          virtual BaseP
          getBase()  const
            {
              return AccessPolicy::convert2base (get());
            }
          
          virtual bool
          empty()  const
            {
              return false;
            }
          
          virtual bool
          isValid()  const
            {
              return validitySelfCheck (this->get());
            }
        };
      
      
      
      enum{ BUFFSIZE = sizeof(Buffer) };
      
      /** embedded buffer actually holding the concrete Buff object,
       *  which in turn holds and manages the target object.
       *  @note Invariant: always contains a valid Buffer subclass */
      char storage_[BUFFSIZE];
      
      
      
      
      
      
    protected: /* === internal interface for managing the storage === */
      
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
      
      
      void
      killBuffer()
        {
          buff().~Buffer();
        }
      
      void
      make_emptyBuff()
        {
          new(&storage_) EmptyBuff();
        }
      
      template<class SUB>
      void
      place_inBuff (SUB const& obj)
        {
          new(&storage_) Buff<SUB> (obj);
        }
      
      void
      clone_inBuff (InPlaceAnyHolder const& ref)
        {
          ref.buff().clone (storage_);
        }
      
      BaseP
      asBase ()  const   ///< @internal backdoor e.g. for comparisons
        {
          BaseP asBase = buff().getBase();
          ASSERT (asBase);
          return asBase;
        }
      
      
      
      
    public:
      
      ~InPlaceAnyHolder()
        {
          killBuffer();
        }
      
      void
      clear ()
        {
          killBuffer();
          make_emptyBuff();
        }
      
      
      InPlaceAnyHolder()
        {
          make_emptyBuff();
        }
      
      template<class SUB>
      InPlaceAnyHolder(SUB const& obj)
        {
          place_inBuff (obj);
        }
      
      InPlaceAnyHolder (InPlaceAnyHolder const& ref)
        {
          clone_inBuff (ref);
        }
      
      InPlaceAnyHolder&
      operator= (InPlaceAnyHolder const& ref)
        {
          if (not isSameObject (*this, ref))
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
      InPlaceAnyHolder&
      operator= (SUB const& newContent)
        {
          if (empty()
             or not isSameObject (*buff().getBase(), newContent)
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
      
      
      
      /** re-accessing the concrete contained object.
       *  This requires exact knowledge of the actual type
       *  of the element currently in storage. OpaqueHolder
       *  does not provide any "probing" or visitation mechanism.
       * @remarks You might consider lib::Variant or some visitor instead.
       * @throws lumiera::error::Logic when conversion/access fails
       * @throws lumiera::error::Invalid when accessing an empty holder
       */
      template<class SUB>
      SUB& get()  const
        {
          typedef const Buffer* Iface;
          typedef const Buff<SUB> * Actual;
          Iface interface = &buff();
          Actual actual = dynamic_cast<Actual> (interface);
          if (actual)
            return actual->get();
          
          if (this->empty())
            throw error::Invalid("accessing empty holder"
                                ,LERR_(BOTTOM_VALUE));
          else
            throw error::Logic ("Attempt to access OpaqueHolder's contents "
                                "specifying incompatible target type"
                               , LERR_(WRONG_TYPE)
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
      
      explicit
      operator bool() const
        {
          return isValid();
        }
    };
  
  
  
  
  
  
  /**
   * Inline buffer to hold and own an object while concealing the
   * concrete type. Access to the contained object is similar to a
   * smart-pointer, but the object isn't heap allocated. OpaqueHolder
   * may be created empty, which can be checked by a bool test.
   * The whole compound is copyable if and only if the contained
   * object is copyable.
   *
   * # using OpaqueHolder
   * OpaqueHolder instances are copyable value objects. They are created
   * either empty, by copy from an existing OpaqueHolder, or by directly
   * specifying the concrete object to embed. This target object will be
   * \em copy-constructed into the internal buffer. Additionally, you
   * may assign a new value, which causes the old value object to be
   * destroyed and a new one to be copy-constructed into the buffer.
   * Later on, the embedded value might be accessed
   * - using the smart-ptr-like access through the common base interface BA
   * - when knowing the exact type to access, the templated #get might be an option
   * - the empty state of the container and a `isValid()` on the target may be checked
   * - a combination of both is available as a `bool` check on the OpaqueHolder instance.
   * 
   * For using OpaqueHolder, several *assumptions* need to be fulfilled
   * - any instance placed into OpaqueHolder is below the specified maximum size
   * - the caller cares for thread safety. No concurrent get calls while in mutation!
   * 
   * @tparam BA  the nominal Base/Interface class for a family of types
   * @tparam siz maximum storage required for the targets to be held inline
   */
  template
    < class BA
    , size_t siz = sizeof(BA)
    >
  class OpaqueHolder
    : public InPlaceAnyHolder<siz, InPlaceAnyHolder_useCommonBase<BA>>
    {
      typedef InPlaceAnyHolder<siz, InPlaceAnyHolder_useCommonBase<BA>> InPlaceHolder;
      
    public:
      OpaqueHolder() : InPlaceHolder() {}
      
      template<class SUB>
      OpaqueHolder(SUB const& obj) : InPlaceHolder(obj) {}
      
      template<class SUB>
      OpaqueHolder&
      operator= (SUB const& newContent)
        {
          static_cast<InPlaceHolder&>(*this) = newContent;
          return *this;
        }
      
      // note: using standard copy operations
      
      
      
      /* === smart-ptr style access === */
      
      BA&
      operator* ()  const
        {
          ASSERT (!InPlaceHolder::empty());
          return *InPlaceHolder::buff().getBase();
        }
      
      BA*
      operator-> ()  const
        {
          ASSERT (!InPlaceHolder::empty());
          return InPlaceHolder::buff().getBase();
        }
      
    };
  
  
  
  
  
  
  template<class BA, class DEFAULT>
  class PlantingHandle;
  
  /**
   * Buffer to place and maintain an object instance privately within another object.
   * Variation of a similar concept as with OpaqueHolder, but implemented here
   * with reduced security and lesser overhead. InPlaceBuffer is just a chunk of
   * storage, which can be accessed through a common base class interface and
   * allows to place new objects there. It has no way to keep track of the
   * actual object living currently in the buffer. Thus, using InPlaceBuffer
   * requires the placed class(es) themselves to maintain their lifecycle,
   * and especially it is mandatory for the base class to provide a
   * virtual dtor. On the other hand, just the (alignment rounded)
   * storage for the object(s) placed into the buffer is required.
   * @remarks as a complement, PlantingHandle may be used on APIs to offer
   *         a lightweight way for clients to provide a callback.
   * @warning InPlaceBuffer really takes ownership, and even creates a
   *         default constructed instance of the base class right away.
   *         Yet the requirement for a virtual dtor is deliberately not
   *         enforced here, to allow use for types without VTable.
   * 
   * @tparam BA the nominal Base/Interface class for a family of types
   * @tparam siz maximum storage required for the targets to be held inline
   * @tparam DEFAULT the default instance to place initially
   */
  template
    < class BA
    , size_t siz = sizeof(BA)
    , class DEFAULT = BA
    >
  class InPlaceBuffer
    : util::NonCopyable
    {
      
      mutable char buf_[siz];
      
      
      BA&
      getObj()  const
        {
          return reinterpret_cast<BA&> (buf_);
        }
      
      void
      placeDefault()
        {
          static_assert (siz >= sizeof(DEFAULT), "InPlaceBuffer too small");
          
          new(&buf_) DEFAULT();
        }
      
      void
      destroy()
        {
          getObj().~BA();
        }
      
      
    public:
     ~InPlaceBuffer ()
        {
          destroy();
        }
      
      InPlaceBuffer ()
        {
          placeDefault();
        }
      
      /** immediately move-emplace an embedded subclass type */
      template<class SUB>
      InPlaceBuffer (SUB&& instance)
        {
          static_assert (siz >= sizeof(SUB), "InPlaceBuffer too small");
          
          new(&buf_) SUB (std::forward<SUB> (instance));
        }
      
      template<typename TY>
      struct TypeTag{ };
      /** helper to mark the subclass type to create.
       * @remarks we can not specify explicit template arguments on ctor calls,
       *  so the only way is to use a dummy marker argument to pass the type.
       *  Use as `InPlaceBuffer(embedType<XYZ>, arg1, arg2, arg3)` */
      template<typename SUB>
      static auto embedType() { return TypeTag<SUB>{}; }
      
      /** immediately emplace an embedded subclass type */
      template<class TY, typename...ARGS>
      InPlaceBuffer (TypeTag<TY>, ARGS&& ...args)
        {
          static_assert (siz >= sizeof(TY), "InPlaceBuffer too small");
          
          new(&buf_) TY (std::forward<ARGS> (args)...);
        }
      
      
      
      /** a "planting handle" can be used to expose an opaque InPlaceBuffer through an API */
      using Handle = PlantingHandle<BA, DEFAULT>;
      
      
      /** Abbreviation for placement new */
      template<class TY, typename...ARGS>
      TY&
      create (ARGS&& ...args)
        {
          static_assert (siz >= sizeof(TY), "InPlaceBuffer too small");
          
          destroy();
          try {
              return *new(&buf_) TY {std::forward<ARGS> (args)...};
            }
          catch (...)
            {
              placeDefault();
              throw;
            }
        }
      
      /** move-construct an instance of subclass into the opaque buffer */
      template<class SUB>
      SUB&
      emplace (SUB&& implementation)
        {
          static_assert (siz >= sizeof(SUB), "InPlaceBuffer too small");
          
          destroy();
          try {
              return *new(&buf_) SUB {std::forward<SUB> (implementation)};
            }
          catch (...)
            {
              placeDefault();
              throw;
            }
        }
      
      DEFAULT&
      reset()
        {
          destroy();
          placeDefault();
          return static_cast<DEFAULT&> (getObj());
        }
      
      
      
      
      /* === smart-ptr style access === */
      
      BA&
      operator* ()  const
        {
          return getObj();
        }
      
      BA*
      operator-> ()  const
        {
          return &getObj();
        }
      
      
      template<class SUB>
      SUB*
      access ()
        {
          BA * asBase = &getObj();
          SUB* content = util::AccessCasted<SUB*>::access (asBase);
          return content;
        }     // NOTE: might be null.
    };
  
  
  
  /**
   * A handle to allow for safe _»remote implantation«_
   * of an unknown subclass into a given opaque InPlaceBuffer,
   * without having to disclose the concrete buffer type or size.
   * @remarks this copyable value object is especially geared towards use
   *    as handle in APIs, allowing a not yet known implementation to implant
   *    an agent or collaboration partner into the likewise undisclosed innards
   *    of the service exposed.
   * @warning the type BA must expose a virtual dtor, since the targeted
   *    InPlaceBuffer has to take ownership of the implanted object.
   * @note the `siz` (buffer size) template parameter from #InPlaceBuffer
   *    is deliberately not part of the `PlantingHandle<BA,DEFAULT>` type,
   *    since buffer size can be considered an opaque implementation detail.
   *    As a consequence, we must capture this size information at construction
   *    time and store it at runtime #maxSiz_, to protect against buffer overrun.
   * @see OpaqueUncheckedBuffer_test
   */
  template<class BA, class DEFAULT = BA>
  class PlantingHandle
    {
      void* buffer_;
      size_t maxSiz_;
      
      static_assert (std::has_virtual_destructor<BA>(),
                     "target interface BA must provide virtual dtor, "
                     "since InPlaceBuffer needs to take ownership.");
      
      template<class SUB>
      void __ensure_can_create();
      
      
    public:
      template<size_t maxSiz>
      PlantingHandle (InPlaceBuffer<BA, maxSiz, DEFAULT>& targetBuffer)
        : buffer_(&targetBuffer)
        , maxSiz_(maxSiz)
        { }
      
      // default copy acceptable...
      
      
      template<class SUB>
      bool
      canCreate()  const
        {
          static_assert(std::is_base_of<BA,SUB>(), "concrete object implanted into the opaque "
                                                   "buffer must implement the defined interface");
          return sizeof(SUB) <= maxSiz_;
        }
      
      /** move-construct an instance of a subclass into the opaque buffer */
      template<class SUB>
      SUB&
      emplace (SUB&& implementation)
        {
          __ensure_can_create<SUB>();
          
          using Holder = InPlaceBuffer<BA, sizeof(SUB), DEFAULT>;
          Holder& holder = *static_cast<Holder*> (buffer_);
          
          return holder.template emplace (std::forward<SUB> (implementation));
        }
      
      /** Abbreviation for placement new of a subclass SUB into the opaque buffer*/
      template<class SUB, typename...ARGS>
      SUB&
      create (ARGS&& ...args)
        {
          __ensure_can_create<SUB>();
          
          using Holder = InPlaceBuffer<BA, sizeof(SUB), DEFAULT>;
          Holder& holder = *static_cast<Holder*> (buffer_);
          
          return holder.template create<SUB> (std::forward<ARGS> (args)...);
        }
      
      
      BA*
      get()  const
        {
          ENSURE (buffer_);
          BA& bufferContent = **static_cast<InPlaceBuffer<BA>*> (buffer_);
          return &bufferContent;
        }
    };
  
  
  
  /** @internal Helper to ensure the opaque buffer provides sufficient storage
   *  @tparam SUB actual subclass type to be implanted into the opaque buffer
   */
  template<class BA, class B0>
  template<class SUB>
  inline void
  PlantingHandle<BA,B0>::__ensure_can_create()
  {
    if (not this->canCreate<SUB>())
      throw error::Fatal("Unable to implant implementation object of size "
                         "exceeding the pre-established storage buffer capacity. "
                         +boost::lexical_cast<std::string>(sizeof(SUB)) + " > "
                         +boost::lexical_cast<std::string>(maxSiz_)
                        ,error::LUMIERA_ERROR_CAPACITY);
  }
  
  
  
} // namespace lib
#endif
