/*
  SCOPED-COLLECTION.hpp  -  managing a fixed collection of noncopyable polymorphic objects

   Copyright (C)
     2012,2026        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file scoped-collection.hpp
 ** Manage a collection of non-copyable polymorphic objects in compact storage.
 ** This helper supports the common situation where a service implementation has
 ** to manage a fixed collection of implementation related sub-components, treated
 ** with reference semantics. Typically, those objects are used polymorphically,
 ** and often even with _type erasure_ and added step by step through a builder.
 ** The storage to hold all those child objects is allocated in one chunk
 ** and never adjusted. Individual children are never re-ordered or removed,
 ** only the collection as a whole is discarded eventually.
 ** 
 ** # usage patterns
 ** 
 ** The common ground for all usage of this container is to hold some elements
 ** with exclusive ownership; when the enclosing container goes out of scope,
 ** all the dtors of the embedded objects will be invoked. Frequently this
 ** side effect is one of the reasons for using the container: we want to own
 ** some resource handles and be sure they are available exactly as long as
 ** the managing scope is alive and able to access them.
 ** 
 ** There are two different usage patterns for populating a ScopedCollection
 ** - the **stack style** usage creates an empty container (using the one arg
 **   ctor to specify the maximum size only). The storage capacity to hold
 **   a number of objects, up to the predefined maximum, is (heap) allocated
 **   immediately at construction of the container, yet no child objects are
 **   created. Later on, individual objects are "pushed" into the collection
 **   by invoking #emplaceElement() to create a new element of the default
 **   type `I`) or #emplace<Type>(args) to create some subtype. This way,
 **   the container is gradually filled up.
 ** - the **RAII-style** usage attempts to create all the contained objects
 **   right away, immediately after memory allocation. This usage pattern
 **   avoids any kind of _lifecycle state_. Either the container is in sane
 **   state and fully populated from the outset, or the call to the ctor fails
 **   and all objects that have already been created are discarded.
 ** @note intentionally there is no operation to discard individual objects,
 **       the only option is to #clear() the whole container.
 ** @note the container can hold instances of a subclass of the type defined
 **       by the template parameter `I`. But you need to ensure in this case
 **       that the defined buffer size for each element (2nt template parameter)
 **       is sufficient to hold any of these subclass instances. This condition
 **       is protected by a static assertion (compilation failure).
 ** @warning when using subclasses, defining a virtual dtor is mandatory!
 ** @warning deliberately **not threadsafe**
 ** 
 ** @see ScopedCollection_test
 ** @see scoped-ptrvect.hpp quite similar, but using individual heap pointers
 */


#ifndef LIB_SCOPED_COLLECTION_H
#define LIB_SCOPED_COLLECTION_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/meta/trait.hpp"
#include "lib/iter-adapter.hpp"

#include <cstddef>
#include <concepts>


namespace lib {
  
  namespace error = lumiera::error;
  using LERR_(INDEX_BOUNDS);
  using LERR_(CAPACITY);
  
  using meta::lum_iter;
  
  
  /**
   * A fixed collection of non-copyable polymorphic objects.
   * 
   * All child objects reside in a common chunk of storage
   * and are owned and managed by this collection holder.
   * Array style access and iteration is provided.
   * @tparam I   the nominal Base/Interface class for a family of types
   * @tparam siz maximum storage required for the targets to be held inline
   */
  template
    < class I
    , size_t siz = sizeof(I)
    >
  class ScopedCollection
    : util::NonCopyable
    {
      
    public:
      /**
       * Storage Frame to hold one Child object.
       * The storage will be an heap allocated array
       * of such Wrapper objects.
       * @note doesn't manage the Child
       */
      class ElementHolder
        : util::NonCopyable
        {
          alignas(I) mutable
            std::byte buf_[siz];
          
        public:
          
          I&
          accessObj()  const
            {
              return * std::launder (reinterpret_cast<I*> (&buf_));
            }
          
          void
          destroy()
            {
              accessObj().~I();
            }
          
          
          
          /** place object of type TY, forwarding ctor arguments */
          template<class TY, typename...ARGS>
          TY&
          create (ARGS&& ...args)
            {
              static_assert ( meta::is_Subclass_v<TY,I>
                             && sizeof(TY) <= siz,
                             "ElementHolder buffer too small");
              
              return *new(&buf_) TY (std::forward<ARGS> (args)...);
            }
        };
      
      
      
      
      
      
     ~ScopedCollection ()
        {
          clear();
        }
      
      /** create an empty ScopedCollection with given max capacity
       * @remark this ctor variant is intended for _stack-like_ usage
       */
      explicit
      ScopedCollection (size_t maxElements)
        : level_(0)
        , capacity_(maxElements)
        , elements_(new ElementHolder[maxElements])
        { }
      
      /** creating a ScopedCollection in RAII-style:
       *  The embedded elements will be created immediately.
       *  Ctor fails in case of any error during element creation.
       * @param builder functor to be invoked for each "slot".
       *        It gets an ElementHolder& as parameter, and should
       *        use this to create an object of suitable I-subclass
       */
      template<std::invocable<ElementHolder&> CTOR>
      ScopedCollection (size_t maxElements, CTOR builder)
        : level_(0)
        , capacity_(maxElements)
        , elements_(new ElementHolder[maxElements])
        {
          populate_by (builder);
        }
      
      /** variation of RAII-style: using a builder function,
       *  which is a member of some object. This supports the
       *  typical usage situation, where a manager object builds
       *  a ScopedCollection of some components
       * @param builder member function used to create the elements
       * @param instance the owning class instance, on which the
       *        builder member function will be invoked ("this").
       */
      template<class TY>
      ScopedCollection (size_t maxElements, void (TY::*builder) (ElementHolder&), TY * const instance)
        : level_(0)
        , capacity_(maxElements)
        , elements_(new ElementHolder[maxElements])
        {
          populate_by (builder,instance);
        }
      
      /** variation of RAII-style: "pull" elements from an iterator.
       *  Each element will be created directly by invoking either the copy ctor,
       *  or a suitable 1-argument ctor, depending on the type yielded by the iterator.
       *  No further elements will be created however, once the iterator is exhausted.
       * @note anything in accordance to the Lumiera Forward concept is acceptable.
       *       This rules out just passing a plain STL iterator (because these can't
       *       tell for themselves when they're exhausted). Use an suitable iter-adapter
       *       instead, e.g. by invoking lib::iter_stl::eachElm(stl_container)
       */
      template<lum_iter IT>
      ScopedCollection (size_t maxElements, IT&& iter)
        : level_(0)
        , capacity_(maxElements)
        , elements_(new ElementHolder[maxElements])
        {
          pull_from (std::forward<IT> (iter));
        }
      
      /* == some pre-defined Builders == */
      
      /** Builder: ills the given storage slot with a I-instance
       *  constructed from copies of the given arguments.
       * @return builder-λ to pass into ScopeCollection's ctor. */
      template<typename...ARGS>
      static auto fill (ARGS&&... args);
      
      /** Builder: fills the given storage slot with a TY-instance
       *  constructed from copies of the given arguments. */
      template<typename TY, typename...ARGS>
      static auto fillWith (ARGS&&... args);
      
      /** Builder: fills the given storage slot from invoking the
       *  provided factory functor, with either an I-element or a
       *  I-subtype, depending on the functor's result type.
       * @note the functor will be copied in the resulting λ */
      template<std::invocable<> FUN>
      static auto invoke (FUN&& factory);
      
      
      void
      clear()
        {
          REQUIRE (level_ <= capacity_, "Storage corrupted");
          
          while (level_)
            {
              --level_;
              try {
                  elements_[level_].destroy();
                }
              ERROR_LOG_AND_IGNORE (progress, "Clean-up of element in ScopedCollection")
            }
        }
      
      /** init all elements default constructed */
      void
      populate()
        try {
            while (level_ < capacity_)
              {
                elements_[level_].template create<I>();
                ++level_;
              }
          }
        catch(...)
          {
            WARN (progress, "Failure while populating ScopedCollection. "
                            "All elements will be discarded");
            clear();
            throw;
          }
      
      /** init all elements at once,
       *  invoking a builder functor for each.
       * @param builder to create the individual elements
       *        this functor is responsible to invoke the appropriate
       *        ElementHolder#create function, which places a new element
       *        into the storage frame passed as parameter.
       */
      template<std::invocable<ElementHolder&> CTOR>
      void
      populate_by (CTOR builder)
        try {
          while (level_ < capacity_)
            {
              ElementHolder& storageFrame (elements_[level_]);
              builder (storageFrame);
              ++level_;
          } }
        catch(...)
          {
            WARN (progress, "Failure while populating ScopedCollection. "
                            "All elements will be discarded");
            clear();
            throw;
          }
      
      /** variation of element initialisation,
       *  invoking a member function of some manager object
       *  for each collection element to be created.
       */
      template<class TY>
      void
      populate_by (void (TY::*builder) (ElementHolder&), TY * const instance)
        try {
          while (level_ < capacity_)
            {
              ElementHolder& storageFrame (elements_[level_]);
              (instance->*builder) (storageFrame);
              ++level_;
          } }
        catch(...)
          {
            WARN (progress, "Failure while populating ScopedCollection. "
                            "All elements will be discarded.");
            clear();
            throw;
          }
      
      template<lum_iter IT>
      struct _BuildFromIter ///< @internal helper to determine what to build from an iterator
        {
          using Yield = iter::Yield<IT>;
          using Value = meta::ValueTypeBinding<IT>::value_type;
          using Type  = std::conditional_t<meta::is_Subclass_v<Value, I>, Value, I>;
          static_assert (std::is_constructible_v<Type, Yield>
                        ,"Iterator must produce something "
                         "that can be used to build contents of this container");
        };
      
      /** variation of element initialisation,
       *  creating further elements from the results of an iterator,
       *  until either the container is filled up or the iterator exhausted.
       */
      template<lum_iter IT>
      void
      pull_from (IT iter)
        try {
          while (iter and level_ < capacity_)
            {
              using Element = _BuildFromIter<IT>::Type;
              ElementHolder& storageFrame (elements_[level_]);
              storageFrame.template create<Element> (*iter);
              ++level_;
              ++iter;
          } }
        catch(...)
          {
            WARN (progress, "Failure while pulling Iterator into ScopedCollection. "
                            "All elements will be discarded.");
            clear();
            throw;
          }

      
      
      /** push a new element of default type
       *  to the end of this container
       * @note EX_STRONG */
      I&
      emplaceElement()
        {
          return emplace<I>();
        }
      
      
      /**
       * push new entry at the end of this container
       * and build object of type TY in place there
       */
      template<class TY =I, typename...ARGS>
      TY&
      emplace (ARGS&& ...args)
        {
          __ensureSufficientCapacity();
          TY& newElm = elements_[level_].template create<TY>(std::forward<ARGS> (args)...);
          ++level_;
          return newElm;
        }
      
      
      
      /* === Element access and iteration === */
      
      I&
      operator[] (size_t index)  const
        {
          if (index < level_)
            return elements_[index].accessObj();
          
          throw error::Logic ("Attempt to access not (yet) existing object in ScopedCollection"
                             , LERR_(INDEX_BOUNDS));
        }
      
      
      
      using iterator       = IterAdapter<      I *, const ScopedCollection *>;
      using const_iterator = IterAdapter<const I *, const ScopedCollection *>;
      
      
      iterator       begin()       { return iterator       (this, _access_begin()); }
      const_iterator begin() const { return const_iterator (this, _access_begin()); }
      iterator       end ()        { return iterator();       }
      const_iterator end ()  const { return const_iterator(); }
      
      
      size_t  size ()        const { return level_;      }
      size_t  capacity()     const { return capacity_;   }
      bool    empty()        const { return 0 == level_; }
      
      
      // use in standard range for loops...
      friend iterator       begin (ScopedCollection& sco)      { return sco.begin(); }
      friend const iterator begin (ScopedCollection const& sco){ return sco.begin(); }
      friend iterator       end   (ScopedCollection& sco)      { return sco.end(); }
      friend const iterator end   (ScopedCollection const& sco){ return sco.end(); }
      
      
    private:
      /* ==== Storage: heap allocated array of element buffers ==== */
      
      using ElementStorage = std::unique_ptr<ElementHolder[]>;
      
      size_t level_;
      size_t capacity_;
      ElementStorage elements_;
      
      
      
      void
      __ensureSufficientCapacity()
        {
          if (level_ >= capacity_)
            throw error::State ("ScopedCollection exceeding the initially defined capacity"
                               , LERR_(CAPACITY));
        }
      
      
      /* ==== internal callback API for the iterator ==== */
      
      /** Iteration-logic: switch to next position
       * @note assuming here that the start address of the embedded object
       *       coincides with the start of an array element (ElementHolder)
       */
      friend void
      iterNext (const ScopedCollection*, I* & pos)
      {
        ElementHolder* & storageLocation = reinterpret_cast<ElementHolder* &> (pos);
        ++storageLocation;
      }
      
      friend void
      iterNext (const ScopedCollection*, const I* & pos)
      {
        const ElementHolder* & storageLocation = reinterpret_cast<const ElementHolder* &> (pos);
        ++storageLocation;
      }
      
      /** Iteration-logic: detect iteration end. */
      template<typename POS>
      friend bool
      checkPoint (const ScopedCollection* src, POS & pos)
      {
        REQUIRE (src);
        if ((pos) && (pos < src->_access_end()))
          return true;
        else
          {
            pos = 0;
            return false;
      }   }
      
      
      I* _access_begin() const { return & elements_[0].accessObj(); }
      I* _access_end()   const { return & elements_[level_].accessObj(); }
      
    };
  
  
  
  
  /* === Supplement: pre-defined element builders === */
  
  /** \par usage
   * On invocation, this function generates a builder-λ to pass as
   * the 2nd parameter to ScopedCollections's ctor. When arguments are given,
   * these are perfect-forwarded into a copy in the closure, which implies that
   * all instances will receive either a copy or const-reference to these values.
   * (It is not possible to pass references this way).
   * Using this variant of the ctor switches the collection to RAII-style:
   * It will immediately try to create all the embedded objects, invoking the
   * builder functor for each "slot" to hold such an embedded object. Actually,
   * this "slot" is an ElementHolder instance, which provides functions for
   * placement-creating objects into this embedded buffer.
   * @note allows also to default-create or copy-create
   */
  template<class I, size_t siz>
  template<typename...ARGS>
  auto
  ScopedCollection<I,siz>::fill (ARGS&&... args)
  {
    return [... args = std::forward<ARGS> (args)]
           (ScopedCollection<I,siz>::ElementHolder& storage)
            {
              storage.template create<I> (args...);
            };
  }
  
  template<class I, size_t siz>
  template<typename TY, typename...ARGS>
  auto
  ScopedCollection<I,siz>::fillWith (ARGS&&... args)
  {
    return [... args = std::forward<ARGS> (args)]
           (ScopedCollection<I,siz>::ElementHolder& storage)
            {
              storage.template create<TY> (args...);
            };
  }
  
  /**
   * @remark this builder handles the technical aspect of creating elements
   *         into the slots of the ElementHolder storage, so that client code
   *         has only to define the constructor argument, which notably can
   *         also be a subclass of \a I (assuming the storage size suffices)
   * @return builder-λ that can be passed into ScopeCollection's ctor.
   */
  template<class I, size_t siz>
  template<std::invocable<> FUN>
  auto
  ScopedCollection<I,siz>::invoke (FUN&& factory)
  {
    using Res   = std::invoke_result_t<FUN>;
    using Elem  = std::conditional_t<meta::is_Subclass_v<Res, I>, Res, I>;
    static_assert (std::is_constructible_v<Elem, Res>
                  ,"Iterator must produce something "
                   "that can be used to build contents of this container");
    
    return [fac = std::forward<FUN> (factory)]
           (ScopedCollection<I,siz>::ElementHolder& storage) mutable
            {
              storage.template create<Elem> (fac());
            };
  }
  
  
  
} // namespace lib
#endif
