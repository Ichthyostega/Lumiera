/*
  ITER-SOURCE.hpp  -  an interface to build an opaque iterator-based data source 

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file iter-source.hpp
 ** Extension module to build an opaque data source, accessible as
 ** <i>Lumiera Forward Iterator</i>. It is based on combining an IterAdapter
 ** with classical polymorphism; here, the data source, which is addressed
 ** by IderAdapter through the "iteration control API", is abstracted behind
 ** an interface (with virtual functions). Together this allows to build
 ** a simple data source type, without needing to disclose details of
 ** the implementation.
 ** 
 ** \par Standard Adapters
 ** As a complement, this header contains a generic implementation
 ** of the IterSource interface by wrapping an existing Lumiera Forward Iterator.
 ** Using this WrappedLumieraIter, the details of this wrapped source iterator
 ** remain opaque. To ease the use of this adapter, a selection of free functions
 ** is provided, allowing to build opaque "all elements" or "all keys" iterators
 ** for various STL containers. 
 ** 
 ** @see iter-adapter.hpp
 ** @see itertool.hpp
 ** @see iter-source-test.cpp
 ** 
 */


#ifndef LIB_ITER_SOURCE_H
#define LIB_ITER_SOURCE_H


#include "lib/meta/util.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/itertools.hpp"

#include <boost/type_traits/remove_const.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <memory>




namespace lib {
  
  using std::string;
  using std::shared_ptr;
  
  
  
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
      
    protected: /* == data source API to implement == */
      
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
      virtual ~IterSource() { };  ///< is ABC
      
      virtual
      operator string()  const    ///< subclasses may offer diagnostics
        {
          return "IterSource<" + lib::meta::typeStr<TY>()+ ">";
        }
      
      
      /* == Iteration control API for IterAdapter frontend == */
      
      friend bool
      checkPoint (DataHandle const&, Pos const& pos)
        {
          return bool(pos);
        }
      
      friend void
      iterNext (DataHandle& source, Pos& pos)
        {
          ASSERT (source);
          source->nextResult(pos);
        }
      
      
      /* == public builder API to create instances == */
      
      struct iterator
        : IterAdapter<Pos, DataHandle>
        {
          using _I = IterAdapter<Pos, DataHandle>
          ;
          using _I::IterAdapter;
          operator string()  const {return _I::source()? string(*_I::source()) : "⟂"; }
        };
      
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
   * a wrapped "Lumiera Forward Iterator". Usually, such a wrapper instance
   * is passed to one of the IterSource's builder functions, thereby
   * erasing the specific type information of the template parameter IT
   */
  template<class IT>
  class WrappedLumieraIter
    : public IterSource<typename IT::value_type>
    , boost::noncopyable
    {
      using _Base = IterSource<typename IT::value_type>;
      using   Pos = typename _Base::Pos;
      
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
      WrappedLumieraIter (IT const& orig)
        : src_(orig)
        { }
    };
  
  
  
  
  /* === pre-defined Adapters for frequently used Containers === */
  
  namespace iter_source {
    namespace {  // traits and helpers...
    
      template<class CON>
      struct _SeqT
        {
          typedef typename CON::iterator::value_type Val;
          typedef typename IterSource<Val>::iterator Iter;
        };
      
      template<class IT>
      struct _RangeT
        {
          typedef typename IT::value_type Val;
          typedef typename IterSource<Val>::iterator Iter;
        };

      template<class MAP>
      struct _MapT
        {
          typedef typename MAP::key_type Key;
          typedef typename MAP::value_type::second_type Val;
          typedef typename IterSource<Key>::iterator KeyIter;
          typedef typename IterSource<Val>::iterator ValIter;
        };
      
      
      template<class IT>
      struct _IterT
        {
          typedef typename IT::value_type Val;
          typedef typename IterSource<Val>::iterator Iter;
        };
      
      template<class IT, class FUN>
      struct _TransformIterT
        {
          typedef typename lib::meta::_Fun<FUN>::Ret  ResVal;
          typedef TransformIter<IT,ResVal>         TransIter;
          typedef typename IterSource<ResVal>::iterator Iter;
        };
      
      template<class IT>
      struct _PairIterT
        {
          typedef typename IT::value_type PairType;
          typedef typename PairType::second_type ValType;
          typedef typename PairType::first_type ConstKeyType;
          
          // since we're returning the keys always by value,
          // we can strip the const added by the STL map types
          typedef typename boost::remove_const<ConstKeyType>::type KeyType;
          
          typedef TransformIter<IT,KeyType> KeyIter;
          typedef TransformIter<IT,ValType> ValIter;
          
          static KeyType  takeFirst (PairType const& pair) { return pair.first; }
          static ValType  takeSecond(PairType const& pair) { return pair.second;}
        };
      
      
      template<class IT>
      typename _PairIterT<IT>::KeyIter
      takePairFirst (IT const& source)
      {
        return transformIterator(source, _PairIterT<IT>::takeFirst );
      }
      
      template<class IT>
      typename _PairIterT<IT>::ValIter
      takePairSecond (IT const& source)
      {
        return transformIterator(source, _PairIterT<IT>::takeSecond );
      }
      
    } //(END) type helpers...
    
    
    
    /** wraps a given Lumiera Forward Iterator,
     *  exposing just a IterSource based frontend.
     */
    template<class IT>
    typename _IterT<IT>::Iter
    wrapIter (IT const& source)
    {
      typedef typename _IterT<IT>::Val ValType;
      
      return IterSource<ValType>::build (new WrappedLumieraIter<IT> (source));
    }
    
    
    /** pipes a given Lumiera Forward Iterator through
     *  a transformation function and wraps the resulting
     *  transforming Iterator, exposing just an IterSource.
     *  This convenience shortcut can be used to build a
     *  processing chain; the resulting IterSource will
     *  hide any detail types involved.
     * @note as with any IterSource, there is one virtual
     *       function call for every fetched element.
     */
    template<class IT, class FUN>
    typename _TransformIterT<IT,FUN>::Iter
    transform (IT const& source, FUN processingFunc)
    {
      typedef typename _TransformIterT<IT,FUN>::ResVal    ValType;
      typedef typename _TransformIterT<IT,FUN>::TransIter TransIT;
      
      return IterSource<ValType>::build (
          new WrappedLumieraIter<TransIT> (
              transformIterator (source, processingFunc)));
    }
    
    
    /** @return a Lumiera Forward Iterator to yield
     *          all the keys of the given Map or Hashtable
     */
    template<class MAP>
    typename _MapT<MAP>::KeyIter
    eachMapKey (MAP& map)
    {
      typedef RangeIter<typename MAP::iterator> Range;
      
      Range contents (map.begin(), map.end());
      return wrapIter (takePairFirst (contents));
    }
    
    
    /** @return a Lumiera Forward Iterator to yield
     *          all the values of the given Map or Hashtable
     */
    template<class MAP>
    typename _MapT<MAP>::ValIter
    eachMapVal (MAP& map)
    {
      typedef RangeIter<typename MAP::iterator> Range;
      
      Range contents (map.begin(), map.end());
      return wrapIter (takePairSecond(contents));
    }
    
    
    /** @return a Lumiera Forward Iterator to yield
     *          all \em distinct keys of a Multimap
     *  @warning we do a full table scan to find
     *          the distinct keys
     */
    template<class MAP>
    typename _MapT<MAP>::KeyIter
    eachDistinctKey (MAP& map)
    {
      typedef RangeIter<typename MAP::iterator> Range;
      
      Range contents (map.begin(), map.end());
      return wrapIter (filterRepetitions (takePairFirst(contents)));
    }
    
    
    /** @return a Lumiera Forward Iterator to yield all values
     *          associated to the given key within this Map or Multimap
     *  @note obviously in case of a Map we'll get at most one result.
     */
    template<class MAP>
    typename _MapT<MAP>::ValIter
    eachValForKey (MAP& map, typename _MapT<MAP>::Key key)
    {
      typedef typename MAP::iterator Pos;
      typedef RangeIter<Pos> Range;
      
      std::pair<Pos,Pos> valuesForKey = map.equal_range(key);
      Range contents (valuesForKey.first, valuesForKey.second);
      return wrapIter (takePairSecond(contents));
    }
    
    
    /** @param container a STL-like container, providing
     *         - a typedef \c iterator
     *         - functions \c begin() and \c end()
     *  @return a Lumiera Forward Iterator yielding all values
     *          starting with \c begin and excluding \c end .
     */
    template<class CON>
    typename _SeqT<CON>::Iter
    eachEntry (CON& container)
    {
      typedef typename _SeqT<CON>::Val ValType;
      typedef RangeIter<typename CON::iterator> Range;
      
      Range contents (container.begin(), container.end());
      return IterSource<ValType>::build (new WrappedLumieraIter<Range>(contents)); 
    }
    

    /** @return a Lumiera Forward Iterator yielding all values
     *          defined by a classical Iterator range.
     */
    template<class IT>
    typename _RangeT<IT>::Iter
    eachEntry (IT const& begin, IT const& end)
    {
      typedef typename _RangeT<IT>::Val ValType;
      typedef RangeIter<IT> Range;

      Range contents (begin, end);
      return IterSource<ValType>::build (new WrappedLumieraIter<Range>(contents));
    }

  }
  using iter_source::wrapIter;
  using iter_source::transform;
  using iter_source::eachMapKey;
  using iter_source::eachDistinctKey;
  using iter_source::eachValForKey;
  using iter_source::eachMapVal;
  using iter_source::eachEntry;
  
  
} // namespace lib
#endif
