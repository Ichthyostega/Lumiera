/*
  LINKED-ELEMENTS.hpp  -  configurable intrusive single linked list template 

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

/** @file linked-elements.hpp
 ** Intrusive single linked list with optional ownership.
 ** This helper template allows to attach a number of tightly integrated
 ** elements with low overhead. Typically, these elements are to be attached
 ** once and never changed. Optionally, elements can be created in-place using
 ** a custom allocation scheme; the holder might also take ownership. These
 ** variations in functionality are controlled by policy templates.
 ** 
 ** The rationale for using this approach is
 ** - variable number of elements
 ** - explicit support for polymorphism
 ** - no need to template the holder on the number of elements
 ** - no heap allocations (contrast this to using std::vector)
 ** - clear and expressive notation at the usage site
 ** - the need to integrate tightly with a custom allocator
 ** 
 ** @note
 ** @warning
 ** 
 ** @see LinkedElements_test
 ** @see llist.h
 ** @see ScopedCollection
 ** @see itertools.hpp
 */


#ifndef LIB_LINKED_ELEMENTS_H
#define LIB_LINKED_ELEMENTS_H


#include "lib/error.hpp"
#include "lib/iter-adapter.hpp"

//#include <boost/noncopyable.hpp>
//#include <boost/static_assert.hpp>
//#include <boost/type_traits/is_same.hpp>
//#include <boost/type_traits/is_base_of.hpp>


namespace lib {
  
  namespace error = lumiera::error;
//  using error::LUMIERA_ERROR_CAPACITY;
//  using error::LUMIERA_ERROR_INDEX_BOUNDS;
  
  
  
  /**
   * TODO write type comment
   */
  template
    < class N                    ///< node class or Base/Interface class for nodes
    >
  class LinkedElements
    : boost::noncopyable
    {
      
    public:
      
      
      
      
     ~LinkedElements ()
        { 
          clear();
        }
      
      explicit
      LinkedElements (size_t maxElements)
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
      LinkedElements (size_t maxElements, CTOR builder)
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
      LinkedElements (size_t maxElements, void (TY::*builder) (ElementHolder&), TY * const instance)
        : level_(0)
        , capacity_(maxElements)
        , elements_(new ElementHolder[maxElements])
        {
          populate_by (builder,instance);
        }
      
      /* == some pre-defined Builders == */
      
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
      I& appendNewElement() { return appendNew<I>(); }
      
      
      template< class TY >
      TY&                                                  //_________________________________________
      appendNew ()                                        ///< add object of type TY, using 0-arg ctor
        {
          __ensureSufficientCapacity();
          TY& newElm = elements_[level_].template create<TY>();
          ++level_;
          return newElm;
        }
      
      
      template< class TY
              , typename A1
              >
      TY&                                                  //_________________________________________
      appendNew (A1 a1)                                   ///< add object of type TY, using 1-arg ctor
        {
          __ensureSufficientCapacity();
          TY& newElm = elements_[level_].template create<TY>(a1);
          ++level_;
          return newElm;
        }
      
      
      template< class TY
              , typename A1
              , typename A2
              >
      TY&                                                  //_________________________________________
      appendNew (A1 a1, A2 a2)                            ///< add object of type TY, using 2-arg ctor
        {
          __ensureSufficientCapacity();
          TY& newElm = elements_[level_].template create<TY>(a1,a2);
          ++level_;
          return newElm;
        }
      
      
      template< class TY
              , typename A1
              , typename A2
              , typename A3
              >
      TY&                                                  //_________________________________________
      appendNew (A1 a1, A2 a2, A3 a3)                     ///< add object of type TY, using 3-arg ctor
        {
          __ensureSufficientCapacity();
          TY& newElm = elements_[level_].template create<TY>(a1,a2,a3);
          ++level_;
          return newElm;
        }
      
      
      template< class TY
              , typename A1
              , typename A2
              , typename A3
              , typename A4
              >
      TY&                                                  //_________________________________________
      appendNew (A1 a1, A2 a2, A3 a3, A4 a4)              ///< add object of type TY, using 4-arg ctor
        {
          __ensureSufficientCapacity();
          TY& newElm = elements_[level_].template create<TY>(a1,a2,a3,a4);
          ++level_;
          return newElm;
        }
      
      
      template< class TY
              , typename A1
              , typename A2
              , typename A3
              , typename A4
              , typename A5
              >
      TY&                                                  //_________________________________________
      appendNew (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)       ///< add object of type TY, using 5-arg ctor
        {
          __ensureSufficientCapacity();
          TY& newElm = elements_[level_].template create<TY>(a1,a2,a3,a4,a5);
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
                             , LUMIERA_ERROR_INDEX_BOUNDS);
        }
      
      
      
      typedef IterAdapter<      I *, const ScopedCollection *> iterator;
      typedef IterAdapter<const I *, const ScopedCollection *> const_iterator;
      
      
      iterator       begin()       { return iterator       (this, _access_begin()); }
      const_iterator begin() const { return const_iterator (this, _access_begin()); }
      iterator       end ()        { return iterator();       }
      const_iterator end ()  const { return const_iterator(); }
      
      
      size_t  size ()        const { return level_;      }
      size_t  capacity ()    const { return capacity_;   }
      bool    empty ()       const { return 0 == level_; }
      
      
      
    private:
      
      
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
      hasNext (const ScopedCollection* src, POS & pos)
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
  
  
  
  
  /* === Supplement: pre-defined  === */
  
  /** \par usage
   */
  
  
  
} // namespace lib
#endif
