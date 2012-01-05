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
 ** Managing a collection of noncopyable polymorphic objects in compact storage.
 ** This helper supports the frequently encountered situation where a service
 ** implementation internally manages a collection of implementation related
 ** sub-components with reference semantics. Typically, those objects are
 ** being used polymorphically, and often they are also added step by step.
 ** The storage holding all those child objects is allocated in one chunk
 ** and never adjusted.
 ** 
 ** - TODO: retro-fit with RefArray interface
 ** 
 ** @warning deliberately \em not threadsafe
 ** 
 ** @see ScopedCollection_test
 ** @see scoped-ptrvect.hpp quite similar, but using individual heap pointers
 */


#ifndef LIB_SCOPED_COLLECTION_H
#define LIB_SCOPED_COLLECTION_H


//#include "include/logging.h"
#include "lib/iter-adapter.hpp"
#include "lib/error.hpp"
//#include "lib/util.hpp"

//#include <vector>
//#include <algorithm>
#include <boost/noncopyable.hpp>
#include <boost/scoped_array.hpp>
#include <boost/static_assert.hpp>


namespace lib {
  
  namespace error = lumiera::error;
  using error::LUMIERA_ERROR_CAPACITY;
  using error::LUMIERA_ERROR_INDEX_BOUNDS;
  
  
  
  /**
   * A fixed collection of noncopyable polymorphic objects.
   * 
   * All child objects reside in a common chunk of storage
   * and are owned and managed by this collection holder.
   * Array style access and iteration.
   */
  template
    < class I                    ///< the nominal Base/Interface class for a family of types
    , size_t siz = sizeof(I)     ///< maximum storage required for the targets to be held inline
    >
  class ScopedCollection
    : boost::noncopyable
    {
      
    public:
      /** 
       * Storage Frame to hold one Child object.
       * The storage will be an heap allocated
       * array of such Wrapper objects.
       * @note doesn't manage the Child
       */
      class ElementHolder
        : boost::noncopyable
        {
      
          mutable char buf_[siz];
          
        public:
         
          I&
          accessObj()  const
            {
              return reinterpret_cast<I&> (buf_);
            }
          
          void
          destroy()
            {
              accessObj().~I();
            }
          
          
          
          
          /** Abbreviation for placement new */ 
#define EMBEDDED_ELEMENT_CTOR(_CTOR_CALL_)    \
              BOOST_STATIC_ASSERT (siz >= sizeof(TY));\
              return *new(&buf_) _CTOR_CALL_;   \
          
          
          template<class TY>
          TY&
          create ()
            {
              EMBEDDED_ELEMENT_CTOR ( TY() )
            }
          
          
          template<class TY, typename A1>
          TY&                                               //___________________________________________
          create (A1 a1)                                   ///< place object of type TY, using 1-arg ctor
            {
              EMBEDDED_ELEMENT_CTOR ( TY(a1) )
            }
          
          
          template< class TY
                  , typename A1
                  , typename A2
                  >
          TY&                                               //___________________________________________
          create (A1 a1, A2 a2)                            ///< place object of type TY, using 2-arg ctor
            {
              EMBEDDED_ELEMENT_CTOR ( TY(a1,a2) )
            }
          
          
          template< class TY
                  , typename A1
                  , typename A2
                  , typename A3
                  >
          TY&                                               //___________________________________________
          create (A1 a1, A2 a2, A3 a3)                     ///< place object of type TY, using 3-arg ctor
            {
              EMBEDDED_ELEMENT_CTOR ( TY(a1,a2,a3) )
            }
          
          
          template< class TY
                  , typename A1
                  , typename A2
                  , typename A3
                  , typename A4
                  >
          TY&                                               //___________________________________________
          create (A1 a1, A2 a2, A3 a3, A4 a4)              ///< place object of type TY, using 4-arg ctor
            {
              EMBEDDED_ELEMENT_CTOR ( TY(a1,a2,a3,a4) )
            }
          
          
          template< class TY
                  , typename A1
                  , typename A2
                  , typename A3
                  , typename A4
                  , typename A5
                  >
          TY&                                               //___________________________________________
          create (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)       ///< place object of type TY, using 5-arg ctor
            {
              EMBEDDED_ELEMENT_CTOR ( TY(a1,a2,a3,a4,a5) )
            }
#undef EMBEDDED_ELEMENT_CTOR
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
      
      template<class CTOR>
      ScopedCollection (size_t maxElements, CTOR builder)
        : level_(0)
        , capacity_(maxElements)
        , elements_(new ElementHolder[maxElements])
        {
        try { 
          while (level_ < capacity_)
            {
              ElementHolder& storageFrame (elements_[level_]);
              builder (storageFrame);
              ++level_;
            }}
        catch(...)
          {
            clear();
            throw;
        } }   
      
      
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
      operator[] (size_t index)
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
      /* ==== Storage: heap allocated array of element buffers ==== */
      
      typedef boost::scoped_array<ElementHolder> ElementStorage;
      
      size_t level_;
      size_t capacity_;
      ElementStorage elements_;
      
      
      
      void
      __ensureSufficientCapacity()
        {
          if (level_ >= capacity_)
            throw error::State ("ScopedCollection exceeding the initially defined capacity"
                               , LUMIERA_ERROR_CAPACITY);
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
  
  
  
  
} // namespace lib
#endif
