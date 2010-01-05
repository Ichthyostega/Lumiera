/*
  ITER-SOURCE.hpp  -  an interface to build an opaque iterator-based data source 
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file iter-source.hpp
 ** Extension module to build an opaque data source, accessible as
 ** <i>Lumiera Forward Iterator</i>. It is based on combining an IterAdapter
 ** with classical polymorphism; here, the data source, which is addressed
 ** by IderAdapter through the "iteration control API", is abstracted behind
 ** an interface (with virtual functions). Together this allows to build
 ** a simple data source type, without needing to disclose details of
 ** the implementation.
 ** 
 ** @see iter-adapter.hpp
 ** @see itertool.hpp
 ** @see iter-source-test.cpp
 ** 
 */


#ifndef LIB_ITER_SOURCE_H
#define LIB_ITER_SOURCE_H


#include "lib/iter-adapter.hpp"
//#include "lib/bool-checkable.hpp"

#include <boost/noncopyable.hpp>
#include <tr1/memory>




namespace lib {

  using std::tr1::shared_ptr;
  
  
  
  /**
   * Iteration source interface to abstract a data source,
   * which then can be accessed through IterAdapter as a frontend,
   * allowing to pull individual elements until exhaustion.
   * 
   * This base class is empty and makes no assumptions regarding
   * identity, instantiation and copying.
   * 
   * @see PlacementIndex::Table#_eachEntry_4check usage example
   * @see iter-source-test.cpp
   */
  template<typename TY>
  class IterSource
    {
      
    protected: /* == data source API for implementation == */
      
      typedef TY* Pos;
      typedef shared_ptr<IterSource> DataHandle;
      
      
      /** iteration start: prepare the first element.
       *  may return NULL in case of empty data source
       */
      virtual Pos firstResult ()         =0;
      
      /** iteration step: switch on to the next element.
       *  The pos pointer may be set to NULL to report
       *  iteration end
       */
      virtual void nextResult(Pos& pos)  =0;
      
      /** disconnect the data source / iteration frontend.
       *  Depending on the way the IterSource got created
       *  this \em might (or might not) be followed by
       *  destroying the data source.
       *  @note must not throw
       */
      virtual void disconnect () { }
      
      
    public:
      
      virtual ~IterSource() { }; ///< is ABC
      
      /* == Iteration control API for IterAdapter frontend == */
      
      friend bool
      hasNext  (DataHandle const&, Pos const& pos)          ////TICKET #410
        {
          return bool(pos);
        }
      
      friend void
      iterNext (DataHandle& source, Pos& pos)
        {
          source->nextResult(pos);
        }
      
      
      /* == public builder API to create instances == */
      
      typedef IterAdapter<Pos, DataHandle> iterator;
      
      /** build an iterator frontend for the given source,
       *  @note the source is allocated separately and
       *        \em not owned by the iterator frontend
       */
      static iterator
      build (IterSource& sourceImpl)
        {
          DataHandle sourceHandle (&sourceImpl, &detach_without_destroy);
          return startIteration(sourceHandle);
        }
      
      /** build an iterator frontend, thereby managing
       *  the given heap allocated source object instance.
       *  @note we take ownership and destroy the source
       *        when the last copy of the created iterator
       *        goes out of scope.
       */
      static iterator
      build (IterSource* sourceImplObject)
        {
          DataHandle sourceHandle (sourceImplObject, &destroy_managed_source);
          return startIteration(sourceHandle);
        }
      
      static iterator EMPTY_SOURCE;
      
      
      
    private:
      static iterator
      startIteration (DataHandle sourceHandle)
        {
          REQUIRE (sourceHandle);
          Pos first = sourceHandle->firstResult();
          return iterator (sourceHandle, first);
        }
      
      
      static void
      detach_without_destroy (IterSource * source)
        {
          WARN_IF (!source, library, "IterSource deleter called with NULL source pointer");
          if (source)
            source->disconnect();
        }
      
      static void
      destroy_managed_source (IterSource * source)
        {
          WARN_IF (!source, library, "IterSource deleter called with NULL source pointer");
          if (source)
            {
              source->disconnect();
              delete source;
            }
        }
    };
  
  
  /** storage for the empty data-source constant */
  template<typename TY>
  typename IterSource<TY>::iterator IterSource<TY>::EMPTY_SOURCE = iterator();
  
  
  
  
  /** 
   * Standard implementation of the IterSource interface:
   * a wrapped "Lumiera Forward Iterator"
   */
  template<class IT>
  class WrappedLumieraIterator
    : public IterSource<typename IT::value_type>
    , boost::noncopyable
    {
      typedef IterSource<typename IT::value_type> _Base;
      typedef typename _Base::Pos Pos;
      
      IT src_;
      
    Pos
    firstResult ()
      {
        if (!src_)
          return 0;
        else
          return & *src_;
      }
    
    void
    nextResult (Pos& pos)
      {
        if (!pos) return;
        if (src_) ++src_;
        if (src_)
          pos = & *src_;
        else
          pos = 0;
      }
    
      
      
    public:
      WrappedLumieraIterator (IT const& orig)
        : src_(orig)
        { }
    };
  
  
  
  
  /* === pre-defined Adapters for frequently used Containers === */
  
  namespace iter_impl {
    
    template<class CON>
    struct _SeqType
      {
        typedef typename CON::iterator::value_type Val;
        typedef typename IterSource<Val>::iterator Iter;
      };
    
    template<class MAP>
    struct _MapType
      {
        typedef typename MAP::key_type Key;
        typedef typename IterSource<Key>::iterator Iter;
      };
  }
  
  template<class MAP>
  typename iter_impl::_MapType<MAP>::Iter
  eachMapKey (MAP& map)
    {
      UNIMPLEMENTED ("standard iter wrapper yielding all map keys");
    }
  
  
  template<class MAP>
  typename iter_impl::_MapType<MAP>::Iter
  eachDistinctKey (MAP& map)
    {
      UNIMPLEMENTED ("standard iter wrapper yielding all distinct keys of a multimap");
    }
  
  
  template<class CON>
  typename iter_impl::_SeqType<CON>::Iter
  eachEntry (CON& container)
    {
      typedef typename iter_impl::_SeqType<CON>::Val ValType;
      typedef RangeIter<typename CON::iterator> Range;
      
      Range contents (container.begin(), container.end());
      return IterSource<ValType>::build (new WrappedLumieraIterator<Range>(contents)); 
    }
  
  
} // namespace lib
#endif
