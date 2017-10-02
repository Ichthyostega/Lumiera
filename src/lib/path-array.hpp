/*
  PATH-ARRAY.hpp  -  sequence of path-like component-IDs in fixed storage

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file path-array.hpp
 ** Foundation abstraction to implement path-like component sequences.
 ** This library class can be used to build a path abstraction for data structure access
 ** or some similar topological coordinate system, like e.g. [UI coordinates](\ref ui-coord.hpp)
 ** A PathArray is an iterable sequence of literal component IDs, implemented as tuple of lib::Literal
 ** held in fixed inline storage with possible heap allocated (and thus unlimited) extension storage.
 ** It offers range checks, standard iteration and array-like indexed component access; as a whole
 ** it is copyable, while actual components are immutable after construction. PathArray can be
 ** bulk initialised from an explicit sequence of literals; it is normalised on construction
 ** to trim and fill interspersed missing elements. Iteration and equality comparison are
 ** built on top of the normalisation; iteration starts with the first nonempty element.
 ** 
 ** @remark the choice of implementation layout is arbitrary and not based on evidence.
 **         A delegate structure with fixed inline storage looked like an interesting programming challenge.
 **         Using just a heap based array storage would have been viable likewise.
 ** @todo when UICoord is in widespread use, collect performance statistics and revisit this design.
 ** 
 ** @see PathArray_test
 ** @see UICoord_test
 ** @see gui::interact::UICoord
 ** @see view-spec-dsl.hpp
 */


#ifndef LIB_PATH_ARRAY_H
#define LIB_PATH_ARRAY_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/meta/variadic-helper.hpp"
#include "lib/format-obj.hpp"
#include "lib/util.hpp"

#include <algorithm>
#include <utility>
#include <string>
#include <memory>
#include <array>


namespace lib {
  namespace error = lumiera::error;
  
  using std::string;
  using std::forward;
  using lib::Literal;
  using util::unConst;
  using util::isnil;
  
  namespace con { // Implementation helper: flexible heap based extension storage....
    
    /**
     * Heap-allocated extension storage for an immutable sequence of literal strings.
     * The size of the allocation is determined and fixed once, at construction time,
     * derived from the number of initialisers. The first slot within the allocation
     * stores this length. Extension can be _empty_ (default constructed),
     * in which case no heap allocation is performed.
     */
    class Extension
      {
        using PStorage = Literal*;
        
        PStorage storage_;
        
        
        static size_t&
        size (PStorage& p)
          {
            REQUIRE (p);
            return reinterpret_cast<size_t&> (p[0]);
          }
        
        /** allocate a copy.
         * @note caller has to manage the allocated memory
         * @warning call to Literal's ctor deliberately elided
         */
        PStorage
        newCopy()  const
          {
            size_t siz = 1 + size (unConst(this)->storage_);
            const char** alloc = new const char*[siz];
            std::copy (storage_, storage_+siz, alloc);
            return reinterpret_cast<PStorage> (alloc);
          }
        
        
      public:
       ~Extension()
          {
            if (storage_)
              delete[] storage_;
          }
        
        Extension()
          : storage_{nullptr}
          { }
        
        template<typename...ELMS>
        explicit
        Extension (ELMS&& ...elms)
          : storage_{new Literal[1 + sizeof...(ELMS)]}
          {
            size(storage_) = sizeof...(ELMS);
            new(storage_+1) Literal[sizeof...(ELMS)] {forward<ELMS>(elms)...};
          }
        
        Extension (Extension const& r)
          : storage_{r.storage_? r.newCopy() : nullptr}
          { }
        
        Extension (Extension&& rr)
          : storage_{nullptr}
          {
            if (rr.storage_)
              std::swap (storage_, rr.storage_);
          }
        
        Extension& operator= (Extension const& o)
          {
            if (this != &o)
              {
                std::unique_ptr<Literal[]> cp;
                if (o.storage_)
                  cp.reset (o.newCopy());
                if (storage_)
                  delete[] storage_;
                storage_ = cp.release();
              }
            return *this;
          }
        
        Extension& operator= (Extension&& rr)
          {
            if (this != &rr)
              {
                std::swap (storage_, rr.storage_);
              }
            return *this;
          }
        
        
        
        operator bool() const { return not empty(); }
        bool empty()    const { return not storage_;}
        
        size_t
        size()  const
          {
            return storage_? size(unConst(this)->storage_)
                           : 0;
          }
        
        Literal const&
        operator[] (size_t idx)  const
          {
            REQUIRE (storage_ and idx < size());
            return storage_[1+idx];
          }
        
        size_t
        indexOf (Literal const* pos)  const
          {
            REQUIRE (isValid (pos));
            return pos - (storage_+1);
          }
        
        
        bool
        isValid (Literal const* pos)  const
          {
            return storage_
               and storage_ < pos
               and pos < storage_ + (1 + size (unConst(this)->storage_));
          }
        
        void
        resizeTo (size_t cnt)
          {
            if (cnt > size())
              { // copy to expanded storage
                auto target = new const char* [cnt+1];
                auto pos = std::copy (storage_, storage_+1+size(), target);
                for ( ; pos < target+1+cnt; ++pos)
                  *pos = nullptr;
                if (storage_)
                  delete[] storage_;
                storage_ = reinterpret_cast<PStorage> (target);
                size (storage_) = cnt;
                return;
              }
            if (not storage_) return;
            if (cnt == 0)
              {
                delete[] storage_;
                storage_ = nullptr;
              }
            else
              size(storage_) = cnt;
              // excess elements now unreachable
              // note: delete[] knows original size
          }
      };
  }//(End)Implementation helper
  
  
  
  using meta::pickArg;
  using meta::pickInit;
  using meta::IndexSeq;
  
  /**
   * Abstraction for path-like topological coordinates.
   * A sequence of Literal strings, with array-like access and
   * standard iteration. Implemented as fixed size inline tuple
   * with heap allocated unlimited extension space.
   * @note contents are normalised on initialisation
   *       - leading empty elements are filled with Symbol::EMPTY
   *       - empty elements in the middle are replaced by "*"
   *       - trailing empty elements and "*" are trimmed
   */
  template<size_t chunk_size>
  class PathArray
    {
      static_assert (0 < chunk_size, "PathArray chunk_size must be nonempty");
      
      using CcP          = const char*;
      using LiteralArray = std::array<Literal, chunk_size>;
      
      LiteralArray elms_;
      con::Extension tail_;
      
      /**
       * @internal delegate ctor to place the initialiser arguments appropriately
       * @remarks the two index sequences passed by pattern match determine which
       *  arguments go to the inline array, and which go to heap allocated extension.
       *  The inline array has fixed size an is thus filled with trailing `NULL` ptrs,
       *  which is achieved with the help of meta::pickInit(). The con::Extension
       *  is an embedded smart-ptr, which, when receiving additional tail arguments,
       *  will place and manage them within a heap allocated array.
       */
      template<size_t...prefix, size_t...rest, typename...ARGS>
      PathArray (IndexSeq<prefix...>
                ,IndexSeq<rest...>
                ,ARGS&& ...args)
        : elms_{pickInit<prefix,CcP> (forward<ARGS>(args)...) ...}
        , tail_{pickArg<rest>        (forward<ARGS>(args)...) ...}
        {
          this->normalise();
        }
      
      /**
       * @internal rebinding helper for building sequences of index numbers,
       *  to route the initialiser arguments into the corresponding storage
       *  - the leading (`chunk_size`) arguments go into the LiteralArray inline
       *  - all the remaining arguments go into heap allocated extension storage
       */
      template<typename...ARGS>
      struct Split
        {
          using Prefix = typename meta::BuildIndexSeq<chunk_size>::Ascending;
          using Rest   = typename meta::BuildIdxIter<ARGS...>::template After<chunk_size>;
        };
      
    public:
      template<typename...ARGS>
      explicit
      PathArray (ARGS&& ...args)
        : PathArray(typename Split<ARGS...>::Prefix()
                   ,typename Split<ARGS...>::Rest()
                   ,forward<ARGS> (args)...)
        { }
      
      PathArray(PathArray&&)                  = default;
      PathArray(PathArray const&)             = default;
      PathArray(PathArray& o)  : PathArray((PathArray const&)o) { }
      PathArray& operator= (PathArray const&) = default;
      PathArray& operator= (PathArray &&)     = default;
                                               ////////////////////////TICKET #963  Forwarding shadows copy operations
      
      
      size_t
      size()  const
        {
          return tail_? chunk_size + tail_.size()
                      : findInlineEnd() - elms_.begin();
        }
      
      bool
      empty()  const
        {
          return not elms_[0]; // normalise() ensures nonnull unless completely empty
        }
      
      /** joins nonempty content, separated by slash */
      operator string()  const;
      
      
      /** Array style indexed access.
       * @throws error::Invalid on bound violation
       * @return reference to the normalised content
       * @note returns Symbol::EMPTY for leading empty elements,
       *       even while iteration will skip such entries.
       */
      Literal const&
      operator[] (size_t idx)  const
        {
          Literal* elm = unConst(this)->getPosition (idx);
          if (not elm)
            throw error::Invalid ("Accessing index "+util::toString(idx)
                                 +" on PathArray of size "+ util::toString(size())
                                 ,error::LUMIERA_ERROR_INDEX_BOUNDS);
          return *elm;
        }
      
      /** reverse lookup of actual path content
       * @param content reference to actual content _residing within the path_
       * @return index position of the content within the path
       * @throws error::Invalid when the given storage location
       *         is outside the data content storage of this path
       */
      size_t
      indexOf (Literal const& content)  const
        {
          if (elms_.begin() <= &content and &content < elms_.end())
            return &content - elms_.begin();
          if (tail_.isValid (&content))
            return chunk_size + tail_.indexOf (&content);
          
          throw error::Invalid ("Referred content "+util::toString(&content)
                               +" is not located within the storage of PathArray "
                               +string(*this));
        }
      
      
    protected:  /* ==== Iteration control API for IterAdapter ==== */
      
      /** Implementation of Iteration-logic: pull next element. */
      friend void
      iterNext (const PathArray*, const Literal*& pos)
      {
        ++pos;
      }
      
      /** Implementation of Iteration-logic: detect iteration end. */
      friend bool
      checkPoint (const PathArray* src, const Literal*& pos)
      {
        REQUIRE (src);
        if (pos == src->elms_.end() and src->tail_)
          pos = &src->tail_[0];
        else
        if (not src->isValid (pos))
          {
            pos = nullptr;
            return false;
          }
        ENSURE (  (src->elms_.begin() <= pos and pos < src->elms_.end())
                or src->tail_.isValid(pos));
        return true;
      }
      
    public:
      using const_iterator = lib::IterAdapter<Literal const*, PathArray const*>;
      using iterator = const_iterator;
      
      /** @remark iteration is defined to begin with real content */
      iterator begin()  const { return firstNonempty(); }
      iterator end()    const { return iterator{}; }
      
      friend iterator begin(PathArray const& pa) { return pa.begin();}
      friend iterator end  (PathArray const& pa) { return pa.end();  }
      
      
      
    private: /* ===== implementation details ===== */
      
      bool
      isValid (Literal const* pos)  const
        {
          return pos
             and (tail_.isValid(pos)
                  or (elms_.begin() <= pos and pos < elms_.end()
                      and *pos));
        }
      
      iterator
      firstNonempty ()  const
        {
          iterator startPos{this, elms_.begin()};
          while (startPos && isnil (*startPos))
            ++startPos;
          return startPos;
        }
      
      /**
       * find _effective end_ of data in the inline array,
       * i.e. the position _behind_ the last usable content
       */
      Literal const*
      findInlineEnd() const
        {
          Literal const* lastPos = elms_.begin() + chunk_size-1;
          Literal const* beforeStart = elms_.begin() - 1;
          while (lastPos != beforeStart and not *lastPos)
            --lastPos;
          return ++lastPos; // at start if empty, else one behind the last
        }
      
    protected:
      /**
       * @internal access content element by index
       * @return pointer to storage, `null` if out of bounds
       */
      Literal*
      getPosition (size_t idx)
        {
          Literal const* elm =nullptr;
          if (idx < chunk_size)
            elm = elms_[idx]? &elms_[idx] : nullptr;
          else
          if (idx-chunk_size < tail_.size())
            elm = &tail_[idx-chunk_size];
          
          return const_cast<Literal*> (elm);
        }
      
      /**
       * @internal ensure storage for the indicated position exists
       * @return pointer to the storage, either existing or reallocated
       */
      Literal*
      maybeExpandTo (size_t idx)
        {
          if (chunk_size <= idx and size() <= idx)
            tail_.resizeTo(idx+1 - chunk_size);
          if (idx < chunk_size)
            return elms_.begin() + idx;
          
          ENSURE (idx-chunk_size < tail_.size());
          return const_cast<Literal*> (&tail_[idx-chunk_size]);
        }
      
      /** @internal force new content into the given entry */
      void
      setContent (Literal* pos, const char* val)
        {
          REQUIRE (pos);
          *reinterpret_cast<const char**> (pos) = val;
        }
      
      /**
       * establish the _contract_ of PathArray
       * - no null pointer within the valid storage range
       * - storage is precisely trimmed at the end
       * - missing leading elements are filled with Symbol::EMPTY
       * - missing inner elements are interpolated as "*"
       */
      void
      normalise()
        {
          if (size() == 0) return;
          static Symbol ANY("*");
          const char* fill = Symbol::EMPTY;
          
          Literal* end = elms_.end();
          Literal* pos = elms_.begin();
          for ( ; pos!=end; ++pos)
            if (isnil (*pos))
              setContent (pos, fill);
            else
              if (fill==Symbol::EMPTY)
                fill = ANY;
          
          if (tail_)
            {
              // normalise data in extension storage
              pos = getPosition (chunk_size);
              end = pos + tail_.size();
              for ( ; pos!=end; ++pos)
                if (isnil (*pos))
                  setContent (pos, fill);
                else
                  if (fill==Symbol::EMPTY)
                    fill = ANY;
            }
          
          size_t idx = size();
          // trim trailing fill
          while (idx and fill == *getPosition (--idx))
            setContent (getPosition(idx), nullptr);
          
          if (idx >= chunk_size-1)
            tail_.resizeTo (idx+1 - chunk_size);
          else
            tail_.resizeTo (0);
        }
    };
  
  
  
  
  template<size_t chunk_size>
  inline
  PathArray<chunk_size>::operator string()  const
  {
    if (this->empty()) return "";
    
    string buff;
    size_t expectedLen = this->size() * 10;
    buff.reserve (expectedLen); // heuristic
    for (Literal elm : *this)
      buff += elm + "/";
    
    // chop off last delimiter
    size_t len = buff.length();
    ASSERT (len >= 1);
    buff.resize(len-1);
    return buff;
  }
  
  
  
  /**
   * Equality comparison of arbitrary PathArray objects
   */
  template<size_t cl, size_t cr>
  bool
  operator== (PathArray<cl> const& l, PathArray<cr> const& r)
  {
    if (l.size() != r.size()) return false;
    
    typename PathArray<cl>::iterator lp = l.begin();
    typename PathArray<cl>::iterator rp = r.begin();
    while (lp and rp)
      {
        if (*lp != *rp) return false;
        ++lp;
        ++rp;
      }
    return isnil(lp) and isnil(rp);
  }
  
  template<size_t cl, size_t cr>
  bool
  operator!= (PathArray<cl> const& l, PathArray<cr> const& r)
  {
    return not (l == r);
  }
  
  
  
}// namespace lib
#endif /*LIB_PATH_ARRAY_H*/
