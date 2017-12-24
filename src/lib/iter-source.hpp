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
 ** @todo the design used for the "iteration control API" is misaligned
 **       with the purpose of this adapter. Rather, it should be shaped
 **       similar to IterStateWrapper with three control functions     //////////////////////////////////////TICKET #1125
 ** 
 ** ## Standard Adapters
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

#include <boost/noncopyable.hpp>
#include <type_traits>
#include <utility>
#include <string>
#include <memory>




namespace lib {
  
  using std::string;
  using std::shared_ptr;
  using std::forward;
  
  
  
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
      
                                                              ///////////////////////////////////////////////TICKET #1125 : this API should use three control functions, similar to IterStateWrapper
      /** iteration start: prepare the first element.
       *  may return NULL in case of empty data source
       */
      virtual Pos firstResult ()         =0;
      
      /** iteration step: switch on to the next element.
       *  The pos pointer should be set to NULL to report
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
      
      friend bool                                                    ////////////////////////////////////////TICKET #1125 : this API should use three control functions, similar to IterStateWrapper
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
        {                                                ////////////////////////////////////////////////////TICKET #1125 : should be build on top of IterStateWrapper rather than IterAdapter!
          using _I = IterAdapter<Pos, DataHandle>;
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
          return std::move(
              startIteration (DataHandle{&sourceImpl, &detach_without_destroy}));
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
          return std::move(
              startIteration (DataHandle{sourceImplObject, &destroy_managed_source}));
        }
      
      static iterator EMPTY_SOURCE;
      
      using value_type = TY;
      using reference  = TY&;
      using pointer    = TY*;
      
      
      
    private:
      static iterator
      startIteration (DataHandle sourceHandle)
        {
          REQUIRE (sourceHandle);
          Pos first = sourceHandle->firstResult();
          return {move(sourceHandle), first};
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
  template<class IT,  class ISO = IterSource<typename IT::value_type>>
  class WrappedLumieraIter
    : public ISO
    , boost::noncopyable
    {
      using Pos = typename ISO::Pos;
      
      IT src_;
      
      Pos                                                ////////////////////////////////////////////////////TICKET #1125 : this API should use three control functions, similar to IterStateWrapper
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
      
      WrappedLumieraIter (IT&& orig)
        : src_(forward<IT>(orig))
        { }
      
      
    protected:
      IT&       wrappedIter()       { return src_; }
      IT const& wrappedIter() const { return src_; }
    };
  
  
  
  
  /* === pre-defined Adapters for frequently used Containers === */
  
  namespace iter_source {
    namespace {  // traits and helpers...
    
      template<class CON>
      struct _SeqT
        {
          using Val  = typename CON::iterator::value_type;
          using Iter = typename IterSource<Val>::iterator;
        };
      
      template<class IT>
      struct _RangeT
        {
          using Val  = typename IT::value_type;
          using Iter = typename IterSource<Val>::iterator;
        };

      template<class MAP>
      struct _MapT
        {
          using Key     = typename MAP::key_type;
          using Val     = typename MAP::value_type::second_type;
          using KeyIter = typename IterSource<Key>::iterator;
          using ValIter = typename IterSource<Val>::iterator;
        };
      
      
      template<class IT>
      struct _IterT
        {
          using Src  = typename std::remove_reference<IT>::type;
          using Val  = typename Src::value_type;
          using Iter = typename IterSource<Val>::iterator;
        };
      
      template<class IT, class FUN>
      struct _TransformIterT
        {
          using       Src = typename std::remove_reference<IT>::type;
          using    ResVal = typename lib::meta::_Fun<FUN>::Ret;
          using TransIter = TransformIter<Src, ResVal>;
          using      Iter = typename IterSource<ResVal>::iterator;
        };
      
      template<class IT>
      struct _PairIterT
        {
          using          Src = typename std::remove_reference<IT>::type;
          using     PairType = typename Src::value_type;
          using      ValType = typename PairType::second_type;
          using ConstKeyType = typename PairType::first_type;
          
          // since we're returning the keys always by value,
          // we can strip the const added by the STL map types
          using KeyType = typename std::remove_const<ConstKeyType>::type;
          
          typedef TransformIter<Src, KeyType> KeyIter;
          typedef TransformIter<Src, ValType> ValIter;
          
          static KeyType  takeFirst (PairType const& pair) { return pair.first; }
          static ValType  takeSecond(PairType const& pair) { return pair.second;}
        };
      
      
      template<class IT>
      typename _PairIterT<IT>::KeyIter
      takePairFirst (IT&& source)
      {
        return transformIterator(forward<IT>(source), _PairIterT<IT>::takeFirst );
      }
      
      template<class IT>
      typename _PairIterT<IT>::ValIter
      takePairSecond (IT&& source)
      {
        return transformIterator(forward<IT>(source), _PairIterT<IT>::takeSecond );
      }
      
    } //(END) type helpers...
    
    
    
    /** wraps a given Lumiera Forward Iterator,
     *  exposing just a IterSource based frontend.
     */
    template<class IT>
    typename _IterT<IT>::Iter
    wrapIter (IT&& source)
    {
      using Src = typename _IterT<IT>::Src;
      using Val = typename _IterT<IT>::Val;
      
      return IterSource<Val>::build (new WrappedLumieraIter<Src> (forward<IT>(source)));
    }
    
    
    /** an IterSource frontend to return just a single value once.
     * @warning behind the scenes, a heap allocation is managed by shared_ptr,
     *          to maintain a copy of the wrapped element. When passing a reference,
     *          only a reference will be wrapped, but a heap allocation happens nonetheless
     */
    template<typename VAL>
    auto
    singleVal (VAL&& something)
    {
      using Src = decltype(singleValIterator (forward<VAL>(something)));
      using Val = typename _IterT<Src>::Val;

      return IterSource<Val>::build (new WrappedLumieraIter<Src>{singleValIterator (forward<VAL>(something))});
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
    transform (IT&& source, FUN processingFunc)
    {
      typedef typename _TransformIterT<IT,FUN>::ResVal    ValType;
      typedef typename _TransformIterT<IT,FUN>::TransIter TransIT;
      
      return IterSource<ValType>::build (
          new WrappedLumieraIter<TransIT> (
              transformIterator (forward<IT>(source), processingFunc)));
    }
    
    
    /** @return a Lumiera Forward Iterator to yield
     *          all the keys of the given Map or Hashtable
     */
    template<class MAP>
    typename _MapT<MAP>::KeyIter
    eachMapKey (MAP& map)
    {
      using Range = RangeIter<typename MAP::iterator>;
      
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
      using Range = RangeIter<typename MAP::iterator>;
      
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
      using Range = RangeIter<typename MAP::iterator>;
      
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
      using Pos = typename MAP::iterator;
      using Range = RangeIter<Pos>;
      
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
      using ValType = typename _SeqT<CON>::Val;
      using Range   = RangeIter<typename CON::iterator>;
      
      Range contents (container.begin(), container.end());
      return IterSource<ValType>::build (new WrappedLumieraIter<Range>(contents));
    }
    
    
    /** @return a Lumiera Forward Iterator to yield all values
     *          defined by a classical Iterator range.
     */
    template<class IT>
    typename _RangeT<IT>::Iter
    eachEntry (IT const& begin, IT const& end)
    {
      using ValType = typename _RangeT<IT>::Val;
      using Range   = RangeIter<IT>;

      Range contents (begin, end);
      return IterSource<ValType>::build (new WrappedLumieraIter<Range>(contents));
    }
  
    
}} // namespace lib::iter_source
#endif
