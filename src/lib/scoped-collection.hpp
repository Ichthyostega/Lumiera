/*
  SCOPED-COLLECTION.hpp  -  managing a fixed collection of noncopyable polymorphic objects 

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file scoped-collection.hpp
 ** Managing a collection of non-copyable polymorphic objects in compact storage.
 ** This helper supports the frequently encountered situation where a service
 ** implementation internally manages a collection of implementation related
 ** sub-components with reference semantics. Typically, those objects are
 ** being used polymorphically, and often they are also added step by step.
 ** The storage holding all those child objects is allocated in one chunk
 ** and never adjusted.
 ** 
 ** # usage patterns
 ** 
 ** The common ground for all usage of this container is to hold some elements
 ** with exclusive ownership; when the enclosing container goes out of scope,
 ** all the dtors of the embedded objects will be invoked. Frequently this
 ** side effect is the reason for using the container: we want to own some
 ** resource handles to be available exactly as long as the managing object
 ** needs and accesses them.
 ** 
 ** There are two different usage patterns for populating a ScopedCollection
 ** - the "stack style" usage creates an empty container (using the one arg
 **   ctor just to specify the maximum size). The storage to hold up to this
 **   number of objects is (heap) allocated right away, but no objects are
 **   created. Later on, individual objects are "pushed" into the collection
 **   by invoking #emplaceElement() to create a new element of the default
 **   type `I`) or #emplace<Type>(args) to create some subtype. This way,
 **   the container is being filled successively.
 ** - the "RAII style" usage strives to create all of the content objects
 **   right away, immediately after the memory allocation. This usage pattern
 **   avoids any kind of "lifecycle state". Either the container comes up sane
 **   and fully populated, or the ctor call fails and any already created
 **   objects are discarded.
 ** @note intentionally there is no operation to discard individual objects,
 **       all you can do is to #clear() the whole container.
 ** @note the container can hold instances of a subclass of the type defined
 **       by the template parameter `I`. But you need to ensure in this case
 **       that the defined buffer size for each element (2nt template parameter)
 **       is sufficient to hold any of these subclass instances. This condition
 **       is protected by a static assertion (compilation failure). 
 ** @warning when using subclasses, a virtual dtor is mandatory
 ** @warning deliberately \em not threadsafe
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
#include <type_traits>


namespace lib {
  
  namespace error = lumiera::error;
  using LERR_(INDEX_BOUNDS);
  using LERR_(CAPACITY);
  
  
  
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
       * The storage will be an heap allocated
       * array of such Wrapper objects.
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
              static_assert ( meta::is_Subclass<TY,I>()
                             && sizeof(TY) <= siz,
                             "ElementHolder buffer too small");
              
              return *new(&buf_) TY (std::forward<ARGS> (args)...);
            }
        };
      
      
      
      
      
      
     ~ScopedCollection ()
        { 
          clear();
        }
      
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
       *        use this to create an object of some I-subclass
       */
      template<class CTOR>
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
      
      /* == some pre-defined Builders == */
      
      class FillAll;           ///< fills the ScopedCollection with default constructed I-instances
      
      template<typename TY>
      class FillWith;          ///< fills the ScopedCollection with default constructed TY-instances
      
      template<typename IT>
      class PullFrom;          ///< fills by copy-constructing values pulled from the iterator IT
      
      template<typename IT>
      static PullFrom<IT>
      pull (IT iter)           ///< convenience shortcut to pull from any given Lumiera Forward Iterator
        {
          return PullFrom<IT> (iter);
        }
      
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
      template<class CTOR>
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
                            "All elements will be discarded");
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
      size_t  capacity ()    const { return capacity_;   }
      bool    empty ()       const { return 0 == level_; }
      
      
      // use in standard range for loops...
      friend iterator       begin (ScopedCollection& sco)      { return sco.begin(); }
      friend const iterator begin (ScopedCollection const& sco){ return sco.begin(); }
      friend iterator       end   (ScopedCollection& sco)      { return sco.end(); }
      friend const iterator end   (ScopedCollection const& sco){ return sco.end(); }
      
      
    private:
      /* ==== Storage: heap allocated array of element buffers ==== */
      
      typedef std::unique_ptr<ElementHolder[]> ElementStorage;
      
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
   * Pass an instance of this builder functor as 2nd parameter
   * to ScopedCollections's ctor. (an anonymous instance is OK).
   * Using this variant of the ctor switches the collection to RAII-style:
   * It will immediately try to create all the embedded objects, invoking this
   * builder functor for each "slot" to hold such an embedded object. Actually,
   * this "slot" is an ElementHolder instance, which provides functions for
   * placement-creating objects into this embedded buffer.
   */
  template<class I, size_t siz>
  class ScopedCollection<I,siz>::FillAll
    {
    public:
      void
      operator() (typename ScopedCollection<I,siz>::ElementHolder& storage)
        {
          storage.template create<I>();
        }
    };
  
  template<class I, size_t siz>
  template<typename TY>
  class ScopedCollection<I,siz>::FillWith
    {
    public:
      void
      operator() (typename ScopedCollection<I,siz>::ElementHolder& storage)
        {
          storage.template create<TY>();
        }
    };
  
  /** \par usage
   * This variant allows to "pull" elements from an iterator.
   * Actually, the collection will try to create each element right away,
   * by invoking the copy ctor and passing the value yielded by the iterator.
   * @note anything in accordance to the Lumiera Forward Iterator pattern is OK.
   *       This rules out just passing a plain STL iterator (because these can't
   *       tell for themselves when they're exhausted). Use an suitable iter-adapter
   *       instead, e.g. by invoking lib::iter_stl::eachElm(stl_container) 
   */
  template<class I, size_t siz>
  template<typename IT>
  class ScopedCollection<I,siz>::PullFrom
    {
      IT iter_;
      
      using ElementType = typename meta::ValueTypeBinding<IT>::value_type;
      
    public:
      PullFrom (IT source)
        : iter_(source)
        { }
      
      void
      operator() (typename ScopedCollection<I,siz>::ElementHolder& storage)
        {
          storage.template create<ElementType> (*iter_);
          ++iter_;
        }
    };
  
  
  
} // namespace lib
#endif
