/*
  EXTENT-FAMILY.hpp  -  maintain a sequence of memory extents used cyclically

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file extent-family.hpp
 ** Memory management scheme for cyclically used memory extents.
 ** In this context, _Extent_ denotes a larger block of memory to hold a
 ** cluster of smaller records, which are closely interrelated and will be managed
 ** and de-allocated together. The typical usage involves a constant demand for further
 ** memory, with is satisfied by putting further unused extents into use; older extents
 ** will fall out of use eventually, and can then be placed back into a buffer of free
 ** extents. In accordance to overall demand, this reserve buffer can be scaled up
 ** and down to avoid holding larger amounts of unused memory, while the availability
 ** of a baseline amount of memory can be enforced.
 ** 
 ** @see ExtentFamily_test
 ** @see gear::BlockFlow usage example
 ** 
 ** @todo WIP-WIP-WIP 7/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_MEM_EXTENT_FAMILY_H_
#define SRC_VAULT_MEM_EXTENT_FAMILY_H_


#include "vault/common.hpp"
//#include "lib/symbol.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

//#include <string>
#include <algorithm>
#include <memory>
#include <vector>
#include <array>


namespace vault{
namespace mem {
  
//  using util::isnil;
//  using std::string;
  using util::unConst;
  
  template<typename T, size_t siz>
  class ExtentDiagnostic;

  
  /**
   * Memory manager to provide a sequence of Extents for cyclic usage.
   * @todo WIP-WIP 7/2023
   * @see ExtentFamily_test
   */
  template<typename T, size_t siz>
  class ExtentFamily
    : util::NonCopyable
    {
      /** number of excess new extents
       *  to add whenever new storage is required
       */
      static const size_t EXCESS_ALLOC{5};
      
    public:
      struct Extent
        : std::array<T,siz>
        {
          using Payload = T;
          using SIZ = std::integral_constant<size_t,siz>;
        };
      
    private:
      struct Storage
        : std::unique_ptr<char[]>
        {
          /**
           * @note default ctor immediately allocates the full storage,
           *       but without initialisation since payload is `char`
           */
          Storage()
            : unique_ptr{new char[sizeof(Extent)]}
            { }
          
          /** access projected Extent storage type
           * @warning payload is uninitialised and dtors won't be invoked
           */
          Extent&
          access()
            {
              ENSURE (get() != nullptr);
              return reinterpret_cast<Extent&> (*get());
            }
        };
      using Extents = std::vector<Storage>;
      
      
      /**
       * Iteration »State Core« based on Extents index position.
       * @remark builds upon the specific wrap-around logic for
       *         cyclic reuse of extent storage and allocation.
       */
      struct IdxLink
        {
          ExtentFamily* exFam{nullptr};
          size_t        index{0};
          
          /* === state protocol API for IterStateWrapper === */
          bool
          checkPoint()  const
            {
              return exFam and index != exFam->after_;
            }
          
          Extent&
          yield()  const
            {
              return exFam->access (index);
            }
          
          void
          iterNext()
            {
              exFam->incWrap (index);
            }
          
          bool
          operator== (IdxLink const& oi)  const
            {
              return exFam == oi.exFam
                 and index == oi.index;
            }
        };
      
      
      /* ==== Management Data ==== */
      
      Extents extents_;
      size_t start_,after_;
      
    public:
      explicit
      ExtentFamily(size_t initialCnt =0)
        : extents_{initialCnt}
        , start_{0}        //  Extents allocated yet marked unused
        , after_{0}
        { }
      
      void
      reserve (size_t expectedMaxExtents)
        {
          extents_.reserve (expectedMaxExtents);
        }
      
      /** claim next \a cnt extents, possibly allocate.
       * @remark the index pos previously marked as #after_
       *         will always become the first new storage slot.
       * @warning in case of #isWrapped, #first_ will be modified
       *         and thus _any existing indices may be invalidated._
       * @note   always allocating slightly in excess
       */
      void
      openNew (size_t cnt =1)
        {
          if (not canAccomodate (cnt))
            {//insufficient reserve -> allocate
              size_t oldSiz = extents_.size();
              size_t addSiz = cnt - freeSlotCnt()
                              + EXCESS_ALLOC;
              // add a strike of new extents at the end
              extents_.resize (oldSiz + addSiz);
              if (isWrapped())
                {// need the new elements in the middle, before the existing start_
                  auto p = extents_.begin();
                  auto first = p + start_;
                  auto mid   = p + oldSiz;
                  auto last  = p + oldSiz + addSiz;
                  // relocate [fist..mid) after [mid..last)
                  std::rotate (first, mid, last);
                  start_ += addSiz;
                }
            }
          // now sufficient reserve extents are available
          ENSURE (canAccomodate (cnt));
          incWrap (after_, cnt);
        }
      
      /** discard oldest \a cnt extents */
      void
      dropOld (size_t cnt)
        {
          REQUIRE (cnt <= activeSlotCnt());
          incWrap (start_, cnt);
        }                        ////////////////////////////////////////////////////////////////////////////TICKET #1316 : should reduce excess allocation (with apropriate damping to avoid oscillations)
      
      
      /** allow transparent iteration of Extents,
       *  with the ability to expand storage */
      struct iterator
        : lib::IterStateWrapper<Extent, IdxLink>
        {
          size_t getIndex() { return this->stateCore().index;    }
          void expandAlloc(){ this->stateCore().exFam->openNew();}
        };
      
      
      iterator begin() { return iterator{IdxLink{this, start_}}; }
      iterator end()   { return iterator{IdxLink{this, after_}}; }
      
      
    private:
      bool
      isWrapped()  const
        {
          return after_ < start_;
        }     // note: both are equal only when empty
      
      /** @return number of allocated slots actually used */
      size_t
      activeSlotCnt()  const
        {
          REQUIRE (start_ <  extents_.size());
          REQUIRE (after_ <= extents_.size());
          
          return not isWrapped()? after_ - start_
                                : (after_ - 0)
                                 +(extents_.size() - start_);
        }
      
      size_t
      freeSlotCnt()  const
        {          // always keep one in reserve...
          REQUIRE (activeSlotCnt() < extents_.size());
          
          return extents_.size() - activeSlotCnt();
        }
      
      bool
      canAccomodate (size_t addCnt)  const
        {
          return addCnt < freeSlotCnt();  // keep one in reserve
        }
      
      /** increment index, but wrap at array end.
       * @remark using the array cyclically
       */
      void
      incWrap (size_t& idx, size_t inc =1)
        {
          idx = (idx+inc) % extents_.size();
        }
      
      bool
      isValidPos (size_t idx)  const
        {
          REQUIRE (idx < extents_.size());
          REQUIRE (activeSlotCnt() > 0);
          
          return isWrapped()? (start_ <= idx and idx < extents_.size())
                               or idx < after_
                            : (start_ <= idx and idx < after_);
        }
      
      Extent&
      access (size_t idx)  const
        {
          REQUIRE (isValidPos (idx));
          return unConst(this)->extents_[idx].access();
        }     // deliberately const-ness does not cover payload
      
      
      /// „backdoor“ to watch internals from tests
      friend class ExtentDiagnostic<T,siz>;
    };
  
  
  
  
  
  
  
  /* ===== Test / Diagnostic ===== */
  
  template<typename T, size_t siz>
  struct ExtentDiagnostic
    {
      using ExFam = ExtentFamily<T,siz>;
      
      ExFam& exFam_;
      
      size_t first()  { return exFam_.start_; }
      size_t last()   { return exFam_.after_; }
      size_t size()   { return exFam_.extents_.size(); }
      size_t active() { return exFam_.activeSlotCnt(); }
    };
  
  template<typename T, size_t siz>
  inline ExtentDiagnostic<T,siz>
  watch (ExtentFamily<T,siz>& extentFamily)
  {
    return ExtentDiagnostic<T,siz>{extentFamily};
  }
  
}} // namespace vault::mem
#endif /*SRC_VAULT_MEM_EXTENT_FAMILY_H_*/
